#include <iostream>
#include <cstdlib>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <map>
#include <vector>
#include <sstream>
#include <fstream>
#include <algorithm>
using namespace std;
int pfd[2];
int pfd2[2];
int pid;
enum {READ = 0, WRITE = 1};
char **arg;

class judger{
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
	typedef struct _cooking{
		int sec;
		string product;
		_cooking(int s, string p){sec = s; product = p;}
	}cooking;
	typedef struct _orderwithTime{
		struct timespec time;
		string order;
		_orderwithTime(struct timespec t, string o){time = t; order = o;}
	}orderwithTime;
	typedef struct _orderwithDoubleTime{
		double time;
		string order;
		_orderwithDoubleTime(double t, string o){time = t; order = o;}
	}orderwithDoubleTime;
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
													int pos;
													while((pos = line.find("\r")) != string::npos) line.erase(pos,1);
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
                        for(i=0;i<data.size();i++){
                                tools.insert(pair<string, int>(data[i][0], atoi(data[i][1].c_str())));
				usingTool.insert(pair<string, int>(data[i][0], 0));
			}
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
                        while(getline(f, line)){
													int pos;
													while((pos = line.find("\r")) != string::npos) line.erase(pos,1);
                                orders.push_back(line);
												}
			reverse(orders.begin(), orders.end());
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
													int pos;
													while((pos = line.find("\r")) != string::npos) line.erase(pos,1);
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
                                steps[i].product = data[i][5];
                                storage.insert(pair<string, int>(steps[i].product, 0));
				usingMaterial.insert(pair<string, int>(steps[i].product, 0));
                        }
                        return s_size;
                }
		
                /**
                * Find the step that will produce specific 'product'
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
                * Get one order from the order list and remove it from the list
                **/
                string getOrder(){
                        if(orders.empty()){
                                //cout << "end" << endl;
                                return string();
                        }
                        string order = orders.back();
                        orders.pop_back();
												struct timespec tmp;
												clock_gettime(CLOCK_MONOTONIC, &tmp);
												announced_order.push_back(orderwithTime(tmp,order));
                        return order;
                }

		void display(){
//			cerr << "Displaying" << endl;
			cerr << "\033[H\033[J";

			int i;
			cerr << "---Score---" << endl;
			cerr << score << endl;
			cerr << "\n\n---Orders---" << endl;
			for(i=0;i<announced_order.size();i++)
				cerr << announced_order[i].order << endl;
			cerr << "\n\n---Finished Dishes---" << endl;
			for(i=0;i<scored_dish.size();i++)
				cerr << scored_dish[i].order << " time cost: " << scored_dish[i].time << endl;;///(double)(CLOCKS_PER_SEC) << endl;
			map<string, int>::iterator it;
			cerr << "\n\n---Storage---" << endl;
			for(it = storage.begin();it != storage.end();it++){
				if(it->second > 0)
					cerr << it->first << ": " << it->second << endl;
			}
			cerr << "\n\n---Tools---" << endl;
			for(it = tools.begin();it != tools.end();it++){
				if(it->second > 0)
					cerr << it->first << ": " << it->second << endl;
			}
			cerr << "\n\n---cooking thread---" << endl;
			for(i=0;i<cooking_thread.size();i++)
				cerr << cooking_thread[i].product << " " << cooking_thread[i].sec << " secs..." << endl;
		}

		string update(string output){

			cerr << "Receiving Output: " << output << endl;
			vector<string> status;
			string temp;
			istringstream temp_line(output);
			while(getline(temp_line,temp,' '))
				status.push_back(temp);
			if((status.size() > 2) || (status.size() < 1))
				return "Wrong output format{"+ output +"}\n";
			log.push_back(output);
			if(status[0] == "get"){
				return get(status[1]);
			}
			else if(status[0] == "use"){
				return use(status[1]);
			}
			else if(status[0] == "take"){
				return take(status[1]);
			}
			else if((status[0] == "return") || (status[0] == "release")){
				return release(status[1]);
			}
			else if (status.size() == 1)
				cerr << "Enter cook section" << endl;
				return cook(status[0]);
			return "Wrong output format{"+ output +"}\n";
		}	
		string cook(string thing){
			int sec = -1;
			string product = "";
			product = thing.substr(0,thing.find("-"));
			sec = atoi(thing.substr(thing.find(",")+1).c_str());
			cerr << product << " " << sec << endl;
			if((product == "" ) || (sec == -1) )
				return "Wrong output format{"+ thing +"}\n";

			const Step *s = find(product);

			if(sec == 0){
				//get tool
				map<string, int>::iterator it;
					if(s->tool != "none"){
					it = usingTool.find(s->tool);
					if(it->second <= 0)
						return "Do not have enough tool {"+ it->first +"} to cook "+ product +"\n";
					it->second --;
				}
				//get material
				int i;
				for(int i=0;i<s->material.size();i++){
					it = usingMaterial.find(s->material[i]);
					if(it->second <= 0)				
						return "Do not have enough material {"+ it->first +"} to cook "+ product +"\n";
					it->second--;
				}
				cooking_thread.push_back(cooking(sec, product));
			}else{
				vector<cooking>::iterator it;
				bool found = false;
				for(it = cooking_thread.begin();it!=cooking_thread.end();it++){
					if((it->product == product) && (it->sec == sec-1)){
						found = true;
						break;
					}
				}
				if(!found)
					return "Does not match the cooking seconds\n";
				cooking_thread.erase(it);
				if((int)s->times != (sec+1))
					//not done cooking
					cooking_thread.push_back(cooking(sec, product));
				else{
					//done cooking
					struct timespec tmp;
					clock_gettime(CLOCK_MONOTONIC, &tmp);
					finished_dish.push_back(orderwithTime(tmp,product));
		


				}
			}


			cerr << "Leaving cook section" << endl;	
			return string();

		}

		string get(string thing){
			struct timespec now;
			clock_gettime(CLOCK_MONOTONIC, &now);
			const Step* s = find(thing);
			map<string,int>::iterator it1;
			it1 = storage.find(thing);
			if(it1 == storage.end())
				return "Can not find Material {" + thing + "}\n";
			if(s->times != 0){
				bool found = false;
				vector<orderwithTime>::iterator it;
				for(it=finished_dish.begin();it!=finished_dish.end();it++){
					if(it->order == thing){
						finished_dish.erase(it);
						found = true;
						break;
					}
				}
				if(!found)
					return "{"+ thing +"} has not been cooked yet\n";
				//remove from announced order
				for(it = announced_order.begin();it!=announced_order.end();it++){
					if(it->order == thing){
						double td = now.tv_sec - it->time.tv_sec;
						td += (now.tv_nsec - it->time.tv_nsec)/ 1000000000.0;
						scored_dish.push_back(orderwithDoubleTime(td , thing));
						if(td <= 25){
							score += 50-td;
						}
						announced_order.erase(it);
						break;
					}
				}
			}

			
			it1->second++;
			return string();
		}
		string use(string thing){
			map<string,int>::iterator it;
			it = storage.find(thing);
			if(it == storage.end())
				return "Can not find Material {" + thing + "}\n";
			if(it->second <= 0)
				return "The storage does not have enough material " + thing + "\n";
			it->second--;
			//add to using material 
			it = usingMaterial.find(thing);
			it->second++;
			return string();
		}
		string take(string thing){
			map<string, int>::iterator it;
			it = tools.find(thing);	
			if(it == tools.end())
				return "Can not find Tool {" + thing + "}\n";
			if(it->second <= 0)
				return "Tool {" + thing + "} is not valid to be taken\n";
			it->second--;
	
			//add to using tools 
			it = usingTool.find(thing);
			it->second++;
			 
			return string();
		}
		string release(string thing){
			map<string,int>::iterator it;
			it = tools.find(thing);
			if(it == tools.end())
				return "Can not find Tool {" + thing + "}\n";
			
			it->second++;
			return string();

		}
		void logToFile(string msg){
			fstream f;
			f.open("output.log", ios::out);
			int i;
			for(i=0;i<log.size();i++){
				f << log[i] << endl;
			}
			f << "Error: " << msg << endl;
			f.close();
		}

	private:
		Step *steps;
		map<string, int> storage;
		map<string, int> tools;
		map<string, int> usingMaterial;
		map<string, int> usingTool;
		vector<cooking> cooking_thread;
		vector<orderwithTime> finished_dish;
		vector<string> orders;
		vector<orderwithTime> announced_order;
		vector<orderwithDoubleTime> scored_dish;
		int s_size;
		vector<string> log;
		int score = 0;
};
