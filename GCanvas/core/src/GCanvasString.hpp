///**
// * uc直调接口返回类型
// */
//
//#include <malloc.h>
//#include <string.h>
//#include "IString.h"
//
//class GCanvasString : public IString {
//public:
//    GCanvasString(const char *str) : m_str(nullptr) {
//        m_str = strdup(str);
//    }
//
//    virtual void release() {
//        free((void *) m_str);
//        delete this;
//    }
//
//    virtual const char *data() {
//        return m_str;
//    }
//
//private:
//    const char *m_str;
//};