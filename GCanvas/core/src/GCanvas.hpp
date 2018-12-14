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

#ifdef ANDROID
#include "./support/DynArray.h"
#include "./support/Log.h"
#include "./export.h"
#endif

#ifdef IOS
#include "DynArray.h"
#include "Log.h"
#include "export.h"
#endif

using namespace gcanvas;

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
//struct CaptureParams 
//{
//    enum {
//        ALLOCATED = 512
//    };
//
//    CaptureParams();
//
//    CaptureParams(int x, int y, int w, int h, const char *callbackID,
//                  const char *fileName);
//
//    int x;
//    int y;
//    int width;
//    int height;
////    char callbackID[ALLOCATED];
//    char fileName[ALLOCATED];
//};

// -----------------------------------------------------------
// --    Callback struct
//
//  Contains the information needed to execute a success or error
//  callback on the cordova side
// -----------------------------------------------------------
#ifdef ANDROID

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



class GCanvas : public GCanvasContext
{
public:
    GCanvas(std::string contextId, bool flip=false, bool onScreen=true);

    ~GCanvas();

    bool AddPngTexture(const unsigned char *buffer, unsigned int size, int textureGroupId,
                       unsigned int *pWidth, unsigned int *pHeight);

    void AddTexture(int textureGroupId, int glID, int width, int height);

#ifdef IOS
    void AddOfflineTexture(int textureGroupId, int glID);
#endif

    void Clear();

    void DrawImage(int textureId, float sx, float sy, float sw, float sh,
                   float dx, float dy, float dw, float dh);

    void DrawText(const char *text, float x, float y, float maxWidth=SHRT_MAX,
                  bool isStroke=false, int strLength = 0);

    void DrawTextWithLength(const char *text, int strLength, float x, float y,
                            bool isStroke=false, float maxWidth=SHRT_MAX);

    void FillRect(float x, float y, float w, float h);
    
    void GetAllParameter(std::string &ret);

#ifdef ANDROID
    void AddCallback(const char *callbackID, const char *result, bool isError);
    Callback *GetNextCallback();
    void PopCallbacks();
#endif
    
    void GetImageData(int x, int y, int w, int h, bool base64Encode,
                      std::string &pixelsData);

    void OnSurfaceChanged(int x, int y, int width, int height);


    void PutImageData(const char *imageData, int dataLength, float dx, float dy,
                      float sw, float sh, float dw, float dh);

//    void QueueCaptureGLLayer(int x, int y, int w, int h, const char *callbackID,
//                             const char *fn);

    void RemoveTexture(int id);

    void Render(const char *renderCommands, int length);

    void SetBackgroundColor(float red, float green, float blue);

    void SetOrtho(int width, int height);

    void SetTyOffsetFlag(bool flag);
    
    void UsePatternRenderPipeline(int textureListId, int width, int height,
                                  const std::string &pattern, bool isStroke=false);

    void UsePatternRenderPipeline(int textureListId, const std::string &pattern,
                                  bool isStroke=false);

#ifdef ANDROID

    std::string exeSyncCmd(int cmd, const char *&args);

    std::string exe2dSyncCmd(int cmd, const char *&args);

//	std::string getProgramparameter(GCanvas* obj, int id,int type);
//	std::string getShaderInfoLog(GCanvas* obj, int id);
//	std::string getShaderparameter(GCanvas* obj, int id,int type);
//	std::string getActiveUniform(GCanvas* obj, int id,int index);
//	std::string getActiveAttrib(GCanvas* obj, int id,int index);
//    
//    std::string getString(GCanvas* obj, int name);
//    std::string isVertexArrayOES(GCanvas* obj, int array);

    void initWebglExt();

    const char *CallNative(int type, const std::string &args);
    
#endif



private:
    void drawFBO(std::string fboName, GCompositeOperation compositeOp = COMPOSITE_OP_SOURCE_OVER,
                 float sx = 0, float sy = 0, float sw = 1, float sh = 1, float dx = 0, float dy = 0,
                 float dw = 1, float dh = 1);

    void calculateFPS();

    void execute2dCommands(const char *renderCommands, int length);
    
    int executeWebGLCommands(const char *&cmd, int length);

    bool isCmd(const char *in, const char *match) { return in[0] == match[0]; }

//    bool captureGLLayer(CaptureParams *params);

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

    const char *parseName(const char *p, std::string &name);

    const char *parseBindingPara(const char *p, std::string &name, float &width, float &height);
    const char *parseBindingPara(const char *p, std::string &name, float& sx, float& sy, float& sw, float& sh,
                                 float& dx, float& dy, float& dw, float& dh);


    const char *parseUnknown(const char *renderCommands);

    const char *extractOneParameterFromCommand(char *outParameter,
                                               const char *commands);

    const Texture *getTextureWithOneImage(int id);

    float fastFloat(const char *str) { return (float) atof(str); }


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

    DynArray<GTransform> mActionStack;

    TextureMgr mTextureMgr;
//    DynArray<CaptureParams *> mCaptureParams;

    std::string mResult = "";


#ifdef ANDROID
    
    DynArray<Callback *> mCallbacks;

    sem_t mSyncSem;
    bool mSync = false;
    bool mExit = false;

    std::queue<struct GCanvasCmd *> mCmdQueue;
    std::queue<struct BitmapCmd *> mBitmapQueue;

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

    void setSyncResult(std::string result);

#ifdef IOS
    protected:
        std::map< int, int > mOfflineTextures;
#endif

    
    
//new
public:
    API_EXPORT void drawFrame();
    API_EXPORT void execSetTransform(float a, float b, float c, float d, float tx, float ty);

    API_EXPORT void
    execDrawImageNew(int textureId, int textureWidth, int textureHeight, float sx, float sy,
                     float sw, float sh, float dx, float dy, float dw, float dh,
                     bool flipY = false);

    API_EXPORT void execTransfrom(float a, float b, float c, float d, float tx, float ty);

    API_EXPORT void execResetTransfrom();

    API_EXPORT void execScale(float sx, float sy);

    API_EXPORT void execRotate(float angle);

    API_EXPORT void execTranslate(float tx, float ty);

    API_EXPORT void execSave();

    API_EXPORT void execRestore();

    API_EXPORT void execSetGlobalAlpha(float alpha);

    API_EXPORT void execSetFillStyle(const char *str);

    API_EXPORT void execSetStrokeStyle(const char *str);

    API_EXPORT void
    execSetFillStylePattern(int textureId, int width, int height, const char *repeatMode, bool isStroke=false);

    API_EXPORT void execSetFillStyleLinearGradient(float startArr[], float endArr[], int stop_count,
                                                   const float posArray[],
                                                   const std::string colorArray[],
                                                   bool isStroke=false);

    API_EXPORT void execSetFillStyleRadialGradient(float startArr[], float endArr[], int stop_count,
                                                   const float posArray[],
                                                   const std::string colorArray[],
                                                   bool isStroke=false);

    API_EXPORT void execSetLineStyle(const char *str);

    API_EXPORT void execSetLineWidth(float w);

    API_EXPORT void execSetLineCap(const char *p);

    API_EXPORT void execSetLineJoin(const char *p);

    API_EXPORT void execSetMiterLimit(float miterlimit);

#ifdef SUPPORT_LINEDASH
    API_EXPORT void execSetLineDash(std::vector<float> lineDash);
    
    API_EXPORT void execSetLineDashOffset(float lineDashOffset);

#endif
    API_EXPORT void execStrokeRect(float x, float y, float w, float h);

    API_EXPORT void execClearRect(float x, float y, float w, float h);

    API_EXPORT void execClip();

    API_EXPORT void execResetClip();

    API_EXPORT void execClosePath();

    API_EXPORT void execMoveTo(float x, float y);

    API_EXPORT void execLineTo(float x, float y);

    API_EXPORT void execQuadraticCurveTo(float cpx, float cpy, float x, float y);

    API_EXPORT void
    execBezierCurveTo(float cp1x, float cp1y, float cp2x, float cp2y, float x, float y);

    API_EXPORT void execArcTo(float x1, float y1, float x2, float y2, float radius);

    API_EXPORT void execBeginPath();

    API_EXPORT void execFillRect(float x, float y, float w, float h);

    API_EXPORT void execRect(float x, float y, float w, float h);

    API_EXPORT void execFill();

    API_EXPORT void execStroke();

    API_EXPORT void
    execArc(float x, float y, float radius, float startAngle, float endAngle, int antiClockwise);

    API_EXPORT void execSetGlobalCompositeOperation(int op);

    API_EXPORT void execSetTextAlign(int textAlign);

    API_EXPORT void execSetTextBaseline(int textbaseline);

    API_EXPORT void
    execDrawText(const char *text, float x, float y, float maxWidth = SHRT_MAX, int strLength = 0);

    API_EXPORT void
    execStrokeText(const char *text, float x, float y, float maxWidth = SHRT_MAX,
                   int strLength = 0);

    API_EXPORT float execMeasureTextWidth(const char *text, int strLength = 0);

    API_EXPORT void execFont(const char *font);

    API_EXPORT void execPutImageData(const unsigned char *rgbaData,
                                     int tw, int th, int x, int y, int sx, int sy,
                                     int sw, int sh);

    API_EXPORT void execGetImageData(int x, int y, int width, int height, uint8_t *pixels);

    API_EXPORT void execGetRawImageData(int width, int height, uint8_t *pixels);

    API_EXPORT int execBindImage(const unsigned char *rgbaData, GLint format, unsigned int width,
                                 unsigned int height);
#ifdef ANDROID
    API_EXPORT void execBeginDraw();

    API_EXPORT void execEndDraw();

    API_EXPORT int execGetFBOTexture();
#endif
    API_EXPORT int execDetachFBOTexture();

    API_EXPORT void execResize(int w, int h);

    API_EXPORT void execSetShadowColor(const char *str);

    API_EXPORT void execSetShadowBlur(int blur);

    API_EXPORT void execSetShadowOffsetX(float x);

    API_EXPORT void execSetShadowOffsetY(float y);


    GTransform mCurrentTransform;
    bool mOnScreen;


};


#ifdef ANDROID
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
#endif

#define GCANVAS_TIMEOUT 800

#endif
