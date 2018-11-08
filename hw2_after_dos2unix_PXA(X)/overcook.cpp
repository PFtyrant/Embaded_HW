#include "header\cook.h"

using namespace std;

int main(){

	char order_file[] = "source\orders.txt";
	char steps_file[] = "source\steps.txt";
	char tools_file[] = "source\tools.txt";
	cook me;
	me.setOrder(order_file);
	me.setSteps(steps_file);
	me.setTool(tools_file);
	string order = "\0";
	string product = "\0";
	order = me.getOrder();
	//find steps
	while (me.find(order) != NULL){
		product = me.make(me.find(order));
		if(product == order) {
			order = me.getOrder();
			cout << endl;
			if(order == "end")
				break;
		}
	}
	me.getStorage();
	cout << endl;
return 0;
}
