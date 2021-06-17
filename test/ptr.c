/*
ptr.c
涉及功能：注释，double，指针，多级指针，指针下标，隐式cast，变量声明和初始化, if

*/

int main()
{
    double c = 1, d = 2, *pc = &c, **ppc, *pd = &d;
    double ***pppc;

    int d_ = d;

    ppc = &pc;
    pppc = &ppc;

    printf("d: %f, *pd: %f\n", d, *pd);
    printf("%d\n", d_);

    if (&(**pppc) == ppc)
    {
        printf("they are equal(1)\n");
    }

    if (*(&(**pppc)) == *ppc)
    {
        printf("they are equal(2)\n");
    }

    if (pppc[0][0] == pc)
    {
        printf("they are equal(3)\n");
    }

    return 0;
}