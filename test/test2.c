// int fib(int x)
// {
//     if(x == 1 || x == 2)
//     {
//         return 1;
//     }
//     else
//     {
//         return fib(x - 1) + fib(x - 2);
//     }
//     return 0;
// }

int main()
{
    int aa[5];
    char a = 'a';
    short b = 2;
    int c = a + b;
    float d = 3.0;
    double e = 7.5;
    

    double f = a + b;
    f = c + e;
    // double x = 1.2;
    // float y = 1.;
    // printf("%d\n", fib(a));
    printf("%d %lf\n", c, f);
    return 0;
}