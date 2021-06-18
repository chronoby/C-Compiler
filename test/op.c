/*
op.c
涉及功能：基本类型，八进制数，十六进制数，一维数组的声明，读写，{ }初始化，运算符类型检查，隐式类型转换,报错

*/

int arr1[5] = {1, 2, 3, 4, 5};

int main()
{
    char a = 'a';
    short b = 2;
    int c = 3;
    int d = 012;
    int e = 0x12;
    float f = 3.0;
    double g = 7.5;
    double arr2[8] = {7.0, 6.0, 5.0, 4.0, 3.0, 2.0, 1.0, 0.};
    double h = a + g;
    double i = d - c;
    double j = e * f;
    int k = d % c;

    // int k = f % c; // error!
    arr2[3] = 10.0;
    
    printf("%lf %lf %lf %d\n", h, i, j, k);
    printf("%d %d %d %d %d\n", arr1[0], arr1[1], arr1[2], arr1[3], arr1[4]);
    printf("%lf %lf %lf %lf %lf\n", arr2[0], arr2[1], arr2[2], arr2[3], arr2[4]);
    if(k == 1)
        printf("bool1\n");
    if(k != 1)
        printf("bool2\n");
    if(k > 2)
        printf("bool3\n");
    if(k == 1 || c != 3)
        printf("bool4\n");
    
    return 0;
}