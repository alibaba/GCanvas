/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#ifndef __GCanvas_GSystemFontInformation__
#define __GCanvas_GSystemFontInformation__

#include "GFontFamily.h"
#include "support/Lesser.h"


namespace gcanvas {
    class SystemFontInformation final {
    public:
        SystemFontInformation();

        ~SystemFontInformation();

        API_EXPORT void SetDefaultFontFile(const char *fontFile);

        char *GetDefaultFontFile() { return mDefaultFontFile; }


        API_EXPORT void SetSystemFontLocation(const char *systemFontLocation);


        char *GetSystemFontLocation() { return mSystemFontLocation; }


        API_EXPORT void SetExtraFontLocation(const char *extraFontLocation);

        char *GetExtraFontLocation() { return mExtraFontLocation; }


        API_EXPORT bool InsertFontFamily(const char *fontName,
                                         std::list<const char *> &fontFileList);

        API_EXPORT void InsertFallbackFontFamily(std::list<const char *> &fontFileList);

        API_EXPORT GFontFamily *FindFontFamily(const char *fontName);


        char *GetClosestFontFamily(GCanvasContext *context,
                                   const char *currentFontLocation,
                                   const wchar_t charCode, const float size,
                                   GFontStyle &fontStyle);

        static SystemFontInformation *GetSystemFontInformation() {
            return &sSystemFontInformation;
        }

    private:
        API_EXPORT static SystemFontInformation sSystemFontInformation;

        char *mDefaultFontFile;
        char *mSystemFontLocation;
        char *mExtraFontLocation;
        std::map<const char *, GFontFamily, Lesser> mFontFamilies;
        std::list<const char *> mFallbackFontFileList;
        std::map<const char *, const char *, Lesser> mFontFiles;
    };
}

#endif