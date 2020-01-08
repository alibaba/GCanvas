#include "NodeBindingUtil.h"
#include "lodepng.h"
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
    struct ChunkContent *mem = (struct ChunkContent *)userp;

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

unsigned int downloadImage(const std::string &src, ChunkContent *content)
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
    curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, 1L);
    res = curl_easy_perform(curl_handle);
    if (res != CURLE_OK)
    {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
        content->size = -1;
    }
    curl_easy_cleanup(curl_handle);
    curl_global_cleanup();

    return content->size;
}

void encodePixelsToFile(std::string filename, uint8_t *buffer, int width, int height)
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
} // namespace NodeBinding