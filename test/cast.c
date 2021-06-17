/*
    cast.c
    涉及功能：隐式、显式cast，指针和数组
*/

int main()
{
    int a = 3., *pa;
    double b;
    double* pb;
    char c, *pc;

    printf("a1: %d\n", a);

    b = a;
    printf("b1: %f\n", b);

    a = 3e-5;
    printf("a2: %d\n", a);

    a = 1.251e+2;
    printf("a3: %d\n", a);
    b = (double)a;
    printf("b2: %f\n", b);

    c = 'c';
    pc = &c;
    pa = (int*)pc;
    pc = (char*)pa;
    printf("c in char: %c\n", *pa);
    printf("c in int: %d\n", *pc);

    return 0; 
}