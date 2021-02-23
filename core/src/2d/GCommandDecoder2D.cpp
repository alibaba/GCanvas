/**
* Created by G-Canvas Open Source Team.
* Copyright (c) 2017, Alibaba, Inc. All rights reserved.
*
* This source code is licensed under the Apache Licence 2.0.
* For the full copyright and license information, please view
* the LICENSE file in the root directory of this source tree.
*/

#ifdef COMMANDBUFFER_2D

#include <stdlib.h>
#include <codecvt>
#include <sstream>
#include <string>

#include "../commandbuffer/GCommandDecoder.hpp"
#include "../support/Log.h"
#include "../support/Util.h"
#include "GCommandDecoder2D.hpp"

namespace gcanvas {

typedef void (*GCommand2DFunc)(GContext2DRenderContext*, GCommandBuffer& , GDecodeRes& );


#define CONTEXT2D_SYNC_FUNC_ID_INSERT(NAME) \
    mContext2DSyncFuncSet.insert(G2D_FUNC_##NAME);

GCommandDecoder2D::GCommandDecoder2D(GContext2DRenderContext* renderCtx)
    : mRenderContext2D(renderCtx)
{
    
}

GCommandDecoder2D::~GCommandDecoder2D()
{
    
}

void GCommandDecoder2D::InitCommands()
{
    //2d sync api
    CONTEXT2D_SYNC_FUNC_ID_INSERT(createImageData_size)
    CONTEXT2D_SYNC_FUNC_ID_INSERT(createImageData_data)
    CONTEXT2D_SYNC_FUNC_ID_INSERT(createLinearGradient)
    CONTEXT2D_SYNC_FUNC_ID_INSERT(createPattern)
    CONTEXT2D_SYNC_FUNC_ID_INSERT(createRadialGradient)
    CONTEXT2D_SYNC_FUNC_ID_INSERT(getImageData)
    CONTEXT2D_SYNC_FUNC_ID_INSERT(getLineDash)
    CONTEXT2D_SYNC_FUNC_ID_INSERT(measureText)
}

bool GCommandDecoder2D::Decode(GCommandBuffer& buffer, GDecodeRes& res)
{
    static GCommand2DFunc mContext2DFuncs[G2D_FUNC_COUNT] = {
        nullptr,
        &GCommandDecoder2D::G2D_arc,
        &GCommandDecoder2D::G2D_arcTo,
        &GCommandDecoder2D::G2D_beginPath,
        &GCommandDecoder2D::G2D_bezierCurveTo,
        &GCommandDecoder2D::G2D_clearRect,
        &GCommandDecoder2D::G2D_clip,
        &GCommandDecoder2D::G2D_closePath,
        &GCommandDecoder2D::G2D_createImageData_size,
        &GCommandDecoder2D::G2D_createImageData_data,
        &GCommandDecoder2D::G2D_createLinearGradient,
        &GCommandDecoder2D::G2D_createPattern,
        &GCommandDecoder2D::G2D_createRadialGradient,
        &GCommandDecoder2D::G2D_drawImage_3,
        &GCommandDecoder2D::G2D_drawImage_5,
        &GCommandDecoder2D::G2D_drawImage_9,
        &GCommandDecoder2D::G2D_fill,
        &GCommandDecoder2D::G2D_fillRect,
        &GCommandDecoder2D::G2D_fillText,
        &GCommandDecoder2D::G2D_getImageData,
        &GCommandDecoder2D::G2D_getLineDash,
        &GCommandDecoder2D::G2D_lineTo,
        &GCommandDecoder2D::G2D_measureText,
        &GCommandDecoder2D::G2D_moveTo,
        &GCommandDecoder2D::G2D_putImageData_3,
        &GCommandDecoder2D::G2D_putImageData_7,
        &GCommandDecoder2D::G2D_quadraticCurveTo,
        &GCommandDecoder2D::G2D_rect,
        &GCommandDecoder2D::G2D_restore,
        &GCommandDecoder2D::G2D_rotate,
        &GCommandDecoder2D::G2D_save,
        &GCommandDecoder2D::G2D_scale,
        &GCommandDecoder2D::G2D_setLineDash,
        &GCommandDecoder2D::G2D_setTransform,
        &GCommandDecoder2D::G2D_storke,
        &GCommandDecoder2D::G2D_strokeRect,
        &GCommandDecoder2D::G2D_strokeText,
        &GCommandDecoder2D::G2D_transform,
        &GCommandDecoder2D::G2D_translate,
         
        &GCommandDecoder2D::G2D_setFillStyle_string,
        &GCommandDecoder2D::G2D_setFillStyle_object,
        &GCommandDecoder2D::G2D_setStrokeStyle_string,
        &GCommandDecoder2D::G2D_setStrokeStyle_object,
        &GCommandDecoder2D::G2D_setShadowColor,
        &GCommandDecoder2D::G2D_setShadowBlur,
        &GCommandDecoder2D::G2D_setShadowOffsetX,
        &GCommandDecoder2D::G2D_setShadowOffsetY,
        &GCommandDecoder2D::G2D_setLineCap,
        &GCommandDecoder2D::G2D_setLineJoin,
        &GCommandDecoder2D::G2D_setLineWidth,
        &GCommandDecoder2D::G2D_setMiterLimit,
        &GCommandDecoder2D::G2D_setLineDashOffset,
        &GCommandDecoder2D::G2D_setFont,
        &GCommandDecoder2D::G2D_setTextAlign,
        &GCommandDecoder2D::G2D_setTextBaseline,
        &GCommandDecoder2D::G2D_setGlobalAlpha,
        &GCommandDecoder2D::G2D_setGlobalCompositeOperation
    };

    uint32_t *v;
    
    while ( (v = buffer.parseValue<uint32_t>()) != nullptr )
    {
        uint32_t funcId = *v;

        if( funcId <= 0 || funcId >= G2D_FUNC_COUNT )
        {
            LOG_E("[Error] Command buffer with wrong Context2D API ID:%d", funcId);
            return false;
        }

        GCommand2DFunc func = mContext2DFuncs[funcId];
        if( !func )
        {
            LOG_E("[Error] Can't find Context2D API ID::%d", funcId);
            return false;
        }
        
        if( !mRenderContext2D )
        {
            LOG_E("[Error] mRenderContext2D is NULL!");
            return false;
        }

//        LOG_D("==========API ID:[%d-%s]==========", funcId, WEBGL_API(funcId) );
        func(mRenderContext2D, buffer, res);
        if( res.hasResult && IsSyncCommand(funcId) )
        {
            return true;
        }
    }
    return true;
}

void GCommandDecoder2D::G2D_arc(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    float mFv[5];
    bool ret = buffer.parseArray<float>(mFv, 5);
    if( !ret ) return;
    
    uint32_t mIv[1];
    ret = buffer.parseArray<uint32_t>(mIv, 1);
    if( !ret ) return; 

    context->mCanvas2D->Arc(mFv[0], mFv[1], mFv[2], mFv[3], mFv[4], mIv[0]);
    LOG_D("[context2d::exec] arc(%f,%f,%f,%f,%f,%d)", mFv[0], mFv[1], mFv[2], mFv[3], mFv[4],  mIv[0]);
}

void GCommandDecoder2D::G2D_arcTo(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    float mFv[5];
    bool ret = buffer.parseArray<float>(mFv, 5);
    if( !ret ) return;
    
    context->mCanvas2D->ArcTo(mFv[0], mFv[1], mFv[2], mFv[3], mFv[4]);
    LOG_D("[context2d::exec] arcTo(%f,%f,%f,%f,%f)", mFv[0], mFv[1], mFv[2], mFv[3], mFv[4]);
}

void GCommandDecoder2D::G2D_beginPath(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    context->mCanvas2D->BeginPath();
    LOG_D("[context2d::exec] beginPath()");
}

void GCommandDecoder2D::G2D_bezierCurveTo(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    float mFv[6];
    bool ret = buffer.parseArray<float>(mFv, 6);
    if( !ret ) return;
    
    context->mCanvas2D->BezierCurveTo(mFv[0], mFv[1], mFv[2], mFv[3], mFv[4], mFv[5]);
    LOG_D("[context2d::exec] bezierCurveTo(%f,%f,%f,%f,%f,%f)", mFv[0], mFv[1], mFv[2], mFv[3], mFv[4], mFv[5]);
}

void GCommandDecoder2D::G2D_clearRect(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    float mFv[4];
    bool ret = buffer.parseArray<float>(mFv, 4);
    if( !ret ) return;
    
    context->mCanvas2D->ClearRect(mFv[0], mFv[1], mFv[2], mFv[3]);
    LOG_D("[context2d::exec] clearRect(%f,%f,%f,%f)", mFv[0], mFv[1], mFv[2], mFv[3]);
}

void GCommandDecoder2D::G2D_clip(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t mIv[1];
    bool ret = buffer.parseArray<uint32_t>(mIv, 1);
    if( !ret ) return;
    
    context->mCanvas2D->Clip(GFillRule(mIv[0]));
    LOG_D("[context2d::exec] clip(%d)", mIv[0]);
}

void GCommandDecoder2D::G2D_closePath(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    context->mCanvas2D->ClosePath();
    LOG_D("[context2d::exec] closePath()");
}

void GCommandDecoder2D::G2D_createImageData_size(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    //TODO
}

void GCommandDecoder2D::G2D_createImageData_data(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    //TODO
}

void GCommandDecoder2D::G2D_createLinearGradient(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    //TODO
}

void GCommandDecoder2D::G2D_createPattern(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    //TODO
}

void GCommandDecoder2D::G2D_createRadialGradient(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    //TODO
}

void GCommandDecoder2D::G2D_drawImage_3(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    uint64_t *uID = buffer.parseValue<uint64_t>();
    if( !uID ) return;
    
    float v[2];
    bool ret = buffer.parseArray<float>(v, 2);
    if( !ret ) return;

    //fetch pixels
    JSBindingPixels gPixels;
    context->FetchPixels(*uID, 0, &gPixels);
    
    if( gPixels.textureId > 0 ) //image/canvas
    {
        float sx = 0, sy = 0;
        float sw = gPixels.width, sh = gPixels.height;
        float dw = sw, dh = sh;
        context->mCanvas2D->DrawImage(gPixels.textureId, gPixels.width, gPixels.height,
                           sx, sy, sw, sh, v[0], v[1], dw, dh);
        LOG_D("[context2d::exec] DrawImage3(%d,%d,%d,%f,%f,%f,%f,%f,%f,%f,%f)",
              gPixels.textureId, gPixels.width, gPixels.height,
              sx, sy, sw, sh, v[0], v[1], dw, dh);
    }
}

void GCommandDecoder2D::G2D_drawImage_5(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    uint64_t *uID = buffer.parseValue<uint64_t>();
    if( !uID ) return;
    
    float v[4];
    bool ret = buffer.parseArray<float>(v, 4);
    if( !ret ) return;

    //fetch pixels
    JSBindingPixels gPixels;
    context->FetchPixels(*uID, 0, &gPixels);
    
    if( gPixels.textureId > 0 ) //image/canvas
    {
        float sx = 0, sy = 0;
        float sw = gPixels.width, sh = gPixels.height;
        context->mCanvas2D->DrawImage(gPixels.textureId, gPixels.width, gPixels.height,
                           sx, sy, sw, sh, v[0], v[1], v[2], v[3]);
        LOG_D("[context2d::exec] DrawImage5(%d,%d,%d,%f,%f,%f,%f,%f,%f,%f,%f)",
              gPixels.textureId, gPixels.width, gPixels.height,
              sx, sy, sw, sh, v[0], v[1], v[2], v[3]);
    }
}

void GCommandDecoder2D::G2D_drawImage_9(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    uint64_t *uID = buffer.parseValue<uint64_t>();
    if( !uID ) return;
    
    float v[8];
    bool ret = buffer.parseArray<float>(v, 8);
    if( !ret ) return;

    //fetch pixels
    JSBindingPixels gPixels;
    context->FetchPixels(*uID, 0, &gPixels);
    
    if( gPixels.textureId > 0 ) //image/canvas
    {
        context->mCanvas2D->DrawImage(gPixels.textureId, gPixels.width, gPixels.height,
                           v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7]);
        LOG_D("[context2d::exec] DrawImage9(%d,%d,%d,%f,%f,%f,%f,%f,%f,%f,%f)",
              gPixels.textureId, gPixels.width, gPixels.height,
              v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7]);
    }
}

void GCommandDecoder2D::G2D_fill(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t mIv[1];
    bool ret = buffer.parseArray<uint32_t>(mIv, 1);
    if( !ret ) return;
    
    context->mCanvas2D->Fill(GFillRule(mIv[0]));
    LOG_D("[context2d::exec] fill(%d)", mIv[0]);
}

void GCommandDecoder2D::G2D_fillRect(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    float mFv[4];
    bool ret = buffer.parseArray<float>(mFv, 4);
    if( !ret ) return;
    
    context->mCanvas2D->FillRect(mFv[0], mFv[1], mFv[2], mFv[3]);
    LOG_D("[context2d::exec] fillRect(%f,%f,%f,%f)", mFv[0], mFv[1], mFv[2], mFv[3]);
}

void GCommandDecoder2D::G2D_fillText(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t mIv[1];
    bool ret = buffer.parseArray<uint32_t>(mIv, 1);
    if( !ret ) return;

    size_t size = mIv[0];
    if( size > 0 )
    {
        char16_t *u16Char = (char16_t*)buffer.parseBufferAlign(size);
        std::string u8Str = U16StrToU8Str(u16Char);
        
        float mFv[3];
        ret = buffer.parseArray<float>(mFv, 3);
        if( !ret ) return;
        
        if( mFv[2] < 0 )
        {
            context->mCanvas2D->DrawText(u8Str.c_str(), mFv[0], mFv[1]);
            LOG_D("[context2d::exec] fillText(%s,%f,%f)", u8Str.c_str(), mFv[0], mFv[1]);
        }
        else
        {
            context->mCanvas2D->DrawText(u8Str.c_str(), mFv[0], mFv[1], mFv[2]);
            LOG_D("[context2d::exec] fillText(%s,%f,%f,%f)", u8Str.c_str(), mFv[0], mFv[1], mFv[2]);
        }
    }
}

void GCommandDecoder2D::G2D_getImageData(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    //TODO
}

void GCommandDecoder2D::G2D_getLineDash(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    //TODO
}

void GCommandDecoder2D::G2D_lineTo(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    float mFv[2];
    bool ret = buffer.parseArray<float>(mFv, 2);
    if( !ret ) return;
   
    context->mCanvas2D->LineTo(mFv[0], mFv[1]);
    LOG_D("[context2d::exec] lineTo(%f,%f)", mFv[0], mFv[1]);
}

void GCommandDecoder2D::G2D_measureText(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t mIv[1];
    bool ret = buffer.parseArray<uint32_t>(mIv, 1);
    if( !ret ) return;

    size_t size = mIv[0];
    if( size > 0 )
    {
        char16_t *u16Char = (char16_t*)buffer.parseBufferAlign(size);
        std::string u8Str = U16StrToU8Str(u16Char);
        float width = context->mCanvas2D->MeasureTextWidth(u8Str.c_str());
        
        res.hasResult = true;
        res.type = GResType_MeasureText;
        res.floatValue = width;
        LOG_D("[context2d::exec] measureText(%s)=%f", u8Str.c_str(), width);
    }
}

void GCommandDecoder2D::G2D_moveTo(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    float mFv[2];
    bool ret = buffer.parseArray<float>(mFv, 2);
    if( !ret ) return;

    context->mCanvas2D->MoveTo(mFv[0], mFv[1]);
    LOG_D("[context2d::exec] moveTo(%f,%f)", mFv[0], mFv[1]);
}

void GCommandDecoder2D::G2D_putImageData_3(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    //TODO
//    self.context->mCanvas2D->DrawImage(texture.textureId, texture.width, texture.height, sx, sy, sw, sh, dx, dy, dw, dh, false);
}

void GCommandDecoder2D::G2D_putImageData_7(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    //TODO
//    self.context->mCanvas2D->DrawImage(texture.textureId, texture.width, texture.height, sx, sy, sw, sh, dx, dy, dw, dh, false);
}

void GCommandDecoder2D::G2D_quadraticCurveTo(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    float mFv[4];
    bool ret = buffer.parseArray<float>(mFv, 4);
    if( !ret ) return;
    
    context->mCanvas2D->QuadraticCurveTo(mFv[0], mFv[1], mFv[2], mFv[3]);
    LOG_D("[context2d::exec] quadraticCurveTo(%f,%f,%f,%f)", mFv[0], mFv[1], mFv[2], mFv[3]);
}

void GCommandDecoder2D::G2D_rect(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    float mFv[4];
    bool ret = buffer.parseArray<float>(mFv, 4);
    if( !ret ) return;
    
    context->mCanvas2D->Rect(mFv[0], mFv[1], mFv[2], mFv[3]);
    LOG_D("[context2d::exec] rect(%f,%f,%f,%f)", mFv[0], mFv[1], mFv[2], mFv[3]);
}

void GCommandDecoder2D::G2D_restore(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    context->mCanvas2D->Restore();
    LOG_D("[context2d::exec] restore()");
}

void GCommandDecoder2D::G2D_rotate(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    float mFv[1];
    bool ret = buffer.parseArray<float>(mFv, 1);
    if( !ret ) return;
    
    context->mCanvas2D->Rotate(mFv[0]);
    LOG_D("[context2d::exec] rotate(%d)", mFv[0]);
}

void GCommandDecoder2D::G2D_save(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    context->mCanvas2D->Save();
    LOG_D("[context2d::exec] save()");
}

void GCommandDecoder2D::G2D_scale(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    float mFv[2];
    bool ret = buffer.parseArray<float>(mFv, 2);
    if( !ret ) return;
    
    context->mCanvas2D->Scale(mFv[0], mFv[1]);
    LOG_D("[context2d::exec] scale(%f,%f)", mFv[0], mFv[1]);
}

void GCommandDecoder2D::G2D_setLineDash(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t mIv[1];
    bool ret = buffer.parseArray<uint32_t>(mIv, 1);
    if( !ret ) return;

    size_t size = mIv[0];
    if( size > 0 )
    {
        float* mFv = (float*)(buffer.parseBuffer(size*4));
        if( mFv )
        {
            std::vector<float>lineDash;
            for (int i=0; i<size; ++i) {
                lineDash.push_back(*(mFv+i));
            }
            context->mCanvas2D->SetLineDash(lineDash);
        }
    }
}

void GCommandDecoder2D::G2D_setTransform(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    float mFv[6];
    bool ret = buffer.parseArray<float>(mFv, 6);
    if( !ret ) return;
    
    context->mCanvas2D->SetTransform(mFv[0], mFv[1], mFv[2], mFv[3], mFv[4], mFv[5]);
    LOG_D("[context2d::exec] setTransform(%f,%f,%f,%f,%f,%f)", mFv[0], mFv[1], mFv[2], mFv[3], mFv[4], mFv[5]);
}

void GCommandDecoder2D::G2D_storke(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    context->mCanvas2D->Stroke();
    LOG_D("[context2d::exec] stroke()");
}

void GCommandDecoder2D::G2D_strokeRect(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    float mFv[4];
    bool ret = buffer.parseArray<float>(mFv, 4);
    if( !ret ) return;
    
    context->mCanvas2D->StrokeRect(mFv[0], mFv[1], mFv[2], mFv[3]);
    LOG_D("[context2d::exec] strokeRect(%f,%f,%f,%f)", mFv[0], mFv[1], mFv[2], mFv[3]);
}

void GCommandDecoder2D::G2D_strokeText(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t mIv[1];
    bool ret = buffer.parseArray<uint32_t>(mIv, 1);
    if( !ret ) return;

    size_t size = mIv[0];
    if( size > 0 )
    {
        char16_t *u16Char = (char16_t*)buffer.parseBufferAlign(size);
        std::string u8Str = U16StrToU8Str(u16Char);
        
        float mFv[3];
        ret = buffer.parseArray<float>(mFv, 3);
        if( !ret ) return;
        
        if( mFv[2] < 0 )
        {
            context->mCanvas2D->StrokeText(u8Str.c_str(), mFv[0], mFv[1]);
            LOG_D("[context2d::exec] strokeText(%s,%f,%f)", u8Str.c_str(), mFv[0], mFv[1]);
        }
        else
        {
            context->mCanvas2D->StrokeText(u8Str.c_str(), mFv[0], mFv[1], mFv[2]);
            LOG_D("[context2d::exec] strokeText(%s,%f,%f,%f)", u8Str.c_str(), mFv[0], mFv[1], mFv[2]);
        }
    }
}

void GCommandDecoder2D::G2D_transform(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    float mFv[6];
    bool ret = buffer.parseArray<float>(mFv, 6);
    if( !ret ) return;
    
    context->mCanvas2D->Transfrom(mFv[0], mFv[1], mFv[2], mFv[3], mFv[4], mFv[5]);
    LOG_D("[context2d::exec] transform(%f,%f,%f,%f,%f,%f)", mFv[0], mFv[1], mFv[2], mFv[3], mFv[4], mFv[5]);
}

void GCommandDecoder2D::G2D_translate(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    float mFv[2];
    bool ret = buffer.parseArray<float>(mFv, 2);
    if( !ret ) return;
    
    context->mCanvas2D->Translate(mFv[0], mFv[1]);
    LOG_D("[context2d::exec] translate(%f,%f)", mFv[0], mFv[1]);
}



//////////////////////////////
/// Set Property
//////////////////////////////
void GCommandDecoder2D::G2D_setFillStyle_string(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t mIv[1];
    bool ret = buffer.parseArray<uint32_t>(mIv, 1);
    if( !ret ) return;
    
    size_t size = mIv[0];
    if( size > 0 )
    {
        char16_t *u16Char = (char16_t*)buffer.parseBufferAlign(size);
        if( !u16Char ) return;
        std::string u8Str = U16StrToU8Str(u16Char);
        context->mCanvas2D->SetFillStyle(u8Str.c_str());
        LOG_D("[context2d::exec] setFillStyle(%s)", u8Str.c_str());
    }
}

void GCommandDecoder2D::G2D_setFillStyle_object(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    uint64_t *uID = buffer.parseValue<uint64_t>();
    if( !uID ) return;
    context->SetFillStrokeStyle(*uID, false);
}

void GCommandDecoder2D::G2D_setStrokeStyle_string(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t mIv[1];
    bool ret = buffer.parseArray<uint32_t>(mIv, 1);
    if( !ret ) return;
    
    size_t size = mIv[0];
    if( size > 0 )
    {
        char16_t *u16Char = (char16_t*)buffer.parseBufferAlign(size);
        if( !u16Char ) return;
        std::string u8Str = U16StrToU8Str(u16Char);
        context->mCanvas2D->SetStrokeStyle(u8Str.c_str());
        LOG_D("[context2d::exec] setStrokeStyle(%s)", u8Str.c_str());
    }
}

void GCommandDecoder2D::G2D_setStrokeStyle_object(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    uint64_t *uID = buffer.parseValue<uint64_t>();
    if( !uID ) return;
    
    context->SetFillStrokeStyle(*uID, true);
}
void GCommandDecoder2D::G2D_setShadowColor(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t mIv[1];
    bool ret = buffer.parseArray<uint32_t>(mIv, 1);
    if( !ret ) return;
    
    size_t size = mIv[0];
    if( size > 0 )
    {
        char16_t *u16Char = (char16_t*)buffer.parseBufferAlign(size);
        if( !u16Char ) return;
        std::string u8Str = U16StrToU8Str(u16Char);
        context->mCanvas2D->SetShadowColor(u8Str.c_str());
        LOG_D("[context2d::exec] setShadowColor(%s)", u8Str.c_str());
    }
}
void GCommandDecoder2D::G2D_setShadowBlur(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    float mFv[1];
    bool ret = buffer.parseArray<float>(mFv, 1);
    if( !ret ) return;
    
    context->mCanvas2D->SetShadowBlur(mFv[0]);
    LOG_D("[context2d::exec] setShadowBlur(%f)", mFv[0]);
}
void GCommandDecoder2D::G2D_setShadowOffsetX(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    float mFv[1];
    bool ret = buffer.parseArray<float>(mFv, 1);
    if( !ret ) return;
    
    context->mCanvas2D->SetShadowOffsetX(mFv[0]);
    LOG_D("[context2d::exec] setShadowOffsetX(%f)", mFv[0]);
}
void GCommandDecoder2D::G2D_setShadowOffsetY(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    float mFv[1];
    bool ret = buffer.parseArray<float>(mFv, 1);
    if( !ret ) return;
    
    context->mCanvas2D->SetShadowOffsetY(mFv[0]);
    LOG_D("[context2d::exec] setShadowOffsetY(%f)", mFv[0]);
}
void GCommandDecoder2D::G2D_setLineCap(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t mIv[1];
    bool ret = buffer.parseArray<uint32_t>(mIv, 1);
    if( !ret ) return;
    
    context->mCanvas2D->SetLineCap((GLineCap)mIv[0]);
    LOG_D("[context2d::exec] setLineCap(%d)", mIv[0]);
}
void GCommandDecoder2D::G2D_setLineJoin(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t mIv[1];
    bool ret = buffer.parseArray<uint32_t>(mIv, 1);
    if( !ret ) return;
    
    context->mCanvas2D->SetLineJoin((GLineJoin)mIv[0]);
    LOG_D("[context2d::exec] setLineJoin(%d)", mIv[0]);
}
void GCommandDecoder2D::G2D_setLineWidth(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    float mFv[1];
    bool ret = buffer.parseArray<float>(mFv, 1);
    if( !ret ) return;
    
    context->mCanvas2D->SetLineWidth(mFv[0]);
    LOG_D("[context2d::exec] setLineWidth(%f)", mFv[0]);
}

void GCommandDecoder2D::G2D_setMiterLimit(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    float mFv[1];
    bool ret = buffer.parseArray<float>(mFv, 1);
    if( !ret ) return;
    
    context->mCanvas2D->SetMiterLimit(mFv[0]);
    LOG_D("[context2d::exec] setMiterLimit(%f)", mFv[0]);
}

void GCommandDecoder2D::G2D_setLineDashOffset(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    float mFv[1];
    bool ret = buffer.parseArray<float>(mFv, 1);
    if( !ret ) return;
    
    context->mCanvas2D->SetLineDashOffset(mFv[0]);
    LOG_D("[context2d::exec] setLineDashOffset(%f)", mFv[0]);
}

void GCommandDecoder2D::G2D_setFont(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t mIv[1];
    bool ret = buffer.parseArray<uint32_t>(mIv, 1);
    if( !ret ) return;
    
    size_t size = mIv[0];
    if( size > 0 )
    {
        char16_t *u16Char = (char16_t*)buffer.parseBufferAlign(size);
        if( !u16Char ) return;
        std::string u8Str = U16StrToU8Str(u16Char);
        context->mCanvas2D->SetFont(u8Str.c_str());
        LOG_D("[context2d::exec] setFont(%s)", u8Str.c_str());
    }
}
void GCommandDecoder2D::G2D_setTextAlign(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t mIv[1];
    bool ret = buffer.parseArray<uint32_t>(mIv, 1);
    if( !ret ) return;
    
    context->mCanvas2D->SetTextAlign((GTextAlign)mIv[0]);
    LOG_D("[context2d::exec] setTextAlign(%d)", mIv[0]);
}
void GCommandDecoder2D::G2D_setTextBaseline(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t mIv[1];
    bool ret = buffer.parseArray<uint32_t>(mIv, 1);
    if( !ret ) return;
    
    context->mCanvas2D->SetTextBaseline((GTextBaseline)mIv[0]);
    LOG_D("[context2d::exec] setTextBaseline(%d)", mIv[0]);
}
void GCommandDecoder2D::G2D_setGlobalAlpha(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    float mFv[1];
    bool ret = buffer.parseArray<float>(mFv, 1);
    if( !ret ) return;
    
    context->mCanvas2D->SetGlobalAlpha(mFv[0]);
    LOG_D("[context2d::exec] setGlobalAlpha(%f)", mFv[0]);
}

void GCommandDecoder2D::G2D_setGlobalCompositeOperation(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res)
{
    uint32_t mIv[1];
    bool ret = buffer.parseArray<uint32_t>(mIv, 1);
    if( !ret ) return;
    
    context->mCanvas2D->SetGlobalCompositeOperation(mIv[0]);
    LOG_D("[context2d::exec] setGlobalCompositeOperation(%d)", mIv[0]);
}

}  // namespace gcanvas

#endif
