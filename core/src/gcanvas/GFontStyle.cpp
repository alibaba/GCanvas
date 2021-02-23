/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#include "GFontStyle.h"
#include "GStrSeparator.h"
#include "GCanvas2dContext.h"
#include "../support/Log.h"

#include <cstdlib>
#include <algorithm>
#include <sstream>

namespace gcanvas {

    GFontStyle::GFontStyle(const char *font, float ratio) : mFontName(font ? font : "") {
        mFullFontStyle = "";
        mStyle = Style::NORMAL;
        mVariant = Variant::NORMAL;
        mWeight = Weight::NORMAL;
        mSize = 10 * ratio;
        mFamily = "sans-serif";
        mRatio = ratio;
        mAscender = -1;
        mDescender = -1;

        if (font != nullptr && mFontName.length() > 0) {
            Initialize(font);
        }
    }


    GFontStyle::GFontStyle(const char *font, Style style, Variant variant, Weight weight, 
        float fontSize, std::string& fontFamily, float ratio) {
        mFontName = font ? font : "";
        mFullFontStyle = font ? font : "";
        mStyle = style;
        mVariant = variant;
        mWeight = weight;
        mSize = fontSize * ratio;
        mRatio = ratio;
        mFamily = fontFamily;

        mAscender = -1;
        mDescender = -1;
    }



    GFontStyle::~GFontStyle() {}

    std::string GFontStyle::GetName() {
        return mFontName;
    }

    void GFontStyle::Initialize(const char *font) {
        if (font != nullptr) {
            mFullFontStyle = font;
        } else {
            mFullFontStyle = "";
        }

        if (font != nullptr) {
            float fSize = 0;
            ParseFields(font, mStyle, mVariant, mWeight, fSize,  mFamily); 
            if (fSize > 0) {
                mSize = fSize * mRatio;
            }
        }
    }

    
    GFontStyle* GFontStyle::Parse(const char *font, float deviceDensity) {
        Style style = Style::INVALID;
        Weight weight = Weight::INVALID;
        Variant variant = Variant::INVALID;
        std::string outFontFamily = "";
        float fontStyleSize = 0;

        ParseFields(font, style, variant, weight, fontStyleSize, outFontFamily);
        if (style == Style::INVALID || weight == Weight::INVALID || variant == Variant::INVALID 
            || outFontFamily.empty() || fontStyleSize == 0) {
            return nullptr;
        }

        std::string formatFont = font;
        std::transform(formatFont.begin(), formatFont.end(), formatFont.begin(), ::tolower);
        GFontStyle* ret = new GFontStyle(formatFont.data(), style, variant, weight, fontStyleSize, outFontFamily, deviceDensity);
        return ret;
    }


    /**
     * [font-style] [font-variant] [font-weight] (font-size) (font-family)
     * font-size, font-family is required, If one of the other values is missing, their default value are used
     * default table:
     * font-style: normal (normal|italic|oblique)
     * font-variant: normal (normal|small-caps)
     * font-weight: normal (normal|bold|bolder|lighter|100|200|300|400|500|600|700|800|900)
     */
    void GFontStyle::ParseFields(const char *font, Style& style, Variant& variant, Weight& weight, 
        float& fontStyleSize, std::string& outFontFamily) {
        std::string newFont(font);
        GStrSeparator sepStr;
        short ct = sepStr.SepStrBySpace((char *) newFont.c_str(), 5);
        char **parts = sepStr.GetSepArray();
        int index = 0;

        if (ct >= 2) {
            // parse fontStyle & fontSize
            std::string fontSize = parts[ct - 2];
            std::transform(fontSize.begin(), fontSize.end(), fontSize.begin(), ::tolower);
            auto pos = fontSize.find("px");
            if (pos != std::string::npos) {
                if (pos == fontSize.length() - 2) {
                    char *endPtr = nullptr;
                    fontStyleSize = strtof(fontSize.data(), &endPtr);
                }
            }
            pos = fontSize.find("pt");
            if (pos != std::string::npos) {
                if (pos == (fontSize.length() - 2)) {
                    char *endPtr = nullptr;
                    fontStyleSize = strtof(fontSize.data(), &endPtr) * 4 / 3;
                }
            }

            std::string fontFamily = parts[ct - 1];
            if (fontFamily.length() > 0) {
                fontFamily.erase(std::remove(fontFamily.begin(), fontFamily.end(), '\"'),
                                    fontFamily.end());
                std::transform(fontFamily.begin(), fontFamily.end(), fontFamily.begin(),
                                ::tolower);
                // 不支持 initial|inherit|default, 排查特殊符号
                if (fontFamily != "initial" && fontFamily != "inherit" && fontFamily != "default"
                   && fontFamily.find("{") == std::string::npos && fontFamily.find("{") == std::string::npos) {
                    outFontFamily = fontFamily;
                }
            }


            // parse font-weight
            if (ct >= 3) {
                std::string weightStr = parts[ct - 3];
                std::transform(weightStr.begin(), weightStr.end(), weightStr.begin(), ::tolower);
                if (weightStr == "normal" ||  weightStr == "400") {
                    weight = Weight::NORMAL;
                } else if (weightStr == "bold" || weightStr == "700") {
                    weight = Weight::BOLD;
                } else if (weightStr == "bolder") {
                    weight = Weight::BOLDER;
                } else if (weightStr == "lighter") {
                    weight = Weight::LIGHTER;
                } else if (weightStr == "100") {
                    weight = Weight::THIN;
                } else if (weightStr == "200") {
                    weight = Weight::EXTRA_LIGHT;
                } else if (weightStr == "300") {
                    weight = Weight::LIGHT;
                } else if (weightStr == "500") {
                    weight = Weight::MEDIUM;
                } else if (weightStr == "600") {
                    weight = Weight::SEMI_BOLD;
                } else if (weightStr == "800") {
                    weight = Weight::EXTRA_BOLD;
                } else if (weightStr == "900") {
                    weight = Weight::BLACK;
                } else {
                    weight = Weight::INVALID;
                }
            } else {
                weight = Weight::NORMAL;
            }
             

            // parse variant
            if (ct >= 4) {
                std::string variantStr = parts[ct - 4];
                std::transform(variantStr.begin(), variantStr.end(), variantStr.begin(), ::tolower);
                if (variantStr == "normal") {
                    variant = Variant::NORMAL;
                } else if (variantStr == "small-caps") {
                    variant = Variant::SMALL_CAPS;
                } else {
                    variant = Variant::INVALID;
                }
            } else {
                variant = Variant::NORMAL;
            }

            // parse style
            if (ct == 5) {
                std::string styleStr = parts[ct - 5];
                std::transform(styleStr.begin(), styleStr.end(), styleStr.begin(), ::tolower);
                if (styleStr == "normal") {
                    style = Style::NORMAL;
                } else if (styleStr == "oblique") {
                    style = Style::OBLIQUE;
                } else if (styleStr == "italic") {
                    style = Style::ITALIC;
                } else {
                    style = Style::INVALID;
                }
            } else {
                style = Style::NORMAL;
            }
        }
    }


}

