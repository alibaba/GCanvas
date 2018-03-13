/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#ifndef __GCANVAS_FILEUTILS_H__
#define __GCANVAS_FILEUTILS_H__

#include <string>
#include <vector>
#include <map>
#include "Value.h"

class FileUtils
{
public:
    static FileUtils *GetInstance();
    static void DestroyInstance();

    virtual ~FileUtils();

    virtual unsigned char *GetFileData(const std::string &filename,
                                       const char *mode, size_t *size);
    virtual size_t WriteFileData(const std::string &filename, const char *data,
                              size_t size);
    virtual std::string fullPathForFilename(const std::string &filename);

    virtual bool isAbsolutePath(const std::string &path) const;

    const std::map< std::string, std::string > &GetFullPathCache() const
    {
        return mFullPathCache;
    }

protected:

    FileUtils();

    virtual std::string getNewFilename(const std::string &filename) const;

    virtual std::string
    getPathForFilename(const std::string &filename,
                       const std::string &resolutionRenderer,
                       const std::string &searchPath);

    ValueMap mFilenameLookupDict;
    std::vector< std::string > mSearchResolutionsOrderArray;
    std::vector< std::string > mSearchPathArray;
    std::map< std::string, std::string > mFullPathCache;
    static FileUtils *s_sharedFileUtils;
};

#endif
