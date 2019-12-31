/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#include "GPreCompiledShaders.h"
#include "support/FileUtils.h"
#include "support/Log.h"

#include <dirent.h>
#include <sys/stat.h>
std::string g_shader_cache_path;

static GPreCompiledShaders *sPreCompiledShaders = NULL;
bool GPreCompiledShaders::sSupportPreCompiledShaders = false;
GLenum GPreCompiledShaders::sProgramBinaryFormat = 0;

GPreCompiledShaders *GPreCompiledShaders::getInstance(void)
{
    if (!sPreCompiledShaders)
    {
        sPreCompiledShaders = new GPreCompiledShaders();
    }

    return sPreCompiledShaders;
}

GPreCompiledShaders::GPreCompiledShaders(void) { Init(); }

void GPreCompiledShaders::Init(void)
{
    mPrograms.clear();
    mPreCompiledPrograms.clear();
    mPreCompiledShaderCachePath = g_shader_cache_path;

    loadPreCompiledPrograms(g_shader_cache_path);
}

GPreCompiledShaders::~GPreCompiledShaders(void)
{
    for (auto iter = mPrograms.begin(); iter != mPrograms.end(); ++iter)
    {
        if (iter->second) delete iter->second;
    }
    mPrograms.clear();

    for (auto iter = mPreCompiledPrograms.begin();
         iter != mPreCompiledPrograms.end(); ++iter)
    {
        if (iter->second) delete iter->second;
    }
    mPreCompiledPrograms.clear();
}

void GPreCompiledShaders::loadPreCompiledPrograms(const std::string path)
{
    mPreCompiledPrograms.clear();

    struct dirent *ent = NULL;
    DIR *pDir = NULL;
    pDir = opendir(path.c_str());
    if (pDir == NULL)
    {
        return;
    }

    while (NULL != (ent = readdir(pDir)))
    {
        if (ent->d_type != DT_DIR)
        {
            std::string fullPath = path + "/" + ent->d_name;
            size_t readLen = 0;
            unsigned char *data = FileUtils::GetInstance()->GetFileData(
                    fullPath.c_str(), "rb", &readLen);
            PreCompiledProgram *p = new PreCompiledProgram();
            p->key = ent->d_name;
            p->program = data;
            p->length = readLen;
            mPreCompiledPrograms[ent->d_name] = p;
        }
    }
}

bool GPreCompiledShaders::LoadProgram(GLuint program, const std::string &id)
{
    auto it = mPreCompiledPrograms.find(id);
    if (it == mPreCompiledPrograms.end())
    {
        return false;
    }

    glGetIntegerv(GL_PROGRAM_BINARY_FORMATS_OES, (GLint *)&sProgramBinaryFormat);

    glProgramBinaryOES(program, sProgramBinaryFormat,
                       (const GLvoid *)it->second->program, it->second->length);

    return true;
}

bool GPreCompiledShaders::AddProgram(GLuint program, const std::string &id)
{
    int length;

    auto it = mPrograms.find(id);
    if (it != mPrograms.end()) return true;

    CompiledProgram *p = new CompiledProgram();
    glGetProgramiv(program, GL_PROGRAM_BINARY_LENGTH_OES, &length);

    unsigned int glErrorStatus = (unsigned int)glGetError();
    if (glErrorStatus)
    {
        if (p)
        {
            delete p;
        }

        LOG_E("GGLPreCompiledShaders:addProgram, glGetProgramiv getlength, "
              "error %d, len %d",
              glErrorStatus, length);
        return false;
    }
    else
    {
        p->program.reserve(length);
        p->length = length;
        p->key = id;

        glGetProgramBinaryOES(program, length, NULL, &p->binaryFormat,
                              &p->program[0]);
        mPrograms[id] = p;
        sProgramBinaryFormat = p->binaryFormat;

        return true;
    }
}

void GPreCompiledShaders::savePreCompiledPrograms(const std::string folder)
{
    for (auto iter = mPrograms.begin(); iter != mPrograms.end(); ++iter)
    {
        CompiledProgram *p = iter->second;
        std::string keyName = p->key;
        std::string programName = SHADER_NAME_PREFIX + keyName;

        const char *buffer = (char *)&p->program[0];
        std::string fullPath = folder + programName;

        FileUtils::GetInstance()->WriteFileData(fullPath, buffer, p->length);
    }
}

void GPreCompiledShaders::SavePreCompiledShaders()
{
    if (mPreCompiledShaderCachePath.empty())
    {
        return;
    }

    DIR *dir = opendir(mPreCompiledShaderCachePath.c_str());
    if (!dir)
    {
        int ret = mkdir(mPreCompiledShaderCachePath.c_str(),
                        S_IRWXU | S_IRWXG | S_IRWXO);
        if (ret != 0)
        {
            return;
        }
    }

    savePreCompiledPrograms(mPreCompiledShaderCachePath);
}
