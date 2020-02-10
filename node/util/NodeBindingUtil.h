#ifndef UTIL_H
#define UTIL_H
#include <napi.h>
#define TIMEOUT_VALUE 5L
namespace NodeBinding
{
struct ImageContent
{
    char *memory;
    unsigned int size;
};
int readLocalImage(const std::string &path, ImageContent *content);
bool checkArgs(const Napi::CallbackInfo &info, int exectedN);
unsigned int downloadImage(const std::string &src, ImageContent *content);
void throwError(const Napi::CallbackInfo &info, const std::string &exception);
void throwError(const Napi::Env &env, const std::string &exception);
} // namespace NodeBinding
#endif