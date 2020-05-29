#include "Canvas.h"
#include "TextMetrics.h"
namespace NodeBinding
{
    Napi::FunctionReference Canvas::constructor;

    Canvas::Canvas(const Napi::CallbackInfo &info) : Napi::ObjectWrap<Canvas>(info)
    {
        Napi::Env env = info.Env();
        Napi::HandleScope scope(env);

        checkArgs(info, 2);
        mWidth = info[0].As<Napi::Number>().Int32Value();
        mHeight = info[1].As<Napi::Number>().Int32Value();
        mRenderContext = std::make_shared<GRenderContext>(mWidth, mHeight);
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
                            InstanceMethod("toBuffer", &Canvas::Buffer),
                        });
        constructor = Napi::Persistent(func);
        constructor.SuppressDestruct();
        return;
    }

    Napi::Object Canvas::NewInstance(Napi::Env env, Napi::Value arg, Napi::Value arg2)
    {
        Napi::Object obj = constructor.New({arg, arg2});
        obj.Set("name", Napi::String::New(env, "gcanvas"));
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
            if (this->context2dRef.IsEmpty())
            {
                Napi::Object obj = Context2D::NewInstance(env);
                this->context2dRef = Napi::ObjectReference::New(obj);
                Context2D *ctx = Napi::ObjectWrap<Context2D>::Unwrap(obj);
                ctx->setRenderContext(this->mRenderContext);
                ctx->setCanvasRef(this);
                return obj;
            }
            else
            {
                return this->context2dRef.Value();
            }
        }
        else
        {
            throwError(info, "only support 2d now");
            return Napi::Object::New(env);
        }
    }
    void Canvas::createPNG(const Napi::CallbackInfo &info)
    {
        NodeBinding::checkArgs(info, 1);
        std::string arg = info[0].As<Napi::String>().Utf8Value();
        if (this->mRenderContext)
        {
            this->mRenderContext->makeCurrent();
            this->mRenderContext->drawFrame();
            this->mRenderContext->render2file(arg.c_str(), PNG_FORAMT);
        }
        return;
    }
    void Canvas::createJPEG(const Napi::CallbackInfo &info)
    {
        NodeBinding::checkArgs(info, 1);
        std::string arg = info[0].As<Napi::String>().Utf8Value();
        if (this->mRenderContext)
        {
            this->mRenderContext->makeCurrent();
            this->mRenderContext->drawFrame();
            this->mRenderContext->render2file(arg.c_str(), JPEG_FORMAT);
        }
        return;
    }
    Napi::Value Canvas::createJPGStreamSync(const Napi::CallbackInfo &info)
    {
        NodeBinding::checkArgs(info, 2);
        unsigned long size = 0;
        Napi::Buffer<unsigned char> buffer = this->getJPGBuffer(info, size);
        Napi::Function callback = info[0].As<Napi::Function>();
        //handlescope 表示作用域,一般调用callback函数时使用
        Napi::HandleScope scope(info.Env());
        callback.Call({info.Env().Null(),
                       buffer,
                       Napi::Number::New(info.Env(), size)});
        // }
        // else
        // {
        //     Napi::HandleScope scope(info.Env());
        //     callback.Call({Napi::String::New(Env(), "createJPGStreamFail"),
        //                    info.Env().Null(),
        //                    info.Env().Null()});
        // }
    }

    Napi::Value Canvas::createPNGStreamSync(const Napi::CallbackInfo &info)
    {
        NodeBinding::checkArgs(info, 2);
        unsigned long size = 0;
        Napi::Buffer<unsigned char> buffer = this->getPNGBuffer(info, size);
        Napi::Function callback = info[0].As<Napi::Function>();

        //handlescope 表示作用域,一般调用callback函数时使用
        Napi::HandleScope scope(info.Env());
        callback.Call({info.Env().Null(),
                       buffer,
                       Napi::Number::New(info.Env(), size)});
        // }
        // else
        // {
        //     Napi::HandleScope scope(info.Env());
        //     callback.Call({Napi::String::New(Env(), "createPNGStreamFail"),
        //                    info.Env().Null(),
        //                    info.Env().Null()});
        // }
    }
    Napi::Buffer<unsigned char> Canvas::getPNGBuffer(const Napi::CallbackInfo &info, unsigned long &size)
    {
        if (this->mRenderContext)
        {
            this->mRenderContext->makeCurrent();
            this->mRenderContext->drawFrame();
        }
        std::vector<unsigned char> in;
        int ret = this->mRenderContext->getImagePixelPNG(in);
        if (ret == 0)
        {
            size = in.size();
            return Napi::Buffer<unsigned char>::Copy(info.Env(), &in[0], in.size());
        }
        else
        {
            throwError(info.Env(), "createPNGBuffer fail");
        }
    }
    Napi::Buffer<unsigned char> Canvas::getJPGBuffer(const Napi::CallbackInfo &info, unsigned long &size)
    {
        if (this->mRenderContext)
        {
            this->mRenderContext->makeCurrent();
            this->mRenderContext->drawFrame();
        }
        unsigned char *data = nullptr;
        int ret = this->mRenderContext->getImagePixelJPG(&data, size);
        if (ret == 0)
        {
            return Napi::Buffer<unsigned char>::Copy(info.Env(), data, size);
        }
        else
        {
            throwError(info.Env(), "createJPGBuffer fail");
        }
    }
    Napi::Buffer<unsigned char> Canvas::getRawDataBuffer(const Napi::CallbackInfo &info, unsigned long &size)
    {
        unsigned char *data = new unsigned char[4 * mWidth * mHeight];
        int ret = this->mRenderContext->readPixelAndSampleFromCurrentCtx(data);
        return Napi::Buffer<unsigned char>::Copy(info.Env(), data, 4 * mWidth * mHeight);
    }
    Napi::Value Canvas::Buffer(const Napi::CallbackInfo &info)
    {
        unsigned long size = 0;
        //默认输出png 编码
        if (info.Length() == 0)
        {
            return this->getPNGBuffer(info, size);
        }
        else
        {
            if (info.Length() == 1)
            {
                std::string mimeType = info[0].As<Napi::String>().Utf8Value();
                if (mimeType == "image/png")
                {
                    return this->getPNGBuffer(info, size);
                }
                else if (mimeType == "image/jpeg")
                {
                    return this->getJPGBuffer(info, size);
                }
                else if (mimeType == "raw")
                {
                    return this->getRawDataBuffer(info, size);
                }
            }
            return info.Env().Null();
        }
    }
    Canvas::~Canvas()
    {
        this->mRenderContext = nullptr;
    }
} // namespace NodeBinding