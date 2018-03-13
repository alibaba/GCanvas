/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#ifndef __GCanvas__GGLProgramCache__
#define __GCanvas__GGLProgramCache__

#include <iostream>
#include <map>

class GShader;

class GShaderManager
{
public:
#ifdef IOS
    static GShaderManager *getSingleton();
    static void release();
#endif
    void addProgram(const std::string &key, GShader *program);
    GShader *programForKey(const std::string &key);

#ifdef IOS
private:
    GShaderManager();
    ~GShaderManager();
#endif

#ifdef ANDROID
    GShaderManager();
    ~GShaderManager();
private:
#endif
    void loadDefaultShaders();

    std::map< std::string, GShader * > mProgramCaches;
    static GShaderManager *mShaderManager;
};

#endif
