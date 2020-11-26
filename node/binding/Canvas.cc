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
        
        NodeBinding::checkArgs(info, 2);
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

    void Canvas::setWidth(const Napi::CallbackInfo &info, const Napi::Value &value)
    {
        if( info.Length() > 0 && info[0].IsNumber() )
        {
            int newWidth = info[0].As<Napi::Number>().Int32Value();
            if( newWidth > 0 && newWidth != mWidth )
            {
                mWidth = newWidth;
                mRenderContext->resize(mWidth, mHeight);
                mContext2dRef.Reset();
            }
        }
    }

    Napi::Value Canvas::getHeight(const Napi::CallbackInfo &info)
    {
        return Napi::Number::New(info.Env(), mHeight);
    }

    void Canvas::setHeight(const Napi::CallbackInfo &info, const Napi::Value &value)
    {
        if( info.Length() > 0 && info[0].IsNumber() )
        {
            int newHeight = info[0].As<Napi::Number>().Int32Value();
            if( newHeight > 0 && newHeight != mHeight )
            {
                mHeight = newHeight;
                mRenderContext->resize(mWidth, mHeight);
                mContext2dRef.Reset();
            }
        }
    }

    void Canvas::Init(Napi::Env env, Napi::Object exports)
    {
        Napi::HandleScope scope(env);

        Napi::Function func = DefineClass(env, "Canvas", {
            InstanceAccessor("width", &Canvas::getWidth, &Canvas::setWidth),
            InstanceAccessor("height", &Canvas::getHeight, &Canvas::setHeight),
            InstanceMethod("getContext", &Canvas::getContext),
            InstanceMethod("createPNGStreamSync", &Canvas::createPNGStreamSync),
            InstanceMethod("createJPGStreamSync", &Canvas::createJPGStreamSync),
            InstanceMethod("toBuffer", &Canvas::toBuffer),
            InstanceMethod("toDataURL", &Canvas::toDataURL),
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
        NodeBinding::checkArgs(info, 1);
        std::string type = info[0].As<Napi::String>().Utf8Value();
        if (type == "2d")
        {
            if (mContext2dRef.IsEmpty())
            {
                Napi::Object obj = Context2D::NewInstance(env);
                Context2D *ctx = Napi::ObjectWrap<Context2D>::Unwrap(obj);
                mRenderContext->setContextType(type);
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
                mRenderContext->setContextType(type);
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
    Napi::Value Canvas::toBuffer(const Napi::CallbackInfo &info)
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
    Napi::Value Canvas::toDataURL(const Napi::CallbackInfo &info)
    {
        if( mWidth == 0 || mHeight == 0 )
        {
            return Napi::String::New(info.Env(), "data:,");
        }

        bool isJPEG = false; //default output
        float jepgQuality = 1.0;
        unsigned long size = 0;
        if (info.Length() >= 1)
        {
            std::string mimeType = info[0].As<Napi::String>().Utf8Value();
            if (mimeType == "image/png")
            {
                isJPEG = false;
            }
            else if (mimeType == "image/jpeg")
            {
                isJPEG = true;
            }

            if( isJPEG && info.Length() >= 2 && info[1].IsNumber() )
            {
                jepgQuality = info[1].As<Napi::Number>().FloatValue();
            }
        }

        if (mRenderContext)
        {
            mRenderContext->makeCurrent();
            mRenderContext->drawFrame();
        }
        
        if ( isJPEG )
        {
            std::string base64Str = "data:image/jpeg;base64,";
            unsigned char *jpegBuffer = nullptr;
            int ret = mRenderContext->getImagePixelJPG(&jpegBuffer, size);
            if (ret == 0)
            {
                //jepgbuffer & size
                std::string jpegStr((const char*)jpegBuffer, (size_t)size);

                std::string tmpStr;
                NodeBinding::toBase64(tmpStr, jpegStr);

                 std::cout << "JPEG in: " << jpegStr.size() << ",out:" << tmpStr.size() << std::endl;

                base64Str.append(tmpStr);
            }
            return Napi::Buffer<unsigned char>::Copy(info.Env(), (unsigned char *)base64Str.c_str(), base64Str.size());
        }
        else
        {
            std::string base64Str = "data:image/png;base64,";
            std::vector<unsigned char> pngData;
            int ret = mRenderContext->getImagePixelPNG(pngData);
            
            if( ret == 0 )
            {
                std::vector<unsigned char> base64Vec;
                NodeBinding::toBase64(base64Vec, pngData);

                std::cout << "PNG in: " << pngData.size() << ",out:" << base64Vec.size() << std::endl;
                base64Str.append((const char*)(&base64Vec[0]), base64Vec.size());
            }
            return Napi::Buffer<unsigned char>::Copy(info.Env(), (unsigned char *)base64Str.c_str(), base64Str.size());
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