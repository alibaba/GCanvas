/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#ifndef NODE_BINDING_UTIL_H
#define NODE_BINDING_UTIL_H
#include <napi.h>
#include <unordered_map>
#include "ImagePixelInfo.h"

#define TIMEOUT_VALUE 5L
namespace NodeBinding
{
struct ImageMemoryChunk
{
    char *memory;
    size_t size;
};

enum PIC_FORMAT
{
    PNG_FORAMT,
    JPEG_FORMAT,
    UNKOWN_PIC_FORMAT,
};
std::shared_ptr<ImagePixelInfo> findCacheByUrl(const std::string &url);
void cachedImage(const std::string url,std::shared_ptr<ImagePixelInfo> imageCached);
PIC_FORMAT parseFormat(char *content, int len);
void decodeImageJPEG(std::vector<unsigned char> &pixels, unsigned int &width, unsigned int &height, const unsigned char *content, int len);
void decodeImagePNG(std::vector<unsigned char> &pixels, unsigned int &width, unsigned int &height, const unsigned char *content, int len);
void encodeJPEGInBuffer(unsigned char **out,unsigned long &size ,unsigned char *data,int width,int height, int quality=75);
void encodePNGInBuffer(std::vector<unsigned char> &in,unsigned char *data,int width,int height);
int readImageFromLocalFile(const std::string &path, ImageMemoryChunk *content);
bool checkArgs(const Napi::CallbackInfo &info, int exectedN);
// bool checkArgs(const Napi::CallbackInfo &info, double*args, int exectedN, int offset=0);
unsigned int downloadImage(const std::string &src, ImageMemoryChunk *content);
void throwError(const Napi::CallbackInfo &info, const std::string &exception);
void throwError(const Napi::Env &env, const std::string &exception);
void pixelsConvertARGBToRGBA(unsigned char * data, int width, int height);
void pixelsConvertRGBAToARGB(unsigned char * data, int width, int height);


template<typename T, typename U> void toBase64(T& out, const U& in);
template<typename T, typename U> void fromBase64(T& out, const U& in);
} // namespace NodeBinding
#endif