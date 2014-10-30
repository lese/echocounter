#include "test.h"

#include <stdio.h>
#include <string.h>

void copy_hello(char* buffer, int length)
{
    const char* hello_str = "Hello World";
    if (length > strlen(hello_str)) {
        memcpy(buffer, hello_str, strlen(hello_str) + 1);
    }
}
