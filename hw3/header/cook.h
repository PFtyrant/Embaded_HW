#pragma once
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <unistd.h>
#include <algorithm>
#include <pthread.h>

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
				//cout << "end" << endl;
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
				pthread_mutex_lock(&mutex);
				for(int i = 0 ; i < s->material.size();i++){
					check[s->material[i]]++;
				}
				pthread_mutex_unlock(&mutex);
				for(int i = 0 ; i < s->material.size();i++) {
					string product;
					if(storage[s->material[i]] < check[s->material[i]] ) {
						product = make(find(s->material[i]));
					}
					
					storage[s->material[i]]--;
				}
				pthread_mutex_lock(&mutex);
				for(int i = 0 ; i < s->material.size();i++) {
					cout << "use " << s->material[i] << endl;
				}
				pthread_mutex_unlock(&mutex);
				//take tool
				if(s->tool != "none"){
					pthread_mutex_lock(&mutex);
					while( tools[s->tool] == 0 );
					tools[s->tool]--;
					cout << "take " << s->tool << endl;
					pthread_mutex_unlock(&mutex);
					//sleep(1);
				}

				for(unsigned int i = 0 ; i < s->times;i++){
					
					pthread_mutex_lock(&mutex);
					cout << s->product << "-" << s->action <<","<<i<<endl;
					pthread_mutex_unlock(&mutex);

					sleep(1);
				}
				pthread_mutex_lock(&mutex);
				cout << "get " << s->product << endl;
				storage[s->product]++;
				if(s->tool != "none"){
					tools[s->tool]++;
					cout << "return " << s->tool << endl;
					//sleep(1);
				}
				pthread_mutex_unlock(&mutex);
				return  s->product;
			}
			else{
				pthread_mutex_lock(&mutex);
				string str = s->product;
				cout << "get " << s->product << endl;
				storage[s->product]++;
				pthread_mutex_unlock(&mutex);
				//sleep(1);
				return s->product;
			}
		}
		void getStorage(){
			string lcd_str = "",c="",result="";
			int i = 0;
			sleep(1);
			for( map<string,int>::iterator iter = storage.begin();iter != storage.end();iter++){
				cout << iter->first << " " << iter->second << endl;
			}
		}

		void setMutex(pthread_mutex_t & tmutex) {
			mutex = tmutex;
		}

		void printToolsleft() {
			cout << tools["knife"] << ", knife." << endl;
			cout << tools["pot"] << ", pot." << endl; 
		}


	private:
		Step *steps;
		map<string, int> storage;
		map<string, int> tools;
		int s_size;
		int fds;
		vector<string> orders;
		pthread_mutex_t mutex;
		
};
