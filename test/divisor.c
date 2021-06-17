/*
    divisor.c: 求因数
*/

void divisor(int c)
{
    int i = 1;
    while (i <= c)
    {
        if (c % i != 0)
        {
            i++;
            continue;
        } else {
            printf("%d, ", i);
        }
        i++; 
    }
    printf("\n");
}

int main()
{
    int x;
    scanf("%d", &x);
    divisor(x);

    return 0;
}