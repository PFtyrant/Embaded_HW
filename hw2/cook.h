#pragma once
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include "asm-arm/arch-pxa/lib/creator_pxa270_lcd.h"
using namespace std;

class cook {
	public:
	struct Step{
	        int index;
        	vector<string> material;
	        string tool;
	        string action;
	        unsigned int times;
	        string product;
	        void setMaterial(string m){
	                istringstream temp(m);
	                string s;
	                while(getline(temp, s, '+')){
	                        material.push_back(s);
			}
		}
	};
	
	public:
		/**
		* Read tools from file, return the amount of tools
		**/
		int setTool(char* filename){
			fstream f;
			string line;
			vector< vector<string> > data;
			f.open(filename, ios::in);
			if(!f){
				cerr << "Can't open file" << endl;
				return -1;	
			}
			while(getline(f, line)){
				string temp;
				vector<string> raw;
				istringstream temp_line(line);
				while(getline(temp_line, temp, ','))
					raw.push_back(temp);
				if(raw.size() != 2){
					cerr << filename << " format error" << endl;
					return -1;
				}
				data.push_back(raw);
			}
			int i;
			for(i=0;i<data.size();i++)
				tools.insert(pair<string, int>(data[i][0], atoi(data[i][1].c_str())));
			return data.size();
		}
		/**
		* Read orders from file, return the amount of orders
		**/
		int setOrder(char* filename){
			fstream f;
			string line;
			f.open(filename, ios::in);
			if(!f){
				cerr << "Can't open file" << endl;
				return -1;	
			}
			while(getline(f, line))
				orders.push_back(line);
			return orders.size();
		}
		/**
		* Read steps from file, return the amount of steps
		**/
		int setSteps(char* filename){
			fstream f;
			string line;
			vector< vector<string> > data;
			f.open(filename, ios::in);
			if(!f){
				cerr << "Can't open file" << endl;
				return -1;	
			}
			while(getline(f, line)){
			//read from file 
				string temp;
				vector<string> raw;
				istringstream temp_line(line);
				while(getline(temp_line, temp, ','))
					raw.push_back(temp);
			
				if(raw.size() != 6){
					cerr << filename << " format error" << endl;
					return -1;
				}
				data.push_back(raw);
			}
			//convert to struct
			s_size = data.size();
			steps = new Step[s_size];
			int i;
			for(i=0;i<s_size;i++){
				steps[i].index = atoi(data[i][0].c_str());
				steps[i].setMaterial(data[i][1]);
				steps[i].tool = data[i][2];
				steps[i].action = data[i][3];
				steps[i].times = atoi(data[i][4].c_str());
				string str = data[i][5];
				str[str.length()-1] = '\0';
				steps[i].product = data[i][5];//str;
				storage.insert(pair<string, int>(steps[i].product, 0));
			}
			return s_size;
		}
		/**
		* Get one order from the order list and remove it from the list
		**/
		string getOrder(){
			if(orders.empty()){
				cout << "end" << endl;
				return string("end");
			}
			string order = orders.back();
			orders.pop_back();
			return order;
		}
		/**
		* Find the Step that will produce specific 'product'
		**/
		const Step* find(string product){
			int i;
			for(i=0;i<s_size;i++){
				if(steps[i].product == product)
					return &steps[i];
			}
			cerr << "Can't find step for " << product << endl;
			return NULL;
		}
		/**
		* Making the product in this function. 
		* Check if there are enough storage to make, then remove those from the storage
		* if not, return the lacking material
		* then check if the tool is avaliable. 
		*At last, cook it and wait for it. 
		*Add the product to the storage and return its name
		**/ 
		string make(const Step *s){
			//check material storage
			map<string,int> check;
			if(s->material[0] != "none") {
				for(int i = 0 ; i < s->material.size();i++){
					check[s->material[i]]++;
				}
				for(int i = 0 ; i < s->material.size();i++) {
					string product;
					if(storage[s->material[i]+'\r'] < check[s->material[i]] ) {
						product = make(find(s->material[i]+'\r'));
						//storage[product]++;
					}
				}
				
				for(int i = 0 ; i < s->material.size();i++) {
					cout << "use " << s->material[i] << endl;
					storage[s->material[i]+'\r']--;
					getStorage();
					unsigned short led = LED_ALL_OFF ;
					ioctl(fds,LED_IOCTL_SET,&led);
					led = 1;
					ioctl(fds,LED_IOCTL_BIT_SET,&led);
				}
			
			//take tool
				if(s->tool != "none"){
					cout << "take " << s->tool << endl;
					
					unsigned short led = LED_ALL_OFF ;
					ioctl(fds,LED_IOCTL_SET,&led);
					led = 2;
					ioctl(fds,LED_IOCTL_BIT_SET,&led);
					sleep(1);
				}
				string str;
				str = s->product;
				str.erase(str.find('\r'),1);
				_7seg_info_t seg;

				ioctl(fds, _7SEG_IOCTL_ON,NULL);
				seg.Mode = _7SEG_MODE_PATTERN;
				seg.Which = _7SEG_ALL;
				seg.Value = 0x00000000;
				ioctl(fds, _7SEG_IOCTL_SET,&seg);
    			unsigned long show[5] = {0x3f,0x06,0x5b,0x4f,0x66};

				for(unsigned int i = 0 ; i < s->times;i++){
					cout << str << "-" << s->action <<","<<i<<endl;
					seg.Which = 8;
					seg.Value = show[i];
					ioctl(fds,_7SEG_IOCTL_SET,&seg);
					sleep(1);
				}
				seg.Which = _7SEG_ALL;
				seg.Value = 0x00000000;
				ioctl(fds, _7SEG_IOCTL_SET,&seg);
				cout << "get " << s->product << endl;
				unsigned short led = LED_ALL_OFF ;
				ioctl(fds,LED_IOCTL_SET,&led);
				led = 0;
				ioctl(fds,LED_IOCTL_BIT_SET,&led);
				sleep(1);
				storage[s->product]++;
				getStorage();

				if(s->tool != "none"){
					cout << "return " << s->tool << endl;
					unsigned short led = LED_ALL_OFF ;
					ioctl(fds,LED_IOCTL_SET,&led);
					led = 3;
					ioctl(fds,LED_IOCTL_BIT_SET,&led);
					sleep(1);
				}
				return  s->product;
			}
			else{
				string str = s->product;
				cout << "get " << s->product << endl;
				storage[s->product]++;
				getStorage();
				unsigned short led = LED_ALL_OFF ;
				ioctl(fds,LED_IOCTL_SET,&led);
				led = 0;
				ioctl(fds,LED_IOCTL_BIT_SET,&led);
				sleep(1);
				return s->product;
			}
		}
		void getStorage(){
			string lcd_str = "",c="",result="";
			int i = 0;
			stringstream ss;
			
			lcd_write_info_t display;
			ioctl(fds,LCD_IOCTL_CLEAR,NULL);
			sleep(1);
			for( map<string,int>::iterator iter = storage.begin();iter != storage.end();iter++){
				//cout << iter->first << " ";//<< iter->second << endl;
				lcd_str = iter->first;
				lcd_str.erase(lcd_str.find('\r'),1);
				//printf("%s, %d\n",lcd_str.c_str(),iter->second);
				//result = lcd_str;
            	display.Count = sprintf((char*) display.Msg,"%s %d",lcd_str.c_str(),iter->second);
				ioctl(fds, LCD_IOCTL_WRITE,&display);

            	//display.Count = sprintf((char*) display.Msg,"%d\n",iter->second);
			}
            //ioctl(fds, LCD_IOCTL_WRITE,&display);
		}

		void openDev(){
			int fd;
			printf ( "Read driver...\n" ) ;
			if  (( fd= open("/dev/lcd", O_RDWR)) < 0) {
			printf("Open /dev/lcd faild. \n");
			exit(-1);
			}
			fds = fd;
	
		}

	private:
		Step *steps;
		map<string, int> storage;
		map<string, int> tools;
		int s_size;
		int fds;
		vector<string> orders;
		
};
