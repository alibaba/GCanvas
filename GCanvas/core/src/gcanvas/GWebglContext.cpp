/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */

#include <iostream>
#ifndef _WIN32
#ifdef ANDROID
#include <GLES2/gl2.h>
#define GL_GLEXT_PROTOTYPES
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#endif

#ifdef IOS
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#endif

#else
#include <GLES2/gl2.h>
#endif
#include "../GCanvas.hpp"
#include "../support/Encode.h"
#include "../support/Util.h"

#include <string>
#include <stdint.h>
#include <string>
#include <sstream>
#include <iostream>

//refrence https://www.khronos.org/registry/OpenGL-Refpages/es2.0/

#ifdef ANDROID
PFNGLGENVERTEXARRAYSOESPROC glGenVertexArraysOESv;
PFNGLBINDVERTEXARRAYOESPROC glBindVertexArrayOESv;
PFNGLDELETEVERTEXARRAYSOESPROC glDeleteVertexArraysOESv;
PFNGLISVERTEXARRAYOESPROC glIsVertexArrayOESv;
#endif

#define WEBGL_EXT_API_OFFSET    300
#define WEBGL_EXT_API_COUNT     7
#define WEBGL_EXT_API_MAX_INDEX (WEBGL_EXT_API_OFFSET+WEBGL_EXT_API_COUNT)

#define WEBGL_API_COUNT     137 //(1 + 136)
int g_encode_type = 0;


namespace gcanvas
{

template<typename T>
std::string toString(const T& value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

const int *ParseTokensInt(const char *&p, int iMaxCount)
{
//    struct timeval before;
//    struct timeval after;
//    gettimeofday(&before, NULL);
    static int tokens[16];
    if (iMaxCount > 16)
    {
        LOG_I("[ParseTokensInt] iMaxCount[%d] is too larger...", iMaxCount);
    }
    int iToken = 0;

    while (*p && *p != ';' && iToken < iMaxCount)
    {
        tokens[iToken++] = atoi(p);
        while (*p && (*p != ',' && *p != ';'))
        {
            ++p;
        }
        if (*p == ',') ++p;
    }
    if (*p == ';') ++p;
//    gettimeofday(&after, NULL);
//    LOG_D("before parseTokensint sec = %d, usec = %d, after sec = %d, usec = %d", before.tv_sec, before.tv_usec, after.tv_sec, after.tv_usec);
    return tokens;
}

//const GGlObject *ParseTokensObj(const char *&p, GGlObjectMgr &mgr)
//{
//    const int id = ParseTokensInt(p, 1)[0];
//    const GGlObject *obj = mgr.Get(id);
//    if (nullptr == obj)
//    {
//        LOG_I("[ParseTokensObj] cann`t get %s by id:%d", mgr.GetFlag(), id);
//    }
//    return obj;
//}

const float *ParseTokensFloat(const char *&p, int iMaxCount)
{
    static float tokens[32];
    int iToken = 0;

    while (*p && *p != ';' && iToken < iMaxCount)
    {
        tokens[iToken++] = atof(p);
        while (*p && (*p != ',' && *p != ';'))
        {
            ++p;
        }
        if (*p == ',') ++p;
    }
    if (*p == ';') ++p;
    return tokens;
}

void ParseTokensSkip(const char *&p)
{
    while (*p && (*p != ',' && *p != ';'))
    {
        ++p;
    }
    ++p;
}


void ParseTokensString(const char *&pos, std::string &str)
{
    const char *begin = pos;
    ParseTokensSkip(pos);
    unsigned int len = (unsigned int)(pos - begin - 1);

    if (pos <= begin)
    {
        str.clear();
        return;
    }

    if (len > str.length()) str.resize(len);

    strncpy((char *)str.c_str(), begin, len);
    *((char *)str.c_str() + len) = '\0';
}

int ParseTokensBase64(const char *&pos, std::string &str)
{
//    struct timeval before;
//    struct timeval after;
//    gettimeofday(&before, NULL);
    const char *begin = pos;
    ParseTokensSkip(pos);
    int len = (int)(pos - begin - 1);

    if (pos <= begin)
    {
        str.clear();
        return 0;
    }

    if (len > (int)str.length()) str.resize((size_t)len);

    gcanvas::Base64DecodeBuf((char *)str.c_str(), begin, len);
    *((char *)str.c_str() + len) = '\0';
//    gettimeofday(&after, NULL);
//    LOG_D("before parseTokensBase64 sec = %d, usec = %d, after sec = %d, usec = %d", before.tv_sec, before.tv_usec, after.tv_sec, after.tv_usec);
    return len;
}

int8_t* SplitStringToInt8Array(const char *pos, const char *delim, unsigned int &size)
{
    std::vector<int8_t> elems;
    char *begin = (char*)pos;
    char *pstr = strtok(begin, delim);

    while (pstr)
    {

        elems.push_back(atoi(pstr));
        pstr = strtok(NULL, delim);
    }

    size = (unsigned int)elems.size();

    if( size > 0 )
    {
        int8_t *int8Array = (int8_t*)malloc(sizeof(int8_t) * size);
        for (int i=0; i<size; i++)
        {
            int8Array[i] = elems.at(i);
        }

        return int8Array;
    }
    else
    {
        return NULL;
    }
}


int16_t* SplitStringToInt16Array(const char * pos, const char *delim, unsigned int &size)
{
    std::vector<int16_t> elems;
    char *begin = (char*)pos;
    char *pstr = strtok(begin, delim);

    while (pstr)
    {

        elems.push_back(atoi(pstr));
        pstr = strtok(NULL, delim);
    }

    size = (unsigned int)elems.size();

    if( size > 0 )
    {
        int16_t *uint16Array = (int16_t*)malloc(sizeof(int16_t) * size);
        for (int i=0; i<size; i++)
        {
            uint16Array[i] = elems.at(i);
        }

        return uint16Array;
    }
    else
    {
        return NULL;
    }
}

int32_t* SplitStringToInt32Array(const char * pos, const char *delim, unsigned int &size)
{
    std::vector<int32_t> elems;
    char *begin = (char*)pos;
    char *pstr = strtok(begin, delim);

    while (pstr)
    {

        elems.push_back(atoi(pstr));
        pstr = strtok(NULL, delim);
    }

    size = (unsigned int)elems.size();

    if( size > 0 )
    {
        int32_t *uint32Array = (int32_t*)malloc(sizeof(int32_t) * size);
        for (int i=0; i<size; i++)
        {
            uint32Array[i] = elems.at(i);
        }

        return uint32Array;
    }
    else
    {
        return NULL;
    }
}


float* SplitStringToFloat32Array(const char * pos, const char *delim, unsigned int &size)
{
//    struct timeval before;
//    struct timeval after;
//    gettimeofday(&before, NULL);
    std::vector<float> elems;
    char *begin = (char*)pos;
    char *pstr = strtok(begin, delim);

    while (pstr)
    {

        elems.push_back(atof(pstr));
        pstr = strtok(NULL, delim);
    }

    size = (unsigned int)elems.size();

    if( size > 0 )
    {
        float *floatArray = (float*)malloc(sizeof(float) * size);
        for (int i=0; i<size; i++)
        {
            floatArray[i] = elems.at(i);
        }
//        gettimeofday(&after, NULL);
//        LOG_D("before SplitStringToFloat32Array sec = %d, usec = %d, after: sec = %d, usec = %d", before.tv_sec, before.tv_usec,after.tv_sec, after.tv_usec);

        return floatArray;
    }
    else
    {
        return NULL;
    }
}

/*
 uint_8             1
 uint_16            2
 uint_32            4
 float32            14
 */
void* SplitStringToArray(const char *pos, int bytes, unsigned int &size)
{
    void *array = NULL;

    if( bytes == 1 )
    {
        int8_t *int8Array = SplitStringToInt8Array(pos, ",", size);
        size *= bytes;
        array = int8Array;
    }
    else if( bytes == 2 )
    {
        int16_t *int16Array = SplitStringToInt16Array(pos, ",", size);
        size *= bytes;
        array = int16Array;
    }
    else if( bytes == 4 )
    {
        int32_t *int32Array = SplitStringToInt32Array(pos, ",", size);
        size *= bytes;
        array = int32Array;
    }
    else if( bytes == 14 )
    {
        float *float32Array = SplitStringToFloat32Array(pos, ",", size);
        size *= (bytes-10);
        array = float32Array;
    }

    return array;
}

//static int g_shapeType_list[] = {GL_VERTEX_SHADER, //
//                                 GL_FRAGMENT_SHADER};
//
//static int g_precisionType_list[] = {GL_LOW_FLOAT,  GL_MEDIUM_FLOAT,
//                                     GL_HIGH_FLOAT, GL_LOW_INT,
//                                     GL_MEDIUM_INT, GL_HIGH_INT};

#define G_BUFFER_OFFSET(i) ((char *)NULL + (i))

typedef enum {
    kReturnBoolean = 1,
    kReturnInt,
    kReturnFloat,
    kReturnIntArray,
    kReturnFloatArray,
    kReturnString,
    kReturnBooleanArray,
    kReturnUintArray
}ParamReturnType;


typedef enum {
    kContinue = 0, // go on deal next cmd
    kGotoNext,     // skip over ";", go to next cmd
    kBreak         // some error happen, stop deal cmd
} FuncReturnType;

//static const int MACRO_LIST_SIZE = sizeof(g_macro_list) / sizeof(int);
#ifdef ANDROID
#define DEBUG
#endif
const char *GetMacroValDebug(int index)
{
#ifdef DEBUG
    switch (index) {
        case 0x0: return "GL_NONE or GL_ZERO or GL_POINTS";
        case 0x1: return "GL_ONE or GL_LINES";
        case 0x2: return "GL_LINE_LOOP";
        case 0x3: return "GL_LINE_STRIP";
        case 0x4: return "GL_TRIANGLES";
        case 0x5: return "GL_TRIANGLE_STRIP";
        case 0x6: return "GL_TRIANGLE_FAN";
        case 0x100: return "GL_DEPTH_BUFFER_BIT";
        case 0x200: return "GL_NEVER";
        case 0x201: return "GL_LESS";
        case 0x202: return "GL_EQUAL";
        case 0x203: return "GL_LEQUAL";
        case 0x204: return "GL_GREATER";
        case 0x205: return "GL_NOTEQUAL";
        case 0x206: return "GL_GEQUAL";
        case 0x207: return "GL_ALWAYS";
        case 0x300: return "GL_SRC_COLOR";
        case 0x301: return "GL_ONE_MINUS_SRC_COLOR";
        case 0x302: return "GL_SRC_ALPHA";
        case 0x303: return "GL_ONE_MINUS_SRC_ALPHA";
        case 0x304: return "GL_DST_ALPHA";
        case 0x305: return "GL_ONE_MINUS_DST_ALPHA";
        case 0x306: return "GL_DST_COLOR";
        case 0x307: return "GL_ONE_MINUS_DST_COLOR";
        case 0x308: return "GL_SRC_ALPHA_SATURATE";
        case 0x400: return "GL_STENCIL_BUFFER_BIT";
        case 0x404: return "GL_FRONT";
        case 0x405: return "GL_BACK";
        case 0x408: return "GL_FRONT_AND_BACK";
        case 0x500: return "GL_INVALID_ENUM";
        case 0x501: return "GL_INVALID_VALUE";
        case 0x502: return "GL_INVALID_OPERATION";
        case 0x505: return "GL_OUT_OF_MEMORY";
        case 0x506: return "GL_INVALID_FRAMEBUFFER_OPERATION";
        case 0x900: return "GL_CW";
        case 0x901: return "GL_CCW";
        case 0xB21: return "GL_LINE_WIDTH";
        case 0xB44: return "GL_CULL_FACE";
        case 0xB45: return "GL_CULL_FACE_MODE";
        case 0xB46: return "GL_FRONT_FACE";
        case 0xB70: return "GL_DEPTH_RANGE";
        case 0xB71: return "GL_DEPTH_TEST";
        case 0xB72: return "GL_DEPTH_WRITEMASK";
        case 0xB73: return "GL_DEPTH_CLEAR_VALUE";
        case 0xB74: return "GL_DEPTH_FUNC";
        case 0xB90: return "GL_STENCIL_TEST";
        case 0xB91: return "GL_STENCIL_CLEAR_VALUE";
        case 0xB92: return "GL_STENCIL_FUNC";
        case 0xB93: return "GL_STENCIL_VALUE_MASK";
        case 0xB94: return "GL_STENCIL_FAIL";
        case 0xB95: return "GL_STENCIL_PASS_DEPTH_FAIL";
        case 0xB96: return "GL_STENCIL_PASS_DEPTH_PASS";
        case 0xB97: return "GL_STENCIL_REF";
        case 0xB98: return "GL_STENCIL_WRITEMASK";
        case 0xBA2: return "GL_VIEWPORT";
        case 0xBD0: return "GL_DITHER";
        case 0xBE2: return "GL_BLEND";
        case 0xC10: return "GL_SCISSOR_BOX";
        case 0xC11: return "GL_SCISSOR_TEST";
        case 0xC22: return "GL_COLOR_CLEAR_VALUE";
        case 0xC23: return "GL_COLOR_WRITEMASK";
        case 0xCF5: return "GL_UNPACK_ALIGNMENT";
        case 0xD05: return "GL_PACK_ALIGNMENT";
        case 0xD33: return "GL_MAX_TEXTURE_SIZE";
        case 0xD3A: return "GL_MAX_VIEWPORT_DIMS";
        case 0xD50: return "GL_SUBPIXEL_BITS";
        case 0xD52: return "GL_RED_BITS";
        case 0xD53: return "GL_GREEN_BITS";
        case 0xD54: return "GL_BLUE_BITS";
        case 0xD55: return "GL_ALPHA_BITS";
        case 0xD56: return "GL_DEPTH_BITS";
        case 0xD57: return "GL_STENCIL_BITS";
        case 0xDE1: return "GL_TEXTURE_2D";
        case 0x1100: return "GL_DONT_CARE";
        case 0x1101: return "GL_FASTEST";
        case 0x1102: return "GL_NICEST";
        case 0x1400: return "GL_BYTE";
        case 0x1401: return "GL_UNSIGNED_BYTE";
        case 0x1402: return "GL_SHORT";
        case 0x1403: return "GL_UNSIGNED_SHORT";
        case 0x1404: return "GL_INT";
        case 0x1405: return "GL_UNSIGNED_INT";
        case 0x1406: return "GL_FLOAT";
        case 0x150A: return "GL_INVERT";
        case 0x1702: return "GL_TEXTURE";
        case 0x1901: return "GL_STENCIL_INDEX";
        case 0x1902: return "GL_DEPTH_COMPONENT";
        case 0x1906: return "GL_ALPHA";
        case 0x1907: return "GL_RGB";
        case 0x1908: return "GL_RGBA";
        case 0x1909: return "GL_LUMINANCE";
        case 0x190A: return "GL_LUMINANCE_ALPHA";
        case 0x1E00: return "GL_KEEP";
        case 0x1E01: return "GL_REPLACE";
        case 0x1E02: return "GL_INCR";
        case 0x1E03: return "GL_DECR";
        case 0x1F00: return "GL_VENDOR";
        case 0x1F01: return "GL_RENDERER";
        case 0x1F02: return "GL_VERSION";
        case 0x2600: return "GL_NEAREST";
        case 0x2601: return "GL_LINEAR";
        case 0x2700: return "GL_NEAREST_MIPMAP_NEAREST";
        case 0x2701: return "GL_LINEAR_MIPMAP_NEAREST";
        case 0x2702: return "GL_NEAREST_MIPMAP_LINEAR";
        case 0x2703: return "GL_LINEAR_MIPMAP_LINEAR";
        case 0x2800: return "GL_TEXTURE_MAG_FILTER";
        case 0x2801: return "GL_TEXTURE_MIN_FILTER";
        case 0x2802: return "GL_TEXTURE_WRAP_S";
        case 0x2803: return "GL_TEXTURE_WRAP_T";
        case 0x2901: return "GL_REPEAT";
        case 0x2A00: return "GL_POLYGON_OFFSET_UNITS";
        case 0x4000: return "GL_COLOR_BUFFER_BIT";
        case 0x8001: return "GL_CONSTANT_COLOR";
        case 0x8002: return "GL_ONE_MINUS_CONSTANT_COLOR";
        case 0x8003: return "GL_CONSTANT_ALPHA";
        case 0x8004: return "GL_ONE_MINUS_CONSTANT_ALPHA";
        case 0x8005: return "GL_BLEND_COLOR";
        case 0x8006: return "GL_FUNC_ADD";
        case 0x8009: return "GL_BLEND_EQUATION_RGB";
        case 0x800A: return "GL_FUNC_SUBTRACT";
        case 0x800B: return "GL_FUNC_REVERSE_SUBTRACT";
        case 0x8033: return "GL_UNSIGNED_SHORT_4_4_4_4";
        case 0x8034: return "GL_UNSIGNED_SHORT_5_5_5_1";
        case 0x8037: return "GL_POLYGON_OFFSET_FILL";
        case 0x8038: return "GL_POLYGON_OFFSET_FACTOR";
        case 0x8056: return "GL_RGBA4";
        case 0x8057: return "GL_RGB5_A1";
        case 0x8069: return "GL_TEXTURE_BINDING_2D";
        case 0x809E: return "GL_SAMPLE_ALPHA_TO_COVERAGE";
        case 0x80A0: return "GL_SAMPLE_COVERAGE";
        case 0x80A8: return "GL_SAMPLE_BUFFERS";
        case 0x80A9: return "GL_SAMPLES";
        case 0x80AA: return "GL_SAMPLE_COVERAGE_VALUE";
        case 0x80AB: return "GL_SAMPLE_COVERAGE_INVERT";
        case 0x80C8: return "GL_BLEND_DST_RGB";
        case 0x80C9: return "GL_BLEND_SRC_RGB";
        case 0x80CA: return "GL_BLEND_DST_ALPHA";
        case 0x80CB: return "GL_BLEND_SRC_ALPHA";
        case 0x812F: return "GL_CLAMP_TO_EDGE";
        case 0x8192: return "GL_GENERATE_MIPMAP_HINT";
        case 0x81A5: return "GL_DEPTH_COMPONENT16";
        case 0x821A: return "GL_DEPTH_STENCIL_ATTACHMENT";
        case 0x8363: return "GL_UNSIGNED_SHORT_5_6_5";
        case 0x8370: return "GL_MIRRORED_REPEAT";
        case 0x846D: return "GL_ALIASED_POINT_SIZE_RANGE";
        case 0x846E: return "GL_ALIASED_LINE_WIDTH_RANGE";
        case 0x84C0: return "GL_TEXTURE0";
        case 0x84C1: return "GL_TEXTURE1";
        case 0x84C2: return "GL_TEXTURE2";
        case 0x84C3: return "GL_TEXTURE3";
        case 0x84C4: return "GL_TEXTURE4";
        case 0x84C5: return "GL_TEXTURE5";
        case 0x84C6: return "GL_TEXTURE6";
        case 0x84C7: return "GL_TEXTURE7";
        case 0x84C8: return "GL_TEXTURE8";
        case 0x84C9: return "GL_TEXTURE9";
        case 0x84CA: return "GL_TEXTURE10";
        case 0x84CB: return "GL_TEXTURE11";
        case 0x84CC: return "GL_TEXTURE12";
        case 0x84CD: return "GL_TEXTURE13";
        case 0x84CE: return "GL_TEXTURE14";
        case 0x84CF: return "GL_TEXTURE15";
        case 0x84D0: return "GL_TEXTURE16";
        case 0x84D1: return "GL_TEXTURE17";
        case 0x84D2: return "GL_TEXTURE18";
        case 0x84D3: return "GL_TEXTURE19";
        case 0x84D4: return "GL_TEXTURE20";
        case 0x84D5: return "GL_TEXTURE21";
        case 0x84D6: return "GL_TEXTURE22";
        case 0x84D7: return "GL_TEXTURE23";
        case 0x84D8: return "GL_TEXTURE24";
        case 0x84D9: return "GL_TEXTURE25";
        case 0x84DA: return "GL_TEXTURE26";
        case 0x84DB: return "GL_TEXTURE27";
        case 0x84DC: return "GL_TEXTURE28";
        case 0x84DD: return "GL_TEXTURE29";
        case 0x84DE: return "GL_TEXTURE30";
        case 0x84DF: return "GL_TEXTURE31";
        case 0x84E0: return "GL_ACTIVE_TEXTURE";
        case 0x84E8: return "GL_MAX_RENDERBUFFER_SIZE";
        case 0x84F9: return "GL_DEPTH_STENCIL";
        case 0x8507: return "GL_INCR_WRAP";
        case 0x8508: return "GL_DECR_WRAP";
        case 0x8513: return "GL_TEXTURE_CUBE_MAP";
        case 0x8514: return "GL_TEXTURE_BINDING_CUBE_MAP";
        case 0x8515: return "GL_TEXTURE_CUBE_MAP_POSITIVE_X";
        case 0x8516: return "GL_TEXTURE_CUBE_MAP_NEGATIVE_X";
        case 0x8517: return "GL_TEXTURE_CUBE_MAP_POSITIVE_Y";
        case 0x8518: return "GL_TEXTURE_CUBE_MAP_NEGATIVE_Y";
        case 0x8519: return "GL_TEXTURE_CUBE_MAP_POSITIVE_Z";
        case 0x851A: return "GL_TEXTURE_CUBE_MAP_NEGATIVE_Z";
        case 0x851C: return "GL_MAX_CUBE_MAP_TEXTURE_SIZE";
        case 0x8622: return "GL_VERTEX_ATTRIB_ARRAY_ENABLED";
        case 0x8623: return "GL_VERTEX_ATTRIB_ARRAY_SIZE";
        case 0x8624: return "GL_VERTEX_ATTRIB_ARRAY_STRIDE";
        case 0x8625: return "GL_VERTEX_ATTRIB_ARRAY_TYPE";
        case 0x8626: return "GL_CURRENT_VERTEX_ATTRIB";
        case 0x8645: return "GL_VERTEX_ATTRIB_ARRAY_POINTER";
        case 0x86A2: return "GL_NUM_COMPRESSED_TEXTURE_FORMATS";
        case 0x86A3: return "GL_COMPRESSED_TEXTURE_FORMATS";
        case 0x8764: return "GL_BUFFER_SIZE";
        case 0x8765: return "GL_BUFFER_USAGE";
        case 0x8800: return "GL_STENCIL_BACK_FUNC";
        case 0x8801: return "GL_STENCIL_BACK_FAIL";
        case 0x8802: return "GL_STENCIL_BACK_PASS_DEPTH_FAIL";
        case 0x8803: return "GL_STENCIL_BACK_PASS_DEPTH_PASS";
        case 0x883D: return "GL_BLEND_EQUATION_ALPHA";
        case 0x8869: return "GL_MAX_VERTEX_ATTRIBS";
        case 0x886A: return "GL_VERTEX_ATTRIB_ARRAY_NORMALIZED";
        case 0x8872: return "GL_MAX_TEXTURE_IMAGE_UNITS";
        case 0x8892: return "GL_ARRAY_BUFFER";
        case 0x8893: return "GL_ELEMENT_ARRAY_BUFFER";
        case 0x8894: return "GL_ARRAY_BUFFER_BINDING";
        case 0x8895: return "GL_ELEMENT_ARRAY_BUFFER_BINDING";
        case 0x889F: return "GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING";
        case 0x88E0: return "GL_STREAM_DRAW";
        case 0x88E4: return "GL_STATIC_DRAW";
        case 0x88E8: return "GL_DYNAMIC_DRAW";
        case 0x8B30: return "GL_FRAGMENT_SHADER";
        case 0x8B31: return "GL_VERTEX_SHADER";
        case 0x8B4C: return "GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS";
        case 0x8B4D: return "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS";
        case 0x8B4F: return "GL_SHADER_TYPE";
        case 0x8B50: return "GL_FLOAT_VEC2";
        case 0x8B51: return "GL_FLOAT_VEC3";
        case 0x8B52: return "GL_FLOAT_VEC4";
        case 0x8B53: return "GL_INT_VEC2";
        case 0x8B54: return "GL_INT_VEC3";
        case 0x8B55: return "GL_INT_VEC4";
        case 0x8B56: return "GL_BOOL";
        case 0x8B57: return "GL_BOOL_VEC2";
        case 0x8B58: return "GL_BOOL_VEC3";
        case 0x8B59: return "GL_BOOL_VEC4";
        case 0x8B5A: return "GL_FLOAT_MAT2";
        case 0x8B5B: return "GL_FLOAT_MAT3";
        case 0x8B5C: return "GL_FLOAT_MAT4";
        case 0x8B5E: return "GL_SAMPLER_2D";
        case 0x8B60: return "GL_SAMPLER_CUBE";
        case 0x8B80: return "GL_DELETE_STATUS";
        case 0x8B81: return "GL_COMPILE_STATUS";
        case 0x8B82: return "GL_LINK_STATUS";
        case 0x8B83: return "GL_VALIDATE_STATUS";
        case 0x8B84: return "GL_INFO_LOG_LENGTH";
        case 0x8B85: return "GL_ATTACHED_SHADERS";
        case 0x8B86: return "GL_ACTIVE_UNIFORMS";
        case 0x8B87: return "GL_ACTIVE_UNIFORM_MAX_LENGTH";
        case 0x8B88: return "GL_SHADER_SOURCE_LENGTH";
        case 0x8B89: return "GL_ACTIVE_ATTRIBUTES";
        case 0x8B8A: return "GL_ACTIVE_ATTRIBUTE_MAX_LENGTH";
        case 0x8B8C: return "GL_SHADING_LANGUAGE_VERSION";
        case 0x8B8D: return "GL_CURRENT_PROGRAM";
        case 0x8CA3: return "GL_STENCIL_BACK_REF";
        case 0x8CA4: return "GL_STENCIL_BACK_VALUE_MASK";
        case 0x8CA5: return "GL_STENCIL_BACK_WRITEMASK";
        case 0x8CA6: return "GL_FRAMEBUFFER_BINDING";
        case 0x8CA7: return "GL_RENDERBUFFER_BINDING";
        case 0x8CD0: return "GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE";
        case 0x8CD1: return "GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME";
        case 0x8CD2: return "GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL";
        case 0x8CD3: return "GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE";
        case 0x8CD5: return "GL_FRAMEBUFFER_COMPLETE";
        case 0x8CD6: return "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
        case 0x8CD7: return "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
        case 0x8CD9: return "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS";
        case 0x8CDD: return "GL_FRAMEBUFFER_UNSUPPORTED";
        case 0x8CE0: return "GL_COLOR_ATTACHMENT0";
        case 0x8D00: return "GL_DEPTH_ATTACHMENT";
        case 0x8D20: return "GL_STENCIL_ATTACHMENT";
        case 0x8D40: return "GL_FRAMEBUFFER";
        case 0x8D41: return "GL_RENDERBUFFER";
        case 0x8D42: return "GL_RENDERBUFFER_WIDTH";
        case 0x8D43: return "GL_RENDERBUFFER_HEIGHT";
        case 0x8D44: return "GL_RENDERBUFFER_INTERNAL_FORMAT";
        case 0x8D48: return "GL_STENCIL_INDEX8";
        case 0x8D50: return "GL_RENDERBUFFER_RED_SIZE";
        case 0x8D51: return "GL_RENDERBUFFER_GREEN_SIZE";
        case 0x8D52: return "GL_RENDERBUFFER_BLUE_SIZE";
        case 0x8D53: return "GL_RENDERBUFFER_ALPHA_SIZE";
        case 0x8D54: return "GL_RENDERBUFFER_DEPTH_SIZE";
        case 0x8D55: return "GL_RENDERBUFFER_STENCIL_SIZE";
        case 0x8D62: return "GL_RGB565";
        case 0x8DF0: return "GL_LOW_FLOAT";
        case 0x8DF1: return "GL_MEDIUM_FLOAT";
        case 0x8DF2: return "GL_HIGH_FLOAT";
        case 0x8DF3: return "GL_LOW_INT";
        case 0x8DF4: return "GL_MEDIUM_INT";
        case 0x8DF5: return "GL_HIGH_INT";
        case 0x8DFA: return "GL_SHADER_COMPILER";
        case 0x8DFB: return "GL_MAX_VERTEX_UNIFORM_VECTORS";
        case 0x8DFC: return "GL_MAX_VARYING_VECTORS";
        case 0x8DFD: return "GL_MAX_FRAGMENT_UNIFORM_VECTORS";
        case 0x9240: return "GL_UNPACK_FLIP_Y_WEBGL";
        case 0x9241: return "GL_UNPACK_PREMULTIPLY_ALPHA_WEBGL";
        case 0x9242: return "GL_CONTEXT_LOST_WEBGL";
        case 0x9243: return "GL_UNPACK_COLORSPACE_CONVERSION_WEBGL";
        case 0x9244: return "GL_BROWSER_DEFAULT_WEBGL";
        default: return "Unkonw Macro";
    }
#endif

    return "";
}

//////////////////////////////////////////////////////////////////////////
//                  WEBGL 1.0 API
// https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext
//////////////////////////////////////////////////////////////////////////
int activeTexture(GCanvas *obj, const char *&p)
{
    const int *tokens = ParseTokensInt(p, 1);
    
    LOG_D("[webgl::exec] glActiveTexture(%s)", GetMacroValDebug(tokens[0]));
    glActiveTexture(tokens[0]);
    return kContinue;
}

int attachShader(GCanvas *obj, const char *&p)
{
    const int *tokens = ParseTokensInt(p, 2);
    GLuint program = tokens[0];
    GLuint shader = tokens[1];
    
    LOG_D("[webgl::exec] glAttachShader(%d, %d)", program, shader);
    glAttachShader(program, shader);
    return kContinue;
}

int bindAttribLocation(GCanvas *obj, const char *&p)
{
    const int *tokens = ParseTokensInt(p, 2);
    GLuint program = tokens[0];
    GLuint index = tokens[1];

    std::string &name = obj->mTempStr;
    ParseTokensString(p, name);

    LOG_D("[webgl::exec] glBindAttribLocation(%d, %d, %s)", program, index, name.c_str());
    glBindAttribLocation(program, index, name.c_str());
    return kContinue;
}

int bindBuffer(GCanvas *obj, const char *&p)
{
    const int *tokens = ParseTokensInt(p, 2);

    GLuint target = tokens[0];
    GLuint buffer = tokens[1];
    
    LOG_D("[webgl::exec] glBindBuffer(%s, %d)", GetMacroValDebug(target), buffer);
    glBindBuffer(target, buffer);
    return kContinue;
}

int bindFramebuffer(GCanvas *obj, const char *&p)
{
    const int *tokens = ParseTokensInt(p, 2);
    GLuint target = tokens[0];
    GLuint framebuffer = tokens[1];

    LOG_D("[webgl::exec] glBindFramebuffer(%s, %d)", GetMacroValDebug(target), framebuffer);

#ifdef IOS
    if ( framebuffer == 0 ){
        GWebGLBindToGLKViewFunc func = obj->GetGWebGLBindToGLKViewFunc();
        if(func){
            func(obj->mContextId);
        }
    }else{
#endif
        glBindFramebuffer(target, framebuffer);
#ifdef IOS
    }
#endif
    return kContinue;
}

int bindRenderbuffer(GCanvas *obj, const char *&p)
{ // bindRenderbuffer0,81;
    const int *tokens = ParseTokensInt(p, 2);
    GLuint target = tokens[0];
    GLuint renderbuffer = tokens[1];

    LOG_D("[webgl::exec] bindRenderbuffer(%s, %d)", GetMacroValDebug(target), renderbuffer);
    glBindRenderbuffer(target, renderbuffer);
    return kContinue;
}

//new
int blendColor(GCanvas *obj, const char *&p)
{ // blendColor0,81;
    const float *tokens = ParseTokensFloat(p, 4);
    GLfloat red = tokens[0];
    GLfloat green = tokens[1];
    GLfloat blue = tokens[2];
    GLfloat alpha = tokens[3];

    LOG_D("[webgl::exec] blendColor(%f, %f, %f, %f)", red, green, blue, alpha);
    glBlendColor(red, green, blue, alpha);
    return kContinue;
}

int bindTexture(GCanvas *obj, const char *&p)
{ // bindTexture0,4;
    const int *tokens = ParseTokensInt(p, 2);
    GLuint target = tokens[0];
    GLuint texture = tokens[1];
    
    LOG_D("[webgl::exec] glBindTexture(%s, %d)", GetMacroValDebug(target), texture);
    glBindTexture(target, texture);
    return kContinue;
}

int blendEquation(GCanvas *obj, const char *&p)
{
    const int *tokens = ParseTokensInt(p, 1);
    
    LOG_D("[webgl::exec] glBlendEquation(%s)", GetMacroValDebug(tokens[0]));
    glBlendEquation(tokens[0]);
    return kContinue;
}

int blendEquationSeparate(GCanvas *obj, const char *&p)
{
    const int *tokens = ParseTokensInt(p, 2);
    
    LOG_D("[webgl::exec] glBlendEquationSeparate(%s, %s)",  GetMacroValDebug(tokens[0]),
          GetMacroValDebug(tokens[1]));
    glBlendEquationSeparate(tokens[0], tokens[1]);
    return kContinue;
}

int blendFunc(GCanvas *obj, const char *&p)
{ // blendFunc1,2;
    const int *tokens = ParseTokensInt(p, 2);
    
    LOG_D("[webgl::exec] glBlendFunc(%s, %s)", GetMacroValDebug(tokens[0]),
          GetMacroValDebug(tokens[1]));
    glBlendFunc(tokens[0], tokens[1]);
    return kContinue;
}

int blendFuncSeparate(GCanvas *obj, const char *&p)
{
    const int *tokens = ParseTokensInt(p, 4);
    
    LOG_D("[webgl::exec] glBlendFuncSeparate(%s, %s, %s, %s)",
          GetMacroValDebug(tokens[0]), GetMacroValDebug(tokens[1]),
          GetMacroValDebug(tokens[2]), GetMacroValDebug(tokens[3]));
    glBlendFuncSeparate(tokens[0], tokens[1], tokens[2], tokens[3]);
    return kContinue;
}

int bufferData(GCanvas *obj, const char *&p)
{ // bufferData0,4,AAEAAAAAAQEAAAAAAP8A/wAA,0;
    const int *tokens = ParseTokensInt(p, 2);
    GLenum target = tokens[0];
    int bytes = tokens[1];
    std::string &buf = obj->mTempStr;
    ParseTokensBase64(p, buf);

    unsigned int size;
    GLvoid *array = SplitStringToArray(buf.c_str(), bytes, size);

    int usage = ParseTokensInt(p, 1)[0];
    if(bytes == 1){
        LOG_D("[webgl::exec] glBufferData: %d",((GLbyte *)array)[0]);
        LOG_D("[webgl::exec] glBufferData: %d",((GLbyte *)array)[1]);
        LOG_D("[webgl::exec] glBufferData: %d",((GLbyte *)array)[2]);
        LOG_D("[webgl::exec] glBufferData: %d",((GLbyte *)array)[3]);
    }

    if (array)
    {
        LOG_D("[webgl::exec] glBufferData(%s, %d, %s)",
              GetMacroValDebug(target), size, GetMacroValDebug(usage));
        glBufferData(target, size, (GLvoid *)array, usage);
        free(array);
    }

    return kContinue;
}

int bufferSubData(GCanvas *obj, const char *&p)
{ //bufferSubData0, offset, bytes, AAEAAAAAAQEAAAAAAP8A/wAA
    const int *tokens = ParseTokensInt(p, 3);
    GLenum target = tokens[0];
    int offset = tokens[1];
    int bytes = tokens[2];
    std::string &buf = obj->mTempStr;
    ParseTokensBase64(p, buf);

    unsigned int size;
    GLvoid *array = SplitStringToArray(buf.c_str(), bytes, size);

    if (array)
    {
        LOG_D("[webgl::exec] glBufferSubData(%s, %d, %d)",
              GetMacroValDebug(target), offset, size);
        glBufferSubData(target, offset, size, (GLvoid *)array);
        free(array);
    }

    return kContinue;
}

//new
int checkFramebufferStatus(GCanvas *obj, const char *&p)
{
    const int *tokens = ParseTokensInt(p, 1);
    GLenum target = tokens[0];

    LOG_D("[webgl::exec] glCheckFramebufferStatus(%s)", GetMacroValDebug(target));
    GLenum status = glCheckFramebufferStatus(target);

    obj->setSyncResult(gcanvas::toString(status));
    return kContinue;
}

int clear(GCanvas *obj, const char *&p)
{ // clear;
    const int mask = ParseTokensInt(p, 1)[0];
    glClear(mask);
    LOG_D("[webgl::exec] glClear(%d)", mask);
    return kContinue;
}

int clearColor(GCanvas *obj, const char *&p)
{ // clearColor0,0,0,1;
    const float *tokens = ParseTokensFloat(p, 4);
    LOG_D("[webgl::exec] glClearColor(%f, %f, %f, %f)", tokens[0], tokens[1],
          tokens[2], tokens[3]);
    glClearColor(tokens[0], tokens[1], tokens[2], tokens[3]);
    return kContinue;
}

int clearDepth(GCanvas *obj, const char *&p)
{ // clearDepth1.0;
    const float *tokens = ParseTokensFloat(p, 1);
    LOG_D("[webgl::exec] glClearDepthf(%f)", tokens[0]);
    glClearDepthf(tokens[0]);
    return kContinue;
}

int clearStencil(GCanvas *obj, const char *&p)
{ // clearStencil2;
    const int *tokens = ParseTokensInt(p, 1);
    LOG_D("[webgl::exec] glClearStencil(%d)", tokens[0]);
    glClearStencil(tokens[0]);
    return kContinue;
}

int colorMask(GCanvas *obj, const char *&p)
{ // colorMask0,0,0,1;
    const int *tokens = ParseTokensInt(p, 4);
    LOG_D("[webgl::exec] glColorMask(%d, %d, %d, %d)", tokens[0], tokens[1],
          tokens[2], tokens[3]);
    glColorMask(tokens[0], tokens[1], tokens[2], tokens[3]);
    return kContinue;
}

int compileShader(GCanvas *obj, const char *&p)
{ // compileShader1
    const int *tokens = ParseTokensInt(p, 1);
    GLuint shader = tokens[0];
    
    LOG_D("[webgl::exec] glCompileShader(%d)", shader);
    glCompileShader(shader);
    return kContinue;
}

//new
int compressedTexImage2D(GCanvas *obj, const char *&p)
{
    const int *tokens = ParseTokensInt(p, 7);
    GLenum target = tokens[0];
    GLuint level = tokens[1];
    GLenum internalformat = tokens[2];
    GLsizei width = tokens[3];
    GLsizei height = tokens[4];
    GLsizei border = tokens[5];
    int bytes = tokens[6];

    std::string &buf = obj->mTempStr;
    ParseTokensBase64(p, buf);

    unsigned int size = 0;
    GLvoid *array = SplitStringToArray(buf.c_str(), bytes, size);

    LOG_D("[webgl::exec] glCompressedTexImage2D(%s, %d, %s, %d, %d, %d, %d)", GetMacroValDebug(target), level,
          GetMacroValDebug(internalformat), width, height, border, size);
    glCompressedTexImage2D(target, level, internalformat, width, height, border, size, array);
    return kContinue;
}

//new
int compressedTexSubImage2D(GCanvas *obj, const char *&p)
{
    const int *intTokens = ParseTokensInt(p, 8);
    GLenum target = intTokens[0];
    GLuint level = intTokens[1];
    GLint xoffset = intTokens[2];
    GLint yoffset = intTokens[3];
    GLsizei width = intTokens[4];
    GLsizei height = intTokens[5];
    GLenum format = intTokens[6];
    int bytes = intTokens[7];

    std::string &buf = obj->mTempStr;
    ParseTokensBase64(p, buf);

    unsigned int size = 0;
    GLvoid *array = SplitStringToArray(buf.c_str(), bytes, size);

    LOG_D("[webgl::exec] glCompressedTexImage2D(%s, %d, %d, %d, %d, %d, %s, %d)", GetMacroValDebug(target), level,
          xoffset, yoffset, width, height, GetMacroValDebug(format), size);
    glCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, size, array);
    return kContinue;
}


int copyTexImage2D(GCanvas *obj, const char *&p)
{//copyTexImage2D gl.copyTexImage2D(gl.TEXTURE_2D, 0, gl.RGBA, 0, 0, 512, 512, 0);
    const int *tokens = ParseTokensInt(p, 8);
    LOG_D("[webgl::exec] glCopyTexImage2D(%s, %d, %s, %d, %d, %d, %d)",
          GetMacroValDebug(tokens[0]), tokens[1], GetMacroValDebug(tokens[2]), tokens[3], tokens[4], tokens[5], tokens[6], tokens[7]);
    glCopyTexImage2D(tokens[0], tokens[1], tokens[2], tokens[3], tokens[4], tokens[5], tokens[6], tokens[7]);
    return kContinue;
}

//new
int copyTexSubImage2D(GCanvas *obj, const char *&p)
{
    const int *tokens = ParseTokensInt(p, 8);
    LOG_D("[webgl::exec] glCopyTexSubImage2D(%s, %d, %d, %d, %d, %d, %d)",
          GetMacroValDebug(tokens[0]), tokens[1], tokens[2], tokens[3], tokens[4], tokens[5], tokens[6], tokens[7]);
    glCopyTexSubImage2D(tokens[0], tokens[1], tokens[2], tokens[3], tokens[4], tokens[5], tokens[6], tokens[7]);
    return kContinue;
}

int createBuffer(GCanvas *obj, const char *&p)
{ // createBuffer;
    GLuint buffer = 0;
    glGenBuffers(1, &buffer);
    ++p;
    LOG_D("[webgl::exec] glGenBuffers(1, %d)", buffer);
    
    obj->setSyncResult(gcanvas::toString(buffer));
    return kContinue;
}

int createFramebuffer(GCanvas *obj, const char *&p)
{ // createFramebuffer;
    GLuint framebuffer = 0;
    glGenFramebuffers(1, &framebuffer);
    ++p;
    LOG_D("[webgl::exec] glGenFramebuffers(1, %d)", framebuffer);
    
    obj->setSyncResult(gcanvas::toString(framebuffer));
    return kContinue;
}

int createProgram(GCanvas *obj, const char *&p)
{//createProgram
    GLuint program = glCreateProgram();
    ++p;
    LOG_D("[webgl::exec] glCreateProgram()=%d", program);
    
    obj->setSyncResult(gcanvas::toString(program));
    return kContinue;
}

int createRenderbuffer(GCanvas *obj, const char *&p)
{ // createRenderbuffer;
    GLuint renderBuffer;
    glGenRenderbuffers(1, &renderBuffer);
    ++p;
    LOG_D("[webgl::exec] glGenRenderbuffers(1, %d)", renderBuffer);
    
    obj->setSyncResult(gcanvas::toString(renderBuffer));
    return kContinue;
}

int createShader(GCanvas *obj, const char *&p)
{ // createShader1;
    const int *tokens = ParseTokensInt(p, 1);
    GLuint shader = glCreateShader(tokens[0]);
    ++p;
    LOG_D("[webgl::exec] glCreateShader(%s)=%d", GetMacroValDebug(tokens[0]), shader);
    
    obj->setSyncResult(gcanvas::toString(shader));
    return kContinue;
}

int createTexture(GCanvas *obj, const char *&p)
{ // createTexture
    GLuint texture;
    glGenTextures(1, &texture);
    ++p;
    LOG_D("[webgl::exec] glGenTextures(1, %u)", texture);
    
    obj->setSyncResult(gcanvas::toString(texture));
    return kContinue;
}

int cullFace(GCanvas *obj, const char *&p)
{ // cullFace1;
    const int *tokens = ParseTokensInt(p, 1);
    LOG_D("[webgl::exec] glCullFace(%s)", GetMacroValDebug(tokens[0]));
    glCullFace(tokens[0]);
    return kContinue;
}

int deleteBuffer(GCanvas *obj, const char *&p)
{
    const int *tokens = ParseTokensInt(p, 1);
    GLuint buffer = tokens[0];
    
    LOG_D("[webgl::exec] glDeleteBuffer(1, %d)", buffer);
    glDeleteBuffers(1, &buffer);
    return kContinue;
}

int deleteFramebuffer(GCanvas *obj, const char *&p)
{
    const int *tokens = ParseTokensInt(p, 1);
    GLuint framebuffer = tokens[0];
    
    LOG_D("[webgl::exec] glDeleteFramebuffers(1, %d)", framebuffer);
    glDeleteFramebuffers(1, &framebuffer);
    return kContinue;
}

int deleteProgram(GCanvas *obj, const char *&p)
{
    const int *tokens = ParseTokensInt(p, 1);
    GLuint program = tokens[0];
    
    LOG_D("[webgl::exec] glDeleteProgram(%d)", program);
    glDeleteProgram(program);
    return kContinue;
}

int deleteRenderbuffer(GCanvas *obj, const char *&p)
{
    const int *tokens = ParseTokensInt(p, 1);
    GLuint renderbuffer = tokens[0];
    
    LOG_D("[webgl::exec] glDeleteRenderbuffers(1, %d)", renderbuffer);
    glDeleteRenderbuffers(1, &renderbuffer);
    return kContinue;
}

int deleteShader(GCanvas *obj, const char *&p)
{ // deleteShader;
    const int *tokens = ParseTokensInt(p, 1);
    GLuint shader = tokens[0];
    
    LOG_D("[webgl::exec] glDeleteShader(%d)", shader);
    glDeleteShader(shader);
    return kContinue;
}

int deleteTexture(GCanvas *obj, const char *&p)
{
    const int *tokens = ParseTokensInt(p, 1);
    GLuint texture = tokens[0];
    
    LOG_D("[webgl::exec] glDeleteTextures(1, %d)", texture);
    glDeleteTextures(1, &texture);
    return kContinue;
}

int depthFunc(GCanvas *obj, const char *&p)
{ // depthFunc1;
    const int *tokens = ParseTokensInt(p, 1);
    LOG_D("[webgl::exec] glDepthFunc(%s)", GetMacroValDebug(tokens[0]));
    glDepthFunc(tokens[0]);
    return kContinue;
}

int depthMask(GCanvas *obj, const char *&p)
{
    const int flag = ParseTokensInt(p, 1)[0];
    LOG_D("[webgl::exec] glDepthMask(%d)", flag);
    glDepthMask((bool)flag);
    return kContinue;
}

int depthRange(GCanvas *obj, const char *&p)
{
    const float *tokens = ParseTokensFloat(p, 2);
    LOG_D("[webgl::exec] glDepthRange(%f, %f)", tokens[0], tokens[1]);
    glDepthRangef(tokens[0], tokens[1]);
    return kContinue;
}

//new
int detachShader(GCanvas *obj, const char *&p)
{
    const float *tokens = ParseTokensFloat(p, 2);
    LOG_D("[webgl::exec] glDetachShader(%d, %d)", tokens[0], tokens[1]);
    glDetachShader(tokens[0], tokens[1]);
    return kContinue;
}

int disable(GCanvas *obj, const char *&p)
{ // disable0;
    const int *tokens = ParseTokensInt(p, 1);
    LOG_D("[webgl::exec] glDisable(%s)", GetMacroValDebug(tokens[0]));
    glDisable(tokens[0]);
    return kContinue;
}

int disableVertexAttribArray(GCanvas *obj, const char *&p)
{ // disableVertexAttribArray0;
    const int *tokens = ParseTokensInt(p, 1);
    LOG_D("[webgl::exec] glDisableVertexAttribArray(%d)",
              tokens[0]);
    glDisableVertexAttribArray(tokens[0]);
    return kContinue;
}

int drawArrays(GCanvas *obj, const char *&p)
{ // drawArrays4,0,3;
    const int *tokens = ParseTokensInt(p, 3);
    LOG_D("[webgl::exec] glDrawArrays(%s, %d, %d)", GetMacroValDebug(tokens[0]),
          tokens[1], tokens[2]);
    glDrawArrays(tokens[0], tokens[1], tokens[2]);
    LOG_D("after glDrawArrays");
    return kContinue;
}

int drawElements(GCanvas *obj, const char *&p)
{ // drawElements4,36,0,0;
    const int *tokens = ParseTokensInt(p, 4);
    LOG_D("[webgl::exec] glDrawElements(%s, %d, %s, %d)",
          GetMacroValDebug(tokens[0]), tokens[1], GetMacroValDebug(tokens[2]),
          tokens[3]);
    glDrawElements(tokens[0], tokens[1], tokens[2], G_BUFFER_OFFSET(tokens[3]));
    return kContinue;
}

int enable(GCanvas *obj, const char *&p)
{ // enable0;
    const int *tokens = ParseTokensInt(p, 1);
    LOG_D("[webgl::exec] glEnable(%s)", GetMacroValDebug(tokens[0]));
    glEnable(tokens[0]);
    return kContinue;
}

int enableVertexAttribArray(GCanvas *obj, const char *&p)
{ // enableVertexAttribArray10;
    GLuint index = ParseTokensInt(p, 1)[0];
    LOG_D("[webgl::exec] glEnableVertexAttribArray(%d)", index);
    glEnableVertexAttribArray(index);
    return kContinue;
}

int flush(GCanvas *obj, const char *&p)
{ // flush;
    LOG_D("[webgl::exec] glFlush()");
    glFlush();
    ++p;
    return kContinue;
}

int framebufferRenderbuffer(GCanvas *obj, const char *&p)
{ // framebufferRenderbuffer0,1,0,81;
    const int *tokens = ParseTokensInt(p, 4);
    LOG_D("[webgl::exec] glFramebufferRenderbuffer(GL_FRAMEBUFFER, %s, xGL_RENDERBUFFER, %d)",
              GetMacroValDebug(tokens[1]), tokens[3]);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, tokens[1], GL_RENDERBUFFER, tokens[3]);
    return kContinue;
}

int framebufferTexture2D(GCanvas *obj, const char *&p)
{ // framebufferTexture2D0,0,0,40,0;
    const int *tokens = ParseTokensInt(p, 5);
    LOG_D("[webgl::exec] glFramebufferTexture2D(GL_FRAMEBUFFER, %s, %s, %d, 0)",
          GetMacroValDebug(tokens[1]), GetMacroValDebug(tokens[2]), tokens[3], tokens[4]);
    glFramebufferTexture2D(GL_FRAMEBUFFER, tokens[1], tokens[2], tokens[3], tokens[4]);
    return kContinue;
}

int frontFace(GCanvas *obj, const char *&p)
{ // enable0;
    const int *tokens = ParseTokensInt(p, 1);
    LOG_D("[webgl::exec] glFrontFace(%s)", GetMacroValDebug(tokens[0]));
    glFrontFace(tokens[0]);
    return kContinue;
}

int generateMipmap(GCanvas *obj, const char *&p)
{ // generateMipmap0;
    GLenum target = ParseTokensInt(p, 1)[0];
    LOG_D("[webgl::exec] glGenerateMipmap(%s)", GetMacroValDebug(target));
    glGenerateMipmap(target);
    return kContinue;
}

int getActiveAttrib(GCanvas *obj, const char *&p)
{//getActiveAttrib
    const int *tokens = ParseTokensInt(p, 2);

    GLuint program = tokens[0];
    GLuint index = tokens[1];

    GLchar name[2048];
    GLsizei size = 0;
    GLsizei len = 0;
    GLenum type;

    glGetActiveAttrib(program, index, sizeof(name), &len, &size, &type, name);
    LOG_D("[webgl::exec] getActiveAttrib(%d, %d)=type:%s,size:%d,name:%s",
          program, index, GetMacroValDebug(type), size, name);
    if(len == 0){
        obj->setSyncResult("null");
    }else{
        //WebGLActiveInfo format: type,size,name
        std::string result = gcanvas::toString(type);
        result.append(",");
        result.append(gcanvas::toString(size));
        result.append(",");
        result.append(name);

        obj->setSyncResult(result);
    }

    return kContinue;
}

int getActiveUniform(GCanvas* obj, const char *&p)
{
    const int *tokens = ParseTokensInt(p, 2);
    GLuint program = tokens[0];
    GLuint index = tokens[1];

    GLchar name[2048];
    GLsizei size = 0;
    GLsizei len = 0;
    GLenum type;

    glGetActiveUniform(program, index, sizeof(name),&len,&size,&type,name);
    LOG_D("[webgl::exec] glGetActiveUniform(%d, %d)=type:%s,size:%d,name:%s",
          program, index, GetMacroValDebug(type), size, name);

    if(len == 0){
        obj->setSyncResult("null");
    }else{
        //WebGLActiveInfo format: type,size,name
        std::string result = gcanvas::toString(type);
        result.append(",");
        result.append(gcanvas::toString(size));
        result.append(",");
        result.append(name);

        obj->setSyncResult(result);
    }

    return kContinue;
}

int getAttachedShaders(GCanvas* obj, const char *&p)
{
    const int *tokens = ParseTokensInt(p, 1);
    GLuint program = tokens[0];
    GLsizei size = 0;
    GLuint shader[256];

    glGetAttachedShaders(program, 256, &size, shader);

    std::string result;
    for (int i=0; i<size; i++)
    {
        result.append(gcanvas::toString(shader[i]));
        if( i<size-1 )
            result.append(",");
    }
    LOG_D("[webgl::exec] glGetAttachedShaders(%d)=[%s]", program, result.c_str());

    obj->setSyncResult(result);
    return kContinue;
}

int getAttribLocation(GCanvas *obj, const char *&p)
{ // getAttribLocation31,position;
    const int *tokens = ParseTokensInt(p, 1);
    GLuint program = tokens[0];
    std::string &name = obj->mTempStr;
    ParseTokensString(p, name);

    GLint ret = glGetAttribLocation(program, name.c_str());
    LOG_D("[webgl::exec] glGetAttribLocation(%d, %s)=%d", program,
          name.c_str(), ret);
    
    obj->setSyncResult(gcanvas::toString(ret));
    return kContinue;
}

//new
int getBufferParameter(GCanvas *obj, const char *&p)
{
    const int *tokens = ParseTokensInt(p, 2);
    GLenum target = tokens[0];
    GLenum pname = tokens[1];

    GLint params = -1;
    glGetBufferParameteriv(target, pname, &params);
    LOG_D("[webgl::exec] glGetBufferParameteriv(%s, %s)=%d",
          GetMacroValDebug(target), GetMacroValDebug(pname), params);
    if(params == -1){
        obj->setSyncResult("null");
    }else{
        GLuint retType = kReturnInt; //int
        std::string result(gcanvas::toString(retType));
        result += ",";
        result += gcanvas::toString(params);
        obj->setSyncResult(result);
    }

    return kContinue;
}

//new OpenGL ES not Support
//int getContextAttributes(GCanvas *obj, const char *&p)
//{ // getContextAttributes
//    return kContinue;
//}

//new
int getError(GCanvas *obj, const char *&p)
{
    GLenum error = glGetError();
    ++p;
    LOG_D("[webgl::exec] glGetError()=%d", error);

    obj->setSyncResult(gcanvas::toString(error));
    return kContinue;
}

//new OpenGL ES not Support
int getExtension(GCanvas *obj, const char *&p)
{
    const char *extString = (const char *)glGetString(GL_EXTENSIONS);
    ++p;
    LOG_D("[webgl::exec] glGetString(GL_EXTENSIONS)=%s", extString);
    obj->setSyncResult(std::string(extString));
    return kContinue;
}

//new
int getFramebufferAttachmentParameter(GCanvas *obj, const char *&p)
{
    const int *tokens = ParseTokensInt(p, 3);
    GLenum target = tokens[0];
    GLenum attachment = tokens[1];
    GLenum pname = tokens[2];

    GLint params = -1;
    glGetFramebufferAttachmentParameteriv(target, attachment, pname, &params);
    LOG_D("[webgl::exec] glGetFramebufferAttachmentParameteriv(%s,%s,%s)=%d",
          GetMacroValDebug(target), GetMacroValDebug(attachment), GetMacroValDebug(pname), params);
    if(params == -1){
        obj->setSyncResult("null");
    }else{
        GLuint retType = kReturnInt; //int
        std::string result(gcanvas::toString(retType));
        result += ",";
        result += gcanvas::toString(params);
        obj->setSyncResult(result);
    }
    return kContinue;
}

//new
int getParameter(GCanvas *obj, const char *&p)
{
    //ref:https://www.khronos.org/registry/OpenGL-Refpages/es2.0/xhtml/glGet.xml

    //glGetBooleanv(<#GLenum pname#>, GLboolean *params)
    //glGetFloatv(<#GLenum pname#>, <#GLfloat *params#>)
    //glGetIntegerv(<#GLenum pname#>, <#GLint *params#>)
    //glGetString(<#GLenum name#>)

    const int *tokens = ParseTokensInt(p, 1);
    GLenum pname = tokens[0];

    switch (pname)
    {
        //return type - GLboolean
        case GL_BLEND:
        case GL_CULL_FACE:
        case GL_DEPTH_TEST:
        case GL_DEPTH_WRITEMASK:
        case GL_DITHER:
        case GL_POLYGON_OFFSET_FILL:
        case GL_SAMPLE_COVERAGE_INVERT:
        case GL_SCISSOR_TEST:
        case GL_STENCIL_TEST:
        {
            GLboolean bParams;
            glGetBooleanv(pname, &bParams);
            
            LOG_D("[webgl::exec] glGetParameter(%s)=%d, glGetBooleanv",
                  GetMacroValDebug(pname), bParams);
            GLuint retValue = bParams;
            GLuint retType = kReturnBoolean; //boolean
            std::string result(gcanvas::toString(retType));
            result += ",";
            result += gcanvas::toString(retValue);
            obj->setSyncResult(result);
            break;
        }

        //return type - GLenum
        case GL_ACTIVE_TEXTURE:
        case GL_BLEND_DST_ALPHA:
        case GL_BLEND_DST_RGB:
        case GL_BLEND_EQUATION_ALPHA:
        case GL_BLEND_EQUATION_RGB://same with GL_BLEND_EQUATION:
        case GL_BLEND_SRC_ALPHA:
        case GL_BLEND_SRC_RGB:
        case GL_CULL_FACE_MODE:
        case GL_DEPTH_FUNC:
        case GL_FRONT_FACE:
        case GL_GENERATE_MIPMAP_HINT:
        case GL_IMPLEMENTATION_COLOR_READ_FORMAT:
        case GL_IMPLEMENTATION_COLOR_READ_TYPE:
        case GL_STENCIL_BACK_FAIL:
        case GL_STENCIL_BACK_FUNC:
        case GL_STENCIL_BACK_PASS_DEPTH_FAIL:
        case GL_STENCIL_BACK_PASS_DEPTH_PASS:
        case GL_STENCIL_FAIL:
        case GL_STENCIL_FUNC:
        case GL_STENCIL_PASS_DEPTH_FAIL:
        case GL_STENCIL_PASS_DEPTH_PASS:

        //return type - GLuint
        case GL_STENCIL_BACK_VALUE_MASK:
        case GL_STENCIL_BACK_WRITEMASK:
        case GL_STENCIL_VALUE_MASK:
        case GL_STENCIL_WRITEMASK:{
            GLint iParams;
            glGetIntegerv(pname, &iParams);

            LOG_D("[webgl::exec] getParameter(%s)=%d, glGetIntegerv",
                  GetMacroValDebug(pname), iParams);

            GLuint retType = kReturnInt; //int
            std::string result(gcanvas::toString(retType));
            result += ",";
            result += gcanvas::toString((unsigned int)iParams);
            obj->setSyncResult(result);
            break;
        }

        //return type - GLint
        case GL_ALPHA_BITS:
        case GL_BLUE_BITS:
        case GL_DEPTH_BITS:
        case GL_GREEN_BITS:
        case GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS:
        case GL_MAX_CUBE_MAP_TEXTURE_SIZE:
        case GL_MAX_FRAGMENT_UNIFORM_VECTORS:
        case GL_MAX_RENDERBUFFER_SIZE:
        case GL_MAX_TEXTURE_IMAGE_UNITS:
        case GL_MAX_TEXTURE_SIZE:
        case GL_MAX_VARYING_VECTORS:
        case GL_MAX_VERTEX_ATTRIBS:
        case GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS:
        case GL_MAX_VERTEX_UNIFORM_VECTORS:
        case GL_PACK_ALIGNMENT:
        case GL_RED_BITS:
        case GL_SAMPLE_BUFFERS:
        case GL_SAMPLES:
        case GL_STENCIL_BACK_REF:
        case GL_STENCIL_BITS:
        case GL_STENCIL_CLEAR_VALUE:
        case GL_STENCIL_REF:
        case GL_SUBPIXEL_BITS:
        case GL_UNPACK_ALIGNMENT:

        case GL_NUM_COMPRESSED_TEXTURE_FORMATS:

        //return type - Buffer & Program & Texture
        case GL_ARRAY_BUFFER_BINDING:
        case GL_CURRENT_PROGRAM:
        case GL_ELEMENT_ARRAY_BUFFER_BINDING:
        case GL_FRAMEBUFFER_BINDING:
        case GL_RENDERBUFFER_BINDING:
        case GL_TEXTURE_BINDING_2D:
        case GL_TEXTURE_BINDING_CUBE_MAP:
        {
            GLint iParams;
            glGetIntegerv(pname, &iParams);
            
            LOG_D("[webgl::exec] getParameter(%s)=%d, glGetIntegerv",
                  GetMacroValDebug(pname), iParams);
            
            GLuint retType = kReturnInt; //int
            std::string result(gcanvas::toString(retType));
            result += ",";
            result += gcanvas::toString(iParams);
            obj->setSyncResult(result);
            break;
        }

        //return type - GLFloat
        case GL_DEPTH_CLEAR_VALUE:
        case GL_LINE_WIDTH:
        case GL_POLYGON_OFFSET_FACTOR:
        case GL_POLYGON_OFFSET_UNITS:
        case GL_SAMPLE_COVERAGE_VALUE:
        {
            GLfloat fParams;
            glGetFloatv(pname, &fParams);
            
            LOG_D("[webgl::exec] getParameter(%s)=%f, glGetFloatv",
                  GetMacroValDebug(pname), fParams);
            
            GLuint retType = kReturnFloat; //float
            std::string result = gcanvas::toString(retType);
            result += ",";
            result += gcanvas::toString(fParams);
            obj->setSyncResult(result);
            break;
        }

        //return type - IntArray 2 element
        case GL_MAX_VIEWPORT_DIMS: //	Int32Array (with 2 elements)
        {
            GLint values[2];
            glGetIntegerv(pname, values);

            GLuint retType = kReturnIntArray; //intArray
            std::string result = gcanvas::toString(retType);

            for (int i=0; i<2; i++)
            {
                result += ",";
                result += gcanvas::toString(values[i]);
            }
            LOG_D("[webgl::exec] getParameter(%s)=%f, glGetIntegerv-array",
                  GetMacroValDebug(pname), result.c_str());

            obj->setSyncResult(result);

            break;
        }

        //return type - IntArray 4 element
        case GL_SCISSOR_BOX: //Int32Array (with 4 elements)
        case GL_VIEWPORT:    //Int32Array (with 4 elements)
        {
            GLint values[4];
            glGetIntegerv(pname, values);

            GLuint retType = kReturnIntArray; //intArray
            std::string result = gcanvas::toString(retType);

            for (int i=0; i<4; i++)
            {
                GLint v = values[i];
                if( i>1 ){
                    v /= obj->mDevicePixelRatio;
                }

                result += ",";
                result += gcanvas::toString(v);
            }
            LOG_D("[webgl::exec] getParameter(%s)=%f, glGetIntegerv-array",
                  GetMacroValDebug(pname), result.c_str());
            obj->setSyncResult(result);
            break;
        }
        case GL_COLOR_WRITEMASK: //sequence<GLboolean> (with 4 values)
        {
            GLint values[4];
            glGetIntegerv(pname, values);

            GLuint retType = kReturnBooleanArray; //booleanArray
            std::string result = gcanvas::toString(retType);

            for (int i=0; i<4; i++)
            {
                GLint v = values[i];
                if( i>1 ){
                    v /= obj->mDevicePixelRatio;
                }

                result += ",";
                result += gcanvas::toString(v);
            }
            LOG_D("[webgl::exec] getParameter(%s)=%f, glGetIntegerv-array",
                  GetMacroValDebug(pname), result.c_str());
            obj->setSyncResult(result);
            break;
        }

        //return type - FloatArray 2 element
        case GL_ALIASED_LINE_WIDTH_RANGE: //Float32Array (with 2 elements)
        case GL_ALIASED_POINT_SIZE_RANGE: //Float32Array (with 2 elements)
        case GL_DEPTH_RANGE:
        {
            GLfloat values[2];
            glGetFloatv(pname, values);

            GLuint retType = kReturnFloatArray; //FloatArray
            std::string result = gcanvas::toString(retType);

            for (int i=0; i<2; i++)
            {
                result += ",";
                result += gcanvas::toString(values[i]);
            }
            
            LOG_D("[webgl::exec] getParameter(%s)=%f, glGetFloatv-array",
                  GetMacroValDebug(pname), result.c_str());
            obj->setSyncResult(result);
            break;
        }

        //return type - FloatArray 4 element
        case GL_BLEND_COLOR:              //Float32Array (with 4 values)
        case GL_COLOR_CLEAR_VALUE:        //Float32Array (with 4 values)
        {
            GLfloat values[4];
            glGetFloatv(pname, values);

            GLuint retType = kReturnFloatArray; //FloatArray
            std::string result = gcanvas::toString(retType);

            for (int i=0; i<4; i++)
            {
                result += ",";
                result += gcanvas::toString(values[i]);
            }
            
            LOG_D("[webgl::exec] getParameter(%s)=%f, glGetFloatv-array",
                  GetMacroValDebug(pname), result.c_str());
            
            obj->setSyncResult(result);
            break;
        }

        //return type - String
        case GL_RENDERER:
        case GL_SHADING_LANGUAGE_VERSION:
        case GL_VENDOR:
        case GL_VERSION:
        {
            const GLubyte* charResult = glGetString(pname);

            GLuint retType = kReturnString; //String
            std::string result = gcanvas::toString(retType);
            result += ",";
            result += gcanvas::toString(charResult);
            
            LOG_D("[webgl::exec] getParameter(%s)=%f, glGetString-array",
                  GetMacroValDebug(pname), result.c_str());

            obj->setSyncResult(result);
            break;
        }

        case GL_COMPRESSED_TEXTURE_FORMATS:
        {
            GLint numFormats = 0;
            glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, &numFormats);
            GLint* formats = new GLint[numFormats];
            glGetIntegerv(GL_COMPRESSED_TEXTURE_FORMATS, formats);

            GLuint retType = kReturnUintArray; //UintArray
            std::string result = gcanvas::toString(retType);

            for (int i=0; i<numFormats; i++)
            {
                result += ",";
                result += gcanvas::toString((unsigned int)formats[i]);
            }
            
            LOG_D("[webgl::exec] getParameter(%s)=%f, glGetIntegerv-array",
                  GetMacroValDebug(pname), result.c_str());


            obj->setSyncResult(result);
            delete [] formats;
            break;
        }
        default:
        {
            LOG_D("[webgl::exec] getParameter(%s) is undefined!!", GetMacroValDebug(pname));
            GLint iParams;
            glGetIntegerv(pname, &iParams);

            GLuint retType = kReturnInt; //int
            std::string result(gcanvas::toString(retType));
            result += ",";
            result += gcanvas::toString(iParams);
            obj->setSyncResult(result);
            break;
        }
    }

    return kContinue;
}

//new
int getProgramInfoLog(GCanvas *obj, const char *&p)
{
    const int *tokens = ParseTokensInt(p, 1);
    GLuint program = tokens[0];

    GLchar infolog[2048];
    GLsizei len;
    glGetProgramInfoLog(program, sizeof(infolog), &len, infolog);
    LOG_D("[webgl::exec] glGetProgramInfoLog(%d)=%s", program, infolog);
    
    std::string result(infolog);
    obj->setSyncResult(result);
    return kContinue;
}

//new
int getProgramParameter(GCanvas *obj, const char *&p)
{
    const int *tokens = ParseTokensInt(p, 2);
    GLuint program = tokens[0];
    GLenum pname = tokens[1];

    GLint params = -1;
    glGetProgramiv(program, pname, &params);
    switch(pname){
        //returnType GLBoolean
        case GL_DELETE_STATUS:
        case GL_LINK_STATUS:
        case GL_VALIDATE_STATUS:{
            if(params == -1){
                obj->setSyncResult("null");
            }else{
                GLuint retType = kReturnBoolean; //boolean
                std::string result(gcanvas::toString(retType));
                result += ",";
                result += gcanvas::toString(params);
                obj->setSyncResult(gcanvas::toString(result));
            }
            break;
        }
        //returnType GLint
        case GL_ATTACHED_SHADERS:
        case GL_ACTIVE_ATTRIBUTES:
        case GL_ACTIVE_UNIFORMS:{
            if(params == -1){
                obj->setSyncResult("null");
            }else{
                GLuint retType = kReturnInt; //int
                std::string result(gcanvas::toString(retType));
                result += ",";
                result += gcanvas::toString(params);
                obj->setSyncResult(gcanvas::toString(result));
            }
            break;
        }

    }
    LOG_D("[webgl::exec] glGetProgramiv(%d, %s, %d)", program, GetMacroValDebug(pname), params);


    return kContinue;
}

//new
int getRenderbufferParameter(GCanvas *obj, const char *&p)
{
    const int *tokens = ParseTokensInt(p, 2);
    GLenum target = tokens[0];
    GLenum pname = tokens[1];

    GLint params = -1;
    glGetRenderbufferParameteriv(target, pname, &params);

    LOG_D("[webgl::exec] glGetRenderbufferParameteriv(%s, %s, %d)",
          GetMacroValDebug(target), GetMacroValDebug(pname), params);
    if(params == -1){
        obj->setSyncResult("null");
    }else{
        GLuint retType = kReturnInt; //int
        std::string result(gcanvas::toString(retType));
        result += ",";
        result += gcanvas::toString(params);
        obj->setSyncResult(result);
    }


    return kContinue;
}

//new
int getShaderInfoLog(GCanvas* obj, const char *&p)
{
    const int *tokens = ParseTokensInt(p, 1);
    GLuint shader = tokens[0];

    GLchar infolog[2048];
    GLsizei len = 0;

    glGetShaderInfoLog(shader, sizeof(infolog), &len, infolog);

    std::string result(infolog);
    LOG_D("[webgl::exec] glGetShaderInfoLog(%d, %d, %d, %s)", shader, sizeof(infolog), len, infolog);
    obj->setSyncResult(result);

    return kContinue;
}



int getShaderParameter(GCanvas *obj, const char *&p)
{ // getShaderParameter1;
    const int *tokens = ParseTokensInt(p, 2);
    GLuint shader = tokens[0];
    GLenum pname = tokens[1];

    GLint params = -1;
    glGetShaderiv(shader, pname, &params);
    LOG_D("[webgl::exec] glGetShaderiv(%d, %d, %d)", shader, GetMacroValDebug(pname), params);

    switch (pname){
        //returnType GLboolean
        case GL_DELETE_STATUS:
        case GL_COMPILE_STATUS:{
            if(params == -1){
                obj->setSyncResult("null");
            }else{
                GLuint retType = kReturnBoolean; //boolean
                std::string result(gcanvas::toString(retType));
                result += ",";
                result += gcanvas::toString(params);
                obj->setSyncResult(result);
            }
            break;
        }
        //returnType GLenum
        case GL_SHADER_TYPE:
            if(params == -1){
                obj->setSyncResult("null");
            }else{
                GLuint retType = kReturnInt; //int
                std::string result(gcanvas::toString(retType));
                result += ",";
                result += gcanvas::toString(params);
                obj->setSyncResult(result);
            }
            break;
    }
    return kContinue;
}

//new
int getShaderPrecisionFormat(GCanvas *obj, const char *&p)
{ // getShaderPrecisionFormat;
    const int *tokens = ParseTokensInt(p, 2);
    GLenum shadertype = tokens[0];
    GLenum precisiontype = tokens[1];

    GLint range[2] = {-1,-1};
    GLint precision = -1;

    glGetShaderPrecisionFormat(shadertype, precisiontype, range, &precision);
    LOG_D("[webgl::exec] glGetShaderPrecisionFormat(%s, %s, %d, %d, %d)",
          GetMacroValDebug(shadertype), GetMacroValDebug(precisiontype), range[0], range[1], precision);
    if(range[0] == -1){
        obj->setSyncResult("null");
    }else{
        //format range[0],range[1],precision
        std::string result;
        for (int i=0; i<2; i++) {
            result.append(gcanvas::toString(range[i]));
            result.append(",");
        }
        result.append(gcanvas::toString(precision));

        obj->setSyncResult(result);
    }

    return kContinue;
}

//new
int getShaderSource(GCanvas *obj, const char *&p)
{
    const int *tokens = ParseTokensInt(p, 1);
    GLuint shader = tokens[0];

    GLchar source[4096];
    GLsizei len = 0;

    glGetShaderSource(shader, sizeof(source), &len, source);
    LOG_D("[webgl::exec] glGetShaderSource(%d, %d, %d, %s)", shader, sizeof(source), len, source);

    std::string result(source);
    obj->setSyncResult(result);
    return kContinue;
}

//new
int getSupportedExtensions(GCanvas *obj, const char *&p)
{
    const char *extString = (const char *)glGetString(GL_EXTENSIONS);
    ++p;
    LOG_D("[webgl::exec] glGetString(GL_EXTENSIONS)=%s", extString);

    obj->setSyncResult(std::string(extString));
    return kContinue;
}

//new
int getTexParameter(GCanvas *obj, const char *&p)
{
    //https://www.khronos.org/registry/OpenGL-Refpages/es2.0/xhtml/glGetTexParameter.xml
    //glGetTexParameterfv(<#GLenum target#>, <#GLenum pname#>, <#GLfloat *params#>)
    //glGetTexParameteriv(<#GLenum target#>, <#GLenum pname#>, <#GLint *params#>)

    //WebGL 1.0 only support glGetTexParameteriv
    const int *tokens = ParseTokensInt(p, 2);
    GLenum target = tokens[0];
    GLenum pname = tokens[1];

    GLint params = -1;
    glGetTexParameteriv(target, pname, &params);
    LOG_D("[webgl::exec] glGetTexParameteriv(%s, %s)=%d", GetMacroValDebug(target), GetMacroValDebug(pname), params);
    if(params == -1){
        obj->setSyncResult("null");
    }else{
        GLuint retType = kReturnInt; //int
        std::string result(gcanvas::toString(retType));
        result += ",";
        result += gcanvas::toString(params);
        obj->setSyncResult(result);
    }
    return kContinue;
}

//new
int getUniform(GCanvas *obj, const char *&p)
{
    const int *tokens = ParseTokensInt(p, 2);
    GLint program = tokens[0];
    GLint location = tokens[1];

    GLint num;
    glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &num);

    for (int index=0; index<num; index++)
    {
        GLchar name[2048];
        GLsizei size = 0;
        GLsizei len = 0;
        GLenum type;

        glGetActiveUniform(program, index, sizeof(name), &len, &size, &type, name);
        GLint loc = glGetUniformLocation(program, name);

        if( loc == location )
        {
            std::string result = "";
            //get type
            LOG_D("getUniform,loc=%d,type=%d\n",loc,type);
            switch (type)
            {
                case GL_INT:
                case GL_BOOL:
                case GL_SAMPLER_2D:
                case GL_SAMPLER_CUBE:
                {
                    LOG_D("start get [GL_INT,GL_BOOL,GL_SAMPLER_2D,GL_SAMPLER_CUBE].");
                    GLint iParam;
                    glGetUniformiv(program, location, &iParam);
                    LOG_D("end get,para=%d\n",iParam);
                    LOG_D("[webgl::exec] glGetUniformiv(%d, %d)=%d", program, location, iParam);
                    result = gcanvas::toString(kReturnInt);
                    result.append(",");
                    result.append(gcanvas::toString(iParam));
                    break;
                }
                case GL_INT_VEC2:
                case GL_BOOL_VEC2:
                case GL_INT_VEC3:
                case GL_BOOL_VEC3:
                case GL_INT_VEC4:
                case GL_BOOL_VEC4:
                {
                    LOG_D("start get [GL_INT_VEC2,GL_BOOL_VEC2,GL_INT_VEC3,GL_BOOL_VEC3,GL_INT_VEC4,GL_BOOL_VEC4].");
                    GLint iParams[4];
                    glGetUniformiv(program, location, iParams);
                    result = gcanvas::toString(kReturnIntArray);
                    LOG_D("end get");

                    int paraNum;
                    if( type == GL_INT_VEC2 || type == GL_BOOL_VEC2  ) paraNum = 2;
                    else if(type == GL_INT_VEC3 || type == GL_BOOL_VEC3) paraNum = 3;
                    else paraNum = 4;

                    for (int i=0; i<paraNum; i++) {
                        result.append(",");
                        LOG_D("num=%d,param=%d\n",i,iParams[i]);
                        result.append(gcanvas::toString(iParams[i]));
                    }
                    LOG_D("[webgl::exec] glGetUniformiv(%d, %d)=[%s]", program, location, result.c_str());
                    break;
                }

                case GL_FLOAT:
                {
                    LOG_D("start get [GL_FLOAT].");
                    GLfloat fParam;
                    glGetUniformfv(program, location, &fParam);
                    LOG_D("end get,param=%f",fParam);
                    LOG_D("[webgl::exec] glGetUniformfv(%d, %d)=%f", program, location, fParam);
                    result = gcanvas::toString(kReturnFloat);
                    result.append(",");
                    result.append(gcanvas::toString(fParam));
                    break;
                }

                case GL_FLOAT_VEC2:
                case GL_FLOAT_MAT2:
                case GL_FLOAT_VEC3:
                case GL_FLOAT_MAT3:
                case GL_FLOAT_VEC4:
                case GL_FLOAT_MAT4:
                {
                    LOG_D("start get [GL_FLOAT_VEC2,GL_FLOAT_MAT2,GL_FLOAT_VEC3,GL_FLOAT_MAT3,GL_FLOAT_VEC4,GL_FLOAT_MAT4].");
                    GLfloat fParams[4];
                    glGetUniformfv(program, location, fParams);
                    LOG_D("end get");
                    result = gcanvas::toString(kReturnFloatArray);

                    int paraNum;
                    if( type == GL_FLOAT_VEC2 || type == GL_FLOAT_MAT2  ) paraNum = 2;
                    else if(type == GL_FLOAT_VEC3 || type == GL_FLOAT_MAT3) paraNum = 3;
                    else paraNum = 4;

                    for (int i=0; i<paraNum; i++) {
                        result.append(",");
                        LOG_D("num=%d,param=%f\n",i,fParams[i]);
                        result.append(gcanvas::toString(fParams[i]));
                    }
                    LOG_D("[webgl::exec] glGetUniformfv(%d, %d)=[%s]", program, location, result.c_str());
                    break;
                }
                default:
                    break;
            }

            if( !result.empty() )
            {
                obj->setSyncResult(result);
            }

            break;
        }
    }
    return kContinue;
}


int getUniformLocation(GCanvas *obj, const char *&p)
{ // getUniformLocation1,s_texture;
    const int *tokens = ParseTokensInt(p, 1);
    GLuint program = tokens[0];
    std::string &name = obj->mTempStr;
    ParseTokensString(p, name);

    GLint ret = glGetUniformLocation(program, name.c_str());
    LOG_D("[webgl::exec] glGetUniformLocation(%d, %s)=%d", program, name.c_str(), ret);
    if(ret == -1){
        obj->setSyncResult("null");
    }else{
        obj->setSyncResult(gcanvas::toString(ret));
    }
    return kContinue;
}

//new
int getVertexAttrib(GCanvas *obj, const char *&p)
{ //https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext/getVertexAttrib
    //glGetVertexAttribfv(<#GLuint index#>, <#GLenum pname#>, <#GLfloat *params#>)
    //glGetVertexAttribiv(<#GLuint index#>, <#GLenum pname#>, <#GLint *params#>)
    const int *tokens = ParseTokensInt(p, 2);
    GLuint index = tokens[0];
    GLenum pname = tokens[1];

    switch (pname) {
        case GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING:
        case GL_VERTEX_ATTRIB_ARRAY_ENABLED:
        case GL_VERTEX_ATTRIB_ARRAY_SIZE:
        case GL_VERTEX_ATTRIB_ARRAY_TYPE:
        case GL_VERTEX_ATTRIB_ARRAY_NORMALIZED:
        {
            GLint params = -1;
            glGetVertexAttribiv(index, pname, &params);
            LOG_D("[webgl::exec] glGetVertexAttribiv(%d, %s)=%d", index, GetMacroValDebug(pname), params);

            if(params == -1){
                obj->setSyncResult("null");
            }else {
                GLuint retType = kReturnInt; //Int
                std::string result = gcanvas::toString(retType);
                result += ",";
                result += gcanvas::toString(params);
                obj->setSyncResult(result);
            }
            break;
        }

        case GL_CURRENT_VERTEX_ATTRIB:
        {
            GLfloat params[4] = {-1,-1,-1,-1};
            glGetVertexAttribfv(index, pname, params);
            LOG_D("[webgl::exec] glGetVertexAttribfv(%d, %s)=(%f, %f, %f, %f)",
                  index, GetMacroValDebug(pname), params[0], params[1], params[2], params[3]);

            GLuint retType = kReturnFloatArray; //FloatArray
            std::string result = gcanvas::toString(retType);

            if(params[0] == -1){
                obj->setSyncResult("null");
            }else{
                for (int i=0; i<4; i++)
                {
                    result += ",";
                    result += gcanvas::toString(params[i]);
                }
                obj->setSyncResult(result);
            }

            break;
        }

        default:
            break;
    }

    return kContinue;
}


//new
int getVertexAttribOffset(GCanvas *obj, const char *&p)
{ // getVertexAttribOffset;
    const int *tokens = ParseTokensInt(p, 2);
    GLuint index = tokens[0];
//    GLenum pname = tokens[1];

    GLvoid *pointer;
    glGetVertexAttribPointerv(index, GL_VERTEX_ATTRIB_ARRAY_POINTER, &pointer);

    int value = (int)(reinterpret_cast<size_t>(pointer));
    obj->setSyncResult(gcanvas::toString(value));
    return kContinue;
}

//new
int isBuffer(GCanvas *obj, const char *&p)
{
    const int *tokens = ParseTokensInt(p, 1);
    GLuint buffer = tokens[0];

    GLuint ret = glIsBuffer(buffer);
    LOG_D("[webgl::exec] glIsBuffer(%d)=%d", buffer, ret);

    obj->setSyncResult(gcanvas::toString(ret));
    return kContinue;
}

//new
int isEnabled(GCanvas *obj, const char *&p)
{
    const int *tokens = ParseTokensInt(p, 1);
    GLenum cap = tokens[0];

    GLuint ret = glIsEnabled(cap);
    LOG_D("[webgl::exec] glIsEnabled(%d)=%d", GetMacroValDebug(cap), ret);

    obj->setSyncResult(gcanvas::toString(ret));
    return kContinue;
}

//new
int isFramebuffer(GCanvas *obj, const char *&p)
{
    const int *tokens = ParseTokensInt(p, 1);
    GLuint framebuffer = tokens[0];

    GLuint ret = glIsFramebuffer(framebuffer);
    LOG_D("[webgl::exec] glIsFramebuffer(%d)=%d", framebuffer, ret);

    obj->setSyncResult(gcanvas::toString(ret));
    return kContinue;
}

//new
int isProgram(GCanvas *obj, const char *&p)
{
    const int *tokens = ParseTokensInt(p, 1);
    GLuint program = tokens[0];

    GLuint ret = glIsProgram(program);
    LOG_D("[webgl::exec] glIsProgram(%d)=%d", program, ret);

    obj->setSyncResult(gcanvas::toString(ret));
    return kContinue;
}

//new
int isRenderbuffer(GCanvas *obj, const char *&p)
{
    const int *tokens = ParseTokensInt(p, 1);
    GLuint renderbuffer = tokens[0];

    GLuint ret = glIsRenderbuffer(renderbuffer);
    LOG_D("[webgl::exec] glIsRenderbuffer(%d)=%d", renderbuffer, ret);

    obj->setSyncResult(gcanvas::toString(ret));
    return kContinue;
}

//new
int isShader(GCanvas *obj, const char *&p)
{
    const int *tokens = ParseTokensInt(p, 1);
    GLuint shader = tokens[0];

    GLuint ret = glIsShader(shader);
    LOG_D("[webgl::exec] glIsShader(%d)=%d", shader, ret);

    obj->setSyncResult(gcanvas::toString(ret));
    return kContinue;
}

//new
int isTexture(GCanvas *obj, const char *&p)
{
    const int *tokens = ParseTokensInt(p, 1);
    GLuint texture = tokens[0];

    GLuint ret = glIsTexture(texture);
    LOG_D("[webgl::exec] glIsShader(%d)=%d", texture, ret);

    obj->setSyncResult(gcanvas::toString(ret));
    return kContinue;
}

int lineWidth(GCanvas *obj, const char *&p)
{ // lineWidth1;
    const GLfloat *tokens = ParseTokensFloat(p, 1);
    LOG_D("[webgl::exec] glLineWidth(%f)", tokens[0]);
    glLineWidth(tokens[0]);
    return kContinue;
}

int linkProgram(GCanvas *obj, const char *&p)
{ // linkProgram1
    const int *tokens = ParseTokensInt(p, 1);
    GLuint program = tokens[0];

    glLinkProgram(program);
    LOG_D("[webgl::exec] glLinkProgram(%d)", program);
    return kContinue;
}

int pixelStorei(GCanvas *obj, const char *&p)
{ // pixelStorei2,1
    const int *tokens = ParseTokensInt(p, 2);
    GLenum pname = tokens[0];
    GLint param = tokens[1];

    glPixelStorei(pname, param);
    LOG_D("[webgl::exec] glPixelStorei(%s, %d)", GetMacroValDebug(pname), param);
    return kContinue;
}

int polygonOffset(GCanvas *obj, const char *&p)
{ // polygonOffset1.0,1.0);
    const float *tokens = ParseTokensFloat(p, 2);
    GLfloat factor = tokens[0];
    GLfloat units = tokens[1];

    LOG_D("[webgl::exec] glPolygonOffset(%f, %f)", factor, units);
    glPolygonOffset(factor, units);
    return kContinue;
}

int readPixels(GCanvas *obj, const char *&p)
{
    const int *tokens = ParseTokensInt(p, 6);
    GLint x = tokens[0] * obj->mDevicePixelRatio;
    GLint y = tokens[1] * obj->mDevicePixelRatio;
    GLsizei width = tokens[2] * obj->mDevicePixelRatio;
    GLsizei height = tokens[3] * obj->mDevicePixelRatio;
    GLenum format = tokens[4];
    GLenum type = tokens[5];

    GLuint pixelSize = 4 * width * height;
    GLubyte *pixels = new GLubyte[pixelSize];
    glReadPixels(x, y, width, height, format, type, pixels);

    LOG_D("[webgl::exec] readPixels(%d, %d, %d, %d, %s, %s)", x, y, width, height,
          GetMacroValDebug(format), GetMacroValDebug(type));

    //convert pixels to string
    GLuint retType = kReturnUintArray; //UintArray
    std::string result = gcanvas::toString(retType);

    for (int i=0; i<pixelSize; i++)
    {
        result += ",";
        result += gcanvas::toString((unsigned int)(pixels[i] & 0xff));
    }
    obj->setSyncResult(result);

    delete[] pixels;
    return kContinue;
}

int renderbufferStorage(GCanvas *obj, const char *&p)
{ // renderbufferStorage0,3,512,512;
    const int *tokens = ParseTokensInt(p, 4);
    GLenum target = tokens[0];
    GLenum internalformat = tokens[1];
    GLsizei width = tokens[2] * obj->mDevicePixelRatio;
    GLsizei height = tokens[3] * obj->mDevicePixelRatio;

    LOG_D("[webgl::exec] glRenderbufferStorage(%s, %s, %d, %d)",
          GetMacroValDebug(target), GetMacroValDebug(internalformat), width, height);
    glRenderbufferStorage(target, internalformat, width, height);
    return kContinue;
}

//new
int sampleCoverage(GCanvas *obj, const char *&p)
{
    const float *fTokens = ParseTokensFloat(p, 1);
    GLfloat value = fTokens[0];

    const int *tokens = ParseTokensInt(p, 1);
    GLuint invert = tokens[0];

    LOG_D("[webgl::exec] glSampleCoverage(%f, %d)", value, invert);
    glSampleCoverage(value, invert);
    return kContinue;
}

//new
int scissor(GCanvas *obj, const char *&p)
{
    const int *tokens = ParseTokensInt(p, 4);
    GLint x = tokens[0] * obj->mDevicePixelRatio;
    GLint y = tokens[1] * obj->mDevicePixelRatio;
    GLsizei width = tokens[2] * obj->mDevicePixelRatio;
    GLsizei height = tokens[3] * obj->mDevicePixelRatio;

    LOG_D("[webgl::exec] glScissor(%d, %d, %d, %d)", x, y, width, height);
    glScissor(x, y, width, height);
    return kContinue;
}


int shaderSource(GCanvas *obj, const char *&p)
{ // shaderSource1,CiAgICAgICAgYXR0
    const int *tokens = ParseTokensInt(p, 1);
    GLuint shader = tokens[0];

    std::string &source = obj->mTempStr;
//    GLint len = ParseTokensBase64(p, source);
//    const char *src = source.c_str();

    char *begin = const_cast<char*>(p);
    while (*p && (*p != '\0'))
    {
        ++p;
    }
    LOG_D("p: %s",p);
    GLint len = (int)(p - begin);

    if (p <= begin){
        source.clear();
        return 0;
    }

    if (len > (int)source.length()) source.resize((size_t)len);
    const char *src = begin;
    
    glShaderSource(shader, 1, &src, &len);
    LOG_D("[webgl::exec] glShaderSource(%d, %d, %d, [%s]) ",
          shader, 1, len, src);
    return kContinue;
}

int stencilFunc(GCanvas *obj, const char *&p)
{//glStencilFunc(GL_LESS, 0x1, 0x1);

    const int *tokens = ParseTokensInt(p, 3);
    GLenum func = tokens[0];
    GLint ref = tokens[1];
    GLuint mask = tokens[2];

    glStencilFunc(func, ref, mask);
    LOG_D("[webgl::exec] glStencilFunc(%s, %d, %d)",
          GetMacroValDebug(func), ref, mask);
    return kContinue;
}

//new
int stencilFuncSeparate(GCanvas *obj, const char *&p)
{
    const int *tokens = ParseTokensInt(p, 4);
    GLenum face = tokens[0];
    GLenum func = tokens[1];
    GLint ref = tokens[2];
    GLuint mask = tokens[3];

    glStencilFuncSeparate(face, func, ref, mask);
    LOG_D("[webgl::exec] glStencilFunc(%s, %s, %d, %d)",
          GetMacroValDebug(face), GetMacroValDebug(func), ref, mask);
    return kContinue;
}

//new
int stencilMask(GCanvas *obj, const char *&p)
{
    const int *tokens = ParseTokensInt(p, 1);
    GLuint mask = tokens[0];

    glStencilMask(mask);
    LOG_D("[webgl::exec] glStencilMask(%d)", mask);
    return kContinue;
}

//new
int stencilMaskSeparate(GCanvas *obj, const char *&p)
{
    const int *tokens = ParseTokensInt(p, 2);
    GLenum face = tokens[0];
    GLuint mask = tokens[1];

    glStencilMaskSeparate(face, mask);
    LOG_D("[webgl::exec] glStencilMaskSeparate(%s, %d)", GetMacroValDebug(face),mask);
    return kContinue;
}

int stencilOp(GCanvas* obj, const char* &p)
{
    const int* tokens = ParseTokensInt(p,3);
    GLenum fail = tokens[0];
    GLenum zfail = tokens[1];
    GLenum zpass = tokens[2];

    glStencilOp(fail, zfail, zpass);
    LOG_D("[webgl::exec] glStencilOp(%s, %s, %s)", GetMacroValDebug(fail), GetMacroValDebug(zfail), GetMacroValDebug(zpass));
    return kContinue;
}

int stencilOpSeparate(GCanvas* obj, const char* &p)
{
    const int* tokens = ParseTokensInt(p,4);
    GLenum face = tokens[0];
    GLenum fail = tokens[1];
    GLenum zfail = tokens[2];
    GLenum zpass = tokens[3];

    glStencilOpSeparate(face, fail, zfail, zpass);
    LOG_D("[webgl::exec] glStencilOpSeparate(%s, %s, %s, %s)",
          GetMacroValDebug(face), GetMacroValDebug(fail), GetMacroValDebug(zfail), GetMacroValDebug(zpass));
    return kContinue;
}

extern int (*g_webglFuncMap[WEBGL_API_COUNT])(GCanvas *, const char *&);
extern int (*g_webglExtFuncMap[WEBGL_EXT_API_COUNT])(GCanvas *, const char *&);

int texImage2D(GCanvas *obj, const char *&p)
{ //  texImage2D0,0,3,3,0,a.bmp;
    const int args = ParseTokensInt(p, 1)[0];
    if (6 == args)
    {
        //   void texImage2D(GLenum target, GLint level, GLint internalformat,
        //                   GLenum format, GLenum type, TexImageSource source)
        const int *tokens = ParseTokensInt(p, 5);
        GLenum target = tokens[0];
        GLint level = tokens[1];
        GLenum internalformat = tokens[2];
        GLenum format = tokens[3];
        GLenum type = tokens[4];

        std::string &src = obj->mTempStr;
        if( strncmp(p, "data:image", 10) == 0 ) //eg: data:image/png;base64,xxxxxx;
        {
            strncpy((char*)src.c_str(), p, strlen(p)-1); //去掉最后分号
            LOG_D("[texImage2D] src=data:image, base64");
        }
        else
        {
            ParseTokensString(p, src);
            LOG_D("[texImage2D] src=%s", src.c_str());
        }

#ifdef IOS
        GWebGLTxtImage2DFunc func = obj->GetGWebGLTxtImage2DFunc();
        if (func)
        {
            func(target, level, internalformat, format, type, src.c_str());
        }
#endif

#ifdef ANDROID
//        int next_cmd = atoi(p);
//        jint flush_flag = 1;
//
//        // if next cmd still is texImage2D, can unflush load cmd with
//        // java.primise has the same  param num .
//        if ((next_cmd >= 1) && (next_cmd <= WEBGL_API_COUNT) &&
//            (g_webglFuncMap[next_cmd] == texImage2D))
//        {
//            flush_flag = 0;
//        }
//
//        JNIEnv *je = obj->mJniEnv;
//        jobject &jCanvasRender = *(obj->mCanvasRender);
//        jclass cls = je->GetObjectClass(jCanvasRender);
//        jmethodID jmid =
//            je->GetMethodID(cls, "dealTexture", "(Ljava/lang/String;III)V");
//        jstring jparam = je->NewStringUTF(src.c_str());
//        jint jflipy = obj->mPixelFlipY;
//
//        LOG_D("[webgl::exec] glTexImage2D(%s, 0, %s, 0)", target, src.c_str());
//        je->CallVoidMethod(jCanvasRender, jmid, jparam, target, jflipy,
//                           flush_flag);

// TODO: here need call ReleaseString for free mem, ref is
// :http://blog.csdn.net/coutcin/article/details/1350245
// TODO: now only use src param,  buf before also need deal.
#endif

    }
    else
    {
        const int *tokens = ParseTokensInt(p, 9);
        const GLvoid * array = nullptr;
        LOG_D("texImage2D after parseTokensInt: %s", p);
        if(tokens[8] == 0 ){
            ParseTokensSkip(p);
        }else{
            std::string &buf = obj->mTempStr;
            ParseTokensBase64(p, buf);
            LOG_D("texImage2D after base64 : %s",buf.c_str());
            int bytes = tokens[8];
            unsigned int size;
            array = SplitStringToArray(buf.c_str(), bytes, size);
        }
        glTexImage2D(tokens[0], tokens[1], tokens[2], tokens[3], tokens[4],
                        tokens[5], tokens[6], tokens[7], array);

        LOG_D("[webgl::exec] glTexImage2D(%s, %d, %s, %d, %d, %d, %s, %s, array)",
              GetMacroValDebug(tokens[0]), tokens[1], GetMacroValDebug(tokens[2]), tokens[3],
              tokens[4], tokens[5], GetMacroValDebug(tokens[6]), GetMacroValDebug(tokens[7]),
              array);
    };
    return kContinue;
}

int texParameterf(GCanvas *obj, const char *&p)
{
    const int *tokens = ParseTokensInt(p, 2);
    GLenum target = tokens[0];
    GLenum pname = tokens[1];

    const float *floatTokens = ParseTokensFloat(p, 1);
    GLfloat param = floatTokens[0];

    LOG_D("[webgl::exec] glTexParameterf(%s, %s, %f) ",
          GetMacroValDebug(target), GetMacroValDebug(pname), param);
    glTexParameterf(target, pname, param);
    return kContinue;
}

int texParameteri(GCanvas *obj, const char *&p)
{ // texParameteri0,undefined,undefined;

    const int *tokens = ParseTokensInt(p, 3);
    GLenum target = tokens[0];
    GLenum pname = tokens[1];
    GLint param = tokens[2];

    LOG_D("[webgl::exec] glTexParameteri(%s, %s, %d) ",
          GetMacroValDebug(target), GetMacroValDebug(pname), param);
    glTexParameteri(target, pname, param);
    return kContinue;
}


//void gl.texSubImage2D(target, level, xoffset, yoffset, width, height, format, type, ArrayBufferView? pixels);
//void gl.texSubImage2D(target, level, xoffset, yoffset, format, type, ImageData? pixels);
int texSubImage2D(GCanvas *obj, const char *&p)
{
    const int args = ParseTokensInt(p, 1)[0];
    if (7 == args)
    {
        const int *tokens = ParseTokensInt(p, 6);

        GLenum target = tokens[0];
        GLint level = tokens[1];
        GLenum xoffset = tokens[2];
        GLenum yoffset = tokens[3];
        GLenum format = tokens[4];
        GLenum type = tokens[5];

        std::string &src = obj->mTempStr;
        if( strncmp(p, "data:image", 10) == 0 ) //eg: data:image/png;base64,xxxxxx;
        {
            strncpy((char*)src.c_str(), p, strlen(p)-1);
            LOG_D("[texImage2D] src=data:image, base64");
        }
        else
        {
            ParseTokensString(p, src);
            LOG_D("[texImage2D] src=%s", src.c_str());
        }

#ifdef IOS
        GWebGLTxtSubImage2DFunc func = obj->GetGWebGLTxtSubImage2DFunc();
        if (func)
        {
            func(target, level, xoffset, yoffset, format, type, src.c_str());
        }
#endif

#ifdef ANDROID
    //TODO
#endif

    }
    else if( 9 == args )
    {
        const int *tokens = ParseTokensInt(p, 9);

        int bytes = tokens[8];
        unsigned int size;
        const GLvoid * array = SplitStringToArray(p, bytes, size);
        glTexSubImage2D(tokens[0], tokens[1], tokens[2], tokens[3], tokens[4],
                        tokens[5], tokens[6], tokens[7], array);

        LOG_D("[webgl::exec] glTexSubImage2D(%s, %d, %d, %d, %d, %d, %s, %s, array)",
              GetMacroValDebug(tokens[0]), tokens[1], tokens[2], tokens[3],
              tokens[4], tokens[5], GetMacroValDebug(tokens[6]), GetMacroValDebug(tokens[7]),
              array);
    }
    return kContinue;
}


int uniformXfv(GCanvas *obj, const char *&p, int type)
{ // uniform3fv_53,/////wj/////
    const int *tokens = ParseTokensInt(p, 2);
    GLint location = tokens[0];

    std::string &value = obj->mTempStr;
    ParseTokensBase64(p, value);

    unsigned int size;
    const GLfloat *fvalue = (const GLfloat *)SplitStringToFloat32Array(value.c_str(), ",", size);

#ifdef DEBUG
//    unsigned int element = size / sizeof(GLfloat);
//    unsigned int element = size;
//    for (int i = 0; i < element; ++i){
        LOG_D("[webgl::glUniform%dfv] location=%d, fvalue[%d]=%f", type, location, 0, fvalue[0]);
//    }
#endif

    switch (type)
    {
        case 1:
            glUniform1fv(location, size, fvalue);
            break;
        case 2:
            glUniform2fv(location, size/2, fvalue);
            break;
        case 3:
            glUniform3fv(location, size/3, fvalue);
            break;
        case 4:
            glUniform4fv(location, size/4, fvalue);
            break;
    }

    return kContinue;
}

int uniformXiv(GCanvas *obj, const char *&p, int type)
{ // uniform3fv_53,/////wj/////
    const int *tokens = ParseTokensInt(p, 2);
    GLint location = tokens[0];

    std::string &value = obj->mTempStr;
    ParseTokensBase64(p, value);

    unsigned int size;
    const GLint *ivalue = (const GLint *)SplitStringToInt32Array(value.c_str(), ",", size);

#ifdef DEBUG
    unsigned int element = size / sizeof(GLint);
    for (int i = 0; i < element; ++i){
        LOG_D("[webgl::glUniform%div] ivalue[%d]=%f", type, i, ivalue[i]);
    }
#endif
    
    LOG_D("[webgl::exec] glUniform%div(%d, %d, %d)", type, location, 1, ivalue[0]);

    switch (type)
    {
        case 1:
            glUniform1iv(location, size, ivalue);
            break;
        case 2:
            glUniform2iv(location, size/2, ivalue);
            break;
        case 3:
            glUniform3iv(location, size/3, ivalue);
            break;
        case 4:
            glUniform4iv(location, size/4, ivalue);
            break;
    }

    if( ivalue )
    {
        free((void*)ivalue);
    }

    return kContinue;
}

int uniform1f(GCanvas *obj, const char *&p)
{ // uniform1f_4,0
    const int *intTokens = ParseTokensInt(p, 1);
    GLint location = intTokens[0];

    const float *tokens = ParseTokensFloat(p, 1);
    LOG_D("[webgl::exec] glUniform1f(%d, %f)", location, tokens[0]);
    glUniform1f(location, tokens[0]);
    return kContinue;
}

int uniform1fv(GCanvas *obj, const char *&p)
{ // uniform1fv_53,/////wj/////
    return uniformXfv(obj, p, 1);
}

int uniform1i(GCanvas *obj, const char *&p)
{ // uniform1i_4,0
    const int *tokens = ParseTokensInt(p, 2);
    GLint location = tokens[0];
    GLint x = tokens[1];
    
    LOG_D("[webgl::exec] glUniform1i(%d, %d)", location, x);
    glUniform1i(location, x);
    return kContinue;
}

int uniform1iv(GCanvas *obj, const char *&p)
{ // uniform1fv_53,/////wj/////
    return uniformXiv(obj, p, 1);
}

int uniform2f(GCanvas *obj, const char *&p)
{ // uniform2f_51,344,154;
    const int *intTokens = ParseTokensInt(p, 1);
    GLint location = intTokens[0];

    const float *tokens = ParseTokensFloat(p, 2);
    GLfloat x = tokens[0];
    GLfloat y = tokens[1];

    LOG_D("[webgl::exec] glUniform2f(%d, %f, %f)", location, x, y);
    glUniform2f(location, x, y);
    return kContinue;
}

int uniform2fv(GCanvas *obj, const char *&p)
{ // uniform2fv_53,/////wj/////
    return uniformXfv(obj, p, 2);
}

int uniform2i(GCanvas *obj, const char *&p)
{
    const int *tokens = ParseTokensInt(p, 3);
    GLint location = tokens[0];
    GLint x = tokens[1];
    GLint y = tokens[2];
    
    LOG_D("[webgl::exec] glUniform2i(%d, %d, %d)", location, x, y);
    glUniform2i(location, x, y);
    return kContinue;
}

int uniform2iv(GCanvas *obj, const char *&p) { return uniformXiv(obj, p, 2); }

int uniform3f(GCanvas *obj, const char *&p)
{ // uniform3f_51,0,0,1
    const int *intTokens = ParseTokensInt(p, 1);
    GLint location = intTokens[0];

    const float *tokens = ParseTokensFloat(p, 3);
    GLfloat x = tokens[0];
    GLfloat y = tokens[1];
    GLfloat z = tokens[2];

    LOG_D("[webgl::exec] glUniform3f(%d, %f, %f, %f)", location, x, y, z);
    glUniform3f(location, x, y, z);
    return kContinue;
}

int uniform3fv(GCanvas *obj, const char *&p)
{ // uniform3fv_53,/////wj/////
    return uniformXfv(obj, p, 3);
}

int uniform3i(GCanvas *obj, const char *&p)
{
    const int *tokens = ParseTokensInt(p, 4);
    GLint location = tokens[0];
    GLint x = tokens[1];
    GLint y = tokens[2];
    GLint z = tokens[3];

    LOG_D("[webgl::exec] glUniform3i(%d, %d, %d, %d)", location, x, y, z);
    glUniform3i(location, x, y, z);
    return kContinue;
}

int uniform3iv(GCanvas *obj, const char *&p) { return uniformXiv(obj, p, 3); }

int uniform4f(GCanvas *obj, const char *&p)
{ // uniform4f_83,0,0,1,1
    const int *intTokens = ParseTokensInt(p, 1);
    GLint location = intTokens[0];

    const float *tokens = ParseTokensFloat(p, 4);
    GLfloat x = tokens[0];
    GLfloat y = tokens[1];
    GLfloat z = tokens[2];
    GLfloat w = tokens[3];

    LOG_D("[webgl::exec] glUniform4f(%d, %f, %f, %f, %f)", location, x, y, z, w);
    glUniform4f(location, x, y, z, w);

    return kContinue;
}

int uniform4fv(GCanvas *obj, const char *&p)
{ // uniform3fv_53,/////wj/////
    return uniformXfv(obj, p, 4);
}

int uniform4i(GCanvas *obj, const char *&p)
{
    const int *tokens = ParseTokensInt(p, 5);
    GLint location = tokens[0];
    GLint x = tokens[1];
    GLint y = tokens[2];
    GLint z = tokens[3];
    GLint w = tokens[4];
    
    LOG_D("[webgl::exec] glUniform4i(%d, %d, %d, %d, %d)", location, x, y, z, w);
    glUniform4i(location, x, y, z, w);

    return kContinue;
}

int uniform4iv(GCanvas *obj, const char *&p) { return uniformXiv(obj, p, 4); }

int uniformMatrixXfv(GCanvas *obj, const char *&p, int type)
{ // uniformMatrix3fv52,0,so9wP0Qdrz4AAAAAAAAAAEQdr76yj3A/AAAAAAAAA
    const int *tokens = ParseTokensInt(p, 3);
    GLint location = tokens[0];
    GLuint transpose = tokens[1];

    unsigned int size = 0;
    const GLfloat *fvalue = NULL;
    if (g_encode_type == 1)
    {
        fvalue = ParseTokensFloat(p, 16);
    }
    else
    {
        std::string &value = obj->mTempStr;
//        LOG_D("uniformMatrixXfv: %s", p);
        ParseTokensBase64(p, value);
//        int len = ParseTokensBase64(p, value);
//        LOG_D("uniformMatrixXfv: %d", len);
//        LOG_D("uniformMatrixXfv: %s\n", value.c_str());

        float *float32Array = SplitStringToFloat32Array(value.c_str(), ",", size);
        fvalue = (const GLfloat *)float32Array;
    }

    if( fvalue )
    {
        LOG_D("[webgl::exec] glUniformMatrix%dfv(%d, %d, %d, %f)", type, location, size/(type*type), transpose, *fvalue);
    #ifdef ANDROID
        struct timeval before;
        struct timeval after;
        gettimeofday(&before, NULL);
    #endif
        switch (type)
        {
            case 2:
                glUniformMatrix2fv(location, size/4, transpose, fvalue);
                break;
            case 3:
                glUniformMatrix3fv(location, size/9, transpose, fvalue);
                break;
            case 4:
                glUniformMatrix4fv(location, size/16, transpose, fvalue);
                break;
        }
        
    #ifdef ANDROID
        gettimeofday(&after, NULL);
        LOG_D("[webgl::exec] glUniformMatrix%dfv(%d, %d, %d, %f) before: sec = %d, usec = %d, after: sec = %d, usec = %d", type, location, size/(type*type), transpose, *fvalue, before.tv_sec, before.tv_usec, after.tv_sec, after.tv_usec);
    #endif

        if( g_encode_type != 1 )
        {
            free((void*)fvalue);
        }
    }

    return kContinue;
}

int uniformMatrix2fv(GCanvas *obj, const char *&p)
{ // uniformMatrix3fv52,0,so9wP0Qdrz4AAAAAAAAAAEQdr76yj3A/AAAAAAAAA
    return uniformMatrixXfv(obj, p, 2);
}

int uniformMatrix3fv(GCanvas *obj, const char *&p)
{ // uniformMatrix3fv52,0,so9wP0Qdrz4AAAAAAAAAAEQdr76yj3A/AAAAAAAAA
    return uniformMatrixXfv(obj, p, 3);
}

int uniformMatrix4fv(GCanvas *obj, const char *&p)
{ // uniformMatrix4fv52,0,so9wP0Qdrz4AAAAAAAAAAEQdr76yj3A/AAAAAAAAA
    return uniformMatrixXfv(obj, p, 4);
}

int useProgram(GCanvas *obj, const char *&p)
{ // useProgram1
    const int *tokens = ParseTokensInt(p, 1);
    GLuint program = tokens[0];

    glUseProgram(program);
    LOG_D("[webgl::exec] glUseProgram(%d)", program);
    return kContinue;
}

//new
int validateProgram(GCanvas *obj, const char *&p)
{ // validateProgram
    const int *tokens = ParseTokensInt(p, 1);
    GLuint program = tokens[0];

    glValidateProgram(program);
    LOG_D("[webgl::exec] glValidateProgram(%d)", program);
    return kContinue;
}

//new
int vertexAttrib1f(GCanvas *obj, const char *&p)
{
    int index = ParseTokensInt(p, 1)[0];
    GLfloat x = ParseTokensFloat(p, 1)[0];
    
    LOG_D("[webgl::exec] glVertexAttrib1f(%d, %f)", index, x);
    glVertexAttrib1f(index, x);

    std::string &vertex = obj->mTempStr;
    ParseTokensString(p, vertex);
    return kContinue;
}

//new
int vertexAttrib2f(GCanvas *obj, const char *&p)
{
    int index = ParseTokensInt(p, 1)[0];
    const float *tokens = ParseTokensFloat(p, 2);
    GLfloat x = tokens[0];
    GLfloat y = tokens[1];
    
    LOG_D("[webgl::exec] glVertexAttrib2f(%d, %f, %f)", index, x, y);
    glVertexAttrib2f(index, x, y);
    return kContinue;
}

//new
int vertexAttrib3f(GCanvas *obj, const char *&p)
{
    int index = ParseTokensInt(p, 1)[0];
    const float *tokens = ParseTokensFloat(p, 3);
    GLfloat x = tokens[0];
    GLfloat y = tokens[1];
    GLfloat z = tokens[2];

    LOG_D("[webgl::exec] glVertexAttrib3f(%d, %f, %f, %f)", index, x, y, z);
    glVertexAttrib3f(index, x, y, z);
    return kContinue;
}

//new
int vertexAttrib4f(GCanvas *obj, const char *&p)
{
    int index = ParseTokensInt(p, 1)[0];
    const float *tokens = ParseTokensFloat(p, 4);
    GLfloat x = tokens[0];
    GLfloat y = tokens[1];
    GLfloat z = tokens[2];
    GLfloat w = tokens[3];

    LOG_D("[webgl::exec] glVertexAttrib4f(%d, %f, %f, %f, %f)", index, x, y, z, w);
    glVertexAttrib4f(index, x, y, z, w);
    return kContinue;
}

int vertexAttribXfv(GCanvas *obj, const char *&p, int type)
{
    const int *tokens = ParseTokensInt(p, 2);
    GLint index = tokens[0];

    std::string &value = obj->mTempStr;
    ParseTokensBase64(p, value);

    unsigned int size;
    float *float32Array = SplitStringToFloat32Array(value.c_str(), ",", size);
    const GLfloat *fvalue = (const GLfloat *)float32Array;

    if( fvalue )
    {
        LOG_D("[webgl::exec] glVertexAttrib%dfv(%d, %f)", type, index, float32Array[0]);
        switch (type)
        {
            case 1: glVertexAttrib1fv(index, fvalue); break;
            case 2: glVertexAttrib2fv(index, fvalue); break;
            case 3: glVertexAttrib3fv(index, fvalue); break;
            case 4: glVertexAttrib4fv(index, fvalue); break;
        }
        free((void*)fvalue);
    }

    return kContinue;
}

//new
int vertexAttrib1fv(GCanvas *obj, const char *&p)
{
    return vertexAttribXfv(obj, p, 1);
}

//new
int vertexAttrib2fv(GCanvas *obj, const char *&p)
{
    return vertexAttribXfv(obj, p, 2);
}

//new
int vertexAttrib3fv(GCanvas *obj, const char *&p)
{
    return vertexAttribXfv(obj, p, 3);
}

//new
int vertexAttrib4fv(GCanvas *obj, const char *&p)
{
    return vertexAttribXfv(obj, p, 4);
}

int vertexAttribPointer(GCanvas *obj, const char *&p)
{ // vertexAttribPointer10,3,22,0,0,0;
    const int *tokens = ParseTokensInt(p, 6);
    GLint index = tokens[0];
    GLint size = tokens[1];
    GLenum type = tokens[2];
    GLuint normalized = tokens[3];
    GLint stride = tokens[4];
    GLint ptr = tokens[5];

    LOG_D("[webgl::exec] glVertexAttribPointer(%d, %d, %s, %d, %d, %d)",
          index, size, GetMacroValDebug(type), normalized, stride, G_BUFFER_OFFSET(ptr));
    glVertexAttribPointer(index, size, type, normalized, stride, G_BUFFER_OFFSET(ptr));
    return kContinue;
}

int viewport(GCanvas *obj, const char *&p)
{ // viewport0,0,600,450;
    const float *tokens = ParseTokensFloat(p, 4);
    LOG_D("[webgl::exec] glViewport(%f, %f, %f, %f)",
          tokens[0] * obj->mDevicePixelRatio,
          tokens[1] * obj->mDevicePixelRatio,
          tokens[2] * obj->mDevicePixelRatio,
          tokens[3] * obj->mDevicePixelRatio);
    glViewport(tokens[0] * obj->mDevicePixelRatio,
               tokens[1] * obj->mDevicePixelRatio,
               tokens[2] * obj->mDevicePixelRatio,
               tokens[3] * obj->mDevicePixelRatio);
    return kContinue;
}

int (*g_webglFuncMap[WEBGL_API_COUNT])(GCanvas *, const char *&) = {
    NULL,   //0
    activeTexture,
    attachShader,
    bindAttribLocation,
    bindBuffer,
    bindFramebuffer,
    bindRenderbuffer,
    bindTexture,
    blendColor,//new
    blendEquation,
    blendEquationSeparate, //10
    blendFunc,
    blendFuncSeparate,
    bufferData,
    bufferSubData,
    checkFramebufferStatus,//new
    clear,
    clearColor,
    clearDepth,
    clearStencil,
    colorMask, //20
    compileShader,
    compressedTexImage2D,//new
    compressedTexSubImage2D,//new
    copyTexImage2D,
    copyTexSubImage2D,//new
    createBuffer,
    createFramebuffer,
    createProgram,
    createRenderbuffer,
    createShader, //30
    createTexture,
    cullFace,
    deleteBuffer,
    deleteFramebuffer,
    deleteProgram,
    deleteRenderbuffer,
    deleteShader,
    deleteTexture,
    depthFunc,
    depthMask, //40
    depthRange,
    detachShader,//new
    disable,
    disableVertexAttribArray,
    drawArrays,
    NULL, // drawArraysInstancedANGLE,
    drawElements,
    NULL, // drawElementsInstancedANGLE,
    enable,
    enableVertexAttribArray, //50
    flush,
    framebufferRenderbuffer,
    framebufferTexture2D,
    frontFace,
    generateMipmap,
    getActiveAttrib,//new
    getActiveUniform,//new
    getAttachedShaders,//new
    getAttribLocation,
    getBufferParameter,//60 new
    NULL, // getContextAttributes,      //OpenGL ES not support
    getError,//new
    NULL, //getExtension,              //OpenGL ES not support
    getFramebufferAttachmentParameter,//new
    getParameter,//new
    getProgramInfoLog,//new
    getProgramParameter,//new
    getRenderbufferParameter,//new
    getShaderInfoLog,//new
    getShaderParameter, //70
    getShaderPrecisionFormat,//new
    getShaderSource,
    getSupportedExtensions, //new, opengl es not support
    getTexParameter,
    getUniform,
    getUniformLocation,
    getVertexAttrib,
    getVertexAttribOffset,
    isBuffer,
    NULL, //80 isContextLost,             //opengl es not support
    isEnabled,
    isFramebuffer,
    isProgram,
    isRenderbuffer,
    isShader,
    isTexture,
    lineWidth,
    linkProgram,
    pixelStorei,
    polygonOffset, //90
    readPixels,
    renderbufferStorage,
    sampleCoverage,
    scissor,
    shaderSource,
    stencilFunc,
    stencilFuncSeparate,
    stencilMask,//new
    stencilMaskSeparate,
    stencilOp, //100
    stencilOpSeparate,//new
    texImage2D,
    texParameterf,//new
    texParameteri,
    texSubImage2D,//new
    uniform1f,
    uniform1fv,
    uniform1i,
    uniform1iv,
    uniform2f, //110
    uniform2fv,
    uniform2i,
    uniform2iv,
    uniform3f,
    uniform3fv,
    uniform3i,
    uniform3iv,
    uniform4f,
    uniform4fv,
    uniform4i, //120
    uniform4iv,
    uniformMatrix2fv,
    uniformMatrix3fv,
    uniformMatrix4fv,
    useProgram,
    validateProgram,//new
    vertexAttrib1f,//new
    vertexAttrib2f,//new
    vertexAttrib3f,//new
    vertexAttrib4f,//130 new
    vertexAttrib1fv,//new
    vertexAttrib2fv,//new
    vertexAttrib3fv,//new
    vertexAttrib4fv,//new
    vertexAttribPointer,
    viewport, //136
};


//////////////////////////////////////////////////////////////////////////
//                  WEBGL Extension
// https://developer.mozilla.org/en-US/docs/Web/API/WebGLRenderingContext
//////////////////////////////////////////////////////////////////////////
int drawArraysInstancedANGLE(GCanvas *obj, const char *&p)
{
    const int *tokens = ParseTokensInt(p, 4);
#ifdef IOS
    glDrawArraysInstancedEXT(tokens[0], tokens[1], tokens[2], tokens[3]);
    LOG_D("[webgl::exec] glDrawArraysInstancedEXT(%d, %d, %d, %d)",
          tokens[0], tokens[1], tokens[2], tokens[3]);
#endif
    return kContinue;
}

int drawElementsInstancedANGLE(GCanvas *obj, const char *&p)
{
    const int *tokens = ParseTokensInt(p, 5);
#ifdef IOS
    glDrawElementsInstancedEXT(tokens[0], tokens[1], tokens[2], G_BUFFER_OFFSET(tokens[3]), tokens[4]);
    LOG_D("[webgl::exec] glDrawElementsInstancedEXT(%d, %d, %d, %d, %d)",
          tokens[0], tokens[1], tokens[2], tokens[3], tokens[4]);
#endif
    return kContinue;
}

int vertexAttribDivisorANGLE(GCanvas *obj, const char *&p)
{
    const int *tokens = ParseTokensInt(p, 2);
#ifdef IOS
    glVertexAttribDivisorEXT(tokens[0], tokens[1]);
    LOG_D("[webgl::exec] glVertexAttribDivisorEXT(%d, %d)", tokens[0], tokens[1]);
#endif
    return kContinue;
}



int createVertexArrayOES(GCanvas *obj, const char *&p)
{
    ++p;
    GLuint array;
#ifdef IOS
    glGenVertexArraysOES(1, &array);
    LOG_D("[webgl::exec] glGenVertexArraysOES()=%d", array);

#elif ANDROID
    if(glGenVertexArraysOESv) {
        glGenVertexArraysOESv(1, &array);
    }
#endif
    obj->setSyncResult(gcanvas::toString(array));
    return kContinue;
}

int deleteVertexArrayOES(GCanvas *obj, const char *&p)
{
    const int* tokens = ParseTokensInt(p, 1);
    GLuint array = tokens[0];
#ifdef IOS
    glDeleteVertexArraysOES(1, &array);
    LOG_D("[webgl::exec] glDeleteVertexArraysOES(1, %d)", array);

#elif ANDROID
    if(glDeleteVertexArraysOESv) {
        glDeleteVertexArraysOESv(1, &array);
    }
#endif
    return kContinue;
}

int isVertexArrayOES(GCanvas *obj, const char *&p)
{
    const int* tokens = ParseTokensInt(p, 1);
    GLuint array = tokens[0];

#ifdef IOS
    GLuint value = glIsVertexArrayOES(array);
    LOG_D("[webgl::exec] glIsVertexArrayOES(%d)", array);
    obj->setSyncResult(gcanvas::toString(value));
#elif ANDROID
    if(glIsVertexArrayOESv)
    {
        GLuint value = glIsVertexArrayOESv(array);
        obj->setSyncResult(gcanvas::toString(value));
    }
#endif
    return kContinue;
}

int bindVertexArrayOES(GCanvas *obj, const char *&p)
{
    const int* tokens = ParseTokensInt(p, 1);
    GLuint array = tokens[0];

#ifdef IOS
    glBindVertexArrayOES(array);
    LOG_D("[webgl::exec] glBindVertexArrayOES(%d)", array);

#elif ANDROID
    if(glBindVertexArrayOESv) {
        glBindVertexArrayOESv(array);
    }
#endif

    return kContinue;
}

int (*g_webglExtFuncMap[WEBGL_EXT_API_COUNT])(GCanvas *, const char *&) = {
    //extension method for ANGLE_instanced_arrays
    drawArraysInstancedANGLE,
    drawElementsInstancedANGLE,
    vertexAttribDivisorANGLE,

    //extension method for OES_vertex_array_object
    createVertexArrayOES,
    deleteVertexArrayOES,
    isVertexArrayOES,
    bindVertexArrayOES
};

}





using namespace gcanvas;

int GCanvas::executeWebGLCommands(const char *&cmd, int length)
{
    const char *end = cmd + length;
//    LOG_D("GCanvas::executeWebGLCommands: end: %s",end);
    while (cmd < end)
    {

        int index = atoi(cmd);
        
//        LOG_I("[executeWebGLCommands]index=%d", index);

        if( index >=1 &&  index < WEBGL_API_COUNT ) //webgl
        {
//            struct timeval before;
//            struct timeval after;
//            gettimeofday(&before, NULL);
//            LOG_D("logtest: sec = %d, usec = %d, after sec = %d, usec = %d",before.tv_sec, before.tv_usec, before.tv_sec, before.tv_usec);
//            gettimeofday(&after, NULL);
//            LOG_D("logtest cost: sec = %d, usec = %d, after sec = %d, usec = %d",before.tv_sec, before.tv_usec, after.tv_sec, after.tv_usec);
            ParseTokensSkip(cmd);
//            gettimeofday(&after, NULL);
//            LOG_D("before parseToken: sec = %d, usec = %d, after sec = %d, usec = %d",before.tv_sec, before.tv_usec, after.tv_sec, after.tv_usec);
            if (NULL == g_webglFuncMap[index])
            {
//                LOG_W("[executeWebGLCommands] uncomplete cmd index:%d", index);
                return -1;
            };
            int error1 = glGetError();
//            gettimeofday(&before, NULL);
            g_webglFuncMap[index](this, cmd);
//            int ret = g_webglFuncMap[index](this, cmd);
//            gettimeofday(&after, NULL);
//            LOG_D("before webglFuncMapCall: sec = %d, usec = %d, after sec = %d, usec = %d",before.tv_sec, before.tv_usec, after.tv_sec, after.tv_usec);
            int error2 = glGetError();
            LOG_D("beofre error: %d, after error: %d",error1, error2);
//            return ret;
        }
        else if( index >= WEBGL_EXT_API_OFFSET && index < WEBGL_EXT_API_MAX_INDEX ) //extension
        {
            ParseTokensSkip(cmd);
            int extFuncIndex = index - WEBGL_EXT_API_OFFSET;
            if( NULL == g_webglExtFuncMap[extFuncIndex] )
            {
                LOG_W("[executeWebGLCommands] uncomplete ext cmd index:%d", index);
                return -1;
            }

            g_webglExtFuncMap[extFuncIndex](this, cmd);
//            int ret = g_webglExtFuncMap[extFuncIndex](this, cmd);
//            return ret;
        }
        else
        {
//            LOG_W("[executeWebGLCommands] Unknow cmd :%d", index);
            return -1;
        }
    }
    return 0;
}

void GCanvas::GetAllParameter(std::string &ret)
{
//    ret.clear();
//
//    GLint val[128]; // here may cause crash in "HUAWEI MT7-UL00" while
//                   // ret.resize(), so change from 32 to 128 size, but still
//                   // unknown reason.
//    int i = 0;
//
//    GLint range, precision;
//    int sSize = 2;
//    int pSize = 6;
//    for (int m = 0; m < sSize; m++)
//    {
//        for (int n = 0; n < pSize; n++)
//        {
//            glGetShaderPrecisionFormat(g_shapeType_list[m],
//                                       g_precisionType_list[n],
//                                       &range,
//                                       &precision);
//            val[i++] = range;
//            val[i++] = precision;
//        }
//    }
//
//    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &val[i++]);
//    glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &val[i++]);
//    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &val[i++]);
//    glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &val[i++]);
//    glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &val[i++]);
//    glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, &val[i++]);
//    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &val[i++]);
//
//    GLfloat *valf = (GLfloat *)&val[i];
//    int j = 0;
//    glGetFloatv(GL_ALIASED_POINT_SIZE_RANGE, &valf[j]);
//    j += 2;
//    i += sizeof(GLfloat) / sizeof(GLint) * j;
//
//    i *= sizeof(GLint);
//
//    ret.resize(gcanvas::Base64EncodeLen(i));
//    gcanvas::Base64EncodeBuf((char *)ret.c_str(), (char *)val, i);
//    LOG_D("[GetAllParameter][%d] %s", i, ret.c_str());
}

#ifdef ANDROID
void GCanvas::initWebglExt()
{
    glGenVertexArraysOESv = (PFNGLGENVERTEXARRAYSOESPROC)eglGetProcAddress ( "glGenVertexArraysOES" );
    glBindVertexArrayOESv = (PFNGLBINDVERTEXARRAYOESPROC)eglGetProcAddress ( "glBindVertexArrayOES" );
    glDeleteVertexArraysOESv = (PFNGLDELETEVERTEXARRAYSOESPROC)eglGetProcAddress ( "glDeleteVertexArraysOES" );
    glIsVertexArrayOESv = (PFNGLISVERTEXARRAYOESPROC)eglGetProcAddress ( "glIsVertexArrayOES" );
}
#endif

