int main()
{
    int a = 5;
    int b = 0;
    while(a > 0)
    {
        if(a == 3)
        {
            break;
        }
        else
        {
            
        }
        if(a == 4)
        {
            a = a - 1;
            continue;
        }
        b = a + b;
        a = a - 1;
    }
    printf("%d %d\n", a, b);

    return 0;
}