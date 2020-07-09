/**
* Created by G-Canvas Open Source Team.
* Copyright (c) 2017, Alibaba, Inc. All rights reserved.
*
* This source code is licensed under the Apache Licence 2.0.
* For the full copyright and license information, please view
* the LICENSE file in the root directory of this source tree.
*/


#include "GCommandBuffer.hpp"
#include "../support/Log.h"
namespace gcanvas
{

    GCommandBuffer::GCommandBuffer():
            mCurrentPos(0), mBufferData(nullptr), mBufferSize(0)
    {

    }

    GCommandBuffer::GCommandBuffer(int8_t* buffer, size_t size):
            mCurrentPos(0), mBufferData(buffer), mBufferSize(size)
    {
        //解析meta信息
        parseArray<uint32_t>(mMeta, 2);

        //计算buffersize
        mBufferSize = mMeta[0] * 4;
//    LOG_E("mBufferSize is %d",mBufferSize);
    }

    GCommandBuffer::~GCommandBuffer()
    {

    }

    void GCommandBuffer::resetBuffer(int8_t* bufffer, size_t size)
    {
        mBufferData = bufffer, mBufferSize = size, mCurrentPos = 0;

        //解析meta信息
        parseArray<uint32_t>(mMeta, 2);

        //计算buffersize
        mBufferSize = mMeta[0] * 4;
//    LOG_E("mBufferSize is %d",mBufferSize);

    }

    int8_t* GCommandBuffer::getCurrentBufferPtr()
    {
        if( mCurrentPos < mBufferSize ) {
            return (mBufferData + mCurrentPos);
        }
        return NULL;
    }

    template <typename T>
    T* GCommandBuffer::parseValue()
    {
        size_t size = sizeof(T);
        if( checkBufferSize(size) )
        {
            T* ptr = (T*)(mBufferData + mCurrentPos);
            mCurrentPos += size;
            return ptr;
        }
        return nullptr;
    }
    template uint32_t* GCommandBuffer::parseValue();
    template float* GCommandBuffer::parseValue();
    template uint64_t* GCommandBuffer::parseValue();

    template <typename T>
    bool GCommandBuffer::parseArray(T *arr, size_t length)
    {
        size_t size = sizeof(T) * length;
        if( checkBufferSize(size) )
        {
            T* basePtr = (T*)(mBufferData + mCurrentPos);
            for (int i=0; i<length; i++)
            {
                arr[i] = *(basePtr + i);
            }
            mCurrentPos += size;
            return true;
        }
        return false;
    }

    template bool GCommandBuffer::parseArray(int32_t* arr, size_t length);
    template bool GCommandBuffer::parseArray(uint32_t* arr, size_t length);
    template bool GCommandBuffer::parseArray(float* arr, size_t length);

    const void* GCommandBuffer::parseBuffer(size_t size)
    {
        if( checkBufferSize(size) )
        {
            const void *bufPtr = (mBufferData + mCurrentPos);
            mCurrentPos += size;
            return bufPtr;
        }
        return nullptr;
    }

    const void* GCommandBuffer::parseBufferAlign(size_t size)
    {
        uint8_t delta = size % 4;
        size_t realSize = size;
        if( delta != 0 )
        {
            realSize = (size + 4 ) - delta;
        }

        if( checkBufferSize(realSize) )
        {
            const void *bufPtr = (mBufferData + mCurrentPos);
            mCurrentPos += realSize;
            return bufPtr;
        }
        return nullptr;
    }



    inline bool GCommandBuffer::checkBufferSize(size_t size)
    {
        if( size <= 0 ) return false;
        return mCurrentPos+size <= mBufferSize;
    }


} //namespace gcanvas