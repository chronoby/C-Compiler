int main()
{
    double a = 12.;
    double *pa = &a;
    double **ppa = &pa;
    double ***pppa = &ppa;

    printf("%lf\n", ***pppa);

<<<<<<< HEAD
    return 0;
=======
int main()
{
    int arr[10];
    int ans = plus(x, y);
    ans = minus(x, y);
    if(ans == 26)
    {
        if(ans != 0)
            ans = 11;
    }
    else
    {
        ans = 22;
    }
    while(ans > 0 && y < 200)
    {
        y = y + 1;
    }
    return y;
>>>>>>> 4c4341d46a4a8a3afed7df5c700be09b85a77a73
}