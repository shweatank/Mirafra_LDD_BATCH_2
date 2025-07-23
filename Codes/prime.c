#include <stdio.h>

int prime(int n)
{
    int i=2;
    while(i)
    {
        if(n<2)
        {
            return 0;
        }
        if(n ==2)
        {
            return 1;
        }
        
        if(n%i == 0)
        {
            return 0;
        }
        if(i*i >2)
        {
            return 1;
        }
        i++;
    }
}

int main()
{
    int num = 53;
    int var = 1;
    while(var < num)
    {
        var = var*10;
        int copy= num;
        while(copy)
        {
            if(copy>(var/10) && prime(copy%var))
            {
                printf("prime = %d\n",copy%var);
            }
            copy = copy/var;
        }
    }
    
    return 0;
}
