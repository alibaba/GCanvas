/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#ifndef CONTEXT2D_H
#define CONTEXT2D_H
#include <napi.h>
#include "GRenderContext.h"
#include "ImageData.h"
#include "CanvasPattern.h"
#include "Canvas.h"
#include "NodeBindingUtil.h"

namespace NodeBinding
{
class Canvas;
class Context2D : public Napi::ObjectWrap<Context2D>
{
public:
    static void Init(Napi::Env env);
    Context2D(const Napi::CallbackInfo &info);
    void setCanvasRef(NodeBinding::Canvas *canvas);
    virtual ~Context2D();
    static Napi::Object NewInstance(Napi::Env env);
    void inline setRenderContext(std::shared_ptr<GRenderContext> renderContext)
    {
        this->mRenderContext = renderContext;
    }

private:
    NodeBinding::Canvas *mCanvas = nullptr;
    std::shared_ptr<GRenderContext> mRenderContext = nullptr;
    static Napi::FunctionReference constructor;
    void fillRect(const Napi::CallbackInfo &info);
    void arc(const Napi::CallbackInfo &info);
    void arcTo(const Napi::CallbackInfo &info);
    void beginPath(const Napi::CallbackInfo &info);
    void bezierCurveTo(const Napi::CallbackInfo &info);
    void clearRect(const Napi::CallbackInfo &info);
    void clip(const Napi::CallbackInfo &info);
    void closePath(const Napi::CallbackInfo &info);
    Napi::Value createImageData(const Napi::CallbackInfo &info);
    Napi::Value createLinearGradient(const Napi::CallbackInfo &info);
    Napi::Value createPattern(const Napi::CallbackInfo &info);
    Napi::Value createRadialGradient(const Napi::CallbackInfo &info);
    void drawImage(const Napi::CallbackInfo &info);
    void fill(const Napi::CallbackInfo &info);
    void fillText(const Napi::CallbackInfo &info);
    Napi::Value getImageData(const Napi::CallbackInfo &info);
    Napi::Value getLineDash(const Napi::CallbackInfo &info);
    void lineTo(const Napi::CallbackInfo &info);
    Napi::Value measureText(const Napi::CallbackInfo &info);
    void moveTo(const Napi::CallbackInfo &info);
    void putImageData(const Napi::CallbackInfo &info);
    void quadraticCurveTo(const Napi::CallbackInfo &info);
    void rect(const Napi::CallbackInfo &info);
    void restore(const Napi::CallbackInfo &info);
    void rotate(const Napi::CallbackInfo &info);
    void save(const Napi::CallbackInfo &info);
    void scale(const Napi::CallbackInfo &info);
    void setLineDash(const Napi::CallbackInfo &info);
    void setTransform(const Napi::CallbackInfo &info);
    void stroke(const Napi::CallbackInfo &info);
    void strokeRect(const Napi::CallbackInfo &info);
    void strokeText(const Napi::CallbackInfo &info);
    void transform(const Napi::CallbackInfo &info);
    void resetTransform(const Napi::CallbackInfo &info);
    void translate(const Napi::CallbackInfo &info);

    Napi::Value getFillStyle(const Napi::CallbackInfo &info);
    void setFillStyle(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value getfont(const Napi::CallbackInfo &info);
    void setfont(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value getglobalAlpha(const Napi::CallbackInfo &info);
    void setglobalAlpha(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value getglobalCompositeOperation(const Napi::CallbackInfo &info);
    void setglobalCompositeOperation(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value getlineCap(const Napi::CallbackInfo &info);
    void setlineCap(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value getlineDashOffset(const Napi::CallbackInfo &info);
    void setlineDashOffset(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value getlineJoin(const Napi::CallbackInfo &info);
    void setlineJoin(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value getlineWidth(const Napi::CallbackInfo &info);
    void setlineWidth(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value getmiterLimit(const Napi::CallbackInfo &info);
    void setmiterLimit(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value getshadowBlur(const Napi::CallbackInfo &info);
    void setshadowBlur(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value getshadowColor(const Napi::CallbackInfo &info);
    void setshadowColor(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value getshadowOffsetX(const Napi::CallbackInfo &info);
    void setshadowOffsetX(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value getshadowOffsetY(const Napi::CallbackInfo &info);
    void setshadowOffsetY(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value getstrokeStyle(const Napi::CallbackInfo &info);
    void setstrokeStyle(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value gettextAlign(const Napi::CallbackInfo &info);
    void settextAlign(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value gettextBaseline(const Napi::CallbackInfo &info);
    void settextBaseline(const Napi::CallbackInfo &info, const Napi::Value &value);
    Napi::Value getCanvas(const Napi::CallbackInfo &info);
};
} // namespace NodeBinding
#endif