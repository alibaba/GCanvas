/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */

#ifndef GCANVAS_GFILLSTYLE_H
#define GCANVAS_GFILLSTYLE_H

#include "GConvert.h"

class GFillStyle
{
public:
    enum Style
    {
        STYLE_PATTERN,
        STYLE_LINEAR_GRADIENT,
        STYLE_RADIAL_GRADIENT,
        STYLE_UNDEFINED
    };

    GFillStyle(Style style) : mStyle(style) {}

    virtual int GetTextureListID() { return -1; }
    virtual GFillStyle *Clone() { return nullptr; }
    virtual bool AddColorStop(float pos, const std::string &color_name){ return false;}

    bool IsPattern() { return mStyle == STYLE_PATTERN; }
    bool IsLinearGradient() { return mStyle == STYLE_LINEAR_GRADIENT; }
    bool IsRadialGradient() { return mStyle == STYLE_RADIAL_GRADIENT; }
    bool IsDefault() { return mStyle == STYLE_UNDEFINED; }

    virtual ~GFillStyle(){};

private:
    Style mStyle;
};

class FillStylePattern : public GFillStyle
{
public:
    FillStylePattern(int texture_list_id, const std::string &pattern)
        : GFillStyle(STYLE_PATTERN), mPattern(pattern), mTextureListId(texture_list_id)
    {
    }
    
    FillStylePattern(int texture_list_id, short textureWidth, short textHeight, const std::string &pattern)
    : GFillStyle(STYLE_PATTERN),mTextureWidth(textureWidth), mTextureHeight(textHeight),
    mPattern(pattern), mTextureListId(texture_list_id)
    {
    }

    virtual ~FillStylePattern() { mPattern.clear(); }

    const std::string &GetPattern() const { return mPattern; }
    int GetTextureListID() { return mTextureListId; }
    short GetTextureWidth() { return mTextureWidth; }
    short GetTextureHeight() { return mTextureHeight; }


    GFillStyle *Clone()
    {
        FillStylePattern *ptr =
            new FillStylePattern(mTextureListId, mPattern);
        return ptr;
    }

private:
    std::string mPattern;
    int mTextureListId;
    short mTextureWidth;
    short mTextureHeight;
};

class FillStyleLinearGradient : public GFillStyle
{
public:
    static const int MAX_STOP_NUM = 5;
    struct ColorStop
    {
        float pos;
        GColorRGBA color;
    };

    FillStyleLinearGradient(const GPoint &start_pos, const GPoint &end_pos)
        : GFillStyle(STYLE_LINEAR_GRADIENT), mStartPos(start_pos),
          mEndPos(end_pos), mStopCount(0)
    {
    }

    bool AddColorStop(float pos, const std::string &color_name)
    {
        if (mStopCount < MAX_STOP_NUM)
        {
            mStops[mStopCount].pos = pos;
            mStops[mStopCount].color = gcanvas::StrValueToColorRGBA(color_name.c_str());
            mStopCount++;
            return true;
        }
        return false;
    }

    GFillStyle *Clone()
    {
        FillStyleLinearGradient *ptr =
            new FillStyleLinearGradient(mStartPos, mEndPos);
        *ptr = *this;
        return ptr;
    }

    const GPoint &GetStartPos() { return mStartPos; }
    const GPoint &GetEndPos() { return mEndPos; }
    int GetColorStopCount() const { return mStopCount; }
    const ColorStop *GetColorStop(int id)
    {
        if (id >= 0 && id < mStopCount)
        {
            return &mStops[id];
        }
        return nullptr;
    }

private:
    GPoint mStartPos, mEndPos;
    int mStopCount;
    ColorStop mStops[MAX_STOP_NUM];
};

class FillStyleRadialGradient : public GFillStyle
{
public:
    static const int MAX_STOP_NUM = 5;
    struct ColorStop
    {
        float pos;
        GColorRGBA color;
    };

    FillStyleRadialGradient(const float *start, const float *end)
        : GFillStyle(STYLE_RADIAL_GRADIENT), mStopCount(0)
    {
        mStart[0] = start[0];
        mStart[1] = start[1];
        mStart[2] = start[2];
        mEnd[0] = end[0];
        mEnd[1] = end[1];
        mEnd[2] = end[2];
    }

    bool AddColorStop(float pos, const std::string &color_name)
    {
        if (mStopCount < MAX_STOP_NUM)
        {
            mStops[mStopCount].pos = pos;
            mStops[mStopCount].color = gcanvas::StrValueToColorRGBA(color_name.c_str());
            mStopCount++;
            return true;
        }
        return false;
    }

    const float *GetStartPos() { return mStart; }
    const float *GetEndPos() { return mEnd; }
    int GetColorStopCount() const { return mStopCount; }
    const ColorStop *GetColorStop(int id)
    {
        if (id >= 0 && id < mStopCount)
        {
            return &mStops[id];
        }
        return nullptr;
    }

    GFillStyle *Clone()
    {
        FillStyleRadialGradient *ptr =
            new FillStyleRadialGradient(mStart, mEnd);
        *ptr = *this;
        return ptr;
    }

private:
    float mStart[3], mEnd[3];
    int mStopCount;
    ColorStop mStops[MAX_STOP_NUM];
};

#endif /* GCANVAS_GFILLSTYLE_H */
