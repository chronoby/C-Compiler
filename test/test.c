int main()
{
    double a = 12.;
    double *pa = &a;
    double **ppa = &pa;
    double ***pppa = &ppa;

    int x = (15 + 4) * 9;

    printf("%lf\n", ***pppa);
    printf("%lf\n", *&*&a);
    printf("%lf\n", *&*pa);

    if (&(**ppa) == pa)
    {
        printf("they are equal\n");
    }

    printf("%d\n", x);

    return 0;
}