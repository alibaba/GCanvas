/**
* Created by G-Canvas Open Source Team.
* Copyright (c) 2017, Alibaba, Inc. All rights reserved.
*
* This source code is licensed under the Apache Licence 2.0.
* For the full copyright and license information, please view
* the LICENSE file in the root directory of this source tree.
*/

#ifndef GCANVAS_GCOMMANDTYPES_H
#define GCANVAS_GCOMMANDTYPES_H

#include <stdio.h>
#include <memory>
#include <functional>
#include "../export.h"

namespace gcanvas
{
class GCommandBuffer;

class JSBindingPixels
{
public:
    API_EXPORT  JSBindingPixels();
    API_EXPORT ~JSBindingPixels();

    JSBindingPixels(JSBindingPixels &&v)
    {
        textureId = v.textureId;
        pixels = v.pixels;
        width = v.width;
        height = v.height;
        
        v.textureId = 0;
        v.pixels = nullptr;
        v.width = 0;
        v.height = 0;
    }
    
    
    uint32_t    textureId;
    uint8_t*    pixels;
    short       width;
    short       height;
};


//Fectch pixels form JSBinding instance with instanceID
using FetchPixelsFunc = std::function< void (void*obj, uint64_t, JSBindingPixels*) >;

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
    GResType_GetContextAttributes,
    
    //2d
    GResType_MeasureText,


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

//convert utf16 string to utf8 string
std::string U16StrToU8Str( char16_t* u16char );

}

#endif /* GCANVAS_GCOMMANDTYPES_H */
