/*
    local.c
    涉及功能: 函数调用，变量作用域

*/

double c;

void setC()
{
    c = 2;
}

void echoC()
{
    printf("global c: %f\n", c);
}

int main()
{
    double c;
    c = 5;
    printf("local c: %f\n", c);
    
    echoC();
    setC();
    echoC();

    return 0;
}

