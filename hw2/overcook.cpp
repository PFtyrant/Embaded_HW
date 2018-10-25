#include "cook.h"

using namespace std;

int main(){

	char order_file[] = "orders.txt";
	char steps_file[] = "steps.txt";
	char tools_file[] = "tools.txt";
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
			me.getStorage();
			order = me.getOrder();
			cout << endl;
		}
	}
return 0;
}
