

int main()
{
    int c[5] = {1, 2, 3, 4, 5};
    int* ptr = c;
    int** pptr = &ptr; 

    printf("%d\n", pptr[0][2]);
    return 0;
}