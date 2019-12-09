//
// Created by yuantong on 2017/8/2.
//

#ifndef G_CANVAS_REF_COUNT_H
#define G_CANVAS_REF_COUNT_H

namespace taobao {
    namespace g_engine {
        namespace gcanvas {
            class RefCount {
            public:
                RefCount() : mRefCount(0) {}

                void AddRef() {
                    ++mRefCount;
                }

                void ReleaseRef() {
                    if (--mRefCount <= 0)
                        delete this;
                }

            protected:
                virtual ~RefCount() {}

            protected:
                unsigned int mRefCount;
            };
        }
    }
}

#endif //G_CANVAS_REF_COUNT_H
