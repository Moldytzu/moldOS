#include <misc/bitmap/bitmap.h>

bool Bitmap::operator[](uint64_t index){
    return Get(index);
}

bool Bitmap::Get(uint64_t index){
    if(index >Size * 8) return false;
    return (Buffer[index/8] & (0b10000000 >> index%8)) > 0;
}

bool Bitmap::Set(uint64_t index, bool value) {
    if(index >Size * 8) return false;
    Buffer[index/8] &= ~0b10000000 >> index%8;
    if(value) Buffer[index/8] |= 0b10000000 >> index%8;
    return true;
}