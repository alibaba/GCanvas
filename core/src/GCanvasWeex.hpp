/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#ifdef GCANVAS_WEEX

#ifndef GCANVAS_GCANVASWEEX_H
#define GCANVAS_GCANVASWEEX_H

#include "GCanvas.hpp"
#include "GCanvas2dContext.h"
#include "support/DynArray.h"
#include "support/Log.h"
#include "export.h"

#ifdef ANDROID

#include <queue>
#include <jni.h>
#include <semaphore.h>
#define GCANVAS_TIMEOUT 800

#endif

namespace gcanvas {


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

// -----------------------------------------------------------
// --    Clip utility class
// --    Used to process drawImage
// --    Populates the textureArr and vertexArr of a Quad
// -----------------------------------------------------------
struct ClipStruct
{
    int textureID;
    float cx, cy, cw, ch, px, py, pw, ph;
};


class GCanvasWeex : public GCanvas
{
public:
    
     GCanvasWeex(std::string contextId, const GCanvasConfig& config);

    virtual ~GCanvasWeex();

    virtual void CreateContext();

    virtual void Clear();
    
    
    //call context API
    void SetClearColor(const GColorRGBA &c);
    void SetDevicePixelRatio(const float ratio);
    
    int GetContextType();
    void SetContextType(int contextType);
    
    GTexture *GetFboTexture();

    //Weex 2D
     bool AddPngTexture(const unsigned char *buffer, unsigned int size, int textureGroupId,
                       unsigned int *pWidth, unsigned int *pHeight);
     void AddTexture(int textureGroupId, int glID, int width, int height);

#ifdef IOS
     void AddOfflineTexture(int textureGroupId, int glID);
#endif
    
    
     void DrawImage(int textureId, float sx, float sy, float sw, float sh,
                            float dx, float dy, float dw, float dh);

     void GetImageData(int x, int y, int w, int h, bool base64Encode,
                               std::string &pixelsData);
     void PutImageData(const char *imageData, int dataLength, float dx, float dy,
                               float sw, float sh, float dw, float dh);
     void RemoveTexture(int id);
     void Render(const char *renderCommands, int length);
     void SetOrtho(int width, int height);
     void SetTyOffsetFlag(bool flag);
     void UsePatternRenderPipeline(int textureListId, int width, int height,
                                  const std::string &pattern, bool isStroke=false);
     void UsePatternRenderPipeline(int textureListId, const std::string &pattern,
                                  bool isStroke=false);
    
    
#ifdef ANDROID
    void AddCallback(const char *callbackID, const char *result, bool isError);
    Callback *GetNextCallback();
    void PopCallbacks();
    
    //Weex WebGL
    std::string exeSyncCmd(int cmd, const char *&args);
    std::string exe2dSyncCmd(int cmd, const char *&args);
    void initWebglExt();
    const char *CallNative(int type, const std::string &args);
    int getCmdType(int type);
    int getSyncAttrib(int type);
    int getOpType(int type);
    std::string canvasProc(int op, int sync, std::string args);
    std::string webglProc(int op, int sync, std::string args);
    std::string vulkanProc(int op, int sync, std::string args);
    std::string metalProc(int op, int sync, std::string args);
    virtual void signalUpGLthread();
    void LinkNativeGLProc();
    void clearCmdQueue();
    void QueueProc(std::queue<struct GCanvasCmd *> *queue);
    virtual void setRefreshFlag(bool refresh);
    void setSyncFlag();
    void setThreadExit();
    void finishProc();
    bool continueProcess();
    void addBitmapQueue(struct BitmapCmd *p);
    void bindTexture(struct BitmapCmd cmd);
    void bindTexture(GTexture *texture);
    void texSubImage2D(struct BitmapCmd cmd);
    void setContextLost(bool lost);
#endif
    
    void GetAllParameter(std::string &ret);
    void setSyncResult(std::string result);

protected:
    //GCanvas Weex API
    void drawFBO(std::string fboName, GCompositeOperation compositeOp = COMPOSITE_OP_SOURCE_OVER,
             float sx = 0, float sy = 0, float sw = 1, float sh = 1, float dx = 0, float dy = 0,
             float dw = 1, float dh = 1);

    void calculateFPS();

    void execute2dCommands(const char *renderCommands, int length);
    int executeWebGLCommands(const char *&cmd, int length);

    const char *parseTokens(const char *renderCommands, float *tokens, int iMaxCount = 6);
    void parseTokesOpt(float *tokens, const char **pp);
    const char *parseDrawImage(const char *renderCommands, ClipStruct *clipOut);
    const char *parseUnknown(const char *renderCommands);
    const char *extractOneParameterFromCommand(char *outParameter,
                                           const char *commands);
    const Texture *getTextureWithOneImage(int id);
    float fastFloat(const char *str) { return (float) atof(str); }

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
    
public:
    int mFrames;
    float mFps;
    std::string mTempStr;
    bool mContextLost;
    TextureMgr mTextureMgr;
    std::string mResult = "";

#ifdef ANDROID
    DynArray<Callback *> mCallbacks;
    sem_t mSyncSem;
    bool mSync = false;
    bool mExit = false;
    std::queue<struct GCanvasCmd *> mCmdQueue;
    std::queue<struct BitmapCmd *> mBitmapQueue;
#endif

    
protected:
#ifdef IOS
    std::map< int, int > mOfflineTextures;
#endif
    
};


} //namespace gcanvas

#endif /* GCANVAS_GCANVASWEEX_H */

#endif
