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

#ifdef ANDROID
#include <jni.h>
#include <semaphore.h>
#endif

#include <queue>
#include "gcanvas/GCanvas2dContext.h"
#include "support/DynArray.h"
#include "support/Log.h"

using namespace gcanvas;

// -----------------------------------------------------------
// --    Synchronize Call Type Enum Ddefinition
// -----------------------------------------------------------
enum {
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

// -----------------------------------------------------------
// --    CaptureParams struct
//
//  Contains the information needed to take a capture of the
//  GL context.
// -----------------------------------------------------------
struct CaptureParams
{
    enum
    {
        ALLOCATED = 512
    };

    CaptureParams();
    CaptureParams(int x, int y, int w, int h, const char *callbackID,
                  const char *fileName);

    int x;
    int y;
    int width;
    int height;
    char callbackID[ALLOCATED];
    char fileName[ALLOCATED];
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

//class GGlObject
//{
//public:
//    int mAppId;
//    GLuint mGlHandle;
//};
//
//class GGlObjectMgr
//{
//public:
//    void SetFlag(const char *flag) { mFlag = flag; };
//    const char *GetFlag() { return mFlag.c_str(); };
//    void Append(int appId, GLuint glId)
//    {
//        GGlObject &obj = mPool[appId];
//        obj.mAppId = appId;
//        obj.mGlHandle = glId;
//    }
//
//    void Remove(int appId)
//    {
//        mPool.erase(appId);
//    }
//
//    const GGlObject *Get(int appId) const
//    {
//        std::map< int, GGlObject >::const_iterator itr;
//        itr = mPool.find(appId);
//        if (mPool.end() != itr)
//        {
//            return &(itr->second);
//        }
//        return nullptr;
//    };
//
//private:
//    std::map< int, GGlObject > mPool;
//    std::string mFlag;
//};

class GCanvas : public GCanvasContext
{
public:
    GCanvas(std::string contextId);
    ~GCanvas();

    void AddCallback(const char *callbackID, const char *result, bool isError);
    bool AddPngTexture(const unsigned char *buffer, unsigned int size, int textureGroupId,
                       unsigned int *pWidth, unsigned int *pHeight);
    void AddTexture(int textureGroupId, int glID, int width, int height);
    
#ifdef IOS
    void AddOfflineTexture(int textureGroupId, int glID);
#endif
    
    void Clear();
    void DrawImage(int textureId, float sx, float sy, float sw, float sh,
                   float dx, float dy, float dw, float dh);
    void DrawText(const char *text, float x, float y, float maxWidth,
                  bool isStroke);
    void FillRect(float x, float y, float w, float h);
    void GetAllParameter(std::string &ret);
    Callback *GetNextCallback();
    void GetImageData(int x, int y, int w, int h, bool base64Encode,
                      std::string &pixelsData);
    void OnSurfaceChanged(int width, int height);
    void PopCallbacks();
    void PutImageData(const char *imageData, int dataLength, float dx, float dy,
                      float sw, float sh, float dw, float dh);
    void QueueCaptureGLLayer(int x, int y, int w, int h, const char *callbackID,
                             const char *fn);
    void RemoveTexture(int id);
    void Render(const char *renderCommands, int length);
    void SetBackgroundColor(float red, float green, float blue);
    void SetOrtho(int width, int height);
    void SetTyOffsetFlag(bool flag);
    void UsePatternRenderPipeline(int textureListId,
                                  const std::string &pattern);
#ifdef ANDROID
	std::string exeSyncCmd(int cmd,const char *&args);
	std::string exe2dSyncCmd(int cmd,const char *&args);
	std::string exeWebglSyncCmd(int cmd,const char *&args);
//	std::string getProgramparameter(GCanvas* obj, int id,int type);
//	std::string getShaderInfoLog(GCanvas* obj, int id);
//	std::string getShaderparameter(GCanvas* obj, int id,int type);
//	std::string getActiveUniform(GCanvas* obj, int id,int index);
//	std::string getActiveAttrib(GCanvas* obj, int id,int index);
//    
//    std::string getString(GCanvas* obj, int name);
//    std::string isVertexArrayOES(GCanvas* obj, int array);

    void initWebglExt();
    const char* CallNative(int type,std::string args);
#endif

private:
    void drawFBO();
    void calculateFPS();
    void execute2dCommands(const char *renderCommands, int length);
    int executeWebGLCommands(const char *&cmd, int length);
    bool isCmd(const char *in, const char *match) { return in[0] == match[0]; }
    bool captureGLLayer(CaptureParams *params);
    const char *parseSetTransform(const char *renderCommands,
                                  int parseMode, // what to read: IDENTITY, FULL_XFORM, etc.
                                  bool concat,   // if true, concatenate, else replace.
                                  GTransform transIn,    // the current xform
                                  GTransform *transOut); // where to write the new xform

    void parseSetTransForTextform(float v1, float v2,
                                  int parseMode,    // what to read: IDENTITY, FULL_XFORM, etc.
                                  bool concat,      // if true, concatenate, else replace.
                                  GTransform transIn, // the current xform
                                  GTransform *transOut);
    const char *parseSetTransformT(
            const char *renderCommands,
            int parseMode,          // what to read: IDENTITY, FULL_XFORM, etc.
            bool concat,            // if true, concatenate, else replace.
            GTransform transIn,    // the current xform
            GTransform *transOut); // where to write the new xform
    const char *parseTokens(const char *renderCommands, float *tokens,
                            int iMaxCount = 6);
    void parseTokesOpt(float *tokens, const char **pp);
    const char *parseDrawImage(const char *renderCommands, Clip *clipOut);
    const char *parseUnknown(const char *renderCommands);
    const char *extractOneParameterFromCommand(char *outParameter,
                                               const char *commands);
    const Texture *getTextureWithOneImage(int id);
    float fastFloat(const char *str) { return (float)atof(str); }


    enum
    {
        IDENTITY,  // rt
        SET_XFORM, // st
        SCALE,     // sc
        ROTATE,    // ro
        TRANSLATE, // tr
        NUM_PARSE_MODES
    };

    enum
    {
        CANVAS,
        WEBGL,
        VULKAN,
        METAL
    };

    enum
    {
        ASYNC,
        SYNC
    };

    enum
    {
        OP_RENDER,
    };

public:
	std::string mContextId;
    clock_t mLastTime;
    int mFrames;
    int mMessages;
    float mFps;
    int mMsgLength;
    bool mTyOffsetFlag;
    short mTyOffset;
    std::string mTempStr;
    int mPixelFlipY;
    int mRenderCount;
    
    bool mContextLost;      //mContextLost

    DynArray< GTransform > mActionStack;
    TextureMgr mTextureMgr;
    DynArray< CaptureParams * > mCaptureParams;
    DynArray< Callback * > mCallbacks;
//    GGlObjectMgr mTextureMap;
//    GGlObjectMgr mShaderMgr;
//    GGlObjectMgr mProgramMgr;
//    GGlObjectMgr mAttribMgr;
//    GGlObjectMgr mBufferMgr;
//    GGlObjectMgr mUniformMgr;
//    GGlObjectMgr mFrameBufferMgr;
//    GGlObjectMgr mRenderBufferMgr;
    
    std::string mResult = "";
    


#ifdef ANDROID
    JNIEnv *mJniEnv;
//    jobject *mCanvasRender;
    JNIEnv *GetEnv();
    sem_t mSyncSem;
    bool mSync = false;
    bool mExit = false;

    std::queue<struct GCanvasCmd *> mCmdQueue;
    std::queue<struct BitmapCmd *> mBitmapQueue;

    int getCmdType(int type);
    int getSyncAttrib(int type);
    int getOpType(int type);
    std::string canvasProc(int op,int sync,std::string args);
    std::string webglProc(int op,int sync,std::string args);
    std::string vulkanProc(int op,int sync,std::string args);
    std::string metalProc(int op,int sync,std::string args);
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
    void texSubImage2D(struct BitmapCmd cmd);
    void setContextLost(bool lost);
#endif
    
    void setSyncResult(std::string result);
    
#ifdef IOS
protected:
    std::map< int, int > mOfflineTextures;
#endif

};


struct GCanvasCmd {
    std::string contextId;
    int type;
    std::string args;
};

struct BitmapCmd {
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

#define GCANVAS_TIMEOUT 800

#endif
