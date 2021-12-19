#pragma once
#include <mlib.h>

#define COUT 1 //console out
#define SOUT 2 //serial out

typedef void FILE;

void puts(const char* str);
void putchar(int ch);

FILE *fopen(const char* file, const char* mode);
void fread(void* buffer, size_t sizeData, size_t numbersData,FILE *file);