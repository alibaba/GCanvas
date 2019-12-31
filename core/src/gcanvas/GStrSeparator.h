/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#ifndef GCANVAS_GSTRSEPARATOR_H
#define GCANVAS_GSTRSEPARATOR_H

namespace gcanvas
{
class GStrSeparator
{
public:
    static const short PARSESEPSTRMAXCOUNT = 50;
    static const short PARSELOCALBUFFERSIZE = 480;

    GStrSeparator();
    short SepStrBySpace(char *str, short maxCount = -1);
    short SepStrByCharArray(char *str, const char *byteArray, short byteCount,
                            short maxCount = -1);
    char **GetSepArray() { return (char **)mPointers; }

private:
    char *mPointers[PARSELOCALBUFFERSIZE];
};
}

#endif /* GCANVAS_GSTRSEPARATOR_H */
