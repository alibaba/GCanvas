/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#ifndef __GGL_PRECOMPILED_SHADERS_H__
#define __GGL_PRECOMPILED_SHADERS_H__

#include <map>
#include <string>
#include <vector>
#include <GLES2/gl2platform.h>
#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES 1
#endif
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#define SHADER_NAME_PREFIX "s_"

typedef struct _PreCompiledProgram
{
    const char *key;
    const unsigned char *program;
    size_t length;
} PreCompiledProgram;

typedef struct _CompiledProgram
{
    std::string key;
    std::vector< unsigned char > program;
    size_t length;
    GLenum binaryFormat;
} CompiledProgram;

class GPreCompiledShaders
{
public:
    GPreCompiledShaders();
    virtual ~GPreCompiledShaders();

    /**
    @brief  Get current precompiled shaders instance.
    @return Current precompiled shaders instance pointer.
    */
    static GPreCompiledShaders *getInstance();

    bool GetSupportPreCompiledShaders() { return sSupportPreCompiledShaders; }

    void SetSupportPreCompiledShaders(bool support)
    {
        sSupportPreCompiledShaders = support;
    }

    bool AddProgram(GLuint program, const std::string &id);
    bool LoadProgram(GLuint program, const std::string &id);
    void SavePreCompiledShaders();

protected:
    void savePreCompiledPrograms(const std::string folder);
    void loadPreCompiledPrograms(const std::string folder);
    void Init();

    std::map< std::string, CompiledProgram * > mPrograms;
    std::map< std::string, PreCompiledProgram * > mPreCompiledPrograms;

    std::string mPreCompiledShaderCachePath;
    static bool sSupportPreCompiledShaders;
    static GLenum sProgramBinaryFormat;
};

#endif
