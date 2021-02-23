/**
* Created by G-Canvas Open Source Team.
* Copyright (c) 2017, Alibaba, Inc. All rights reserved.
*
* This source code is licensed under the Apache Licence 2.0.
* For the full copyright and license information, please view
* the LICENSE file in the root directory of this source tree.
*/


#ifdef COMMANDBUFFER_2D

#ifndef GCANVAS_GCOMMANDDECODER2D_H
#define GCANVAS_GCOMMANDDECODER2D_H

#include <string>
#include <unordered_set>

#include "../commandbuffer/GCommandDecoder.hpp"
#include "GContext2DRenderContext.hpp"


namespace gcanvas
{

class GCommandDecoder2D : public GCommandDecoder
{
    enum G2D_FUNC_ID
    {
        G2D_FUNC_arc = 1,
        G2D_FUNC_arcTo,
        G2D_FUNC_beginPath,
        G2D_FUNC_bezierCurveTo,
        G2D_FUNC_clearRect,
        G2D_FUNC_clip,
        G2D_FUNC_closePath,
        G2D_FUNC_createImageData_size,
        G2D_FUNC_createImageData_data,
        G2D_FUNC_createLinearGradient = 10,
        G2D_FUNC_createPattern,
        G2D_FUNC_createRadialGradient,
        G2D_FUNC_drawImage_3,
        G2D_FUNC_drawImage_6,
        G2D_FUNC_drawImage_9,
        G2D_FUNC_fill,
        G2D_FUNC_fillRect,
        G2D_FUNC_fillText,
        G2D_FUNC_getImageData,
        G2D_FUNC_getLineDash = 20,
        G2D_FUNC_lineTo,
        G2D_FUNC_measureText,
        G2D_FUNC_moveTo,
        G2D_FUNC_putImageData_3,
        G2D_FUNC_putImageData_7,
        G2D_FUNC_quadraticCurveTo,
        G2D_FUNC_rect,
        G2D_FUNC_restore,
        G2D_FUNC_rotate,
        G2D_FUNC_save = 30,
        G2D_FUNC_scale,
        G2D_FUNC_setLineDash,
        G2D_FUNC_setTransform,
        G2D_FUNC_storke,
        G2D_FUNC_strokeRect,
        G2D_FUNC_strokeText,
        G2D_FUNC_transform,
        G2D_FUNC_translate,
        //property
        G2D_FUNC_setFillStyle_string,
        G2D_FUNC_setFillStyle_object = 40,
        G2D_FUNC_setStrokeStyle_string,
        G2D_FUNC_setStrokeStyle_object,
        G2D_FUNC_setShadowColor,
        G2D_FUNC_setShadowBlur,
        G2D_FUNC_setShadowOffsetX,
        G2D_FUNC_setShadowOffsetY,
        G2D_FUNC_setLineCap,
        G2D_FUNC_setLineJoin,
        G2D_FUNC_setLineWidth,
        G2D_FUNC_setMiterLimit = 50,
        G2D_FUNC_setLineDashOffset,
        G2D_FUNC_setFont,
        G2D_FUNC_setTextAlign,
        G2D_FUNC_setTextBaseline,
        G2D_FUNC_setGlobalAlpha,
        G2D_FUNC_setGlobalCompositeOperation,
        
        //API数量
        G2D_FUNC_COUNT,
    };
    
public:
    API_EXPORT GCommandDecoder2D(GContext2DRenderContext* renderCtx);
    API_EXPORT ~GCommandDecoder2D();
    
    virtual void InitCommands();
    virtual bool IsSyncCommand(uint32_t cmdId){
        auto iter = mContext2DSyncFuncSet.find(cmdId);
        return iter != mContext2DSyncFuncSet.end();
    }
    
    API_EXPORT virtual bool Decode(GCommandBuffer& buffer, GDecodeRes& res);
    
    static void G2D_arc(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    static void G2D_arcTo(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    static void G2D_beginPath(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    static void G2D_bezierCurveTo(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    static void G2D_clearRect(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    static void G2D_clip(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    static void G2D_closePath(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    static void G2D_createImageData_size(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    static void G2D_createImageData_data(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    static void G2D_createLinearGradient(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    static void G2D_createPattern(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    static void G2D_createRadialGradient(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    static void G2D_drawImage_3(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    static void G2D_drawImage_5(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    static void G2D_drawImage_9(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    static void G2D_fill(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    static void G2D_fillRect(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    static void G2D_fillText(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    static void G2D_getImageData(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    static void G2D_getLineDash(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    static void G2D_lineTo(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    static void G2D_measureText(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    static void G2D_moveTo(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    static void G2D_putImageData_3(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    static void G2D_putImageData_7(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    static void G2D_quadraticCurveTo(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    static void G2D_rect(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    static void G2D_restore(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    static void G2D_rotate(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    static void G2D_save(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    static void G2D_scale(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    static void G2D_setLineDash(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    static void G2D_setTransform(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    static void G2D_storke(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    static void G2D_strokeRect(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    static void G2D_strokeText(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    static void G2D_transform(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    static void G2D_translate(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    
    static void G2D_setFillStyle_string(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    static void G2D_setFillStyle_object(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    static void G2D_setStrokeStyle_string(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    static void G2D_setStrokeStyle_object(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    static void G2D_setShadowColor(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    static void G2D_setShadowBlur(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    static void G2D_setShadowOffsetX(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    static void G2D_setShadowOffsetY(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    static void G2D_setLineCap(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    static void G2D_setLineJoin(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    static void G2D_setLineWidth(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    static void G2D_setMiterLimit(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    static void G2D_setLineDashOffset(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    static void G2D_setFont(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    static void G2D_setTextAlign(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    static void G2D_setTextBaseline(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    static void G2D_setGlobalAlpha(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    static void G2D_setGlobalCompositeOperation(GContext2DRenderContext* context, GCommandBuffer& buffer, GDecodeRes& res);
    
protected:
    std::unordered_set<unsigned int> mContext2DSyncFuncSet;
//    GCanvasContext * mCanvasContext;
    GContext2DRenderContext * mRenderContext2D;
//    static float mFv[8];        //max float param num is 8
//    static uint32_t mIv[4];
};

} //namespace gcanvas

#endif /* GCANVAS_GCOMMANDDECODERWEBGL_H */

#endif
