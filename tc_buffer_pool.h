#ifndef __TC_BUFFERPOOL_H
#define __TC_BUFFERPOOL_H

#include <list>
#include <vector>
#include <string>


namespace tars
{

struct TC_Slice
{
    explicit TC_Slice(void* d = NULL , size_t ds = 0, size_t l = 0);
    void* data;
    size_t dataLen;
    size_t len;

};

}

#endif
