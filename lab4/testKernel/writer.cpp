#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <sys/fcntl.h>
#include <unistd.h>
#include <time.h>
#include <sstream>
using namespace std;

int main(int argc, char * argv[]){
	static char map[16] = {
    0b1111110, // 0
    0b0110000, // 1
    0b1101101, // 2
    0b1111001, // 3
    0b0110011, // 4
    0b1011011, // 5
    0b1011111, // 6
    0b1110000, // 7
    0b1111111, // 8
    0b1111011, // 9
    0b1110111, // A,a
    0b0011111, // B,b
    0b1001110, // C,c
    0b0111101, // D,d
    0b1001111, // E,e
    0b1000111  // F,f
};
	char mapping[16][7] = {
	{1,1,1,1,1,1,0},
	{0,1,1,0,0,0,0},
	{1,1,0,1,1,0,1},
	{1,1,1,1,0,0,1},
	{0,1,1,0,0,1,1},
	{1,0,1,1,0,1,1},
	{1,0,1,1,1,1,1},
	{1,1,1,0,0,0,0},
	{1,1,1,1,1,1,1},
	{1,1,1,1,0,1,1},
	{1,1,1,0,1,1,1},
	{0,0,1,1,1,1,1},
	{1,0,0,1,1,1,0},
	{0,1,1,1,1,0,1},
	{1,0,0,1,1,1,1},
	{1,0,0,0,1,1,1}
	};
	int fd;
	string str;
	stringstream ss;
	ss << argv[1];
	ss >> str;
	if((fd = open("/dev/mydev",O_RDWR)) < 0) {
		printf("Error open %s\n","/dev/mydev");
		//exit(-1);
	}
	int count = 0,i=0,ia=0;
	char a = str[i];
    	int n;
    	char buf[7]={1,1,1,1,1,1,0};
	/*for(int j = 0; j < 7; j++) {
		buf[j] = mapping[1][j];
		cout << buf[j] << endl;
	}*/
//	write(fd,mapping[3],7);
    
	while(i<str.length()){
		if (isalpha(a)){
		    	ia = toupper(a);
			ia = ia -55;
		}
		else {
       			ia = a;
			ia = ia -48;
		}
		if (ia > 15)
			ia = 0;
		write(fd,mapping[ia],7);
		i++;
		a = str[i];
		sleep(3);
	}

	close(fd);
	return 0;
}
