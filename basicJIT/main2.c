#include <stdio.h>

int mul(int a, int b)
{
    return a * b;
}

int main()
{
    printf("%d * %d = %d\n", 5, 11, mul(5, 11));
}