//
// Created by 胡帅 on 8/9/17.
//

#ifndef ANDROID_GCANVAS_PROXY_H
#define ANDROID_GCANVAS_PROXY_H

#include "../../GCanvas.hpp"

using namespace gcanvas;

//namespace taobao {
//    namespace g_engine {
//        namespace gcanvas {
            class gcanvas_proxy:public GCanvas {
                public:
                    gcanvas_proxy(std::string componentid,void *context);
                    ~gcanvas_proxy();
                    void signalUpGLthread();
                    void setRefreshFlag(bool refresh);
                    void waitResponse();
            public:
                void *m_context;
            };
//        }
//    }
//}

#endif //ANDROID_GCANVAS_PROXY_H