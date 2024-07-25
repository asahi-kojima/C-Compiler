#include <stdio.h>

extern "C"
{
    void foo()
    {
        printf("OK\n");
    }


    int add2(int x, int y)
    {
        return x + y;
    }

    int add3(int x, int y, int z)
    {
        return add2(x, y) + z;
    }

    int add4(int x, int y, int z, int w)
    {
        return add3(x, y, z) + w;
    }
}