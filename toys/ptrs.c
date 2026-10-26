/*
 * CS 261 Example
 *
 * Pointer tracing exercises. Demonstrates pointer declaration, initialization,
 * and dereferencing.
 */

#include <stdio.h>
#include <stdlib.h>

#define PRINT_STATUS printf("x=%d y=[%d,%d,%d,%d] p=%lx *p=%d\n", x, y[0], y[1], y[2], y[3], (unsigned long)p, *p)

void p1()
{
    int x = 1;
    int y[4] = { 2, 3, 4, 5 };
    int *p = &x;
    PRINT_STATUS;

    *p = 6;
    PRINT_STATUS;

    p = y;
    PRINT_STATUS;

    *p = 7;
    PRINT_STATUS;
}

void p2()
{
    int a = 42;
    int b = 7;
    int c = 999;
    int *t = &a;
    int *u = NULL;
    printf("%d %d\n", a, *t);

    c = b;
    u = t;
    printf("%d %d\n", c, *u);

    a = 8;
    b = 8;
    printf("%d %d %d %d\n", b, c, *t, *u);

    *t = 123;
    printf("%d %d %d %d %d\n", a, b, c, *t, *u);
}

void p3()
{
    int a = 42;
    static int b = 7;

    int *t = &a;
    printf("a=%d *t=%d\n", a, *t);

    t = (int *) malloc( sizeof(int) ) ;
    if ( ! t )   // same as typing if( t == NULL )
    {
        puts("Ran out of Heap memory");
        return ;
    }
    *t = b ;
    printf("a=%d *t=%d  b=%d\n", a, *t , b);

    *t = a+b ;
    printf("a=%d *t=%d  b=%d\n", a, *t , b);

    free( t ) ;
}

int main()
{
    printf("Calling p1()\n");
    p1() ;

    printf("\nCalling p2()\n");
    p2() ;

    printf("\nCalling p3()\n");
    p3() ;
    return 0;
}
