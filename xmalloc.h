#ifndef XMALLOC_H
#define XMALLOC_H

#include <stdlib.h>
// 本文件依赖 malloc 函数
void *malloc_or_exit(size_t size, const char *file, int line);

#define xmalloc(size) malloc_or_exit((size), __FILE__, __LINE__)

#endif