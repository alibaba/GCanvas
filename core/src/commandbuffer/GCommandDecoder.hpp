/**
* Created by G-Canvas Open Source Team.
* Copyright (c) 2017, Alibaba, Inc. All rights reserved.
*
* This source code is licensed under the Apache Licence 2.0.
* For the full copyright and license information, please view
* the LICENSE file in the root directory of this source tree.
*/

#ifndef GCANVAS_GCOMMANDDECODER_H
#define GCANVAS_GCOMMANDDECODER_H

#include <stdio.h>
#include <memory>
#include <functional>
#include "GCommandBuffer.hpp"


namespace gcanvas
{

enum GResType
{
    GResType_NONE = 0,
    GResType_NULL,
    GResType_Bool,
    GResType_Int32,
    GResType_UInt32,
    GResType_UInt32_CreateBuffer,
    GResType_UInt32_CreateTexture,
    GResType_UInt32_CreateFrameBuffer,
    GResType_UInt32_CreateShader,
    GResType_UInt32_CreateProgram,
    GResType_UInt32_CreateRenderBuffer,
    GResType_UInt32_CreateVertexArray,//extension
    GResType_Float,
    GResType_BoolPtr,
    GResType_CharPtr,
    GResType_Int32Ptr,
    GResType_UInt32Ptr,
    GResType_FloatPtr,
    GResType_CharPtr_GetExtention,
    GResType_CharPtr_GetSupportedExtention,
    GResType_GetContextAttributes

};

/**
    `GDecodeRes` struct save the value of each batch render while has result
 */
struct GDecodeRes
{
    bool hasResult;
    GResType type;
    //result is a typed value
    union
    {
        uint8_t         boolValue;
        int32_t         intValue;
        uint32_t        uintValue;
        float           floatValue;
    };

    //multi return value, while point value not nullptr, delete it before use
    union
    {
        char *          charPtrValue;
        int32_t *       intPtrValue;
        uint32_t *      uintPtrValue;
        float *         floatPtrValue;
    };
    //size of element
    size_t length;
    //size of byte size
    size_t byteSize;
    
    size_t maxSize;
    
    GDecodeRes()
    {
        maxSize = 2048;
        hasResult = false;
        type = GResType_NONE;
        uintValue = 0;
        charPtrValue = new char[maxSize];
    }
    
    ~GDecodeRes()
    {
        if( charPtrValue )
        {
            delete charPtrValue;
            charPtrValue = nullptr;
        }
    }
    
    inline void checkSize(size_t size)
    {
        if( size <= maxSize ) return;
        
        maxSize += size*2;
        char * newCharPtrValue = new char[maxSize];
        if( newCharPtrValue ){
            delete charPtrValue;
            charPtrValue = newCharPtrValue;
        }
    }
    
    //reset before use
    inline void reset()
    {
        hasResult = false;
        type = GResType_NONE;
        uintValue = 0;
        length = byteSize = 0;
    }
};

//define WebGL API function
//using GCommandFunc = std::function< void(GCommandBuffer& buffer, GDecodeRes& res ) >;
typedef void (*GCommandFuncPtr)(GCommandBuffer& , GDecodeRes& );

//convert utf16 string to utf8 string
std::string U16StrToU8Str( char16_t* u16char );

class GCommandDecoder
{
public:
    GCommandDecoder(){};
    ~GCommandDecoder(){};

    void SetChangeContextFunc( std::function<void(unsigned int)> func){
        mChangeContextFunc = func;
    }

    std::function<void(unsigned int)> GetChangeContextFunc(){
        return mChangeContextFunc;
    }

    virtual void InitCommands() = 0;
    virtual bool Decode(GCommandBuffer& buffer, GDecodeRes& res) = 0;
    virtual bool IsSyncCommand(uint32_t commandId) = 0;

protected:
    std::function<void(unsigned int)> mChangeContextFunc;
};

} //namespace gcanvas

#endif /* GCANVAS_GCOMMANDDECODER_H */
