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
    if(ans == 26)
    {
        if(ans != 0)
            ans = 11;
    }
    else
    {
        ans = 22;
    }
    while(ans > 0)
    {
        y = y + 1;
    }
    return ans;
}