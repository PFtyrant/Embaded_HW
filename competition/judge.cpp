#include "judge.h"
#include <pthread.h>

judger jg;
string ret;
void* Receive(void* data){
	string output;
	ret = "";
	while(!cin.eof()){
		getline(cin, output);
		if(output == "end")
			break;
		if(output == "")
			continue;
		ret = jg.update(output);
		if(ret != ""){
			cerr << ret << "testing";
			cout << "timeout" << endl;
			break;
		}
		jg.display();
	}
	jg.logToFile(ret);
	pthread_exit(NULL);
}

void* Send(void*data){
	string order;
	while((order = jg.getOrder()) != ""){
		cout << order << endl;
		sleep(3);
	}
	cout << "timeout" << endl;
	pthread_exit(NULL);
}

int main(int args, char**argv){
	if(args < 2){
		cerr << "Usage: ./judge [overcook exe] [args]" << endl;
		return 1;
	}
	//get executable path
	arg = new char*[args];
	int i=0;
	for(i=1;i<args;i++){
		arg[i-1] = new char [strlen(argv[i])];
		strcpy(arg[i-1], argv[i]);
	}
	arg[args-1] = new char[1];
	strcpy(arg[args-1], "\0");

	cout << "Test file: " << arg[0] << endl;
	cout << "Arg: " << arg[1] << endl;
	//read tools from file
	char tools_file[] = "tools.txt";
	jg.setTool(tools_file);
	//read steps from file
	char steps_file[] = "steps.txt";
	jg.setSteps(steps_file);
	//read orders from file
	char orders_file[] = "orders.txt";
	jg.setOrder(orders_file);
	//create pipe and process
	if(pipe(pfd) < 0){
		cerr << "Cannot create pipe" << endl;
		return 1;
	}
	if(pipe(pfd2) < 0){
		cerr << "Cannot create pipe" << endl;
		return 1;
	}
	
	if((pid = fork()) < 0){
		cerr << "Cannot create fork" << endl;
		return 1;
	}
	if(pid == 0){
		//child process
		close(pfd[READ]);
		dup2(pfd[WRITE], STDOUT_FILENO);
		close(pfd[WRITE]);
		close(pfd2[WRITE]);
		dup2(pfd2[READ], STDIN_FILENO);
		close(pfd2[READ]);
		execvp(arg[0], arg);
		cerr << "Fail to run overcook" << endl;

	}else{
		//parent process
		close(pfd[WRITE]);
		dup2(pfd[READ], STDIN_FILENO);
		close(pfd[READ]);
		close(pfd2[READ]);
		dup2(pfd2[WRITE], STDOUT_FILENO);
		close(pfd2[WRITE]);
		cerr << "start game" << endl;
		pthread_t tr, ts;
		pthread_create(&tr, NULL, Receive, NULL);
		pthread_create(&ts, NULL, Send, NULL);
		pthread_join(ts, NULL);
		pthread_join(tr, NULL);
		
		int status;
		waitpid(pid, &status, 0);
		cerr << "end game" << endl;




	}
}
