#include <serial.h>
#include <string.h>

char *hint  = "fear@jz2440$ ";
char *lable  = "Fear OS !!!\n\r";


int start_kernel()
{
    unsigned char c;
    int prev_new_line = 0;
    uart0_init();   // ������115200��8N1(8������λ����У��λ��1��ֹͣλ)
    
    putc('\n');
    putc('\r');
    int i = 0;
    for(i = 0; i < strlen(hint); i++) {
        putc(*(lable+i));
    }

   init_sys_mmu();
   start_mmu();

    for(i = 0; i < strlen(hint); i++) {
        putc_mmu(*(hint+i));
    }
    while(1)
    {
        // �Ӵ��ڽ������ݺ��ж����Ƿ����ֻ���ĸ���������1�����
        c = getc_mmu();
        if (isDigit(c) || isLetter(c)) {
            if(prev_new_line == 1)
                prev_new_line = 0;
            putc_mmu(c);
        }
        if(c == '\n') {
            prev_new_line = 1;
            putc_mmu('n');
        } 
        if(c == '\r') {
            prev_new_line = 0;
            putc_mmu('\n');
            putc_mmu('\r');
            for(i = 0; i < strlen(hint); i++) {
                putc_mmu(*(hint+i));
            }
        }
    }

    return 0;
}
