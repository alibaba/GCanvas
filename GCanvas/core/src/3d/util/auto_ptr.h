//
// Created by yuantong on 2017/8/2.
//

#ifndef G_CANVAS_AUTO_PTR_H
#define G_CANVAS_AUTO_PTR_H

#include "i_delete.h"
#include "ref_count.h"

namespace taobao {
    namespace g_engine {
        namespace gcanvas {
            template<typename T>
            class AutoPtr final {
            public:
                AutoPtr(T *r = nullptr) : mPtr(nullptr) {
                    InternalAdd(r);
                }

                AutoPtr(const AutoPtr<T> &r) : mPtr(nullptr) {
                    InternalAdd(r.RawData());
                }

                ~AutoPtr() {
                    InternalRelease(mPtr);
                }

                AutoPtr<T> &operator=(const AutoPtr<T> &r) {
                    InternalAdd(r.RawData());
                    return *this;
                }

                AutoPtr<T> &operator=(T *r) {
                    InternalAdd(r);
                    return *this;
                }

                T *operator->() {
                    return RawData();
                }

                T &operator*() {
                    return *RawData();
                }

                // It's an unsafe method, it's deprecated to be used outside.
                T *RawData() {
                    return mPtr;
                }

                bool IsNULL() {
                    return (mPtr == nullptr);
                }

            private:
                void InternalAdd(RefCount *r) {
                    if (r == nullptr)
                        return;

                    r->AddRef();

                    if (mPtr)
                        static_cast<RefCount *>(mPtr)->ReleaseRef();

                    mPtr = (T *) r;
                }

                void InternalAdd(IDelete *r) {
                    if (r == mPtr)
                        return;

                    if (mPtr)
                        static_cast<IDelete *>(mPtr)->Delete();

                    mPtr = (T *) r;
                }

                void InternalAdd(void *r) {
                    if (r == mPtr)
                        return;

                    delete mPtr;

                    mPtr = (T *) r;
                }

                void InternalRelease(RefCount *r) {
                    static_cast<RefCount *>(mPtr)->ReleaseRef();
                    mPtr = nullptr;
                }

                void InternalRelease(IDelete *r) {
                    static_cast<IDelete *>(mPtr)->Delete();
                    mPtr = nullptr;
                }

                void InternalRelease(void *r) {
                    delete mPtr;
                    mPtr = nullptr;
                }

            private:
                T *mPtr;
            };
        }
    }
}

#endif //G_CANVAS_AUTO_PTR_H
