#include <stdio.h>
#include "input.h"

#define RETVALUE INT5 + INT5

int foo()
{
    return INT5;
}

int main(void) // This is the main
{
    int a;
    int b = a + foo();
    printf("%d\n", b); // We print the value of b
    return RETVALUE;
}