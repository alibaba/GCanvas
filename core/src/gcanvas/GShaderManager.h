/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#ifndef GCANVAS_GSHADERMANAGER_H
#define GCANVAS_GSHADERMANAGER_H

#include <iostream>
#include <map>

class GShader;

class GShaderManager
{
public:

    // Android depend constructor, set it public
    GShaderManager();
    ~GShaderManager();

    static GShaderManager *getSingleton();
    static void release();

    void addProgram(const std::string &key, GShader *program);
    GShader *programForKey(const std::string &key);

private:
    void loadDefaultShaders();

    std::map< std::string, GShader * > mProgramCaches;
    static GShaderManager *mShaderManager;
};

#endif /* GCANVAS_GSHADERMANAGER_H */
