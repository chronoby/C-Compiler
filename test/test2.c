int fib(int x)
{
    if(x == 1 || x == 2)
    {
        return 1;
    }
    else
    {
        return fib(x - 1) + fib(x - 2);
    }
    return 0;
}

int main()
{
    int a;
    double c = 10;
    scanf("%d", &a);
    c = (double)a;
    printf("%f\n", c);
    return 0;
}