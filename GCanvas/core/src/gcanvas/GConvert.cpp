/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#include "GConvert.h"
#include <ctype.h>
#include <map>
#include <cstdlib>

GColorRGBA StrValueToColorRGBA(const char *value)
{
    static std::map< std::string, GColorRGBA > colorMap;
    if (colorMap.empty())
    {
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "black", {{0.0f / 255, 0.0f / 255, 0.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "silver", {{192.0f / 255, 192.0f / 255, 192.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "gray", {{128.0f / 255, 128.0f / 255, 128.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "white", {{255.0f / 255, 255.0f / 255, 255.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "maroon", {{128.0f / 255, 0.0f / 255, 0.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "red", {{255.0f / 255, 0.0f / 255, 0.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "purple", {{128.0f / 255, 0.0f / 255, 128.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "fuchsia", {{255.0f / 255, 0.0f / 255, 255.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "green", {{0.0f / 255, 128.0f / 255, 0.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "lime", {{0.0f / 255, 255.0f / 255, 0.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "olive", {{128.0f / 255, 128.0f / 255, 0.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "yellow", {{255.0f / 255, 255.0f / 255, 0.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "navy", {{0.0f / 255, 0.0f / 255, 128.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "blue", {{0.0f / 255, 0.0f / 255, 255.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "teal", {{0.0f / 255, 128.0f / 255, 128.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "aqua", {{0.0f / 255, 255.0f / 255, 255.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "aliceblue", {{240.0f / 255, 248.0f / 255, 255.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "antiquewhite",
            {{250.0f / 255, 235.0f / 255, 215.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "aqua", {{0.0f / 255, 255.0f / 255, 255.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "aquamarine", {{127.0f / 255, 255.0f / 255, 212.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "azure", {{240.0f / 255, 255.0f / 255, 255.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "beige", {{245.0f / 255, 245.0f / 255, 220.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "bisque", {{255.0f / 255, 228.0f / 255, 196.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "black", {{0.0f / 255, 0.0f / 255, 0.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "blanchedalmond",
            {{255.0f / 255, 235.0f / 255, 205.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "blue", {{0.0f / 255, 0.0f / 255, 255.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "blueviolet", {{138.0f / 255, 43.0f / 255, 226.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "brown", {{165.0f / 255, 42.0f / 255, 42.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "burlywood", {{222.0f / 255, 184.0f / 255, 135.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "cadetblue", {{95.0f / 255, 158.0f / 255, 160.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "chartreuse", {{127.0f / 255, 255.0f / 255, 0.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "chocolate", {{210.0f / 255, 105.0f / 255, 30.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "coral", {{255.0f / 255, 127.0f / 255, 80.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "cornflowerblue",
            {{100.0f / 255, 149.0f / 255, 237.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "cornsilk", {{255.0f / 255, 248.0f / 255, 220.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "crimson", {{220.0f / 255, 20.0f / 255, 60.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "cyan", {{0.0f / 255, 255.0f / 255, 255.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "darkblue", {{0.0f / 255, 0.0f / 255, 139.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "darkcyan", {{0.0f / 255, 139.0f / 255, 139.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "darkgoldenrod",
            {{184.0f / 255, 134.0f / 255, 11.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "darkgray", {{169.0f / 255, 169.0f / 255, 169.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "darkgreen", {{0.0f / 255, 100.0f / 255, 0.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "darkgrey", {{169.0f / 255, 169.0f / 255, 169.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "darkkhaki", {{189.0f / 255, 183.0f / 255, 107.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "darkmagenta", {{139.0f / 255, 0.0f / 255, 139.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "darkolivegreen",
            {{85.0f / 255, 107.0f / 255, 47.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "darkorange", {{255.0f / 255, 140.0f / 255, 0.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "darkorchid", {{153.0f / 255, 50.0f / 255, 204.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "darkred", {{139.0f / 255, 0.0f / 255, 0.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "darksalmon", {{233.0f / 255, 150.0f / 255, 122.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "darkseagreen",
            {{143.0f / 255, 188.0f / 255, 143.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "darkslateblue", {{72.0f / 255, 61.0f / 255, 139.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "darkslategray", {{47.0f / 255, 79.0f / 255, 79.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "darkslategrey", {{47.0f / 255, 79.0f / 255, 79.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "darkturquoise", {{0.0f / 255, 206.0f / 255, 209.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "darkviolet", {{148.0f / 255, 0.0f / 255, 211.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "deeppink", {{255.0f / 255, 20.0f / 255, 147.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "deepskyblue", {{0.0f / 255, 191.0f / 255, 255.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "dimgray", {{105.0f / 255, 105.0f / 255, 105.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "dimgrey", {{105.0f / 255, 105.0f / 255, 105.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "dodgerblue", {{30.0f / 255, 144.0f / 255, 255.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "firebrick", {{178.0f / 255, 34.0f / 255, 34.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "floralwhite", {{255.0f / 255, 250.0f / 255, 240.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "forestgreen", {{34.0f / 255, 139.0f / 255, 34.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "fuchsia", {{255.0f / 255, 0.0f / 255, 255.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "gainsboro", {{220.0f / 255, 220.0f / 255, 220.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "ghostwhite", {{248.0f / 255, 248.0f / 255, 255.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "gold", {{255.0f / 255, 215.0f / 255, 0.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "goldenrod", {{218.0f / 255, 165.0f / 255, 32.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "gray", {{128.0f / 255, 128.0f / 255, 128.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "green", {{0.0f / 255, 128.0f / 255, 0.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "greenyellow", {{173.0f / 255, 255.0f / 255, 47.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "grey", {{128.0f / 255, 128.0f / 255, 128.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "honeydew", {{240.0f / 255, 255.0f / 255, 240.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "hotpink", {{255.0f / 255, 105.0f / 255, 180.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "indianred", {{205.0f / 255, 92.0f / 255, 92.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "indigo", {{75.0f / 255, 0.0f / 255, 130.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "ivory", {{255.0f / 255, 255.0f / 255, 240.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "khaki", {{240.0f / 255, 230.0f / 255, 140.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "lavender", {{230.0f / 255, 230.0f / 255, 250.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "lavenderblush",
            {{255.0f / 255, 240.0f / 255, 245.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "lawngreen", {{124.0f / 255, 252.0f / 255, 0.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "lemonchiffon",
            {{255.0f / 255, 250.0f / 255, 205.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "lightblue", {{173.0f / 255, 216.0f / 255, 230.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "lightcoral", {{240.0f / 255, 128.0f / 255, 128.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "lightcyan", {{224.0f / 255, 255.0f / 255, 255.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "lightgoldenrodyellow",
            {{250.0f / 255, 250.0f / 255, 210.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "lightgray", {{211.0f / 255, 211.0f / 255, 211.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "lightgreen", {{144.0f / 255, 238.0f / 255, 144.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "lightgrey", {{211.0f / 255, 211.0f / 255, 211.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "lightpink", {{255.0f / 255, 182.0f / 255, 193.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "lightsalmon", {{255.0f / 255, 160.0f / 255, 122.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "lightseagreen",
            {{32.0f / 255, 178.0f / 255, 170.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "lightskyblue",
            {{135.0f / 255, 206.0f / 255, 250.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "lightslategray",
            {{119.0f / 255, 136.0f / 255, 153.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "lightslategrey",
            {{119.0f / 255, 136.0f / 255, 153.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "lightsteelblue",
            {{176.0f / 255, 196.0f / 255, 222.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "lightyellow", {{255.0f / 255, 255.0f / 255, 224.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "lime", {{0.0f / 255, 255.0f / 255, 0.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "limegreen", {{50.0f / 255, 205.0f / 255, 50.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "linen", {{250.0f / 255, 240.0f / 255, 230.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "magenta", {{255.0f / 255, 0.0f / 255, 255.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "maroon", {{128.0f / 255, 0.0f / 255, 0.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "mediumaquamarine",
            {{102.0f / 255, 205.0f / 255, 170.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "mediumblue", {{0.0f / 255, 0.0f / 255, 205.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "mediumorchid", {{186.0f / 255, 85.0f / 255, 211.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "mediumpurple",
            {{147.0f / 255, 112.0f / 255, 219.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "mediumseagreen",
            {{60.0f / 255, 179.0f / 255, 113.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "mediumslateblue",
            {{123.0f / 255, 104.0f / 255, 238.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "mediumspringgreen",
            {{0.0f / 255, 250.0f / 255, 154.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "mediumturquoise",
            {{72.0f / 255, 209.0f / 255, 204.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "mediumvioletred",
            {{199.0f / 255, 21.0f / 255, 133.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "midnightblue", {{25.0f / 255, 25.0f / 255, 112.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "mintcream", {{245.0f / 255, 255.0f / 255, 250.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "mistyrose", {{255.0f / 255, 228.0f / 255, 225.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "moccasin", {{255.0f / 255, 228.0f / 255, 181.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "navajowhite", {{255.0f / 255, 222.0f / 255, 173.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "navy", {{0.0f / 255, 0.0f / 255, 128.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "oldlace", {{253.0f / 255, 245.0f / 255, 230.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "olive", {{128.0f / 255, 128.0f / 255, 0.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "olivedrab", {{107.0f / 255, 142.0f / 255, 35.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "orange", {{255.0f / 255, 165.0f / 255, 0.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "orangered", {{255.0f / 255, 69.0f / 255, 0.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "orchid", {{218.0f / 255, 112.0f / 255, 214.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "palegoldenrod",
            {{238.0f / 255, 232.0f / 255, 170.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "palegreen", {{152.0f / 255, 251.0f / 255, 152.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "paleturquoise",
            {{175.0f / 255, 238.0f / 255, 238.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "palevioletred",
            {{219.0f / 255, 112.0f / 255, 147.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "papayawhip", {{255.0f / 255, 239.0f / 255, 213.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "peachpuff", {{255.0f / 255, 218.0f / 255, 185.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "peru", {{205.0f / 255, 133.0f / 255, 63.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "pink", {{255.0f / 255, 192.0f / 255, 203.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "plum", {{221.0f / 255, 160.0f / 255, 221.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "powderblue", {{176.0f / 255, 224.0f / 255, 230.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "purple", {{128.0f / 255, 0.0f / 255, 128.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "red", {{255.0f / 255, 0.0f / 255, 0.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "rosybrown", {{188.0f / 255, 143.0f / 255, 143.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "royalblue", {{65.0f / 255, 105.0f / 255, 225.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "saddlebrown", {{139.0f / 255, 69.0f / 255, 19.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "salmon", {{250.0f / 255, 128.0f / 255, 114.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "sandybrown", {{244.0f / 255, 164.0f / 255, 96.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "seagreen", {{46.0f / 255, 139.0f / 255, 87.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "seashell", {{255.0f / 255, 245.0f / 255, 238.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "sienna", {{160.0f / 255, 82.0f / 255, 45.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "silver", {{192.0f / 255, 192.0f / 255, 192.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "skyblue", {{135.0f / 255, 206.0f / 255, 235.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "slateblue", {{106.0f / 255, 90.0f / 255, 205.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "slategray", {{112.0f / 255, 128.0f / 255, 144.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "slategrey", {{112.0f / 255, 128.0f / 255, 144.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "snow", {{255.0f / 255, 250.0f / 255, 250.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "springgreen", {{0.0f / 255, 255.0f / 255, 127.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "steelblue", {{70.0f / 255, 130.0f / 255, 180.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "tan", {{210.0f / 255, 180.0f / 255, 140.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "teal", {{0.0f / 255, 128.0f / 255, 128.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "thistle", {{216.0f / 255, 191.0f / 255, 216.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "tomato", {{255.0f / 255, 99.0f / 255, 71.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "turquoise", {{64.0f / 255, 224.0f / 255, 208.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "violet", {{238.0f / 255, 130.0f / 255, 238.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "wheat", {{245.0f / 255, 222.0f / 255, 179.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "white", {{255.0f / 255, 255.0f / 255, 255.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "whitesmoke", {{245.0f / 255, 245.0f / 255, 245.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "yellow", {{255.0f / 255, 255.0f / 255, 0.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "yellowgreen", {{154.0f / 255, 205.0f / 255, 50.0f / 255, 1.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "transparent", {{0, 0, 0, 0.0f}}));
        colorMap.insert(std::pair< std::string, GColorRGBA >(
            "transparent_white", {{1.f, 1.f, 1.f, 0.f}}));
    }

    GColorRGBA c = {{0.0f, 0.0f, 0.0f, 1.0f}};
    if (value == nullptr)
    {
        return c;
    }

    std::map< std::string, GColorRGBA >::const_iterator iter =
        colorMap.find(value);
    if (iter != colorMap.end())
    {
        return iter->second;
    }

    int length = (int)strlen(value);
    char str[] = "ffffff";

    // #f0f format
    if (length == 4)
    {
        str[0] = str[1] = value[3];
        str[2] = str[3] = value[2];
        str[4] = str[5] = value[1];
        unsigned int hex =
            (unsigned int)(0x00000000 | strtol(str, nullptr, 16));
        c.rgba = {(hex & 0xff) / 255.0f, ((hex & 0xffff) >> 8) / 255.0f,
                  (hex >> 16) / 255.0f, 1.0};
    }
    // #ff00ff format
    else if (length == 7)
    {
        str[0] = value[5];
        str[1] = value[6];
        str[2] = value[3];
        str[3] = value[4];
        str[4] = value[1];
        str[5] = value[2];
        unsigned int hex =
            (unsigned int)(0x00000000 | strtol(str, nullptr, 16));
        c.rgba = {(hex & 0xff) / 255.0f, ((hex & 0xffff) >> 8) / 255.0f,
                  (hex >> 16) / 255.0f, 1.0};
    }

    // assume rgb(255,0,255) or rgba(255,0,255,0.5) format
    else
    {
        int current = 0;
        for (int i = 4; i < length && current < 4; i++)
        {
            if (current == 3)
            {
                // If we have an alpha component, copy the rest of the wide
                // string into a char array and use atof() to parse it.
                char alpha[8] = {0, 0, 0, 0, 0, 0, 0, 0};
                for (int j = 0; i + j < length - 1 && j < 7; j++)
                {
                    alpha[j] = value[i + j];
                }
                c.components[current] = atof(alpha);
                current++;
            }
            else if (isdigit(value[i]))
            {
                c.components[current] =
                    (c.components[current] * 10 + (value[i] - '0'));
            }
            else if (value[i] == ',' || value[i] == ')')
            {
                c.components[current] /= 255.0f;
                current++;
            }
        }
    }

    return c;
}
