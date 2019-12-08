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
#include <string.h>
#include <algorithm>
#include <sstream>

namespace gcanvas {
    GFontStyle::GFontStyle(const char *font, float ratio) : mFontName(font ? font : "") {
        if (font != nullptr) {
            mFullFontStyle = font;
        } else {
            mFullFontStyle = "";
        }

        mStyle = Style::NORMAL;
        mVariant = Variant::NORMAL;
        mWeight = Weight::NORMAL;
        mSize = 12 * ratio;
        mRatio = ratio;
        mFamily = "sans-serif";
        mAscender = -1;
        mDescender = -1;

        if (font != nullptr && mFontName.length() > 0) {
            Initialize(font);
        }
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

        std::string newFont(font);
        GStrSeparator sepStr;
        short ct = sepStr.SepStrBySpace((char *) newFont.c_str(), 5);
        char **parts = sepStr.GetSepArray();
        int index = 0;

        while (index < ct) {
            std::string curStr = parts[index];
            std::transform(curStr.begin(), curStr.end(), curStr.begin(), ::tolower);
            const char *curCStr = curStr.c_str();

            if (strcmp("normal", curCStr) == 0 ||
                strcmp("400", curCStr) == 0) {
                // "normal" should be skipped in any case
            } else if (strcmp("oblique", curCStr) == 0) {
                mStyle = Style::OBLIQUE;
            } else if (strcmp("italic", curCStr) == 0) {
                mStyle = Style::ITALIC;
            } else if (strcmp("small-caps", curCStr) == 0) {
                mVariant = Variant::SMALL_CAPS;
            } else if (strcmp("bold", curCStr) == 0 ||
                       strcmp("700", curCStr) == 0) {
                mWeight = Weight::BOLD;
            } else if (strcmp("bolder", curCStr) == 0) {
                mWeight = Weight::BOLDER;
            } else if (strcmp("lighter", curCStr) == 0) {
                mWeight = Weight::LIGHTER;
            } else if (strcmp("100", curCStr) == 0) {
                mWeight = Weight::THIN;
            } else if (strcmp("200", curCStr) == 0) {
                mWeight = Weight::EXTRA_LIGHT;
            } else if (strcmp("300", curCStr) == 0) {
                mWeight = Weight::LIGHT;
            } else if (strcmp("500", curCStr) == 0) {
                mWeight = Weight::MEDIUM;
            } else if (strcmp("600", curCStr) == 0) {
                mWeight = Weight::SEMI_BOLD;
            } else if (strcmp("800", curCStr) == 0) {
                mWeight = Weight::EXTRA_BOLD;
            } else if (strcmp("900", curCStr) == 0) {
                mWeight = Weight::BLACK;
            } else {
                std::string fontFamily = "";
                std::string fontSize(curCStr);
                auto pos = fontSize.find("px");
                if (pos != std::string::npos) {
                    auto len = strlen(curCStr);
                    if (pos == (len - 2)) {
                        char *endPtr = nullptr;
                        mSize = strtof(curCStr, &endPtr);
                        mSize *= mRatio;

                        if (++index < ct) {
                            fontFamily = parts[index];
                        }
                    }
                }

                pos = fontSize.find("pt");
                if (pos != std::string::npos) {
                    auto len = strlen(curCStr);
                    if (pos == (len - 2)) {
                        char *endPtr = nullptr;
                        mSize = strtof(curCStr, &endPtr) * 4 / 3;
                        mSize *= mRatio;

                        if (++index < ct) {
                            fontFamily = parts[index];
                        }
                    }
                }

                if (fontFamily.length() > 0) {
                    fontFamily.erase(std::remove(fontFamily.begin(), fontFamily.end(), '\"'),
                                     fontFamily.end());
                    std::transform(fontFamily.begin(), fontFamily.end(), fontFamily.begin(),
                                   ::tolower);
                    mFamily = fontFamily;
                }
            }

            ++index;
        }
        if (mSize == 0) {
            mSize = 12 * mRatio;
        }
    }
}
