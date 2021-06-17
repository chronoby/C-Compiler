// fib.c ： 计算斐波那契数列
// 涉及功能，含参的函数，全局和局部变量, 整数运算，递归，输入输出，注释, if-else, while, logical-or, break

int fib(int x)
{
    if (x == 1 || x == 2)
    {
        return 1;
    }
    else {
        return fib(x - 1) + fib(x - 2);
    }
    
    return 0;
}

int a = 5;

int main()
{
    int b = -2;
    printf("the No.%d item of fibonacci sequence is %d\n", a, fib(a));
    
    printf("please enter a POSITIVE integer\n");
    
    while (b < 0)
    {
        scanf("%d", &b);
        if (b > 0)
        {
            break;
        }
        else
        {
            printf("ERROR: cannot handle non-positive integer\n");
        }
    }
    
    printf("the No.%d item of fibonacci sequence is %d\n", b, fib(b));

    return 0;
}