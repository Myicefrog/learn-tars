#include "tc_ex.h"
#include <execinfo.h>
#include <string.h>
#include <stdlib.h>
#include <cerrno>

namespace tars
{

TC_Exception::TC_Exception(const string &buffer)
:_buffer(buffer), _code(0)
{
//    getBacktrace();
}

TC_Exception::TC_Exception(const string &buffer, int err)
{
    _buffer = buffer + " :" + strerror(err);
    _code   = err;
//    getBacktrace();
}

TC_Exception::~TC_Exception() throw()
{
}

const char* TC_Exception::what() const throw()
{
    return _buffer.c_str();
}

void TC_Exception::getBacktrace()
{
    void * array[64];
    int nSize = backtrace(array, 64);
    char ** symbols = backtrace_symbols(array, nSize);

    for (int i = 0; i < nSize; i++)
    {
        _buffer += symbols[i];
        _buffer += "\n";
    }
    free(symbols);
}

}

