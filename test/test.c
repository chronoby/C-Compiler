void print(char* s)
{
    printf("%s", s);
}

int fib(int x)
{
    if (x == 0 || x == 1)
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
    double c = 15;
    int d = !!c;
    int e = !d;
    
    printf("%d\n", d);
    printf("%d\n", e);

    return 0;
}