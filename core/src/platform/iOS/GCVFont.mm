/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */

#import "GCVFont.h"

#include <malloc/malloc.h>
#include <unordered_map>

#include "GFontStyle.h"
#include "GCanvas2dContext.h"

#define GCV_FONT_GLYPH_PADDING 0

#pragma mark - GFontLayout
@implementation GFontLayout

@end

#pragma mark - GCVFont
@interface GCVFont()
{
    // Font preferences
    float pointSize, ascent, descent, glyphPadding, xHeight, capHeight;
    float lineWidth;
    float scale;
    
    // mem reuse
    CGGlyph *glyphsBuffer;
    CGPoint *positionsBuffer;
    
    // font cache
    NSMutableDictionary *fontCache;
    NSString *curFontName;
}

@property (nonatomic, assign) BOOL isStroke;
@property (nonatomic, assign) CTFontRef ctMainFont;

@end

@implementation GCVFont
{
    NSString            *_key;
}

static NSMutableDictionary *staticFontInstaceDict;
+ (NSMutableDictionary*)staticFontInstaceDict
{
    static NSMutableDictionary *fontInstanceDict;
    if( !fontInstanceDict ){
        fontInstanceDict = NSMutableDictionary.dictionary;
    }
    return fontInstanceDict;
}

+ (instancetype)createGCFontWithKey:(NSString*)key
{
    GCVFont *gcvFont = [[GCVFont alloc] initWithKey:key];
    self.staticFontInstaceDict[key] = gcvFont;
    return gcvFont;
}

+ (GCVFont *)getGCVFontWithKey:(NSString*)key
{
    return self.staticFontInstaceDict[key];
}

+ (void)removeGCVFontWithKey:(NSString*)key
{
    return [self.staticFontInstaceDict removeObjectForKey:key];
}

- (instancetype)initWithKey:(NSString *)key
{
    if (self = [super init]) {
        _key = key;
        fontCache = NSMutableDictionary.dictionary;
    }
    return self;
}

- (void)dealloc
{
    if (self.ctMainFont) {
        CFRelease(self.ctMainFont);
    }
    fontCache = nil;
}

- (void)cleanFont
{
    @synchronized(self){
        [GCVFont removeGCVFontWithKey:_key];
        [fontCache removeAllObjects];
    }
}

- (BOOL)isFamilySupportBold:(NSString *)family
{
    NSMutableDictionary *attrstmp = [NSMutableDictionary dictionary];
    attrstmp[(NSString *)kCTFontFamilyNameAttribute] = family;
    attrstmp[(NSString *)kCTFontSizeAttribute] = @(10);
    
    CTFontSymbolicTraits symbolicTraitstmp = 0;
    symbolicTraitstmp |= kCTFontBoldTrait;
    NSDictionary *traitstmp = @{(NSString *)kCTFontSymbolicTrait:@(symbolicTraitstmp)};
    attrstmp[(NSString*)kCTFontTraitsAttribute] = traitstmp;
    
    CTFontDescriptorRef fontDesctmp =CTFontDescriptorCreateWithAttributes((CFDictionaryRef)attrstmp);
    CTFontRef fonttmp = CTFontCreateWithFontDescriptor(fontDesctmp, 0, NULL);
    if (fontDesctmp){
        CFRelease(fontDesctmp);
    }
    
    CTFontSymbolicTraits traits_tmp = CTFontGetSymbolicTraits(fonttmp);
    BOOL isBoldtmp = ((traits_tmp & kCTFontBoldTrait) == kCTFontBoldTrait);
    CFRelease(fonttmp);
    return isBoldtmp;
}

- (CTFontRef)createFontFromStyle:(gcanvas::GFontStyle *)fontStyle
{
    NSString *family = [NSString stringWithUTF8String:fontStyle->GetFamily().c_str()];
    // 这段逻辑主要是对不支持字体加粗的默认走Arial字体
    if (fontStyle->GetWeight() == gcanvas::GFontStyle::Weight::BOLD) {
        BOOL isBoldtmp = [self isFamilySupportBold:family];
        if (!isBoldtmp) {
            family = @"Arial";
        }
    }

    NSMutableDictionary *attrs = [NSMutableDictionary dictionary];
    // family
    attrs[(NSString *)kCTFontFamilyNameAttribute] = family;
    // size
    attrs[(NSString *)kCTFontSizeAttribute] = @(fontStyle->GetSize() * scale);
    // symbolicTraits
    CTFontSymbolicTraits symbolicTraits = 0;
    if (fontStyle->GetWeight() == gcanvas::GFontStyle::Weight::BOLD)
    {
        symbolicTraits |= kCTFontBoldTrait;
    }
    BOOL isItalic = NO;
    if (fontStyle->GetStyle()==gcanvas::GFontStyle::Style::ITALIC)
    {
        isItalic = YES;
        symbolicTraits |= kCTFontItalicTrait;
    }
    if (symbolicTraits)
    {
        NSDictionary *traits = @{(NSString *)kCTFontSymbolicTrait:@(symbolicTraits)};
        attrs[(NSString*)kCTFontTraitsAttribute] = traits;
    }
    
    CTFontDescriptorRef fontDesc = CTFontDescriptorCreateWithAttributes((CFDictionaryRef)attrs);
    
    CGAffineTransform matrix = CGAffineTransformIdentity;
    
    if (isItalic)
    {
        matrix.c = tan(15*M_PI/180);  //这里有个bug，当有缩放时，会影响倾斜角度。
    }
    
    CTFontRef curFont = CTFontCreateWithFontDescriptor(fontDesc, 0, &matrix);
    
    if (fontDesc)
    {
        CFRelease(fontDesc);
    }
    
    return curFont;
}

- (void)resetWithFontStyle:(gcanvas::GFontStyle *)fontStyle
                  isStroke:(BOOL)isStroke
                   context:(GCanvasContext *)context
{
    self.isStroke = isStroke;
    
    glyphPadding = GCV_FONT_GLYPH_PADDING;
    if (isStroke) {
        lineWidth = context->LineWidth() * context->mDevicePixelRatio;
        glyphPadding = GCV_FONT_GLYPH_PADDING + lineWidth;
    }
    
    scale = 1.0;
    if (context) {
        scale = GTransformGetMaxScale(context->mCurrentState->mTransform);
    }
    
    curFontName = [self getFontNameWithCurrentScale:fontStyle context:context];
    CTFontRef fontRef = (__bridge CTFontRef)([fontCache objectForKey:curFontName]);
    if (!fontRef) {
        fontRef = [self createFontFromStyle:fontStyle];
        if (fontRef) {
            [fontCache setObject:(__bridge id)fontRef forKey:curFontName];
        }
    }
    
    if (fontRef) {
        self.ctMainFont = fontRef;
        pointSize = fontStyle->GetSize();
        ascent = CTFontGetAscent(self.ctMainFont); // 基线到最顶
        descent = CTFontGetDescent(self.ctMainFont); // 基线到最低
        xHeight = CTFontGetXHeight(self.ctMainFont); // 小写字母高度,x为参考
        capHeight = CTFontGetCapHeight(self.ctMainFont); //大写字母高度,C为参考
    }
}

- (GFontLayout *)getLayoutForString:(NSString *)string withFontName:(NSString*)fontName
{
    GTextMetrics metrics = {
        .width = 0,
        .ascent = 0,
        .descent = 0,
    };
    NSMutableData *layoutData = [NSMutableData dataWithCapacity:string.length];

    GFontLayout *fontLayout = [[GFontLayout alloc] init];
    fontLayout.glyphLayout = layoutData;
    fontLayout.glyphCount = 0;
  
    int offsetX = 0;
    NSInteger i,n = [string length];
    unichar c;
    // 这里对编码格式的支持有问题
    for (i = 0; i < n; i++) {
        c = [string characterAtIndex:i];
        [self getGlyphForChar:c withFontName:fontName withFontLayout:fontLayout withOffsetX:&offsetX];
        ++fontLayout.glyphCount;
    }
    
    metrics.width = offsetX;
    metrics.ascent = fontLayout.metrics.ascent;
    metrics.descent = fontLayout.metrics.descent;
    fontLayout.metrics = metrics;
    
    return fontLayout;
}

- (void )drawString:(NSString*)string
       withFontName:(NSString*)fontName
         withLayout:(GFontLayout*)fontLayout
       withPosition:(CGPoint)destPoint
            context:(GCanvasContext *)context
{
    bool needDrawShadow = context->NeedDrawShadow();
    int offsetX = 0;
    GColorRGBA color = self.isStroke ? BlendStrokeColor(context) : BlendFillColor(context);

    float scale = GTransformGetMaxScale( context->mCurrentState->mTransform );
    
    NSInteger i,n = [string length];
    unichar c;
    for (i = 0; i < n; i++) {
        c = [string characterAtIndex:i];
        
        const GGlyph *pGlyph = _glyphCache->GetGlyph([fontName UTF8String], c, fontName.UTF8String, self.isStroke);
        
        if (pGlyph == nullptr) {
            // texture is full，get font glyph again
            GFontLayout *fontLayout = [[GFontLayout alloc] init];
            fontLayout.glyphLayout = [NSMutableData dataWithCapacity:string.length];
            fontLayout.glyphCount = 0;
            int offsetX = 0;
            [self getGlyphForChar:c withFontName:fontName withFontLayout:fontLayout withOffsetX:&offsetX];
            // get glyph again
            pGlyph = _glyphCache->GetGlyph([fontName UTF8String], c, [fontName UTF8String], self.isStroke);
        }
        
        if (pGlyph) {
            GFontGlyphLayout gl;
            
            gl.info = *pGlyph;
            gl.xpos = offsetX;
            offsetX += gl.info.advanceX;
            
            GLuint textureId = InvalidateTextureId;
            if (gl.info.texture != NULL){
                textureId = gl.info.texture->GetTextureID();
            }
            context->SetTexture(textureId);
            
            GGlyph &glyphInfo = gl.info;
            float gx = destPoint.x + gl.xpos + glyphInfo.offsetX / scale;
            float gy = destPoint.y - (glyphInfo.height / scale + glyphInfo.offsetY / scale);
            
            
            if( needDrawShadow ){
                std::vector<GVertex> vec;
                context->PushRectangle(gx,
                    gy,
                    glyphInfo.width / scale,
                    glyphInfo.height / scale,
                    glyphInfo.s0,
                    glyphInfo.t1,
                    (glyphInfo.s1-glyphInfo.s0),
                    (glyphInfo.t0-glyphInfo.t1),
                    color,
                    context->mCurrentState->mTransform,
                                       false, &vec);

                GRectf rect;
                GPath::GetRectCoverVertex(rect, vec);
                context->DrawShadow(rect, [&] {
                   context->PushVertexs(vec);
                });
                context->PushVertexs(vec);
            } else {
                context->PushRectangle(gx,
                                       gy,
                                       glyphInfo.width / scale,
                                       glyphInfo.height / scale,
                                       glyphInfo.s0,
                                       glyphInfo.t1,
                                       (glyphInfo.s1-glyphInfo.s0),
                                       (glyphInfo.t0-glyphInfo.t1),
                                       color,
                                       context->mCurrentState->mTransform);
            }
            
            #if 0 //test font outline
            context->SendVertexBufferToGPU();
            context->Save();
            context->SetFillStyle({0,0,1.0,0.05});
            context->FillRect(gx, gy, glyphInfo.width / scale, glyphInfo.height / scale);
            context->Restore();
            #endif
        }
    }
}

- (void)getGlyphForChar:(wchar_t)c
           withFontName:(NSString*)fontName
         withFontLayout:(GFontLayout *)fontLayout
            withOffsetX:(int *)offsetX
{
    GTextMetrics metrics = fontLayout.metrics;
    NSMutableData *layoutData = fontLayout.glyphLayout;
    const GGlyph *pGlyph = _glyphCache->GetGlyph([fontName UTF8String], c, [fontName UTF8String], self.isStroke);
    
    if (pGlyph) {
        GFontGlyphLayout gl;
        
        gl.info = *pGlyph;
        gl.xpos = *offsetX;
        *offsetX += gl.info.advanceX;
        metrics.ascent = MAX(metrics.ascent, gl.info.height + gl.info.offsetY - glyphPadding);
        metrics.descent = MAX(metrics.descent, -gl.info.offsetY - glyphPadding);
        
        NSData *data = [NSData dataWithBytes:&gl length:sizeof(gl)];
        [layoutData appendData:data];
        fontLayout.metrics = metrics;
        return ;
    }
    
    GCVFont *curFont = self;
    
    NSString *nsString =[[NSString alloc] initWithBytes:&c length:sizeof(c) encoding:NSUTF32LittleEndianStringEncoding];
    if (!nsString) {
        return;
    }
    // Create attributed line
    NSAttributedString *attributes = [[NSAttributedString alloc]
                                      initWithString:nsString
                                      attributes:@{ (id)kCTFontAttributeName: (__bridge id)curFont.ctMainFont }];
    
    CTLineRef line = CTLineCreateWithAttributedString((CFAttributedStringRef)attributes);
    CFArrayRef runs = CTLineGetGlyphRuns(line);
    NSInteger runCount = CFArrayGetCount(runs);
    
    int layoutIndex = 0;
    for (int i = 0; i < runCount; i++) {
        CTRunRef run = (CTRunRef)CFArrayGetValueAtIndex(runs, i);
        NSInteger runGlyphCount = CTRunGetGlyphCount(run);
        CTFontRef runFont = (CTFontRef)CFDictionaryGetValue(CTRunGetAttributes(run), kCTFontAttributeName);
        // Fetch glyphs buffer
        const CGGlyph *glyphs = CTRunGetGlyphsPtr(run);
        if (!glyphs) {
            size_t glyphsBufferSize = sizeof(CGGlyph) * runGlyphCount;
            if (malloc_size(glyphsBuffer) < glyphsBufferSize) {
                glyphsBuffer = (CGGlyph *)realloc(glyphsBuffer, glyphsBufferSize);
            }
            CTRunGetGlyphs(run, CFRangeMake(0, 0), glyphsBuffer);
            glyphs = glyphsBuffer;
        }
        // Fetch Positions buffer
        CGPoint *positions = (CGPoint *)CTRunGetPositionsPtr(run);
        if (!positions) {
            size_t positionsBufferSize = sizeof(CGPoint) * runGlyphCount;
            if (malloc_size(positionsBuffer) < positionsBufferSize) {
                positionsBuffer = (CGPoint *)realloc(positionsBuffer, positionsBufferSize);
            }
            CTRunGetPositions(run, CFRangeMake(0, 0), positionsBuffer);
            positions = positionsBuffer;
        }
        
        // Go through all glyphs for this run, create the textures and collect the glyph
        // info and positions as well as the max ascent and descent
        for (int g = 0; g < runGlyphCount; g++) {
            GFontGlyphLayout gl;

            CGGlyph glyph = glyphs[g];
            gl.info.charcode = c;
            gl.xpos = positions[g].x + *offsetX;

            [curFont createGlyph:glyph withFont:runFont withFontName:fontName glyphInfo:&gl.info];
            *offsetX += gl.info.advanceX;
            metrics.ascent = MAX(metrics.ascent, gl.info.height + gl.info.offsetY - glyphPadding);
            metrics.descent = MAX(metrics.descent, -gl.info.offsetY - glyphPadding);
            layoutIndex++;
            
            NSData *data = [NSData dataWithBytes:&gl length:sizeof(gl)];
            [layoutData appendData:data];
        }
    }
    // Create the layout object and add it to the cache
    fontLayout.metrics = metrics;
    
    CFRelease(line);
}

- (void)createGlyph:(CGGlyph)glyph
           withFont:(CTFontRef)font
       withFontName:(NSString*)fontName
          glyphInfo:(GGlyph *)glyphInfo
{
    CGRect bbRect;
    CTFontGetBoundingRectsForGlyphs(font, kCTFontOrientationDefault, &glyph, &bbRect, 1);
    CGSize advance;
    CTFontGetAdvancesForGlyphs(font, kCTFontOrientationDefault, &glyph, &advance, 1);
    glyphInfo->advanceX = advance.width / scale;
    glyphInfo->advanceY = advance.height /scale;
    
    // Add some padding around the glyphs
    glyphInfo->offsetY = floorf(bbRect.origin.y) - glyphPadding;
    glyphInfo->offsetX = floorf(bbRect.origin.x) - glyphPadding;
    glyphInfo->width = bbRect.size.width / scale + glyphPadding * 2;
    glyphInfo->height = bbRect.size.height / scale + glyphPadding * 2;
    
    // Size needed for this glyph in pixels; must be a multiple of 8 for CG
    int pxWidth = floorf((glyphInfo->width) / 8 + 1) * 8;
    int pxHeight = floorf((glyphInfo->height) / 8 + 1) * 8;
    
    pxWidth *= scale;
    pxHeight *= scale;
    
    NSMutableData *pixels = [NSMutableData dataWithLength:pxWidth * pxHeight];
    CGContextRef context = CGBitmapContextCreate(pixels.mutableBytes, pxWidth, pxHeight, 8, pxWidth, NULL, kCGImageAlphaOnly);
    
    CGContextSetFontSize(context, pointSize);
    CGContextTranslateCTM(context, 0.0, pxHeight);
    CGContextScaleCTM(context, 1, -1.0);
    
    // Fill or stroke?
    if (!self.isStroke) {
        CGContextSetTextDrawingMode(context, kCGTextFill);
        CGContextSetGrayFillColor(context, 1.0, 1.0);
    } else {
        CGContextSetTextDrawingMode(context, kCGTextStroke);
        CGContextSetGrayStrokeColor(context, 1.0, 1.0);
        CGContextSetLineWidth(context, lineWidth*scale); //jwx
    }
    
    // Render glyph and update the texture
    CGPoint p = CGPointMake(-glyphInfo->offsetX, -glyphInfo->offsetY);
    CTFontDrawGlyphs(font, &glyph, &p, 1, context);
    
    glyphInfo->width = pxWidth;
    glyphInfo->height = pxHeight;
    glyphInfo->texture = nullptr;
    glyphInfo->bitmapBuffer = new unsigned char[pxWidth * pxHeight];
    memcpy(glyphInfo->bitmapBuffer, pixels.bytes, pxWidth * pxHeight);
    _glyphCache->Insert([fontName UTF8String], glyphInfo->charcode, [fontName UTF8String], self.isStroke, *glyphInfo);
    
    CGContextRelease(context);
}

- (CGPoint)adjustTextPenPoint:(CGPoint)srcPoint
                   textAlign:(GTextAlign)textAlign
                    baseLine:(GTextBaseline)baseLine
                     metrics:(GTextMetrics)metrics
{
    switch (textAlign) {
        case GTextAlign::TEXT_ALIGN_START:
        case GTextAlign::TEXT_ALIGN_LEFT:
            break;
        case GTextAlign::TEXT_ALIGN_CENTER:
            srcPoint.x -= metrics.width/2.0f;
            break;
        case GTextAlign::TEXT_ALIGN_END:
        case GTextAlign::TEXT_ALIGN_RIGHT:
            srcPoint.x -= metrics.width;
            break;
    }
    
    switch( baseLine ) {
        case GTextBaseline::TEXT_BASELINE_ALPHABETIC:
        case GTextBaseline::TEXT_BASELINE_IDEOGRAPHIC:
            break;
        case GTextBaseline::TEXT_BASELINE_TOP:
        case GTextBaseline::TEXT_BASELINE_HANGING:
            srcPoint.y += ascent;
            break;
        case GTextBaseline::TEXT_BASELINE_MIDDLE:
            srcPoint.y += capHeight/2;
            break;
        case GTextBaseline::TEXT_BASELINE_BOTTOM:
            srcPoint.y -= descent;
            break;
    }
    
    return srcPoint;
}

- (NSString *)getFontNameWithCurrentScale:(gcanvas::GFontStyle *)fontStyle
                                  context:(GCanvasContext *)context
{
    NSString *fontName = [NSString stringWithUTF8String:fontStyle->GetName().c_str()];
    float scaleX = 1.0;
    float scaleY = 1.0;
    
    if (context) {
        GCanvasState *currentState = context->GetCurrentState();
        scaleX = GTransformGetScaleX(currentState->mTransform);
        scaleY = GTransformGetScaleY(currentState->mTransform);
    }
    return [NSString stringWithFormat:@"%@_%f_%f", fontName, scaleX, scaleY];
}

@end
