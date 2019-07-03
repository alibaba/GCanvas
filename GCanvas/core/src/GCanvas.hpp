/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */

#ifndef __GCANVAS__
#define __GCANVAS__

#define API_WEEX

#include "gcanvas/GCanvas2dContext.h"

#ifdef ANDROID
#include <queue>
#include <jni.h>
#include <semaphore.h>
#include "./support/DynArray.h"
#include "./support/Log.h"
#include "./export.h"
#define GCANVAS_TIMEOUT 800
#endif

#ifdef IOS
#include "DynArray.h"
#include "Log.h"
#include "export.h"
#endif

using namespace gcanvas;

#ifdef GCANVAS_WEEX

// -----------------------------------------------------------
// --    Clip utility class
// --    Used to process drawImage
// --    Populates the textureArr and vertexArr of a Quad
// -----------------------------------------------------------
struct Clip
{
    int textureID;
    float cx, cy, cw, ch, px, py, pw, ph;
};


#ifdef ANDROID

// -----------------------------------------------------------
// --    Synchronize Call Type Enum Ddefinition
// -----------------------------------------------------------
enum
{
    LOAD,
    UNLOAD,
    RELOAD,
    RENDER,
    SET_ORTHO,
    CAPTURE,
    SET_BACKGROUND,
    SET_TYOFFSETFLAG,
    GET_IMAGEDATA,
    SET_DEVICE_PIXEL_RATIO,
    SET_CLEAR_COLOR,
    SET_HI_QUALITY,
    SET_CONTEXT_TYPE,
    SET_RENDERCMDLIMIT,
    GET_PROGRAMPARAMETER,
    GET_SHADERINFOLOG,
    GET_SHADERPARAMETER,
    GET_ACTIVEUNIFORM,
    GET_ACTIVEATTRIB,
    TODATAURL,
    BIND_TEXTURE,
    GET_STRING,             //glGetString
    IS_VERTEX_ARRAY_OES,    //glIsVertexArrayOES
};

struct GCanvasCmd
{
    std::string contextId;
    int type;
    std::string args;
};

struct BitmapCmd
{
    void *Bitmap;
    int width;
    int height;
    int target;
    int level;
    int interformat;
    int format;
    int type;
    int xoffset;
    int yoffset;
    int id;
};

// -----------------------------------------------------------
// --    Callback struct
//
//  Contains the information needed to execute a success or error
//  callback on the cordova side
// -----------------------------------------------------------
struct Callback
{
    enum
    {
        ALLOCATED = 512
    };

    Callback(const char *id, const char *res, bool error);

    char callbackId[ALLOCATED];
    char result[ALLOCATED];
    bool isError;
};
#endif

#endif

class GCanvas : public GCanvasContext
{
public:
    API_EXPORT GCanvas(std::string contextId, bool flip=false, std::string appInfo="", bool useFbo=true);

    ~GCanvas();
    
    void Clear();
    
    void DrawText(const char *text, float x, float y, float maxWidth=SHRT_MAX,
                  bool isStroke=false, int strLength = 0);
    
    void DrawTextWithLength(const char *text, int strLength, float x, float y,
                            bool isStroke=false, float maxWidth=SHRT_MAX);
    
    API_EXPORT void OnSurfaceChanged(int x, int y, int width, int height);
    
    void SetBackgroundColor(float red, float green, float blue);
    
    //Weex 2D
#ifdef GCANVAS_WEEX
    API_WEEX bool AddPngTexture(const unsigned char *buffer, unsigned int size, int textureGroupId,
                       unsigned int *pWidth, unsigned int *pHeight);
    API_WEEX void AddTexture(int textureGroupId, int glID, int width, int height);

    #ifdef IOS
    API_WEEX void AddOfflineTexture(int textureGroupId, int glID);
    #endif
    
    API_WEEX void GetAllParameter(std::string &ret);
    
    #ifdef ANDROID
    API_WEEX void AddCallback(const char *callbackID, const char *result, bool isError);
    API_WEEX Callback *GetNextCallback();
    API_WEEX void PopCallbacks();
    #endif
    
    API_WEEX void DrawImage(int textureId, float sx, float sy, float sw, float sh,
                            float dx, float dy, float dw, float dh);

    API_WEEX void GetImageData(int x, int y, int w, int h, bool base64Encode,
                               std::string &pixelsData);
    API_WEEX void PutImageData(const char *imageData, int dataLength, float dx, float dy,
                               float sw, float sh, float dw, float dh);

    API_WEEX void RemoveTexture(int id);
    API_WEEX void Render(const char *renderCommands, int length);
    API_WEEX void SetOrtho(int width, int height);
    API_WEEX void SetTyOffsetFlag(bool flag);
    API_WEEX void UsePatternRenderPipeline(int textureListId, int width, int height,
                                  const std::string &pattern, bool isStroke=false);
    API_WEEX void UsePatternRenderPipeline(int textureListId, const std::string &pattern,
                                  bool isStroke=false);
    
    //Weex WebGL
    API_WEEX void setSyncResult(std::string result);
    #ifdef ANDROID
    API_WEEX std::string exeSyncCmd(int cmd, const char *&args);
    API_WEEX std::string exe2dSyncCmd(int cmd, const char *&args);
    API_WEEX void initWebglExt();
    API_WEEX const char *CallNative(int type, const std::string &args);
    API_WEEX int getCmdType(int type);
    API_WEEX int getSyncAttrib(int type);
    API_WEEX int getOpType(int type);
    API_WEEX std::string canvasProc(int op, int sync, std::string args);
    API_WEEX std::string webglProc(int op, int sync, std::string args);
    API_WEEX std::string vulkanProc(int op, int sync, std::string args);
    API_WEEX std::string metalProc(int op, int sync, std::string args);
    API_WEEX virtual void signalUpGLthread();
    API_WEEX void LinkNativeGLProc();
    API_WEEX void clearCmdQueue();
    API_WEEX void QueueProc(std::queue<struct GCanvasCmd *> *queue);
    API_WEEX virtual void setRefreshFlag(bool refresh);
    API_WEEX void setSyncFlag();
    API_WEEX void setThreadExit();
    API_WEEX void finishProc();
    API_WEEX bool continueProcess();
    API_WEEX void addBitmapQueue(struct BitmapCmd *p);
    API_WEEX void bindTexture(struct BitmapCmd cmd);
    API_WEEX void bindTexture(GTexture *texture);
    API_WEEX void texSubImage2D(struct BitmapCmd cmd);
    API_WEEX void setContextLost(bool lost);
    #endif
    
#endif


private:
    void drawFBO(std::string fboName, GCompositeOperation compositeOp = COMPOSITE_OP_SOURCE_OVER,
                 float sx = 0, float sy = 0, float sw = 1, float sh = 1,
                 float dx = 0, float dy = 0, float dw = 1, float dh = 1);
    
    //GCanvas Weex API
#ifdef GCANVAS_WEEX
    API_WEEX void calculateFPS();
    API_WEEX void execute2dCommands(const char *renderCommands, int length);
    API_WEEX int executeWebGLCommands(const char *&cmd, int length);
    API_WEEX bool isCmd(const char *in, const char *match) { return in[0] == match[0]; }
    API_WEEX const char *parseSetTransform(const char *renderCommands,
                                  int parseMode, // what to read: IDENTITY, FULL_XFORM, etc.
                                  bool concat,   // if true, concatenate, else replace.
                                  GTransform transIn,    // the current xform
                                  GTransform *transOut); // where to write the new xform

    API_WEEX void parseSetTransForTextform(float v1, float v2,
                                  int parseMode,    // what to read: IDENTITY, FULL_XFORM, etc.
                                  bool concat,      // if true, concatenate, else replace.
                                  GTransform transIn, // the current xform
                                  GTransform *transOut);

    API_WEEX const char *parseSetTransformT( const char *renderCommands,
                                int parseMode,          // what to read: IDENTITY, FULL_XFORM, etc.
                                bool concat,            // if true, concatenate, else replace.
                                GTransform transIn,    // the current xform
                                GTransform *transOut); // where to write the new xform
    API_WEEX const char *parseTokens(const char *renderCommands, float *tokens,
                                     int iMaxCount = 6);
    API_WEEX void parseTokesOpt(float *tokens, const char **pp);
    API_WEEX const char *parseDrawImage(const char *renderCommands, Clip *clipOut);
    API_WEEX const char *parseName(const char *p, std::string &name);
    API_WEEX const char *parseBindingPara(const char *p, std::string &name, float &width, float &height);
    API_WEEX const char *parseBindingPara(const char *p, std::string &name,
                                          float& sx, float& sy, float& sw, float& sh,
                                          float& dx, float& dy, float& dw, float& dh);
    API_WEEX const char *parseUnknown(const char *renderCommands);
    API_WEEX const char *extractOneParameterFromCommand(char *outParameter,
                                               const char *commands);
    API_WEEX const Texture *getTextureWithOneImage(int id);
    API_WEEX float fastFloat(const char *str) { return (float) atof(str); }
    
    enum
    {
        IDENTITY,  // rt
        SET_XFORM, // st
        SCALE,     // sc
        ROTATE,    // ro
        TRANSLATE, // tr
        NUM_PARSE_MODES
    };

    enum {
        CANVAS,
        WEBGL,
        VULKAN,
        METAL
    };

    enum {
        ASYNC,
        SYNC
    };

    enum {
        OP_RENDER,
    };
#endif

public:
    std::string mContextId;
    
#ifdef GCANVAS_WEEX
    int mFrames;
    float mFps;
    std::string mTempStr;
    GTransform mCurrentTransform;
    bool mContextLost;
    DynArray<GTransform> mActionStack;
    TextureMgr mTextureMgr;
    std::string mResult = "";

#ifdef ANDROID
    clock_t mLastTime;
    DynArray<Callback *> mCallbacks;
    sem_t mSyncSem;
    bool mSync = false;
    bool mExit = false;
    std::queue<struct GCanvasCmd *> mCmdQueue;
    std::queue<struct BitmapCmd *> mBitmapQueue;
#endif

#ifdef IOS
    protected:
        std::map< int, int > mOfflineTextures;
#endif

#endif

public:
    
    API_EXPORT float execMeasureTextWidth(const char *text, int strLength = 0);

    API_EXPORT void drawFrame(bool clear = true);

};

#endif
