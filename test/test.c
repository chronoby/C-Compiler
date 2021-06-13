int x = 12 + 6 * 5 - 4; 
int y = 2;

int plus(int a, int b)
{
    return a * b;
}

int minus(int a, int b)
{
    return a - b;
}

int main()
{
    int ans = plus(x, y);
    ans = minus(x, y);

    return ans + 5;
}