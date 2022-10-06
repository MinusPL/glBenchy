#include <stdio.h>

int main(int argc, char** argv)
{
#ifndef CONSOLE
    printf("NO CONSOLE!");
#endif
    printf("Welcome from Linux Main!");
    return 0;
}