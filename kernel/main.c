#include <serial.h>
#include <string.h>

char *hint  = "fear@jz2440$ ";


int main()
{
    unsigned char c;
    int prev_new_line = 0;
    uart0_init();   // 波特率115200，8N1(8个数据位，无校验位，1个停止位)
    
    putc('\n');
    putc('\r');
    int i = 0;
    for(i = 0; i < strlen(hint); i++) {
        putc(*(hint+i));
    }

    while(1)
    {
        // 从串口接收数据后，判断其是否数字或子母，若是则加1后输出
        c = getc();
        if (isDigit(c) || isLetter(c)) {
            if(prev_new_line == 1)
                prev_new_line = 0;
            putc(c);
        }
        if(c == '\n') {
            prev_new_line = 1;
            putc('n');
        } 
        if(c == '\r') {
            prev_new_line = 0;
            putc('\n');
            putc('\r');
            for(i = 0; i < strlen(hint); i++) {
                putc(*(hint+i));
            }
        }
    }

    return 0;
}
