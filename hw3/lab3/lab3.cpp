#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <vector>
#include "asm-arm/arch-pxa/lib/creator_pxa270_lcd.h"

using namespace std;

int main()
{
	int fd;
    int ret;

    /*LED*/
    unsigned short led = LED_ALL_ON;

    /*Key pad*/
    unsigned short key;
    //vector<unsigned short> orders;
    /*LCD monitor*/
    int i = 0, size = 4;
    lcd_write_info_t display;
    char * ary = new char[size];
    /*7-seg*/
    _7seg_info_t seg;

    printf ( "Read driver...\n" ) ;
    if  (( fd= open("/dev/lcd", O_RDWR)) < 0) {

        printf("Open /dev/lcd faild. \n");
        exit(-1);
    }

    printf ( "Initialize devices...\n" ) ;
    /*Clear device*/
    ioctl(fd,LED_IOCTL_SET,&led);
    ioctl(fd, KEY_IOCTL_CLEAR,key);
    ioctl(fd,LCD_IOCTL_CLEAR,NULL);
    
    printf ( "Section 1 start...\n" ) ;
    cout << "waiting for key pad input...\n" << endl;
    /*Section 1*/
    while(1) {
        ret = ioctl(fd,KEY_IOCTL_CHECK_EMTPY, &key);
        if (ret < 0) {
            sleep(1);
            continue;
        }
        ret = ioctl(fd, KEY_IOCTL_GET_CHAR, &key);
        if((key & 0xff) == '#')
            break;
        else {
            ary[i] = key;
            printf("%c, %d\n",ary[i],ary[i]);
            display.Count = sprintf((char*) display.Msg,&ary[i++]);
            ioctl(fd, LCD_IOCTL_WRITE,&display);
            if(i == size ) {
                cout << "out of range!\nEnter the next section!" << endl;
                break;
            }
        }
    }
    /*Excution mode*/
    // LCD, LED, 7-seg clear
    unsigned short led_bit[8] = {0,0,0,0,0,0,0,0};
    cout << "LED off" << endl; 
    led = LED_ALL_OFF;
    ioctl(fd,LED_IOCTL_SET,& led);
    cout << "LCD clear" << endl;
    ioctl(fd,LCD_IOCTL_CLEAR,NULL);
    cout << "7-seg off" << endl;
    ioctl(fd, _7SEG_IOCTL_ON,NULL);
    seg.Mode = _7SEG_MODE_PATTERN;
    seg.Which = _7SEG_ALL;
    seg.Value = 0x00000000;
    ioctl(fd, _7SEG_IOCTL_SET,&seg);
    sleep(1);

    /*Final function*/    

    cout << "Second Section start!" << endl;
    unsigned long table[18] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x67,0x77,0x7c,0x39,0x5e,0x79,0x71,0x80,0x00};
    unsigned long show[4] = {0x00,0x00,0x00,0x00};
    i =0;
    while(1) {
        int mark = ary[i];
        mark -= 48;
        if(mark == -6)
            mark = 16;
        else if(mark >= 17 && mark < 21)
            mark -= 7;
        else;
        cout << "7-seg : " << mark << endl;
        show[3] = show[2];
        show[2] = show[1];
        show[1] = show[0];
        show[0] = table[mark];
        seg.Which = 8;
        seg.Value = show[0];
        ioctl(fd,_7SEG_IOCTL_SET,&seg);
        seg.Which = 4;
        seg.Value = show[1];
        ioctl(fd,_7SEG_IOCTL_SET,&seg);
        seg.Which = 2;
        seg.Value = show[2];
        ioctl(fd,_7SEG_IOCTL_SET,&seg);
        seg.Which = 1;
        seg.Value = show[3];
        ioctl(fd,_7SEG_IOCTL_SET,&seg);
    //led
        mark = ary[i];
        mark -= 48;
        led = (mark&0x0f);
        led -= 1;
        cout << "led" <<  mark <<endl;
        if(mark < 9 && mark > 0) {            
            if(led_bit[led] == 0) {
                led_bit[led] = 1;
                ioctl(fd,LED_IOCTL_BIT_SET,&led);
            }
            else {
                led_bit[led] = 0;
                ioctl(fd,LED_IOCTL_BIT_CLEAR,&led);
            }
        }
        else {
            cout << "less or bigger than led size" << endl;
        }
    //lcd
        mark = ary[i];
        mark -= 48;
        if( mark >= 0 && mark < 9 ) {
            if ( mark%2 == 0){
                cout << " even number : " << mark << endl;
                display.Count = sprintf((char*) display.Msg,
                "       X       \n       X       \n       X       \n       X       \n       X       \n       X       \nXXXXXXXXXXXXXXX\n");
                ioctl(fd, LCD_IOCTL_WRITE,&display);
                display.Count = sprintf((char*) display.Msg,
                "       X       \n       X       \n       X       \n       X      \n       X       \n       X       \n");
            }
            else {
                cout << " odd number :  " << mark << endl;
                display.Count = sprintf((char*) display.Msg,
                " X           X \n  X         X  \n   X       X   \n    X     X    \n     X   X     \n      X X      \n       X       \n");
                ioctl(fd, LCD_IOCTL_WRITE,&display);
                display.Count = sprintf((char*) display.Msg,
                "      X X      \n     X   X     \n    X     X    \n   X       X   \n  X         X  \n X           X \n");
            }
            ioctl(fd, LCD_IOCTL_WRITE,&display);
        }
        else {
            cout << "IsAlpha! abendon it : " << ary[i] << endl;
        }
        sleep(2);
        ioctl(fd,LCD_IOCTL_CLEAR,NULL);
        sleep(1);
        i++;
        if(i == size)
            i = 0;
    }
    close(fd);
	return 0;
}
