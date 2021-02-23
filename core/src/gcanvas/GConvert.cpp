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
#include <sstream>
#include <cstdlib>
#include <unordered_map>
#include "../support/Log.h"
#include <iomanip>

namespace gcanvas {


    void InitColorMapIfEmpty(std::map<std::string, GColorRGBA>& colorMap) {
        if (colorMap.empty()) {
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "black", {{0.0f / 255, 0.0f / 255, 0.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "silver", {{192.0f / 255, 192.0f / 255, 192.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "gray", {{128.0f / 255, 128.0f / 255, 128.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "white", {{255.0f / 255, 255.0f / 255, 255.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "maroon", {{128.0f / 255, 0.0f / 255, 0.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "red", {{255.0f / 255, 0.0f / 255, 0.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "purple", {{128.0f / 255, 0.0f / 255, 128.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "fuchsia", {{255.0f / 255, 0.0f / 255, 255.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "green", {{0.0f / 255, 128.0f / 255, 0.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "lime", {{0.0f / 255, 255.0f / 255, 0.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "olive", {{128.0f / 255, 128.0f / 255, 0.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "yellow", {{255.0f / 255, 255.0f / 255, 0.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "navy", {{0.0f / 255, 0.0f / 255, 128.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "blue", {{0.0f / 255, 0.0f / 255, 255.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "teal", {{0.0f / 255, 128.0f / 255, 128.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "aqua", {{0.0f / 255, 255.0f / 255, 255.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "aliceblue", {{240.0f / 255, 248.0f / 255, 255.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "antiquewhite",
                    {{250.0f / 255, 235.0f / 255, 215.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "aqua", {{0.0f / 255, 255.0f / 255, 255.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "aquamarine", {{127.0f / 255, 255.0f / 255, 212.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "azure", {{240.0f / 255, 255.0f / 255, 255.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "beige", {{245.0f / 255, 245.0f / 255, 220.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "bisque", {{255.0f / 255, 228.0f / 255, 196.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "black", {{0.0f / 255, 0.0f / 255, 0.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "blanchedalmond",
                    {{255.0f / 255, 235.0f / 255, 205.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "blue", {{0.0f / 255, 0.0f / 255, 255.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "blueviolet", {{138.0f / 255, 43.0f / 255, 226.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "brown", {{165.0f / 255, 42.0f / 255, 42.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "burlywood", {{222.0f / 255, 184.0f / 255, 135.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "cadetblue", {{95.0f / 255, 158.0f / 255, 160.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "chartreuse", {{127.0f / 255, 255.0f / 255, 0.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "chocolate", {{210.0f / 255, 105.0f / 255, 30.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "coral", {{255.0f / 255, 127.0f / 255, 80.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "cornflowerblue",
                    {{100.0f / 255, 149.0f / 255, 237.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "cornsilk", {{255.0f / 255, 248.0f / 255, 220.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "crimson", {{220.0f / 255, 20.0f / 255, 60.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "cyan", {{0.0f / 255, 255.0f / 255, 255.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "darkblue", {{0.0f / 255, 0.0f / 255, 139.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "darkcyan", {{0.0f / 255, 139.0f / 255, 139.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "darkgoldenrod",
                    {{184.0f / 255, 134.0f / 255, 11.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "darkgray", {{169.0f / 255, 169.0f / 255, 169.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "darkgreen", {{0.0f / 255, 100.0f / 255, 0.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "darkgrey", {{169.0f / 255, 169.0f / 255, 169.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "darkkhaki", {{189.0f / 255, 183.0f / 255, 107.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "darkmagenta", {{139.0f / 255, 0.0f / 255, 139.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "darkolivegreen",
                    {{85.0f / 255, 107.0f / 255, 47.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "darkorange", {{255.0f / 255, 140.0f / 255, 0.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "darkorchid", {{153.0f / 255, 50.0f / 255, 204.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "darkred", {{139.0f / 255, 0.0f / 255, 0.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "darksalmon", {{233.0f / 255, 150.0f / 255, 122.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "darkseagreen",
                    {{143.0f / 255, 188.0f / 255, 143.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "darkslateblue", {{72.0f / 255, 61.0f / 255, 139.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "darkslategray", {{47.0f / 255, 79.0f / 255, 79.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "darkslategrey", {{47.0f / 255, 79.0f / 255, 79.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "darkturquoise", {{0.0f / 255, 206.0f / 255, 209.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "darkviolet", {{148.0f / 255, 0.0f / 255, 211.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "deeppink", {{255.0f / 255, 20.0f / 255, 147.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "deepskyblue", {{0.0f / 255, 191.0f / 255, 255.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "dimgray", {{105.0f / 255, 105.0f / 255, 105.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "dimgrey", {{105.0f / 255, 105.0f / 255, 105.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "dodgerblue", {{30.0f / 255, 144.0f / 255, 255.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "firebrick", {{178.0f / 255, 34.0f / 255, 34.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "floralwhite", {{255.0f / 255, 250.0f / 255, 240.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "forestgreen", {{34.0f / 255, 139.0f / 255, 34.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "fuchsia", {{255.0f / 255, 0.0f / 255, 255.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "gainsboro", {{220.0f / 255, 220.0f / 255, 220.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "ghostwhite", {{248.0f / 255, 248.0f / 255, 255.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "gold", {{255.0f / 255, 215.0f / 255, 0.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "goldenrod", {{218.0f / 255, 165.0f / 255, 32.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "gray", {{128.0f / 255, 128.0f / 255, 128.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "green", {{0.0f / 255, 128.0f / 255, 0.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "greenyellow", {{173.0f / 255, 255.0f / 255, 47.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "grey", {{128.0f / 255, 128.0f / 255, 128.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "honeydew", {{240.0f / 255, 255.0f / 255, 240.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "hotpink", {{255.0f / 255, 105.0f / 255, 180.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "indianred", {{205.0f / 255, 92.0f / 255, 92.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "indigo", {{75.0f / 255, 0.0f / 255, 130.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "ivory", {{255.0f / 255, 255.0f / 255, 240.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "khaki", {{240.0f / 255, 230.0f / 255, 140.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "lavender", {{230.0f / 255, 230.0f / 255, 250.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "lavenderblush",
                    {{255.0f / 255, 240.0f / 255, 245.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "lawngreen", {{124.0f / 255, 252.0f / 255, 0.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "lemonchiffon",
                    {{255.0f / 255, 250.0f / 255, 205.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "lightblue", {{173.0f / 255, 216.0f / 255, 230.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "lightcoral", {{240.0f / 255, 128.0f / 255, 128.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "lightcyan", {{224.0f / 255, 255.0f / 255, 255.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "lightgoldenrodyellow",
                    {{250.0f / 255, 250.0f / 255, 210.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "lightgray", {{211.0f / 255, 211.0f / 255, 211.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "lightgreen", {{144.0f / 255, 238.0f / 255, 144.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "lightgrey", {{211.0f / 255, 211.0f / 255, 211.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "lightpink", {{255.0f / 255, 182.0f / 255, 193.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "lightsalmon", {{255.0f / 255, 160.0f / 255, 122.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "lightseagreen",
                    {{32.0f / 255, 178.0f / 255, 170.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "lightskyblue",
                    {{135.0f / 255, 206.0f / 255, 250.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "lightslategray",
                    {{119.0f / 255, 136.0f / 255, 153.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "lightslategrey",
                    {{119.0f / 255, 136.0f / 255, 153.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "lightsteelblue",
                    {{176.0f / 255, 196.0f / 255, 222.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "lightyellow", {{255.0f / 255, 255.0f / 255, 224.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "lime", {{0.0f / 255, 255.0f / 255, 0.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "limegreen", {{50.0f / 255, 205.0f / 255, 50.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "linen", {{250.0f / 255, 240.0f / 255, 230.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "magenta", {{255.0f / 255, 0.0f / 255, 255.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "maroon", {{128.0f / 255, 0.0f / 255, 0.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "mediumaquamarine",
                    {{102.0f / 255, 205.0f / 255, 170.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "mediumblue", {{0.0f / 255, 0.0f / 255, 205.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "mediumorchid", {{186.0f / 255, 85.0f / 255, 211.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "mediumpurple",
                    {{147.0f / 255, 112.0f / 255, 219.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "mediumseagreen",
                    {{60.0f / 255, 179.0f / 255, 113.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "mediumslateblue",
                    {{123.0f / 255, 104.0f / 255, 238.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "mediumspringgreen",
                    {{0.0f / 255, 250.0f / 255, 154.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "mediumturquoise",
                    {{72.0f / 255, 209.0f / 255, 204.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "mediumvioletred",
                    {{199.0f / 255, 21.0f / 255, 133.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "midnightblue", {{25.0f / 255, 25.0f / 255, 112.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "mintcream", {{245.0f / 255, 255.0f / 255, 250.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "mistyrose", {{255.0f / 255, 228.0f / 255, 225.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "moccasin", {{255.0f / 255, 228.0f / 255, 181.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "navajowhite", {{255.0f / 255, 222.0f / 255, 173.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "navy", {{0.0f / 255, 0.0f / 255, 128.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "oldlace", {{253.0f / 255, 245.0f / 255, 230.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "olive", {{128.0f / 255, 128.0f / 255, 0.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "olivedrab", {{107.0f / 255, 142.0f / 255, 35.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "orange", {{255.0f / 255, 165.0f / 255, 0.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "orangered", {{255.0f / 255, 69.0f / 255, 0.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "orchid", {{218.0f / 255, 112.0f / 255, 214.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "palegoldenrod",
                    {{238.0f / 255, 232.0f / 255, 170.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "palegreen", {{152.0f / 255, 251.0f / 255, 152.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "paleturquoise",
                    {{175.0f / 255, 238.0f / 255, 238.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "palevioletred",
                    {{219.0f / 255, 112.0f / 255, 147.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "papayawhip", {{255.0f / 255, 239.0f / 255, 213.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "peachpuff", {{255.0f / 255, 218.0f / 255, 185.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "peru", {{205.0f / 255, 133.0f / 255, 63.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "pink", {{255.0f / 255, 192.0f / 255, 203.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "plum", {{221.0f / 255, 160.0f / 255, 221.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "powderblue", {{176.0f / 255, 224.0f / 255, 230.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "purple", {{128.0f / 255, 0.0f / 255, 128.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "red", {{255.0f / 255, 0.0f / 255, 0.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "rosybrown", {{188.0f / 255, 143.0f / 255, 143.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "royalblue", {{65.0f / 255, 105.0f / 255, 225.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "saddlebrown", {{139.0f / 255, 69.0f / 255, 19.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "salmon", {{250.0f / 255, 128.0f / 255, 114.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "sandybrown", {{244.0f / 255, 164.0f / 255, 96.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "seagreen", {{46.0f / 255, 139.0f / 255, 87.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "seashell", {{255.0f / 255, 245.0f / 255, 238.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "sienna", {{160.0f / 255, 82.0f / 255, 45.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "silver", {{192.0f / 255, 192.0f / 255, 192.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "skyblue", {{135.0f / 255, 206.0f / 255, 235.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "slateblue", {{106.0f / 255, 90.0f / 255, 205.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "slategray", {{112.0f / 255, 128.0f / 255, 144.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "slategrey", {{112.0f / 255, 128.0f / 255, 144.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "snow", {{255.0f / 255, 250.0f / 255, 250.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "springgreen", {{0.0f / 255, 255.0f / 255, 127.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "steelblue", {{70.0f / 255, 130.0f / 255, 180.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "tan", {{210.0f / 255, 180.0f / 255, 140.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "teal", {{0.0f / 255, 128.0f / 255, 128.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "thistle", {{216.0f / 255, 191.0f / 255, 216.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "tomato", {{255.0f / 255, 99.0f / 255, 71.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "turquoise", {{64.0f / 255, 224.0f / 255, 208.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "violet", {{238.0f / 255, 130.0f / 255, 238.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "wheat", {{245.0f / 255, 222.0f / 255, 179.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "white", {{255.0f / 255, 255.0f / 255, 255.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "whitesmoke", {{245.0f / 255, 245.0f / 255, 245.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "yellow", {{255.0f / 255, 255.0f / 255, 0.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "yellowgreen", {{154.0f / 255, 205.0f / 255, 50.0f / 255, 1.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "transparent", {{0, 0, 0, 0.0f}}));
            colorMap.insert(std::pair<std::string, GColorRGBA>(
                    "transparent_white", {{1.f, 1.f, 1.f, 0.f}}));
        }
    }


    GColorRGBA StrValueToColorRGBA(const char *value) {
        static std::map<std::string, GColorRGBA> colorMap;
        GColorRGBA c = {{0.0f, 0.0f, 0.0f, 1.0f}};
        StrValueToColorRGBA(value, c);
        return c;
    }


    /**
     * support format:
     * normal: rgb(100, 100, 100)
     * percent: rgb(100%, 10%, 10%)
     * no end: rgb(100, 100, 100
     * space seperate: rgb(100 100 100)
     * alpha: rgba(100 100 100 / 0.4)
     */
    bool HandleBraceRGBAColor(char* value, GColorRGBA& output) {
        int length = (int) strlen(value);
        int current = 0;
        std::string temp = "";
        float colorV;

        // separator format
        bool isSpaceSep = false;
        bool isCommaSep = false;
        bool isPercent = false;

        int start = 4;
        if (value[start] == '(') {
            start = 5;
        }

        bool endFlag;
        bool startFlag = false;
        for (int i = start; i < length && current < 4; i++) {
            if (current == 3) { // parse alpha
                // If we have an alpha component, copy the rest of the wide
                // string into a char array and use atof() to parse it.
                char alpha[8] = {0, 0, 0, 0, 0, 0, 0, 0};
                int offset = 0;
                int k = 0;
                char lastChar = '\0';
                for (int j = 0; (i + j) <= length - 1 && j <= 7; j++) {
                    offset = i + j;
                    if (isdigit(value[offset]) || value[offset] == '.' || value[offset] == '%' || value[offset] == '-') {
                        alpha[k] = value[i + j];
                        lastChar = alpha[k];
                        k++;
                    }
                }
                if (alpha[0] == '\0') { // wrong format
                    return false;
                }

                size_t ret;
                std::stof(alpha, &ret);

                double d = atof(alpha);
                if (lastChar == '%') {
                    d = d / 100;
                }
                if (d > 1) {
                    d = 1;
                }
                output.components[current] = d;
                current ++;
                // LOG_E("HandleBraceRGBAColor(alpha):str=%s, value=%f", alpha, d);
            } else { // not alpha
                // LOG_E("iterate value[%i]=%i", i, value[i]);
                if (isdigit(value[i]) || value[i] == '.' || value[i] == '%' || value[i] == '-' || value[i] == '+') {
                    // LOG_E("set start=true push back %c", value[i]);
                    if (!startFlag) {
                        startFlag = true;
                    }
                    // reset not end
                    endFlag = false;
                    temp.push_back(value[i]);
                } else if (value[i] == ',' || value[i] == ')' || isspace(value[i])) { // end
                    // LOG_E("is , ) or space: c=%c, set end=true", value[i]);
                    endFlag = true;
                } else { // wrong format
                    // LOG_E("wrong digit: %c, exit", value[i]);
                    return false;
                }

                if (i == length - 1) {
                    endFlag = true;
                }

                if (startFlag && endFlag) {
                    // check is all percent?
                    char lastChar = temp.at(temp.length() - 1);
                    if (current == 0) {
                        if (value[i] == ',') {
                            isCommaSep = true;
                        } else if (isspace(value[i])) {
                            isSpaceSep = true;
                        }
                        if (lastChar == '%') {
                            isPercent = true;
                        }
                    } else {
                        if (lastChar != '%' && isPercent) { // not percent consistent
                            return false;
                        }
                        if (value[i] == ',' && !isCommaSep) { // not comma  consistent
                            return false;
                        }
                        if (isspace(value[i]) && !isSpaceSep) { // not space consistent
                            return false;
                        }
                    }

                    colorV = (float)atof(temp.data());
                    if (isPercent) {
                        colorV = colorV < 0 ? 0 : (colorV > 100 ? 100 : colorV);
                        output.components[current] = colorV / 100;
                    } else {
                        colorV = colorV < 0 ? 0 : (colorV > 255 ? 255 : colorV);
                        output.components[current] = colorV / 255.0f;
                    }

                    //LOG_E("HandleBraceRGBAColor(RGB): temp=%s, current=%i, result=%f",
                    //      temp.data(), current, output.components[current]);
                    current++;
                    temp = "";
                    startFlag = false;
                    endFlag = false;
                }
            }
        }

        if (current < 3) { // not match count, at least include r,g,b all
            return false;
        }
        return true;
    }


    GColorRGBA ParseRGBAFullString(char value[8]) {
        GColorRGBA ret;
        ParseRGBAFullString(value, ret);
        return ret;
    }


    void ParseRGBAFullString(char value[8], GColorRGBA& output) {
        unsigned long hex = 0x00000000 | strtoul(value, nullptr, 16);
        output.rgba = {(hex >> 24) / 255.0f, ((hex & 0xff0000) >> 16) / 255.0f,
                       ((hex & 0xff00) >> 8) / 255.0f, (hex & 0xff) / 255.0f};
    }


    bool StrValueToColorRGBA(const char *value, GColorRGBA& output) {
        static std::map<std::string, GColorRGBA> colorMap;
        InitColorMapIfEmpty(colorMap);

        output = {{0.0f, 0.0f, 0.0f, 1.0f}};
        if (value == nullptr) {
            return false;
        }

        // LOG_E("StrValueToColorRGBA: input=%s", value);
        if (value[0] == '#') {
            int length = (int) strlen(value);
            char str[] = "ffffffff";

            // check value valid?
            for (int i = 1; i < length; i++) {
                if (!((value[i] >= '0' && value[i] <= '9') || (value[i] >= 'a' && value[i] <= 'f')
                    || (value[i] >= 'A' && value[i] <= 'F'))) { // wrong format
                    // LOG_E("check # wrong format: %c=", value[i]);
                    return false;
                }
            }

            if (length == 4) {
                str[0] = str[1] = value[1];
                str[2] = str[3] = value[2];
                str[4] = str[5] = value[3];
                output = ParseRGBAFullString(str);
            } else if (length == 5) {
                str[0] = str[1] = value[1];
                str[2] = str[3] = value[2];
                str[4] = str[5] = value[3];
                str[6] = str[7] = value[4];
                output = ParseRGBAFullString(str);
            } else if (length == 7) { // #RRGGBB format
                str[0] = value[1];
                str[1] = value[2];
                str[2] = value[3];
                str[3] = value[4];
                str[4] = value[5];
                str[5] = value[6];
                output = ParseRGBAFullString(str);
            } else if (length == 9) { // #RRGGBBAA
                memcpy(str, value + 1, 8);
                output = ParseRGBAFullString(str);
            } else {
                return false;
            }
            // LOG_E("StrValueToColorRGBA: input=%s, output=%f %f %f %f", str,
            //        output.components[0], output.components[1], output.components[2], output.components[3]);
            return true;
        } else {
            std::string colorVal = value;
            colorVal = Trim(value);
            // colorVal.erase(std::remove(colorVal.begin(), colorVal.end(), ' '), colorVal.end());
            std::transform(colorVal.begin(), colorVal.end(), colorVal.begin(), ::tolower);
            value = colorVal.c_str();

            // match color name
            auto iter = colorMap.find(value);
            if (iter != colorMap.end()) {
                output = iter->second;
                return true;
            }

            // handle hsla( format color (not support now)
            if (strncmp(value, "hsl(", 4) == 0 || strncmp(value, "hsla(", 5) == 0) {
                return false;
            }

            // handle rgb( | rgba(  format
            if (strncmp(value, "rgb(", 4) == 0 || strncmp(value, "rgba(", 5) == 0) {
                return HandleBraceRGBAColor((char*)value, output);
            }
            // parse error
            return false;
        }
    }


    std::string Trim(const std::string& str, const std::string& whitespace) {
        const auto strBegin = str.find_first_not_of(whitespace);
        if (strBegin == std::string::npos) {
            return ""; // no content
        }

        const auto strEnd = str.find_last_not_of(whitespace);
        const auto strRange = strEnd - strBegin + 1;

        return str.substr(strBegin, strRange);
    }


    std::string ColorToString(const GColorRGBA &color) {
        std::ostringstream ss;
        ss << "rgba(" << (int) (color.rgba.r * 255)
           << "," << (int) (color.rgba.g * 255) << "," << (int) (color.rgba.b * 255)
           << "," << (int) color.rgba.a << ")";
        return ss.str();
    }


    /**
     * convert #RRGGBBAA format
     */
    std::string ColorToRGBASharpString(const GColorRGBA &color) {
        std::ostringstream ss;
        ss << "#" << std::setfill ('0') << std::setw(2) << std::hex << (int) (color.rgba.r * 255)
           << std::setfill ('0') << std::setw(2) << std::hex << (int) (color.rgba.g * 255)
           << std::setfill ('0') << std::setw(2) << std::hex << (int) (color.rgba.b * 255);
        if (color.rgba.a < 1.0f) {
            ss << std::setfill ('0') << std::setw(2) << std::hex << (int) (color.rgba.a * 255);
        }
        return ss.str();
    }


    /**
     * int (RGBA) convert to GColorRGBA
     */
    GColorRGBA IntValueToColorRGBA(int value) {
        if (value == 0) {
            return GColorTransparent;
        }

        GColorRGBA c;
        c.rgba = {((value & 0xff000000) >> 24) / 255.0f, ((value & 0xff0000) >> 16) / 255.0f,
                  ((value & 0xff00) >> 8) / 255.0f, (value & 0xff) / 255.0f };
        return c;
    }


//    /*
//     * H(Hue): 0 - 360 degree (integer)
//     * S(Saturation): 0 - 1.00 (double)
//     * V(Value): 0 - 1.00 (double)
//     *
//     * output[3]: Output, array size 3, int
//     */
//    void HSVtoRGB(int H, double S, double V, int output[3]) {
//        double C = S * V;
//        double X = C * (1 - abs(fmod(H / 60.0, 2) - 1));
//        double m = V - C;
//        double Rs, Gs, Bs;
//
//        if(H >= 0 && H < 60) {
//            Rs = C;
//            Gs = X;
//            Bs = 0;
//        } else if(H >= 60 && H < 120) {
//            Rs = X;
//            Gs = C;
//            Bs = 0;
//        } else if(H >= 120 && H < 180) {
//            Rs = 0;
//            Gs = C;
//            Bs = X;
//        } else if(H >= 180 && H < 240) {
//            Rs = 0;
//            Gs = X;
//            Bs = C;
//        } else if(H >= 240 && H < 300) {
//            Rs = X;
//            Gs = 0;
//            Bs = C;
//        } else {
//            Rs = C;
//            Gs = 0;
//            Bs = X;
//        }
//
//        output[0] = (int)((Rs + m) * 255);
//        output[1] = (int)((Gs + m) * 255);
//        output[2] = (int)((Bs + m) * 255);
//    }


}
