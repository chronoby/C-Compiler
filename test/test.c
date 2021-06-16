int* parr[10];

int x = 10;

int main()
{
    parr[0] = &x;
    printf("%d\n", *(parr[0]));    

    return 0;
}
