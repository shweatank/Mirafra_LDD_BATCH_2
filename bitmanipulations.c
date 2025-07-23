
//BIT MANIPULATIONS
//bit manipulations
#include<stdio.h>
int printbinary(int num);
int main()
{
    int num,choice,pos,n;
    printf("Enter the number ");
    scanf("%d",&num);
   // printbinary(num);
    while(1)
    {
        printf("1 to check bit is set or not\n");
        printf("2 set the bit\n");
        printf("3 clear the bit\n");
        printf("4 toggle the bit\n");
        printf("5 count the no of set bits\n");
        printf("6 create a bit mask for n no of bits\n");
        printf("7 create a bit mask for n no of bits starting from pos\n");
        printf("8 Extracting n bits from pos\n");
        printf("9 masking the bit\n");
        printf("Exit\n");
        
        printf("Enter the choice\n");
        scanf("%d",&choice);
        
        
        
        if(choice>=1 && choice<=9)
        {
            switch(choice)
            {
                case 1:
                {
                    printf("Enter the pos\n");
                    scanf("%d",&pos);
                    int bit=num&(1<<pos);
                    if(bit)
                    {
                        printf("bit is set\n");
                        
                    }
                    else
                    {
                        printf("Not a set bit\n");
                    }
                    
                    break;
                }
                case 2:
                {
                    printf("Enter the pos\n");
                   scanf("%d",&pos);
                    int bit = num | (1<<pos);
                    printbinary(bit);
                    break;
                }
                case 3:
                {
                    printf("Enter the pos\n");
                    scanf("%d",&pos);
                    int bit = num & ~(1<<pos);
                    printbinary(bit);
                    break;
                }
                case 4:
                {
                    printf("Enter the pos\n");
                    scanf("%d",&pos);
                    int bit = num ^ (1<<pos);
                    printbinary(bit);
                    break;
                }
                case 5:
                {
                    int count = printbinary(num);
                    printf("Count is %d\n",count);
                    break;
                }
                case 6:
                {
                   
                    printf("Enter no of bits needed to be masked");
                    scanf("%d",&n);
                    int bit = (1<<n)-1;
                    printbinary(bit);
                    break;
                }
                case 7:
                {
                     printf("Enter no of bits needed to be masked");
                     scanf("%d",&n);
                     printf("Enter the pos\n");
                     scanf("%d",&pos);
                     int bit = ((1<<n)-1)<<pos;
                     printbinary(bit);
                     break;
                }
                case 8:
                {
                    //extracting bits
                     printf("Enter no of bits needed to be extracted");
                    scanf("%d",&n);
                    printf("Enter the pos");
                    scanf("%d",&pos);
                    int bit = (num>>pos) & ((1<<n)-1);
                    printbinary(bit);
                    break;
                }
                case 9:
                {
                    printf("Enter the bit position to mask: ");
                    scanf("%d", &pos);
                    int mask = 1 << pos;
                    printf("Bit mask for position %d: ", pos);
                    printbinary(mask);
                    break;
                }
                default:
                {
                    printf("Invalid choice");
                }
            }
        }
    }
    return 0;
}
int printbinary(int num)
{
    int bit,count = 0;
    for(int i=7;i>=0;i--)
    {
        int bit = (num>>i)&1;
        printf("%d",bit);
        if(bit)
        {
            count++;
        }
    }
    printf("\n");
    return count;
}
