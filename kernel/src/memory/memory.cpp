#include <memory/memory.h>
#include <xmmintrin.h> // SSE

const char* EFI_MEMORY_TYPE_STRINGS[] {
    "EfiReservedMemoryType",
    "EfiLoaderCode",
    "EfiLoaderData",
    "EfiBootServicesCode",
    "EfiBootServicesData",
    "EfiRuntimeServicesCode",
    "EfiRuntimeServicesData",
    "EfiConventionalMemory",
    "EfiUnusableMemory",
    "EfiACPIReclaimMemory",
    "EfiACPIMemoryNVS",
    "EfiMemoryMappedIO",
    "EfiMemoryMappedIOPortSpace",
    "EfiPalCode",
};

int memcmp(const void *aptr, const void *bptr, size_t n) {
	const unsigned char *a = (const unsigned char*)aptr, *b = (const unsigned char*)bptr;
	for (size_t i = 0; i < n; i++) {
		if (a[i] < b[i])
			return -1;
		else if (a[i] > b[i])
			return 1;
	}
	return 0;
}

uint64_t GetMemorySize(EFI_MEMORY_DESCRIPTOR* mMap, uint64_t mMapEntries, uint64_t mMapDescSize){
    static uint64_t memorySizeBytes = 0;
    if (memorySizeBytes > 0) return memorySizeBytes;

    for (int i = 0; i < mMapEntries; i++){
        EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)mMap + (i * mMapDescSize));
        memorySizeBytes += desc->numPages * 4096;
    }

    return memorySizeBytes;
}

__attribute__((optimize("-fno-tree-loop-distribute-patterns"))) void memset(void* start, uint64_t value, uint64_t num)  {
    if (num <= 8) {
        uint8_t* valPtr = (uint8_t*)&value;
        for (uint8_t* ptr = (uint8_t*)start; ptr < (uint8_t*)((uint64_t)start + num); ptr++) {
            *ptr = *valPtr;
            valPtr++;
        }
        return;
    }
    uint64_t proceedingBytes = num % 8;
    uint64_t newnum = num - proceedingBytes;

    for (uint64_t* ptr = (uint64_t*)start; ptr < (uint64_t*)((uint64_t)start + newnum); ptr++) {
        *ptr = value;
    }
    uint8_t* valPtr = (uint8_t*)&value;
    for (uint8_t* ptr = (uint8_t*)((uint64_t)start + newnum); ptr < (uint8_t*)((uint64_t)start + newnum); ptr++) {
        *ptr = *valPtr;
        valPtr++;
    }
};
__attribute__((optimize("-fno-tree-loop-distribute-patterns"))) void memcpy(void* destination, void* source, uint64_t num) {
    if (num <= 8) {
        uint8_t* valPtr = (uint8_t*)&source;
        for (uint8_t* ptr = (uint8_t*)destination; ptr < (uint8_t*)((uint64_t)destination + num); ptr++) {
            *ptr = *valPtr;
            valPtr++;
        }
        return;
    }
    uint64_t proceedingBytes = num % 8;
    uint64_t newnum = num - proceedingBytes;
    uint64_t* srcptr = (uint64_t*)source;

    for (uint64_t* destptr = (uint64_t*)destination; destptr < (uint64_t*)((uint64_t)destination + newnum); destptr++) {
        *destptr = *srcptr;
        srcptr++;
    }
    uint8_t* srcptr8 = (uint8_t*)((uint64_t)source + newnum);
    for (uint8_t* destptr8 = (uint8_t*)((uint64_t)destination + newnum); destptr8 < (uint8_t*)((uint64_t)destination + newnum); destptr8++) {
        *destptr8 = *srcptr8;
        srcptr8++;
    }
}

void slowmemset(void* start, uint8_t value, uint64_t num){
    for (uint64_t i = 0; i < num; i++){
        *(uint8_t*)((uint64_t)start + i) = value;
    }
}

__attribute__((optimize("-fno-tree-loop-distribute-patterns"))) void ssememcpy(void* dst, void* src, size_t size) {
    size_t stride = 8 * sizeof(__m128i);
    while (size)
    {
        __m128 a = _mm_load_ps((float*)(src + 0*sizeof(__m128)));
        __m128 b = _mm_load_ps((float*)(src + 1*sizeof(__m128)));
        __m128 c = _mm_load_ps((float*)(src + 2*sizeof(__m128)));
        __m128 d = _mm_load_ps((float*)(src + 3*sizeof(__m128)));
        __m128 e = _mm_load_ps((float*)(src + 4*sizeof(__m128)));
        __m128 f = _mm_load_ps((float*)(src + 5*sizeof(__m128)));
        __m128 g = _mm_load_ps((float*)(src + 6*sizeof(__m128)));
        __m128 h = _mm_load_ps((float*)(src + 7*sizeof(__m128)));
        _mm_store_ps((float*)(dst + 0*sizeof(__m128)), a);
        _mm_store_ps((float*)(dst + 1*sizeof(__m128)), b);
        _mm_store_ps((float*)(dst + 2*sizeof(__m128)), c);
        _mm_store_ps((float*)(dst + 3*sizeof(__m128)), d);
        _mm_store_ps((float*)(dst + 4*sizeof(__m128)), e);
        _mm_store_ps((float*)(dst + 5*sizeof(__m128)), f);
        _mm_store_ps((float*)(dst + 6*sizeof(__m128)), g);
        _mm_store_ps((float*)(dst + 7*sizeof(__m128)), h);

        size -= stride;
        src += stride;
        dst += stride;
    }
}

void asmmemcpy(void *d, const void *s, size_t n) {
    uint64_t d0, d1, d2; 
    asm volatile(
        "rep ; movsq\n\t""movq %4,%%rcx\n\t""rep ; movsb\n\t": "=&c" (d0),                                                                                   
        "=&D" (d1),
        "=&S" (d2): "0" (n >> 3), 
        "g" (n & 7), 
        "1" (d),
        "2" (s): "memory"
    );  //not mine (found it on reddit: https://www.reddit.com/r/C_Programming/comments/ivoqhk/understanding_the_assembly_code_of_memcpy/)
}