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
#include "ImageCahced.h"
#define TIMEOUT_VALUE 5L
namespace NodeBinding
{
struct ImageContent
{
    char *memory;
    unsigned int size;
};
enum PIC_FORMAT
{
    PNG_FORAMT,
    JPEG_FORMAT,
    UNKOWN_PIC_FORMAT,
};
std::shared_ptr<ImageCached> findCacheByUrl(const std::string &url);
void cachedImage(const std::string url,std::shared_ptr<ImageCached> imageCached);
PIC_FORMAT parseFormat(char *content, int len);
void decodeImageJPEG(std::vector<unsigned char> &pixels, unsigned int &width, unsigned int &height, const unsigned char *content, int len);
void decodeImagePNG(std::vector<unsigned char> &pixels, unsigned int &width, unsigned int &height, const unsigned char *content, int len);
int readImageFromLocalFile(const std::string &path, ImageContent *content);
bool checkArgs(const Napi::CallbackInfo &info, int exectedN);
unsigned int downloadImage(const std::string &src, ImageContent *content);
void throwError(const Napi::CallbackInfo &info, const std::string &exception);
void throwError(const Napi::Env &env, const std::string &exception);
} // namespace NodeBinding
#endif