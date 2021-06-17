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
    scanf("%d", &a);
    printf("%d\n", fib(a));
    return 0;
}