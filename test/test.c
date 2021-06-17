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
    int c = 0x1F;

    printf("%d\n", c);

    c = 010;

    printf("%d\n", c);

    return 0;
}