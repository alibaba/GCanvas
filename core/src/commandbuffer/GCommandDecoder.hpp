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
#include "GCommandTypes.hpp"

namespace gcanvas
{

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
//    virtual bool Decode(GCommandBuffer& buffer, GDecodeRes& res) = 0;
    virtual bool IsSyncCommand(uint32_t commandId) = 0;

protected:
    std::function<void(unsigned int)> mChangeContextFunc;
};

} //namespace gcanvas

#endif /* GCANVAS_GCOMMANDDECODER_H */
