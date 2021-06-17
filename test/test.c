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
    long a = 5;
    long * ptr = &a;
    short* ptrl = (short*)ptr;
    
    printf("%d", *ptrl);

    print("violate");
    return 0;
}