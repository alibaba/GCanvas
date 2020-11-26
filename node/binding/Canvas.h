/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#ifndef CANVAS_H
#define CANVAS_H
#include "GRenderContext.h"
#include "CanvasRenderingContext2D.h"
#include "CanvasRenderingContextWebGL.h"
#include "CanvasGradient.h"
#include "ImageData.h"
#include "CanvasPattern.h"
#include <napi.h>
#include <memory>
namespace NodeBinding
{
    class Canvas : public Napi::ObjectWrap<Canvas>
    {
    public:
        static void Init(Napi::Env env, Napi::Object exports);
        static Napi::Object NewInstance(Napi::Env env, Napi::Value arg, Napi::Value arg2);

        Canvas(const Napi::CallbackInfo &info);
        virtual ~Canvas();
        int getWidth();
        int getHeight();
        
        Napi::ObjectReference mRef;
        std::shared_ptr<GRenderContext> mRenderContext;
        
    private:
        static Napi::FunctionReference constructor;
        Napi::Value getWidth(const Napi::CallbackInfo &info);
        void setWidth(const Napi::CallbackInfo &info, const Napi::Value &value);
        Napi::Value getHeight(const Napi::CallbackInfo &info);
        void setHeight(const Napi::CallbackInfo &info, const Napi::Value &value);
        Napi::Value getContext(const Napi::CallbackInfo &info);
        Napi::Value createPNGStreamSync(const Napi::CallbackInfo &info);
        Napi::Value createJPGStreamSync(const Napi::CallbackInfo &info);
        Napi::Value toBuffer(const Napi::CallbackInfo &info);
        Napi::Value toDataURL(const Napi::CallbackInfo &info);
        Napi::Buffer<unsigned char> getPNGBuffer(const Napi::CallbackInfo &info, unsigned long &size);
        Napi::Buffer<unsigned char> getJPGBuffer(const Napi::CallbackInfo &info, unsigned long &size);
        Napi::Buffer<unsigned char> getRawDataBuffer(const Napi::CallbackInfo &info, unsigned long &size);
        
        Napi::ObjectReference mContext2dRef;
        Napi::ObjectReference mContextWebGLRef;

        int mWidth = 0;
        int mHeight = 0;
        unsigned char *mDataRaw;
    };
} // namespace NodeBinding
#endif