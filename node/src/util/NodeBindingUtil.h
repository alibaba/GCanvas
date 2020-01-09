#ifndef UTIL_H
#define UTIL_H
#include <napi.h>
namespace NodeBinding
{
struct ChunkContent
{
    char *memory;
    unsigned int size;
};
int readlocalFile(const std::string &path, ChunkContent *content);
bool checkArgs(const Napi::CallbackInfo &info, int exectedN);
unsigned int downloadImage(const std::string &src, ChunkContent *content);
void throwError(const Napi::CallbackInfo &info, const std::string &exception);
} // namespace NodeBinding
#endif