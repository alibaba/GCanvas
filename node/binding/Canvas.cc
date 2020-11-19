/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#include "Canvas.h"
#include "TextMetrics.h"
namespace NodeBinding
{
    Napi::FunctionReference Canvas::constructor;

    Canvas::Canvas(const Napi::CallbackInfo &info) : Napi::ObjectWrap<Canvas>(info), mDataRaw(nullptr)
    {
        Napi::Env env = info.Env();
        Napi::HandleScope scope(env);

        checkArgs(info, 2);
        mWidth = info[0].As<Napi::Number>().Int32Value();
        mHeight = info[1].As<Napi::Number>().Int32Value();
        mRenderContext = std::make_shared<GRenderContext>(mWidth, mHeight, 2.0);
        mRenderContext->initRenderEnviroment();
    }

    int Canvas::getWidth()
    {
        return mWidth;
    }

    int Canvas::getHeight()
    {
        return mHeight;
    }

    Napi::Value Canvas::getWidth(const Napi::CallbackInfo &info)
    {
        return Napi::Number::New(info.Env(), mWidth);
    }

    Napi::Value Canvas::getHeight(const Napi::CallbackInfo &info)
    {
        return Napi::Number::New(info.Env(), mHeight);
    }

    void Canvas::Init(Napi::Env env, Napi::Object exports)
    {
        Napi::HandleScope scope(env);

        Napi::Function func =
            DefineClass(env,
                        "Canvas",
                        {
                            InstanceAccessor("width", &Canvas::getWidth, nullptr),
                            InstanceAccessor("height", &Canvas::getHeight, nullptr),
                            InstanceMethod("getContext", &Canvas::getContext),
                            InstanceMethod("createPNG", &Canvas::createPNG),
                            InstanceMethod("createJPEG", &Canvas::createJPEG),
                            InstanceMethod("createPNGStreamSync", &Canvas::createPNGStreamSync),
                            InstanceMethod("createJPGStreamSync", &Canvas::createJPGStreamSync),
                            InstanceMethod("toBuffer", &Canvas::ToBuffer),
                        });
        constructor = Napi::Persistent(func);
        constructor.SuppressDestruct();
        return;
    }

    Napi::Object Canvas::NewInstance(Napi::Env env, Napi::Value arg, Napi::Value arg2)
    {
        Napi::Object obj = constructor.New({arg, arg2});
        obj.Set("name", Napi::String::New(env, "canvas"));
        Canvas *canvas = Napi::ObjectWrap<Canvas>::Unwrap(obj);
        canvas->mRef = Napi::ObjectReference::New(obj);
        return obj;
    }

    Napi::Value Canvas::getContext(const Napi::CallbackInfo &info)
    {
        Napi::Env env = info.Env();
        checkArgs(info, 1);
        std::string type = info[0].As<Napi::String>().Utf8Value();
        if (type == "2d")
        {
            if (mContext2dRef.IsEmpty())
            {
                Napi::Object obj = Context2D::NewInstance(env);
                Context2D *ctx = Napi::ObjectWrap<Context2D>::Unwrap(obj);
                mRenderContext->setType(type);
                ctx->setRenderContext(mRenderContext);
                ctx->setCanvasRef(this);

                //save reference
                mContext2dRef = Napi::ObjectReference::New(obj);
                return obj;
            }
            else
            {
                return mContext2dRef.Value();
            }
        }
        else if (type == "webgl")
        {
            if (mContextWebGLRef.IsEmpty())
            {
                Napi::Object obj = ContextWebGL::NewInstance(env);
                ContextWebGL *ctx = Napi::ObjectWrap<ContextWebGL>::Unwrap(obj);
                ctx->setRenderContext(mRenderContext);
                mRenderContext->setType(type);
                obj.Set("canvas",  this->Value());

                 // save reference
                mContextWebGLRef = Napi::ObjectReference::New(obj);
                return obj;
            }
            else
            {
                return mContextWebGLRef.Value();
            }
        }
        else
        {
            throwError(info, "type is invalid \n");
            return Napi::Object::New(env);
        }
    }
    void Canvas::createPNG(const Napi::CallbackInfo &info)
    {
        NodeBinding::checkArgs(info, 1);
        std::string arg = info[0].As<Napi::String>().Utf8Value();
        if (mRenderContext)
        {
            mRenderContext->makeCurrent();
            mRenderContext->drawFrame();
            mRenderContext->render2file(arg.c_str(), PNG_FORAMT);
        }
        return;
    }
    void Canvas::createJPEG(const Napi::CallbackInfo &info)
    {
        NodeBinding::checkArgs(info, 1);
        std::string arg = info[0].As<Napi::String>().Utf8Value();
        if (mRenderContext)
        {
            mRenderContext->makeCurrent();
            mRenderContext->drawFrame();
            mRenderContext->render2file(arg.c_str(), JPEG_FORMAT);
        }
        return;
    }
    Napi::Value Canvas::createJPGStreamSync(const Napi::CallbackInfo &info)
    {
        NodeBinding::checkArgs(info, 2);
        unsigned long size = 0;
        Napi::Buffer<unsigned char> buffer = getJPGBuffer(info, size);
        if (size >= 0)
        {
            Napi::Function callback = info[0].As<Napi::Function>();
            //handlescope 表示作用域,一般调用callback函数时使用
            Napi::HandleScope scope(info.Env());
            callback.Call({info.Env().Null(),
                           buffer,
                           Napi::Number::New(info.Env(), size)});
        }
        else
        {
            Napi::Function callback = info[0].As<Napi::Function>();
            Napi::HandleScope scope(info.Env());
            callback.Call({Napi::String::New(Env(), "createJPGStreamFail"),
                           info.Env().Null(),
                           info.Env().Null()});
        }
        return info.Env().Undefined();
    }

    Napi::Value Canvas::createPNGStreamSync(const Napi::CallbackInfo &info)
    {
        NodeBinding::checkArgs(info, 2);
        unsigned long size = 0;
        Napi::Buffer<unsigned char> buffer = getPNGBuffer(info, size);
        if (size >= 0)
        {
            Napi::Function callback = info[0].As<Napi::Function>();
            //handlescope 表示作用域,一般调用callback函数时使用
            Napi::HandleScope scope(info.Env());
            callback.Call({info.Env().Null(),
                           buffer,
                           Napi::Number::New(info.Env(), size)});
        }
        else
        {
            Napi::Function callback = info[0].As<Napi::Function>();
            Napi::HandleScope scope(info.Env());
            callback.Call({Napi::String::New(Env(), "createPNGStreamFail"),
                           info.Env().Null(),
                           info.Env().Null()});
        }
        return info.Env().Undefined();
    }
    Napi::Buffer<unsigned char> Canvas::getPNGBuffer(const Napi::CallbackInfo &info, unsigned long &size)
    {
        if (mRenderContext)
        {
            mRenderContext->makeCurrent();
            mRenderContext->drawFrame();
        }
        std::vector<unsigned char> dataPNGFormat;
        int ret = mRenderContext->getImagePixelPNG(dataPNGFormat);
        if (ret == 0)
        {
            size = dataPNGFormat.size();
            return Napi::Buffer<unsigned char>::Copy(info.Env(), &dataPNGFormat[0], dataPNGFormat.size());
        }
        else
        {
            return Napi::Buffer<unsigned char>::New(info.Env(), nullptr, 0);
        }
    }
    Napi::Buffer<unsigned char> Canvas::getJPGBuffer(const Napi::CallbackInfo &info, unsigned long &size)
    {
        if (mRenderContext)
        {
            mRenderContext->makeCurrent();
            mRenderContext->drawFrame();
        }
        unsigned char *dataJPGFormat = nullptr;
        int ret = mRenderContext->getImagePixelJPG(&dataJPGFormat, size);
        if (ret == 0)
        {
            return Napi::Buffer<unsigned char>::Copy(info.Env(), dataJPGFormat, size);
        }
        else
        {
            size = -1;
            return Napi::Buffer<unsigned char>::New(info.Env(), nullptr, 0);
        }
    }
    Napi::Buffer<unsigned char> Canvas::getRawDataBuffer(const Napi::CallbackInfo &info, unsigned long &size)
    {

        if (mDataRaw == nullptr)
        {
            mDataRaw = new unsigned char[4 * mWidth * mHeight];
        }
        int ret = mRenderContext->readPixelAndSampleFromCurrentCtx(mDataRaw);
        if (ret == 0)
        {
            return Napi::Buffer<unsigned char>::Copy(info.Env(), mDataRaw, 4 * mWidth * mHeight);
        }
        else
        {
            size = -1;
            return Napi::Buffer<unsigned char>::Copy(info.Env(), nullptr, 0);
        }
    }
    Napi::Value Canvas::ToBuffer(const Napi::CallbackInfo &info)
    {
        unsigned long size = 0;
        //默认输出png 编码
        if (info.Length() == 0)
        {
            return getPNGBuffer(info, size);
        }
        else
        {
            Napi::Buffer<unsigned char> ret;
            if (info.Length() == 1)
            {
                std::string mimeType = info[0].As<Napi::String>().Utf8Value();
                if (mimeType == "image/png")
                {
                    ret = getPNGBuffer(info, size);
                }
                else if (mimeType == "image/jpeg")
                {
                    ret = getJPGBuffer(info, size);
                }
                else if (mimeType == "raw")
                {
                    ret = getRawDataBuffer(info, size);
                }
            }
            if (size < 0)
            {
                return info.Env().Null();
            }
            else
            {
                return ret;
            }
        }
    }
    Canvas::~Canvas()
    {
        mRenderContext = nullptr;
        if (mDataRaw != nullptr)
        {
            free(mDataRaw);
            mDataRaw = nullptr;
        }
        printf("canvas destroy called \n");
    }
} // namespace NodeBinding