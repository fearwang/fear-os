#include <serial.h>
#include <string.h>

char *hint  = "fear@jz2440$";


int main()
{
    unsigned char c;
    uart0_init();   // ������115200��8N1(8������λ����У��λ��1��ֹͣλ)

    int i = 0;
    for(i = 0; i < strlen(hint); i++) {
   // for(i = 0; i < 5; i++) {
        putc(*(hint+i));
    }

    while(1)
    {
        // �Ӵ��ڽ������ݺ��ж����Ƿ����ֻ���ĸ���������1�����
        c = getc();
        if (isDigit(c) || isLetter(c))
            putc(c+1);
    }

    return 0;
}
