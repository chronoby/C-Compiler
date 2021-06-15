int main()
{
    double a = 12.;
    double *pa = &a;
    double **ppa = &pa;
    double ***pppa = &ppa;

    printf("%lf\n", ***pppa);

    return 0;
}