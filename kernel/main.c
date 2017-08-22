#include <serial.h>
#include <string.h>

char *hint  = "fear@jz2440$ ";


int main()
{
    unsigned char c;
    int prev_new_line = 0;
    uart0_init();   // ������115200��8N1(8������λ����У��λ��1��ֹͣλ)
    
    putc('\n');
    putc('\r');
    int i = 0;
    for(i = 0; i < strlen(hint); i++) {
        putc(*(hint+i));
    }

    while(1)
    {
        // �Ӵ��ڽ������ݺ��ж����Ƿ����ֻ���ĸ���������1�����
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
