#include <stdio.h>
#include <s3c24xx.h>

#define TXD0READY   (1<<2)
#define RXD0READY   (1)
int main()
{
    char *tmp = "this is main.elf\n\r";
    int i = 0;
    while(tmp[i]) {
        while (!(UTRSTAT0 & TXD0READY));
        UTXH0 = tmp[i];
        i++;
    }
}

