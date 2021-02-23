/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */

#ifndef GCANVAS_GCONVERT_H
#define GCANVAS_GCONVERT_H

#include "export.h"
#include "GPoint.h"

#define GColorTransparent       GColorRGBA({0,0,0,0})
#define GColorTransparentWhite  GColorRGBA({1.0,1.0,1.0,0})
#define GColorWhite             GColorRGBA({1.0,1.0,1.0,1.0})
namespace gcanvas{


    API_EXPORT GColorRGBA StrValueToColorRGBA(const char *value);


    API_EXPORT bool StrValueToColorRGBA(const char *value, GColorRGBA& output);


    API_EXPORT GColorRGBA ParseRGBAFullString(char value[8]);

    API_EXPORT void ParseRGBAFullString(char value[8], GColorRGBA&);


    API_EXPORT GColorRGBA IntValueToColorRGBA(int value);


    API_EXPORT std::string ColorToString(const GColorRGBA &color);

    API_EXPORT std::string ColorToRGBASharpString(const GColorRGBA &color);


    std::string Trim(const std::string& str, const std::string& whitespace = " \t");

}


#endif /* GCANVAS_GCONVERT_H */
