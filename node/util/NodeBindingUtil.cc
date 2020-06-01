/**
 * Created by G-Canvas Open Source Team.
 * Copyright (c) 2017, Alibaba, Inc. All rights reserved.
 *
 * This source code is licensed under the Apache Licence 2.0.
 * For the full copyright and license information, please view
 * the LICENSE file in the root directory of this source tree.
 */
#include "NodeBindingUtil.h"
#include "lodepng.h"
#include "jpeglib.h"
#include <curl/curl.h>
#include <string>
#include <vector>
#include <iostream>
namespace NodeBinding
{
static size_t
writeMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct ImageContent *mem = (struct ImageContent *)userp;

    char *ptr = (char *)realloc(mem->memory, mem->size + realsize + 1);
    if (ptr == NULL)
    {
        /* out of memory! */
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    return realsize;
}
bool checkArgs(const Napi::CallbackInfo &info, int exectedNumber)
{
    if (info.Length() != exectedNumber)
    {
        throwError(info, "wrong argument number");
        return false;
    }
    return true;
}

void throwError(const Napi::CallbackInfo &info, const std::string &exception)
{
    Napi::TypeError::New(info.Env(), exception)
        .ThrowAsJavaScriptException();
}

void throwError(const Napi::Env &env, const std::string &exception)
{
    Napi::TypeError::New(env, exception)
        .ThrowAsJavaScriptException();
}
unsigned int downloadImage(const std::string &src, ImageContent *content)
{
    CURL *curl_handle;
    CURLcode res;

    content->memory = (char *)malloc(1); /* will be grown as needed by the realloc above */
    content->size = 0;                   /* no data at this point */

    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init();
    curl_easy_setopt(curl_handle, CURLOPT_URL, src.c_str());
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, writeMemoryCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)content);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, TIMEOUT_VALUE);
    res = curl_easy_perform(curl_handle);
    if (res != CURLE_OK)
    {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
        content->size = 0;
    }
    curl_easy_cleanup(curl_handle);
    curl_global_cleanup();

    return content->size;
}

void encodePixelsToPNGFile(std::string filename, uint8_t *buffer, int width, int height)
{
    //write the pixles to file
    unsigned error = lodepng::encode(filename.c_str(), buffer, width, height);
    if (error)
    {
        std::cout << "encoder error " << error << ": " << lodepng_error_text(error) << filename << std::endl;
    }
}

void decodeFile2Pixels(std::string filename, std::vector<unsigned char> &image)
{
    unsigned width, height;
    //decode image
    unsigned error = lodepng::decode(image, width, height, filename.c_str());
    //if there's an error, display it
    if (error)
        std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
}

//根据图片的内容查看图片格式
PIC_FORMAT getImageTypeByMagic(const unsigned char *data, unsigned int len)
{
    if (len < 16)
        return UNKOWN_PIC_FORMAT;

    // .jpg:  FF D8 FF
    // .png:  89 50 4E 47 0D 0A 1A 0A
    // .gif:  GIF87a
    //        GIF89a
    // .tiff: 49 49 2A 00
    //        4D 4D 00 2A
    // .bmp:  BM
    // .webp: RIFF ???? WEBP
    // .ico   00 00 01 00
    //        00 00 02 00 ( cursor files )

    switch (data[0])
    {
    case (unsigned char)'\xFF':
        return (!strncmp((const char *)data, "\xFF\xD8\xFF", 3)) ? JPEG_FORMAT : UNKOWN_PIC_FORMAT;

    case (unsigned char)'\x89':
        return (!strncmp((const char *)data,
                         "\x89\x50\x4E\x47\x0D\x0A\x1A\x0A", 8))
                   ? PNG_FORAMT
                   : UNKOWN_PIC_FORMAT;
    default:
        return UNKOWN_PIC_FORMAT;
    }
}

PIC_FORMAT getPicFormatFromContent(char *content, int len)
{
    if (content == nullptr || len <= 0)
    {
        return UNKOWN_PIC_FORMAT;
    }
    return getImageTypeByMagic((unsigned char *)content, (unsigned int)len);
}

int readLocalImage(const std::string &path, ImageContent *content)
{
    FILE *pFile;
    size_t result; // 返回值是读取的内容数量

    pFile = fopen(path.c_str(), "rb");

    if (pFile == NULL)
    {
        printf("file not exist \n");
        return -1;
    }
    fseek(pFile, 0, SEEK_END);    // 指针移到文件末位
    content->size = ftell(pFile); // 获得文件长度
    rewind(pFile);
    content->memory = (char *)malloc(sizeof(char) * content->size); // 分配缓冲区，按前面的 lSize

    if (content->memory == nullptr)
    {
        printf("memory allocate fail\n");
        free(content->memory);
        return -1;
    } // 内存分配错误，退出2

    result = fread(content->memory, 1, content->size, pFile); // 返回值是读取的内容数量
    if (result != content->size)
    {
        printf("read file error\n");
        return -1;
    }
    fclose(pFile);
    return content->size;
}

void encodePixelsToJPEGFile(std::string filename, uint8_t *buffer, int width, int height)
{
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPROW row_pointer[1]; /* pointer to JSAMPLE row[s] */
    cinfo.err = jpeg_std_error(&jerr);
    int row_stride;
    jpeg_create_compress(&cinfo);
    FILE *outfile;
    if ((outfile = fopen(filename.c_str(), "wb")) == NULL)
    {
        std::cout << "file not found " << filename << std::endl;
        return;
    }
    jpeg_stdio_dest(&cinfo, outfile);
    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = 4;
    cinfo.in_color_space = JCS_EXT_RGBA;
    jpeg_set_defaults(&cinfo);
    jpeg_start_compress(&cinfo, TRUE);
    row_stride = width * 4;
    while (cinfo.next_scanline < cinfo.image_height)
    {
        row_pointer[0] = &buffer[cinfo.next_scanline * row_stride];
        (void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }
    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
    fclose(outfile);
}

void decodeFromJEPGImage(std::vector<unsigned char> &pixels, unsigned int &width, unsigned int &height, const unsigned char *content, int len)
{
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPARRAY buffer;
    cinfo.err = jpeg_std_error(&jerr);
    int row_stride;
    jpeg_create_decompress(&cinfo);
    jpeg_mem_src(&cinfo, content, len);
    (void)jpeg_read_header(&cinfo, TRUE);
    (void)jpeg_start_decompress(&cinfo);
    //rgba
    cinfo.output_components = 4;
    cinfo.out_color_space = JCS_EXT_RGBA;
    width = cinfo.output_width;
    height = cinfo.output_height;
    row_stride = cinfo.output_width * cinfo.output_components;
    buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, 1);
    while (cinfo.output_scanline < cinfo.output_height)
    {
        (void)jpeg_read_scanlines(&cinfo, buffer, 1);
        for (int i = 0; i < row_stride; i++)
        {
            pixels.push_back(buffer[0][i]);
        }
    }
    (void)jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
}

void encodeJPEGInBuffer(unsigned char **out,unsigned long &size ,unsigned char *data,int width,int height)
{   
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPROW row_pointer[1]; /* pointer to JSAMPLE row[s] */
    cinfo.err = jpeg_std_error(&jerr);
    int row_stride;
    jpeg_create_compress(&cinfo);
    unsigned long tmpSize=0;
    jpeg_mem_dest(&cinfo,out,&tmpSize);
    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = 4;
    cinfo.in_color_space = JCS_EXT_RGBA;
    jpeg_set_defaults(&cinfo);
    jpeg_start_compress(&cinfo, TRUE);
    row_stride = width * 4;
    while (cinfo.next_scanline < cinfo.image_height)
    {
        row_pointer[0] = &data[cinfo.next_scanline * row_stride];
        (void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }
    //todo fix jpeg_mem_dest tmpsize always 4096
    size= 4*width*height;
    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
}

void decodeFromPNGImage(std::vector<unsigned char> &pixels, unsigned int &width, unsigned int &height, const unsigned char *content, int len)
{
    lodepng::decode(pixels, width, height, content, len);
}

void encodePNGInBuffer(std::vector<unsigned char> &in,unsigned char *data,int width,int height)
{
    lodepng::encode(in, data, width, height);
}

} // namespace NodeBinding