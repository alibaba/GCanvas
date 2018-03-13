//
// Created by yuantong on 2017/8/2.
//

#ifndef G_CANVAS_I_DELETE_H
#define G_CANVAS_I_DELETE_H
namespace taobao {
    namespace g_engine {
        namespace gcanvas {
            class IDelete {
            public:
                IDelete() {}

                virtual void Delete() = 0;

            protected:
                virtual ~IDelete() {}

            };
        }
    }
}
#endif //G_CANVAS_I_DELETE_H
