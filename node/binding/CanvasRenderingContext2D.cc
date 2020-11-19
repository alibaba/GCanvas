/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#include "CanvasRenderingContext2D.h"
#include <iostream>
#include "CanvasGradient.h"
#include "Image.h"
#include <GL/gl.h>
#include "Canvas.h"
#include "TextMetrics.h"
#include <time.h>

//测量耗时的调试开关
// #define DUMP_RUNNING_TIME 1

#ifdef DUMP_RUNNING_TIME
#define RECORD_TIME_BEGIN  \
    clock_t start, finish; \
    start = clock();

#define RECORD_TIME_END \
    finish = clock();   \
    printf("cost time %f ms\n", (double)(finish - start) * 1000.0f / CLOCKS_PER_SEC);
#else
#define RECORD_TIME_BEGIN
#define RECORD_TIME_END
#endif

#define DEFINE_VOID_METHOD(methodName)              \
    \ 
     void                                                 \
    Context2D::methodName(const Napi::CallbackInfo &info) \
    {                                                     \
        RECORD_TIME_BEGIN                                 \
        mRenderContext->makeCurrent();                    \
        // printf("the function :  " #methodName " is  called \n");                            \
        \ 


#define DEFINE_RETURN_VALUE_METHOD(methodName)            \
    \ 
     Napi::Value                                          \
    Context2D::methodName(const Napi::CallbackInfo &info) \
    {                                                     \
        RECORD_TIME_BEGIN                                 \
        mRenderContext->makeCurrent();                    \
        // printf("the function : " #methodName " is  called \n");                         \
            \  


#define DEFINE_SETTER_METHOD(methodName)                                            \
    \ 
     void                                                                           \
    Context2D::methodName(const Napi::CallbackInfo &info, const Napi::Value &value) \
    {                                                                               \
        \ 
          RECORD_TIME_BEGIN                                                         \
            NodeBinding::checkArgs(info, 1);                                        \
        mRenderContext->makeCurrent();                                              \
        if (info[0].As<Napi::Value>().IsUndefined())                                \
        {                                                                           \
            return;                                                                 \
        }                                                                           \
        // printf("the function : " #methodName " is  called \n");                     \
                \ 

#define DEFINE_GETTER_METHOD(methodName)                  \
    \ 
     Napi::Value                                          \
    Context2D::methodName(const Napi::CallbackInfo &info) \
    {                                                     \
        RECORD_TIME_BEGIN                                 \
        mRenderContext->makeCurrent();                    \
        // printf("the function : " #methodName " is  called \n");                 \
                    \ 

namespace NodeBinding
{
    Napi::FunctionReference Context2D::constructor;
    Context2D::Context2D(const Napi::CallbackInfo &info) : Napi::ObjectWrap<Context2D>(info)
    {
    }

    void Context2D::Init(Napi::Env env)
    {
        Napi::HandleScope scope(env);

        Napi::Function func =
            DefineClass(env,
                        "CanvasRenderingContext2D",
                        {
                            InstanceMethod("fillRect", &Context2D::fillRect),
                            InstanceMethod("arc", &Context2D::arc),
                            InstanceMethod("arcTo", &Context2D::arcTo),
                            InstanceMethod("beginPath", &Context2D::beginPath),
                            InstanceMethod("bezierCurveTo", &Context2D::bezierCurveTo),
                            InstanceMethod("clearRect", &Context2D::clearRect),
                            InstanceMethod("clip", &Context2D::clip),
                            InstanceMethod("closePath", &Context2D::closePath),
                            InstanceMethod("createImageData", &Context2D::createImageData),
                            InstanceMethod("createLinearGradient", &Context2D::createLinearGradient),
                            InstanceMethod("createPattern", &Context2D::createPattern),
                            InstanceMethod("createRadialGradient", &Context2D::createRadialGradient),
                            InstanceMethod("drawImage", &Context2D::drawImage),
                            InstanceMethod("fill", &Context2D::fill),
                            InstanceMethod("fillText", &Context2D::fillText),
                            InstanceMethod("getImageData", &Context2D::getImageData),
                            InstanceMethod("getLineDash", &Context2D::getLineDash),
                            InstanceMethod("lineTo", &Context2D::lineTo),
                            InstanceMethod("measureText", &Context2D::measureText),
                            InstanceMethod("moveTo", &Context2D::moveTo),
                            InstanceMethod("putImageData", &Context2D::putImageData),
                            InstanceMethod("quadraticCurveTo", &Context2D::quadraticCurveTo),
                            InstanceMethod("rect", &Context2D::rect),
                            InstanceMethod("resetTransform", &Context2D::resetTransform),
                            InstanceMethod("restore", &Context2D::restore),
                            InstanceMethod("rotate", &Context2D::rotate),
                            InstanceMethod("save", &Context2D::save),
                            InstanceMethod("scale", &Context2D::scale),
                            InstanceMethod("setLineDash", &Context2D::setLineDash),
                            InstanceMethod("setTransform", &Context2D::setTransform),
                            InstanceMethod("stroke", &Context2D::stroke),
                            InstanceMethod("strokeRect", &Context2D::strokeRect),
                            InstanceMethod("strokeText", &Context2D::strokeText),
                            InstanceMethod("transform", &Context2D::transform),
                            InstanceMethod("translate", &Context2D::translate),

                            InstanceAccessor("fillStyle", &Context2D::getFillStyle, &Context2D::setFillStyle),
                            InstanceAccessor("font", &Context2D::getfont, &Context2D::setfont),
                            InstanceAccessor("globalAlpha", &Context2D::getglobalAlpha, &Context2D::setglobalAlpha),
                            InstanceAccessor("globalCompositeOperation", &Context2D::getglobalCompositeOperation, &Context2D::setglobalCompositeOperation),
                            InstanceAccessor("lineCap", &Context2D::getlineCap, &Context2D::setlineCap),
                            InstanceAccessor("lineDashOffset", &Context2D::getlineDashOffset, &Context2D::setlineDashOffset),
                            InstanceAccessor("lineJoin", &Context2D::getlineJoin, &Context2D::setlineJoin),
                            InstanceAccessor("lineWidth", &Context2D::getlineWidth, &Context2D::setlineWidth),
                            InstanceAccessor("miterLimit", &Context2D::getmiterLimit, &Context2D::setmiterLimit),
                            InstanceAccessor("shadowBlur", &Context2D::getshadowBlur, &Context2D::setshadowBlur),
                            InstanceAccessor("shadowColor", &Context2D::getshadowColor, &Context2D::setshadowColor),
                            InstanceAccessor("shadowOffsetX", &Context2D::getshadowOffsetX, &Context2D::setshadowOffsetX),
                            InstanceAccessor("shadowOffsetY", &Context2D::getshadowOffsetY, &Context2D::setshadowOffsetY),
                            InstanceAccessor("strokeStyle", &Context2D::getstrokeStyle, &Context2D::setstrokeStyle),
                            InstanceAccessor("textAlign", &Context2D::gettextAlign, &Context2D::settextAlign),
                            InstanceAccessor("textBaseline", &Context2D::gettextBaseline, &Context2D::settextBaseline),
                            InstanceAccessor("canvas", &Context2D::getCanvas, nullptr),

                        });
        constructor = Napi::Persistent(func);
        constructor.SuppressDestruct();
        return;
    }

    Napi::Object Context2D::NewInstance(Napi::Env env)
    {
        Napi::Object obj = constructor.New({});
        obj.Set("name", Napi::String::New(env, "context2d"));
        return obj;
    }
    DEFINE_VOID_METHOD(fillRect)
    Napi::Env env = info.Env();
    NodeBinding::checkArgs(info, 4);

    float x = info[0].As<Napi::Number>().FloatValue();
    float y = info[1].As<Napi::Number>().FloatValue();
    float width = info[2].As<Napi::Number>().FloatValue();
    float height = info[3].As<Napi::Number>().FloatValue();

    if (mRenderContext)
    {
        mRenderContext->getCtx2d()->FillRect(x, y, width, height);
    }
    RECORD_TIME_END
    return;
} // namespace NodeBinding

DEFINE_SETTER_METHOD(setFillStyle)
if (mRenderContext)
{
    if (value.IsString())
    {
        std::string arg = value.As<Napi::String>().Utf8Value();
        mRenderContext->getCtx2d()->SetFillStyle(arg.c_str());
    }
    else if (value.IsObject())
    {
        Napi::Object object = value.As<Napi::Object>();
        Napi::Value name = object.Get("name");
        if (!name.IsString())
        {
            throwError(info, "wrong argument for fillstyle");
            return;
        }
        std::string namePropetry = name.As<Napi::String>().Utf8Value();
        if (namePropetry == "linearGradient")
        {
            Gradient *gradient = Napi::ObjectWrap<Gradient>::Unwrap(object);
            float startArr[] = {gradient->mLinearGradientInfo->startX, gradient->mLinearGradientInfo->startY};
            float endArr[] = {gradient->mLinearGradientInfo->endX, gradient->mLinearGradientInfo->endY};
            const std::vector<ColorStop> colorStop = gradient->getColorStops();
            float offsetArray[colorStop.size()];
            std::string colorArray[colorStop.size()];
            for (int i = 0; i < colorStop.size(); i++)
            {
                offsetArray[i] = colorStop[i].offset;
                colorArray[i] = colorStop[i].color;
            }
            mRenderContext->getCtx2d()->SetFillStyleLinearGradient(startArr, endArr, gradient->getCount(), offsetArray, colorArray);
        }
        else if (namePropetry == "radialGradient")
        {
            Gradient *gradient = Napi::ObjectWrap<Gradient>::Unwrap(object);
            float startArr[] = {gradient->mRadialGradientInfo->startX, gradient->mRadialGradientInfo->startY, gradient->mRadialGradientInfo->startR};
            float endArr[] = {gradient->mRadialGradientInfo->endX, gradient->mRadialGradientInfo->endY, gradient->mRadialGradientInfo->endR};
            const std::vector<ColorStop> colorStop = gradient->getColorStops();
            float offsetArray[colorStop.size()];
            std::string colorArray[colorStop.size()];
            for (int i = 0; i < colorStop.size(); i++)
            {
                offsetArray[i] = colorStop[i].offset;
                colorArray[i] = colorStop[i].color;
            }
            mRenderContext->getCtx2d()->SetFillStyleRadialGradient(startArr, endArr, gradient->getCount(), offsetArray, colorArray);
        }
        else if (namePropetry == "pattern")
        {
            Pattern *pattern = Napi::ObjectWrap<Pattern>::Unwrap(object);
            int textureId = mRenderContext->getCtx2d()->BindImage(
                &pattern->content->getPixels()[0], GL_RGBA,
                pattern->content->getWidth(),
                pattern->content->getHeight());
            mRenderContext->getCtx2d()->SetFillStylePattern(
                textureId, pattern->content->getWidth(),
                pattern->content->getHeight(),
                pattern->getRepetition().c_str(), false);
            mRenderContext->recordTextures(textureId);
        }
        else
        {
            throwError(info, "fill style argment vaild");
        }
    }
}
RECORD_TIME_END
return;
}

DEFINE_RETURN_VALUE_METHOD(getFillStyle)
Napi::Env env = info.Env();
if (mRenderContext)
{
    return Napi::String::New(env, gcanvas::ColorToString(mRenderContext->getCtx2d()->FillStyle()));
}
RECORD_TIME_END
return Napi::String::New(env, "");
}

DEFINE_VOID_METHOD(clearRect)
Napi::Env env = info.Env();
NodeBinding::checkArgs(info, 4);
float x = info[0].As<Napi::Number>().FloatValue();
float y = info[1].As<Napi::Number>().FloatValue();
float width = info[2].As<Napi::Number>().FloatValue();
float height = info[3].As<Napi::Number>().FloatValue();
if (mRenderContext)
{
    mRenderContext->getCtx2d()->ClearRect(x, y, width, height);
}
RECORD_TIME_END
}

DEFINE_VOID_METHOD(arc)
Napi::Env env = info.Env();
if (info.Length() < 5)
{
    throwError(info, "wrong argument number");
    return;
}
float x = info[0].As<Napi::Number>().FloatValue();
float y = info[1].As<Napi::Number>().FloatValue();
float r = info[2].As<Napi::Number>().FloatValue();
float startAngle = info[3].As<Napi::Number>().FloatValue();
float endAngle = info[4].As<Napi::Number>().FloatValue();

bool clockwise = true;
if (info.Length() == 6)
{
    clockwise = info[5].As<Napi::Boolean>().ToBoolean();
}
if (mRenderContext)
{
    mRenderContext->getCtx2d()->Arc(x, y, r, startAngle, endAngle, clockwise);
}
RECORD_TIME_END
}

DEFINE_VOID_METHOD(arcTo)
Napi::Env env = info.Env();

NodeBinding::checkArgs(info, 5);
float x1 = info[0].As<Napi::Number>().FloatValue();
float y1 = info[1].As<Napi::Number>().FloatValue();
float x2 = info[2].As<Napi::Number>().FloatValue();
float y2 = info[3].As<Napi::Number>().FloatValue();
float r = info[4].As<Napi::Number>().FloatValue();
if (mRenderContext)
{
    mRenderContext->getCtx2d()->ArcTo(x1, y1, x2, y2, r);
}
RECORD_TIME_END
}

DEFINE_VOID_METHOD(beginPath)
Napi::Env env = info.Env();

NodeBinding::checkArgs(info, 0);
if (mRenderContext)
{
    mRenderContext->getCtx2d()->BeginPath();
}
RECORD_TIME_END
}

DEFINE_VOID_METHOD(bezierCurveTo)
Napi::Env env = info.Env();

NodeBinding::checkArgs(info, 6);
float cp1x = info[0].As<Napi::Number>().FloatValue();
float cp1y = info[1].As<Napi::Number>().FloatValue();
float cp2x = info[2].As<Napi::Number>().FloatValue();
float cp2y = info[3].As<Napi::Number>().FloatValue();
float x = info[4].As<Napi::Number>().FloatValue();
float y = info[5].As<Napi::Number>().FloatValue();
if (mRenderContext)
{
    mRenderContext->getCtx2d()->BezierCurveTo(cp1x, cp1y, cp2x, cp2y, x, y);
}
RECORD_TIME_END
}

DEFINE_VOID_METHOD(clip)
Napi::Env env = info.Env();

GFillRule rule = FILL_RULE_NONZERO;
if (info.Length() == 1)
{
    std::string value = info[0].As<Napi::String>().Utf8Value();
    if (value == "nonzero")
    {
        rule = FILL_RULE_NONZERO;
    }
    else if (value == "evenodd")
    {
        rule = FILL_RULE_EVENODD;
    }
    else
    {
        throwError(info, "fill rule value invaild");
    }
}

if (mRenderContext)
{
    mRenderContext->getCtx2d()->Clip(rule);
}
RECORD_TIME_END
}

DEFINE_VOID_METHOD(closePath)
Napi::Env env = info.Env();

NodeBinding::checkArgs(info, 0);
if (mRenderContext)
{
    mRenderContext->getCtx2d()->ClosePath();
}
RECORD_TIME_END
}

DEFINE_GETTER_METHOD(createImageData)
Napi::Env env = info.Env();

NodeBinding::checkArgs(info, 2);
return ImageData::NewInstance(info.Env(), info[0], info[1]);
}

DEFINE_GETTER_METHOD(createLinearGradient)
Napi::Env env = info.Env();

NodeBinding::checkArgs(info, 4);
return Gradient::NewInstance(env, info);
}

DEFINE_GETTER_METHOD(createPattern)
Napi::Env env = info.Env();
NodeBinding::checkArgs(info, 2);

Image *img = Napi::ObjectWrap<Image>::Unwrap(info[0].As<Napi::Object>());
Napi::Object ret = Pattern::NewInstance(env, info[1]);
Pattern *pattern = Napi::ObjectWrap<Pattern>::Unwrap(ret);
pattern->content = std::make_shared<Image>(*img);
return ret;
}

DEFINE_GETTER_METHOD(createRadialGradient)
Napi::Env env = info.Env();
NodeBinding::checkArgs(info, 6);
return Gradient::NewInstance(env, info);
}

DEFINE_VOID_METHOD(drawImage)
Napi::Env env = info.Env();
if (info.Length() < 3 || (info.Length() != 3 && info.Length() != 5 && info.Length() != 9))
{
    Napi::TypeError::New(info.Env(), "wrong argument number")
        .ThrowAsJavaScriptException();
    return;
}

if (!mRenderContext)
{
    throwError(info, "rendercontext invaild");
    return;
}

float srcX = 0, srcY = 0, srcWidth = 0, srcHeight = 0;
float desX = 0, desY = 0, desWidth = mRenderContext->getWdith(), desHeight = mRenderContext->getHeight();

int textureId = 0;
int textureWidth = 0, textureHeight = 0;

Napi::Object object = info[0].As<Napi::Object>();
Napi::Value name = object.Get("name");
//todo fixme refactor
glEnable(GL_BLEND);
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
if (name.IsString())
{
    std::string namePropetry = name.As<Napi::String>().Utf8Value();
    if (namePropetry == "canvas")
    {
        Canvas *canvas = Napi::ObjectWrap<Canvas>::Unwrap(info[0].As<Napi::Object>());
        textureWidth = canvas->getWidth();
        textureHeight = canvas->getHeight();
        srcWidth = textureWidth;
        srcHeight = textureHeight;

        size_t size = srcWidth * srcHeight * 4;
        uint8_t *pixels = new uint8_t[size];
        if (!pixels)
        {
            printf("DrawImage with canvas, memory allocate failed!\n");
            return;
        }

        //fixme later
        canvas->mRenderContext->BindFBO();
        canvas->mRenderContext->getCtx2d()->GetImageData(0, 0, textureWidth, textureHeight, pixels);
        textureId = canvas->mRenderContext->getCtx2d()->BindImage(pixels, GL_RGBA, textureWidth, textureHeight);
        printf("drawImage with canvas, textureId=%d, textureWidth=%d, textureHeight=%d\n", textureId, textureWidth, textureHeight);

        delete[] pixels;
        pixels = nullptr;
    }
    else if (namePropetry == "image")
    {
        Image *image = Napi::ObjectWrap<Image>::Unwrap(info[0].As<Napi::Object>());
        srcWidth = image->getWidth();
        srcHeight = image->getHeight();
        textureWidth = srcWidth;
        textureHeight = srcHeight;
        if (image->getTextureId() == -1)
        {
            int id = mRenderContext->getTextureIdByUrl(image->getUrl());
            if (id == -1)
            {
                id = mRenderContext->getCtx2d()->BindImage(&image->getPixels()[0], GL_RGBA, srcWidth, srcHeight);
                //缓存下url和纹理id的关系,避免重复bind
                mRenderContext->recordImageTexture(image->getUrl(), id);
            }
            else
            {
                // printf("the cached image id is %d \n",id);
            }
            image->setTextureId(id);
        }
        textureId = image->getTextureId();
    }
}

if (info.Length() == 3)
{
    desX = info[1].As<Napi::Number>().FloatValue();
    desY = info[2].As<Napi::Number>().FloatValue();
    desWidth = textureWidth;
    desHeight = textureHeight;
}
else if (info.Length() == 5)
{
    desX = info[1].As<Napi::Number>().FloatValue();
    desY = info[2].As<Napi::Number>().FloatValue();
    desWidth = info[3].As<Napi::Number>().FloatValue();
    desHeight = info[4].As<Napi::Number>().FloatValue();
}
else if (info.Length() == 9)
{
    srcX = info[1].As<Napi::Number>().FloatValue();
    srcY = info[2].As<Napi::Number>().FloatValue();
    srcWidth = info[3].As<Napi::Number>().FloatValue();
    srcHeight = info[4].As<Napi::Number>().FloatValue();

    desX = info[5].As<Napi::Number>().FloatValue();
    desY = info[6].As<Napi::Number>().FloatValue();
    desWidth = info[7].As<Napi::Number>().FloatValue();
    desHeight = info[8].As<Napi::Number>().FloatValue();
}
if (mRenderContext)
{
    mRenderContext->getCtx2d()->DrawImage(textureId,
                                        textureWidth,
                                        textureHeight, // image width & height
                                        srcX,          // srcX
                                        srcY,          // srcY
                                        srcWidth,      // srcWidth
                                        srcHeight,     //srcHeight
                                        desX,          //desStartX
                                        desY,          //desStartY
                                        desWidth,      //desWidth
                                        desHeight);    //desHeight
    mRenderContext->recordTextures(textureId);
}
}

DEFINE_VOID_METHOD(fill)
Napi::Env env = info.Env();

GFillRule rule = FILL_RULE_NONZERO;
if (info.Length() == 1)
{
    std::string value = info[0].As<Napi::String>().Utf8Value();
    if (value == "nonzero")
    {
        rule = FILL_RULE_NONZERO;
    }
    else if (value == "evenodd")
    {
        rule = FILL_RULE_EVENODD;
    }
    else
    {
        throwError(info, "fill rule value invaild");
    }
}
if (mRenderContext)
{
    mRenderContext->getCtx2d()->Fill(rule);
}
RECORD_TIME_END
}

DEFINE_VOID_METHOD(fillText)
Napi::Env env = info.Env();

if (info.Length() < 3)
{
    Napi::TypeError::New(info.Env(), "wrong argument number")
        .ThrowAsJavaScriptException();
}
if (mRenderContext)
{
    std::string content = info[0].As<Napi::String>().Utf8Value();
    float x = info[1].As<Napi::Number>().FloatValue();
    float y = info[2].As<Napi::Number>().FloatValue();
    if (info.Length() == 4)
    {
        float maxWidth = info[3].As<Napi::Number>().FloatValue();
        mRenderContext->getCtx2d()->DrawText(content.c_str(), x, y, maxWidth);
    }
    else
    {
        mRenderContext->getCtx2d()->DrawText(content.c_str(), x, y);
    }
}
RECORD_TIME_END
}

DEFINE_RETURN_VALUE_METHOD(getImageData)
Napi::Env env = info.Env();

NodeBinding::checkArgs(info, 4);
if (mRenderContext)
{
    int x = info[0].As<Napi::Number>().Int32Value();
    int y = info[1].As<Napi::Number>().Int32Value();

    int width = info[2].As<Napi::Number>().Int32Value();
    int height = info[3].As<Napi::Number>().Int32Value();
    //offscreen condition: y need convert
    y = mRenderContext->getHeight() - (y + height);

    Napi::Object imageDataObj = ImageData::NewInstance(env, info[2], info[3]);
    ImageData *ptr = Napi::ObjectWrap<ImageData>::Unwrap(imageDataObj);
    mRenderContext->getCtx2d()->GetImageData(x, y, width, height, &ptr->getPixles()[0]);

    //flipY
    gcanvas::FlipPixel(&ptr->getPixles()[0], width, height);

    return imageDataObj;
}
RECORD_TIME_END
}

DEFINE_RETURN_VALUE_METHOD(getLineDash)
Napi::Env env = info.Env();

NodeBinding::checkArgs(info, 0);
Napi::Array ret = Napi::Array::New(env);
if (mRenderContext)
{
    std::vector<float> dash = mRenderContext->getCtx2d()->LineDash();
    for (int i = 0; i < dash.size(); i++)
    {
        ret.Set(i, Napi::Number::New(env, dash[i]));
    }
}
RECORD_TIME_END
return ret;
}

DEFINE_VOID_METHOD(lineTo)
Napi::Env env = info.Env();
NodeBinding::checkArgs(info, 2);
float x = info[0].As<Napi::Number>().FloatValue();
float y = info[1].As<Napi::Number>().FloatValue();
if (mRenderContext)
{
    mRenderContext->getCtx2d()->LineTo(x, y);
}
RECORD_TIME_END
}

DEFINE_RETURN_VALUE_METHOD(measureText)
Napi::Env env = info.Env();

NodeBinding::checkArgs(info, 1);
std::string text = info[0].As<Napi::String>().Utf8Value();
if (mRenderContext)
{
    float width = mRenderContext->getCtx2d()->MeasureTextWidth(text.c_str());
    return TextMetrics::NewInstance(env, Napi::Number::New(env, width));
}
else
{
    return env.Undefined();
}
RECORD_TIME_END
}

DEFINE_VOID_METHOD(moveTo)
Napi::Env env = info.Env();

NodeBinding::checkArgs(info, 2);
float x = info[0].As<Napi::Number>().FloatValue();
float y = info[1].As<Napi::Number>().FloatValue();
if (mRenderContext)
{
    mRenderContext->getCtx2d()->MoveTo(x, y);
}
RECORD_TIME_END
}

DEFINE_VOID_METHOD(putImageData)
Napi::Env env = info.Env();
if (info.Length() < 3)
{
    throwError(info, "wrong argument number");
    return;
}
if (!info[0].IsObject())
{
    throwError(info, "imgData must be object");
    return;
}
ImageData *imgData = Napi::ObjectWrap<ImageData>::Unwrap(info[0].As<Napi::Object>());
if (mRenderContext)
{
    int x = info[1].As<Napi::Number>().Int32Value();
    int y = info[2].As<Napi::Number>().Int32Value();
    int dirtyX = 0;
    int dirtyY = 0;
    int dirtyWidth = imgData->getWidth();
    int dirtyHeight = imgData->getHeight();

    if (info.Length() == 7)
    {
        dirtyX = info[3].As<Napi::Number>().Int32Value();
        dirtyY = info[4].As<Napi::Number>().Int32Value();
        dirtyWidth = info[5].As<Napi::Number>().Int32Value();
        dirtyHeight = info[6].As<Napi::Number>().Int32Value();
    }
    mRenderContext->getCtx2d()->PutImageData(
        &imgData->getPixles()[0], //content
        imgData->getWidth(),      //imageData width
        imgData->getHeight(),     //imageData height
        x,                        // draw start x
        y,                        //draw start y
        dirtyX,                   // dirtyX
        dirtyY,                   //// dirtyY
        dirtyWidth,               //dirtyWidth
        dirtyHeight               //dirtyHeight
    );
}
RECORD_TIME_END
}

DEFINE_VOID_METHOD(quadraticCurveTo)
Napi::Env env = info.Env();
NodeBinding::checkArgs(info, 4);
float cpx = info[0].As<Napi::Number>().FloatValue();
float cpy = info[1].As<Napi::Number>().FloatValue();
float x = info[2].As<Napi::Number>().FloatValue();
float y = info[3].As<Napi::Number>().FloatValue();
if (mRenderContext)
{
    mRenderContext->getCtx2d()->QuadraticCurveTo(cpx, cpy, x, y);
}
RECORD_TIME_END
}

DEFINE_VOID_METHOD(rect)
Napi::Env env = info.Env();

NodeBinding::checkArgs(info, 4);
float x = info[0].As<Napi::Number>().FloatValue();
float y = info[1].As<Napi::Number>().FloatValue();
float width = info[2].As<Napi::Number>().FloatValue();
float height = info[3].As<Napi::Number>().FloatValue();
if (mRenderContext)
{
    mRenderContext->getCtx2d()->Rect(x, y, width, height);
}
RECORD_TIME_END
}

DEFINE_VOID_METHOD(resetTransform)
Napi::Env env = info.Env();

NodeBinding::checkArgs(info, 0);
if (mRenderContext)
{
    mRenderContext->getCtx2d()->ResetTransform();
}
RECORD_TIME_END
}

DEFINE_VOID_METHOD(restore)
Napi::Env env = info.Env();
NodeBinding::checkArgs(info, 0);
if (mRenderContext)
{
    mRenderContext->getCtx2d()->Restore();
}
RECORD_TIME_END
}

DEFINE_VOID_METHOD(rotate)
Napi::Env env = info.Env();
float angle = info[0].As<Napi::Number>().FloatValue();
NodeBinding::checkArgs(info, 1);
if (mRenderContext)
{
    mRenderContext->getCtx2d()->Rotate(angle);
}
RECORD_TIME_END
}

DEFINE_VOID_METHOD(save)
Napi::Env env = info.Env();
NodeBinding::checkArgs(info, 0);
if (mRenderContext)
{
    mRenderContext->getCtx2d()->Save();
}
RECORD_TIME_END
}

DEFINE_VOID_METHOD(scale)
Napi::Env env = info.Env();
float x = info[0].As<Napi::Number>().FloatValue();
float y = info[1].As<Napi::Number>().FloatValue();
NodeBinding::checkArgs(info, 2);
if (mRenderContext)
{
    mRenderContext->getCtx2d()->Scale(x, y);
}
RECORD_TIME_END
}

DEFINE_VOID_METHOD(setLineDash)
Napi::Env env = info.Env();
NodeBinding::checkArgs(info, 1);
Napi::Array array = info[0].As<Napi::Array>();

std::vector<float> dash;
for (int i = 0; i < array.Length(); i++)
{
    dash.push_back(array.Get(i).As<Napi::Number>().FloatValue());
}
if (mRenderContext)
{
    mRenderContext->getCtx2d()->SetLineDash(std::move(dash));
}
RECORD_TIME_END
}

void Context2D::setCanvasRef(NodeBinding::Canvas *canvas)
{
    this->mCanvas = canvas;
}
DEFINE_VOID_METHOD(setTransform)
Napi::Env env = info.Env();

NodeBinding::checkArgs(info, 6);

float scaleX = info[0].As<Napi::Number>().FloatValue();
float scaleY = info[1].As<Napi::Number>().FloatValue();
float rotateX = info[2].As<Napi::Number>().FloatValue();
float rototaY = info[3].As<Napi::Number>().FloatValue();
float translateX = info[4].As<Napi::Number>().FloatValue();
float translateY = info[5].As<Napi::Number>().FloatValue();
if (mRenderContext)
{
    mRenderContext->getCtx2d()->SetTransform(scaleX, scaleY, rotateX, rototaY, translateX, translateY);
}
RECORD_TIME_END
}

DEFINE_VOID_METHOD(stroke)
Napi::Env env = info.Env();

NodeBinding::checkArgs(info, 0);
if (mRenderContext)
{
    mRenderContext->getCtx2d()->Stroke();
}
RECORD_TIME_END
}

DEFINE_VOID_METHOD(strokeRect)
Napi::Env env = info.Env();
NodeBinding::checkArgs(info, 4);
float x = info[0].As<Napi::Number>().FloatValue();
float y = info[1].As<Napi::Number>().FloatValue();
float width = info[2].As<Napi::Number>().FloatValue();
float height = info[3].As<Napi::Number>().FloatValue();
if (mRenderContext)
{
    mRenderContext->getCtx2d()->StrokeRect(x, y, width, height);
}
RECORD_TIME_END
}

DEFINE_VOID_METHOD(strokeText)
Napi::Env env = info.Env();
if (info.Length() < 3)
{
    Napi::TypeError::New(info.Env(), "wrong argument number")
        .ThrowAsJavaScriptException();
}
if (mRenderContext)
{
    std::string content = info[0].As<Napi::String>().Utf8Value();
    float x = info[1].As<Napi::Number>().FloatValue();
    float y = info[2].As<Napi::Number>().FloatValue();
    if (info.Length() == 4)
    {
        float maxWidth = info[3].As<Napi::Number>().FloatValue();
        mRenderContext->getCtx2d()->StrokeText(content.c_str(), x, y, maxWidth);
    }
    else
    {
        mRenderContext->getCtx2d()->StrokeText(content.c_str(), x, y);
    }
}
RECORD_TIME_END
}

DEFINE_VOID_METHOD(transform)
Napi::Env env = info.Env();

NodeBinding::checkArgs(info, 6);
float scaleX = info[0].As<Napi::Number>().FloatValue();
float rotateX = info[1].As<Napi::Number>().FloatValue();
float rotateY = info[2].As<Napi::Number>().FloatValue();
float scaleY = info[3].As<Napi::Number>().FloatValue();
float translateX = info[4].As<Napi::Number>().FloatValue();
float translateY = info[5].As<Napi::Number>().FloatValue();
// printf("the Transfrom called  scaleX %f scaleY %f rotateX %f rototaY %f translateX %f  translateY %f \n",scaleX,scaleY,rotateX,rotateY,translateX,translateY);
if (mRenderContext)
{
    mRenderContext->getCtx2d()->Transfrom(scaleX, rotateX, rotateY, scaleY, translateX, translateY);
}
RECORD_TIME_END
}

DEFINE_VOID_METHOD(translate)
Napi::Env env = info.Env();
float tx = info[0].As<Napi::Number>().FloatValue();
float ty = info[1].As<Napi::Number>().FloatValue();
NodeBinding::checkArgs(info, 2);
if (mRenderContext)
{
    mRenderContext->getCtx2d()->Translate(tx, ty);
}
RECORD_TIME_END
}

DEFINE_SETTER_METHOD(setfont)
std::string font = value.As<Napi::String>().Utf8Value();
// printf("the set fon value is %s \n",font.c_str());
if (mRenderContext)
{
    mRenderContext->getCtx2d()->SetFont(font.c_str());
}
RECORD_TIME_END
}

DEFINE_SETTER_METHOD(setglobalAlpha)
float colorValue = info[0].As<Napi::Number>().FloatValue();
if (mRenderContext)
{
    mRenderContext->getCtx2d()->SetGlobalAlpha(colorValue);
}
RECORD_TIME_END
}

//TODO
DEFINE_SETTER_METHOD(setglobalCompositeOperation)
std::string opValue = info[0].As<Napi::String>().Utf8Value();
if (mRenderContext)
{
    if (opValue == "source-over")
    {
        mRenderContext->getCtx2d()->DoSetGlobalCompositeOperation(COMPOSITE_OP_SOURCE_OVER);
    }
    else if (opValue == "source-out")
    {
        mRenderContext->getCtx2d()->DoSetGlobalCompositeOperation(COMPOSITE_OP_SOURCE_OUT);
    }
    else if (opValue == "source-atop")
    {
        mRenderContext->getCtx2d()->DoSetGlobalCompositeOperation(COMPOSITE_OP_SOURCE_ATOP);
    }
    else if (opValue == "destination-over")
    {
        mRenderContext->getCtx2d()->DoSetGlobalCompositeOperation(COMPOSITE_OP_DESTINATION_OVER);
    }
    else if (opValue == "destination-in")
    {
        mRenderContext->getCtx2d()->DoSetGlobalCompositeOperation(COMPOSITE_OP_DESTINATION_IN);
    }
    else if (opValue == "destination-out")
    {
        mRenderContext->getCtx2d()->DoSetGlobalCompositeOperation(COMPOSITE_OP_DESTINATION_OUT);
    }
    else if (opValue == "lighter")
    {
        mRenderContext->getCtx2d()->DoSetGlobalCompositeOperation(COMPOSITE_OP_LIGHTER);
    }
    else if (opValue == "xor")
    {
        mRenderContext->getCtx2d()->DoSetGlobalCompositeOperation(COMPOSITE_OP_XOR);
    }
    else
    {
        throwError(info, "compite value invaild or not support");
    }
}
RECORD_TIME_END
}

DEFINE_SETTER_METHOD(setlineCap)
std::string lineCap = info[0].As<Napi::String>().Utf8Value();
if (mRenderContext)
{
    mRenderContext->getCtx2d()->SetLineCap(lineCap.c_str());
}
RECORD_TIME_END
}

DEFINE_SETTER_METHOD(setlineDashOffset)
if (mRenderContext)
{
    float offset = info[0].As<Napi::Number>().FloatValue();
    mRenderContext->getCtx2d()->SetLineDashOffset(offset);
}
RECORD_TIME_END
}

DEFINE_SETTER_METHOD(setlineJoin)
std::string lineJoin = info[0].As<Napi::String>().Utf8Value();
if (mRenderContext)
{
    mRenderContext->getCtx2d()->SetLineJoin(lineJoin.c_str());
}
RECORD_TIME_END
}

DEFINE_SETTER_METHOD(setlineWidth)
float lineWidth = info[0].As<Napi::Number>().FloatValue();
if (mRenderContext)
{
    mRenderContext->getCtx2d()->SetLineWidth(lineWidth);
}
RECORD_TIME_END
}

DEFINE_SETTER_METHOD(setmiterLimit)
float miterLimit = info[0].As<Napi::Number>().FloatValue();
if (mRenderContext)
{
    mRenderContext->getCtx2d()->SetMiterLimit(miterLimit);
}
RECORD_TIME_END
}

DEFINE_SETTER_METHOD(setshadowBlur)
float shadowBlur = info[0].As<Napi::Number>().FloatValue();
if (mRenderContext)
{
    mRenderContext->getCtx2d()->SetShadowBlur(shadowBlur);
}
RECORD_TIME_END
}

DEFINE_SETTER_METHOD(setshadowColor)
std::string color = info[0].As<Napi::String>().Utf8Value();
if (mRenderContext)
{
    mRenderContext->getCtx2d()->SetShadowColor(color.c_str());
}
RECORD_TIME_END
}

DEFINE_SETTER_METHOD(setshadowOffsetX)
float offsetX = info[0].As<Napi::Number>().FloatValue();
if (mRenderContext)
{
    mRenderContext->getCtx2d()->SetShadowOffsetX(offsetX);
}
RECORD_TIME_END
}

DEFINE_SETTER_METHOD(setshadowOffsetY)
float offsetY = info[0].As<Napi::Number>().FloatValue();
if (mRenderContext)
{
    mRenderContext->getCtx2d()->SetShadowOffsetY(offsetY);
}
RECORD_TIME_END
}

DEFINE_SETTER_METHOD(setstrokeStyle)
if (mRenderContext)
{
    if (value.IsString())
    {
        std::string arg = value.As<Napi::String>().Utf8Value();
        mRenderContext->getCtx2d()->SetStrokeStyle(arg.c_str());
    }
    else if (value.IsObject())
    {
        Napi::Object object = value.As<Napi::Object>();
        Napi::Value name = object.Get("name");
        if (!name.IsString())
        {
            throwError(info, "wrong argument for fillstyle");
            return;
        }
        std::string namePropetry = name.As<Napi::String>().Utf8Value();
        if (namePropetry == "linearGradient")
        {
            Gradient *gradient = Napi::ObjectWrap<Gradient>::Unwrap(object);
            float startArr[] = {gradient->mLinearGradientInfo->startX, gradient->mLinearGradientInfo->startY};
            float endArr[] = {gradient->mLinearGradientInfo->endX, gradient->mLinearGradientInfo->endY};
            const std::vector<ColorStop> colorStop = gradient->getColorStops();
            float offsetArray[colorStop.size()];
            std::string colorArray[colorStop.size()];
            for (int i = 0; i < colorStop.size(); i++)
            {
                offsetArray[i] = colorStop[i].offset;
                colorArray[i] = colorStop[i].color;
            }
            mRenderContext->getCtx2d()->SetFillStyleLinearGradient(startArr, endArr, gradient->getCount(), offsetArray, colorArray, true);
        }
        else if (namePropetry == "radialGradient")
        {
            Gradient *gradient = Napi::ObjectWrap<Gradient>::Unwrap(object);
            float startArr[] = {gradient->mRadialGradientInfo->startX, gradient->mRadialGradientInfo->startY, gradient->mRadialGradientInfo->startR};
            float endArr[] = {gradient->mRadialGradientInfo->endX, gradient->mRadialGradientInfo->endY, gradient->mRadialGradientInfo->endR};
            const std::vector<ColorStop> colorStop = gradient->getColorStops();
            float offsetArray[colorStop.size()];
            std::string colorArray[colorStop.size()];
            for (int i = 0; i < colorStop.size(); i++)
            {
                offsetArray[i] = colorStop[i].offset;
                colorArray[i] = colorStop[i].color;
            }
            mRenderContext->getCtx2d()->SetFillStyleRadialGradient(startArr, endArr, gradient->getCount(), offsetArray, colorArray, true);
        }
        else if (namePropetry == "pattern")
        {
            Pattern *pattern =
                Napi::ObjectWrap<Pattern>::Unwrap(object);
            int textureId = mRenderContext->getCtx2d()->BindImage(
                &pattern->content->getPixels()[0], GL_RGBA,
                pattern->content->getWidth(),
                pattern->content->getHeight());
            mRenderContext->getCtx2d()->SetFillStylePattern(
                textureId, pattern->content->getWidth(),
                pattern->content->getHeight(),
                pattern->getRepetition().c_str(), true);
            mRenderContext->recordTextures(textureId);
        }
        else
        {
            throwError(info, "stroke style argment vaild");
        }
    }
}
RECORD_TIME_END
return;
}

DEFINE_SETTER_METHOD(settextAlign)
std::string textAlign = value.As<Napi::String>().Utf8Value();
if (mRenderContext)
{
    if (textAlign == "start")
    {
        mRenderContext->getCtx2d()->SetTextAlign(TEXT_ALIGN_START);
    }
    else if (textAlign == "end")
    {
        mRenderContext->getCtx2d()->SetTextAlign(TEXT_ALIGN_END);
    }
    else if (textAlign == "left")
    {
        mRenderContext->getCtx2d()->SetTextAlign(TEXT_ALIGN_LEFT);
    }
    else if (textAlign == "center")
    {
        mRenderContext->getCtx2d()->SetTextAlign(TEXT_ALIGN_CENTER);
    }
    else if (textAlign == "right")
    {
        mRenderContext->getCtx2d()->SetTextAlign(TEXT_ALIGN_RIGHT);
    }
    else
    {
        throwError(info, "wrong text align value");
    }
}
RECORD_TIME_END
}

DEFINE_SETTER_METHOD(settextBaseline)
std::string baseline = value.As<Napi::String>().Utf8Value();
if (mRenderContext)
{
    if (baseline == "top")
    {
        mRenderContext->getCtx2d()->SetTextBaseline(TEXT_BASELINE_TOP);
    }
    else if (baseline == "bottom")
    {
        mRenderContext->getCtx2d()->SetTextBaseline(TEXT_BASELINE_BOTTOM);
    }
    else if (baseline == "middle")
    {
        mRenderContext->getCtx2d()->SetTextBaseline(TEXT_BASELINE_MIDDLE);
    }
    else if (baseline == "alphabetic")
    {
        mRenderContext->getCtx2d()->SetTextBaseline(TEXT_BASELINE_ALPHABETIC);
    }
    else if (baseline == "hanging")
    {
        mRenderContext->getCtx2d()->SetTextBaseline(TEXT_BASELINE_HANGING);
    }
    else
    {
        throwError(info, "wrong text align value");
    }
}
RECORD_TIME_END
}

DEFINE_GETTER_METHOD(getfont)
Napi::Env env = info.Env();
if (mRenderContext)
{
    std::string value = mRenderContext->getCtx2d()->mCurrentState->mFont->GetOriginFontName();
    return Napi::String::New(env, value);
}
RECORD_TIME_END
return Napi::String::New(env, "");
}

DEFINE_GETTER_METHOD(getglobalAlpha)
Napi::Env env = info.Env();
if (mRenderContext)
{
    float value = mRenderContext->getCtx2d()->GlobalAlpha();
    return Napi::Number::New(env, value);
}
RECORD_TIME_END
return Napi::Number::New(env, -1.0f);
}

DEFINE_GETTER_METHOD(getglobalCompositeOperation)
Napi::Env env = info.Env();
if (mRenderContext)
{
    GCompositeOperation value = mRenderContext->getCtx2d()->GlobalCompositeOperation();
    if (value == COMPOSITE_OP_SOURCE_OVER)
    {
        return Napi::String::New(env, "source-over");
    }
    else if (value == COMPOSITE_OP_SOURCE_OUT)
    {
        return Napi::String::New(env, "source-out");
    }
    else if (value == COMPOSITE_OP_SOURCE_ATOP)
    {
        return Napi::String::New(env, "source-atop");
    }
    else if (value == COMPOSITE_OP_DESTINATION_OVER)
    {
        return Napi::String::New(env, "destination-over");
    }
    else if (value == COMPOSITE_OP_DESTINATION_IN)
    {
        return Napi::String::New(env, "destination-in");
    }
    else if (value == COMPOSITE_OP_DESTINATION_OUT)
    {
        return Napi::String::New(env, "destination-out");
    }
    else if (value == COMPOSITE_OP_XOR)
    {
        return Napi::String::New(env, "xor");
    }
    else if (value == COMPOSITE_OP_LIGHTER)
    {
        return Napi::String::New(env, "lighter");
    }
}
RECORD_TIME_END
return Napi::String::New(env, "");
}

DEFINE_GETTER_METHOD(getlineCap)
Napi::Env env = info.Env();
if (mRenderContext)
{
    GLineCap cap = mRenderContext->getCtx2d()->LineCap();
    if (cap == LINE_CAP_BUTT)
    {
        return Napi::String::New(env, "butt");
    }
    else if (cap == LINE_CAP_ROUND)
    {
        return Napi::String::New(env, "round");
    }
    else if (cap == LINE_CAP_SQUARE)
    {
        return Napi::String::New(env, "square");
    }
}
RECORD_TIME_END
return Napi::String::New(env, "");
}

DEFINE_GETTER_METHOD(getlineDashOffset)
Napi::Env env = info.Env();
if (mRenderContext)
{
    float value = mRenderContext->getCtx2d()->LineDashOffset();
    return Napi::Number::New(env, value);
}
RECORD_TIME_END
return Napi::Number::New(env, -1);
}

DEFINE_GETTER_METHOD(getlineJoin)
Napi::Env env = info.Env();
if (mRenderContext)
{
    GLineJoin value = mRenderContext->getCtx2d()->LineJoin();
    if (value == LINE_JOIN_BEVEL)
    {
        return Napi::String::New(env, "bevel");
    }
    else if (value == LINE_JOIN_ROUND)
    {
        return Napi::String::New(env, "round");
    }
    else if (value == LINE_JOIN_MITER)
    {
        return Napi::String::New(env, "miter");
    }
}
RECORD_TIME_END
return Napi::String::New(env, "");
}

DEFINE_GETTER_METHOD(getlineWidth)
Napi::Env env = info.Env();
if (mRenderContext)
{
    return Napi::Number::New(env, mRenderContext->getCtx2d()->LineWidth());
}
RECORD_TIME_END
return Napi::Number::New(env, -1);
}

DEFINE_GETTER_METHOD(getmiterLimit)
Napi::Env env = info.Env();
if (mRenderContext)
{
    float value = mRenderContext->getCtx2d()->MiterLimit();
    return Napi::Number::New(env, value);
}
RECORD_TIME_END
return Napi::Number::New(env, -1);
}

DEFINE_GETTER_METHOD(getshadowBlur)
Napi::Env env = info.Env();
if (mRenderContext)
{
    return Napi::Number::New(env, mRenderContext->getCtx2d()->mCurrentState->mShadowBlur);
}
RECORD_TIME_END
return Napi::String::New(env, "");
}

DEFINE_GETTER_METHOD(getshadowColor)
Napi::Env env = info.Env();
if (mRenderContext)
{
    return Napi::String::New(env, gcanvas::ColorToString(mRenderContext->getCtx2d()->mCurrentState->mShadowColor));
}
RECORD_TIME_END
return Napi::String::New(env, "");
}

DEFINE_GETTER_METHOD(getshadowOffsetX)
Napi::Env env = info.Env();
if (mRenderContext)
{
    return Napi::Number::New(env, mRenderContext->getCtx2d()->mCurrentState->mShadowOffsetX);
}
RECORD_TIME_END
return Napi::Number::New(env, -1);
}

DEFINE_GETTER_METHOD(getshadowOffsetY)
Napi::Env env = info.Env();
if (mRenderContext)
{
    return Napi::Number::New(env, mRenderContext->getCtx2d()->mCurrentState->mShadowOffsetY);
}
RECORD_TIME_END
return Napi::String::New(env, "");
}

DEFINE_GETTER_METHOD(getstrokeStyle)
Napi::Env env = info.Env();
if (mRenderContext)
{
    return Napi::String::New(env, gcanvas::ColorToString(mRenderContext->getCtx2d()->StrokeStyle()));
}
RECORD_TIME_END
return Napi::String::New(env, "");
}

DEFINE_GETTER_METHOD(gettextAlign)
Napi::Env env = info.Env();
if (mRenderContext)
{
    GTextAlign value = mRenderContext->getCtx2d()->TextAlign();
    if (value == TEXT_ALIGN_LEFT)
    {
        return Napi::String::New(env, "left");
    }
    else if (value == TEXT_ALIGN_RIGHT)
    {
        return Napi::String::New(env, "right");
    }
    else if (value == TEXT_ALIGN_CENTER)
    {
        return Napi::String::New(env, "center");
    }
    else if (value == TEXT_ALIGN_START)
    {
        return Napi::String::New(env, "start");
    }
    else if (value == TEXT_ALIGN_END)
    {
        return Napi::String::New(env, "end");
    }
}
RECORD_TIME_END
return Napi::String::New(env, "");
}

DEFINE_GETTER_METHOD(gettextBaseline)
Napi::Env env = info.Env();
if (mRenderContext)
{
    GTextBaseline value = mRenderContext->getCtx2d()->TextBaseline();
    if (value == TEXT_BASELINE_TOP)
    {
        return Napi::String::New(env, "top");
    }
    else if (value == TEXT_BASELINE_HANGING)
    {
        return Napi::String::New(env, "hanging");
    }
    else if (value == TEXT_BASELINE_ALPHABETIC)
    {
        return Napi::String::New(env, "alphabetic");
    }
    else if (value == TEXT_BASELINE_BOTTOM)
    {
        return Napi::String::New(env, "bottom");
    }
    else if (value == TEXT_BASELINE_IDEOGRAPHIC)
    {
        return Napi::String::New(env, "ideographic");
    }
}
RECORD_TIME_END
return Napi::String::New(env, "");
}

DEFINE_GETTER_METHOD(getCanvas)
RECORD_TIME_END
return mCanvas->mRef.Value();
}

Context2D::~Context2D()
{
    mRenderContext = nullptr;
}
} // namespace NodeBinding