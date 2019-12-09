/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#include "FileUtils.h"

#include <stack>
#include <stdlib.h>

FileUtils *FileUtils::s_sharedFileUtils = nullptr;

FileUtils *FileUtils::GetInstance()
{
    if (!s_sharedFileUtils) s_sharedFileUtils = new FileUtils();
    return s_sharedFileUtils;
}

void FileUtils::DestroyInstance() { GC_SAFE_DELETE(s_sharedFileUtils); }

FileUtils::FileUtils() {}

FileUtils::~FileUtils() {}

unsigned char *FileUtils::GetFileData(const std::string &filename,
                                      const char *mode, size_t *size)
{
    unsigned char *buffer = nullptr;
    *size = 0;
    do
    {
        const std::string fullPath = fullPathForFilename(filename);
        FILE *fp = fopen(fullPath.c_str(), mode);
        GC_BREAK_IF(!fp);

        fseek(fp, 0, SEEK_END);
        *size = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        buffer = (unsigned char *)malloc(*size);
        *size = fread(buffer, sizeof(unsigned char), *size, fp);
        fclose(fp);
    } while (0);

    return buffer;
}

size_t FileUtils::WriteFileData(const std::string &filename, const char *data,
                             size_t size)
{
    size_t ret = -1;
    do
    {
        FILE *fp = fopen(filename.c_str(), "wb");
        if (fp == NULL)
        {
            break;
        }

        ret = fwrite(data, size, sizeof(char), fp);

        fclose(fp);
    } while (0);

    return ret;
}

std::string FileUtils::getNewFilename(const std::string &filename) const
{
    std::string newFileName;

    auto iter = mFilenameLookupDict.find(filename);

    if (iter == mFilenameLookupDict.end())
    {
        newFileName = filename;
    }
    else
    {
        newFileName = iter->second.asString();
    }
    return newFileName;
}

std::string
FileUtils::getPathForFilename(const std::string &filename,
                              const std::string &resolutionRenderer,
                              const std::string &searchPath)
{
    std::string file = filename;
    std::string filePath = "";
    size_t pos = filename.find_last_of("/");
    if (pos != std::string::npos)
    {
        filePath = filename.substr(0, pos + 1);
        file = filename.substr(pos + 1);
    }

    std::string path = searchPath;
    path += filePath;
    path += resolutionRenderer;

    return path;
}

std::string FileUtils::fullPathForFilename(const std::string &filename)
{
    if (filename.empty())
    {
        return "";
    }

    if (isAbsolutePath(filename))
    {
        return filename;
    }

    auto cacheIter = mFullPathCache.find(filename);
    if (cacheIter != mFullPathCache.end())
    {
        return cacheIter->second;
    }

    const std::string newFilename(getNewFilename(filename));

    std::string fullPath;

    for (auto searchIt = mSearchPathArray.begin();
         searchIt != mSearchPathArray.end(); ++searchIt)
    {
        for (auto resolutionIt = mSearchResolutionsOrderArray.begin();
             resolutionIt != mSearchResolutionsOrderArray.end(); ++resolutionIt)
        {
            fullPath =
                this->getPathForFilename(newFilename, *resolutionIt, *searchIt);

            if (fullPath.length() > 0)
            {
                mFullPathCache.insert(std::make_pair(filename, fullPath));
                return fullPath;
            }
        }
    }

    return filename;
}

bool FileUtils::isAbsolutePath(const std::string &path) const
{
    return (path[0] == '/');
}
