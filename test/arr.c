/*
    arr.c
    涉及功能：八进制，十六进制。全局、局部数组的声明、初始化，读写，全局变量声明；++, --, 
*/

int arr1[3] = {10, 010, 0x10}, i;

int main()
{
    int arr2[3] = {199, 0171, 0x3F3F};
    
    while (i < 3)
    {
        printf("%d, ", arr1[i++]);
        printf("%d\n", arr2[--i]);
        ++i;
        i--;
        ++i;
    }

    arr2[0] = 15;
    arr2[1] = i;
    arr2[2] = arr1[2];

    i = 0;
    while (i < 3)
    {
        arr1[i] = arr2[i];
        printf("%d, ", arr1[i]);
        printf("%d\n", arr2[i++]);
    }

    return 0;
}