/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */

#include "GSystemFontInformation.h"
#include "GCanvas2dContext.h"
#include "support/Log.h"

namespace gcanvas
{
SystemFontInformation SystemFontInformation::sSystemFontInformation;

SystemFontInformation::SystemFontInformation()
    : mDefaultFontFile(nullptr), mSystemFontLocation(nullptr)
{
}

SystemFontInformation::~SystemFontInformation()
{
    if (this->mDefaultFontFile != nullptr)
    {
        delete[] this->mDefaultFontFile;
        this->mDefaultFontFile = nullptr;
    }

    if (this->mSystemFontLocation != nullptr)
    {
        delete[] this->mSystemFontLocation;
        this->mSystemFontLocation = nullptr;
    }

    for (auto it = this->mFontFamilies.begin();
         it != this->mFontFamilies.end(); ++it)
    {
        delete[]((*it).first);
    }
    for (auto it = this->mFallbackFontFileList.begin();
         it != this->mFallbackFontFileList.end(); ++it)
    {
        // delete font name
        delete[](*it);
    }

    this->mFontFamilies.clear();
    this->mFallbackFontFileList.clear();

    for (auto it = this->mFontFiles.begin(); it != this->mFontFiles.end();
         ++it)
    {
        // delete font files
        delete[]((*it).first);
    }
    this->mFontFiles.clear();
}

void SystemFontInformation::SetDefaultFontFile(const char *fontFile)
{
    if (fontFile == mDefaultFontFile)
    {
        return;
    }

    if (mDefaultFontFile != nullptr)
    {
        delete[] mDefaultFontFile;
        mDefaultFontFile = nullptr;
    }

    if (fontFile != nullptr)
    {
        int length = strlen(fontFile);
        mDefaultFontFile = new char[length + 1];
        strcpy(mDefaultFontFile, fontFile);
    }
}

void SystemFontInformation::SetSystemFontLocation(
    const char *systemFontLocation)
{
    if (systemFontLocation == mSystemFontLocation)
    {
        return;
    }

    if (mSystemFontLocation != nullptr)
    {
        delete[] mSystemFontLocation;
        mSystemFontLocation = nullptr;
    }

    if (systemFontLocation != nullptr)
    {
        int length = strlen(systemFontLocation);
        mSystemFontLocation = new char[length + 1];
        strcpy(mSystemFontLocation, systemFontLocation);
    }
}

bool SystemFontInformation::InsertFontFamily(
    const char *fontName, std::list< const char * > &fontFileList)
{
    if (fontName == nullptr)
    {
        return false;
    }

    auto fontListPtr = FindFontFamily(fontName);
    if (fontListPtr != nullptr)
    {
        return false;
    }

    int length = strlen(fontName);
    char *newFontName = new char[length + 1];
    strcpy(newFontName, fontName);

    std::list< const char * > newFontFileList;
    for (auto it = fontFileList.begin(); it != fontFileList.end(); ++it)
    {
        int strLength = strlen(*it);
        char *newFontFile = new char[strLength + 1];
        strcpy(newFontFile, *it);

        auto font_file_it = this->mFontFiles.find(newFontFile);
        if (font_file_it != this->mFontFiles.end())
        {
            delete[] newFontFile;
            newFontFileList.push_back((*font_file_it).second);
        }
        else
        {
            this->mFontFiles.insert(std::pair< const char *, const char * >(
                newFontFile, newFontFile));

            newFontFileList.push_back(newFontFile);
        }
    }

    GFontFamily fontFamily(newFontFileList);

    mFontFamilies.insert(
        std::pair< const char *, GFontFamily >(newFontName, fontFamily));

    return true;
}

void SystemFontInformation::InsertFallbackFontFamily(
    std::list< const char * > &fontFileList)
{
    for (auto it = fontFileList.begin(); it != fontFileList.end();
         it++)
    {
        int length = strlen(*it);
        char *newFontFile = new char[length + 1];
        strcpy(newFontFile, *it);
        this->mFallbackFontFileList.push_back(newFontFile);
    }
}

GFontFamily *SystemFontInformation::FindFontFamily(const char *fontName)
{
    if (fontName == nullptr)
    {
        return nullptr;
    }

    auto it = mFontFamilies.find(fontName);

    if (it == mFontFamilies.end())
    {
        return nullptr;
    }
    else
    {
        return &((*it).second);
    }
}

char *SystemFontInformation::GetClosestFontFamily(
    GCanvasContext *context, const char *currentFontLocation,
    const wchar_t charCode, const float size, GFontStyle &fontStyle)
{
    const char *currentFontFile = nullptr;
    std::string fontFileFullPath = currentFontLocation;
    std::string fontFileName = fontFileFullPath;
    GFontFamily *family;

    for (auto it = this->mFontFamilies.begin();
         it != this->mFontFamilies.end(); ++it)
    {
        family = &((*it).second);
        currentFontFile = family->MatchFamilyStyle(fontStyle);
        if (currentFontFile != nullptr)
        {
            fontFileName = fontFileFullPath + currentFontFile;
            if (context->IsGlyphExistedInFont(charCode, size, fontFileName))
            {
                break;
            }
            else
            {
                currentFontFile = nullptr;
            }
        }
    }

    if (currentFontFile == nullptr)
    {
        auto it = this->mFallbackFontFileList.begin();
        while (it != this->mFallbackFontFileList.end())
        {
            currentFontFile = *it;
            if (currentFontFile != nullptr)
            {
                fontFileName = fontFileFullPath + currentFontFile;
                if (context->IsGlyphExistedInFont(charCode, size, fontFileName))
                {
                    break;
                }
                else
                {
                    currentFontFile = nullptr;
                }
            }
            ++it;
        }
    }

    if (currentFontFile == nullptr)
    {
        auto it = this->mFontFamilies.begin();
        family = &((*it).second);
        return family->GetProperFontFile();
    }

    return (char *)currentFontFile;
}
}
