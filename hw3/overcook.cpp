#include "header/cook.h"
#define NUM_THREADS	4
#define NUM_CHEF (NUM_THREADS-1)

using namespace std;

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
vector<string > my_order;
int cus = 1;
static bool flag = true;

void * workChef(void * t ) {

	cook * chef = (cook *)t;
	string order = "\0";
	string product = "\0";
	int my_cus = 0 ;
	bool startCook = false;
	while(flag) {
		pthread_mutex_lock( &mutex);
        if(my_order.size()!=0) {
			//cout << "size of order : " << my_order.size() << endl;
            my_cus = cus;
            cus++;
            order = my_order.front();
            my_order.erase(my_order.begin(),my_order.begin()+1);
            startCook = true;
			//cout << "size of order after : " << my_order.size() << endl;
        }
        pthread_mutex_unlock( &mutex );

		while(startCook){
			if(chef->find(order) == NULL) {
				startCook = false;
				break;
			}
			product = chef->make(chef->find(order));
			if(product == order) {
				cout << "Order[" << my_cus << "] : " << product << " Serve!" << endl; 
				startCook = false;
				break;
			}
		}
			//chef->getStorage();
	}
	
    pthread_exit(NULL);
}

void * takeOrder(void * t) {
	string order;
    while(getline(cin,order)){
		if(order == "timeout"){
			flag = false;
			pthread_exit(NULL);
		}
		my_order.push_back(order);
		order.clear();
    }
    pthread_exit(NULL);
}


int main(){

	//char order_file[] = "source/orders.txt";
	char steps_file[] = "source/steps.txt";
	char tools_file[] = "source/tools.txt";

	/*Pthread initialization*/
	pthread_t thread[NUM_THREADS];
	pthread_attr_t attr;
	int rc;
	long t;
	void * status;
	pthread_attr_init(&attr);

	/*Chef initialization*/
	//cook * chef = new cook[NUM_THREADS-1];
	cook * chef = new cook;
	/*Create pthread*/
    cout << "Waiting for open" << endl;
    for( t = 0 ; t < NUM_THREADS; t++) {
        cout << "Main: creating : " << t << endl;
        if ( t == 0 )
            rc = pthread_create(&thread[t],&attr, takeOrder, (void *)t);
        else {
			//chef[t-1].setSteps(steps_file);
			//chef[t-1].setTool(tools_file);
            //rc = pthread_create(&thread[t],&attr, workChef, (void *)&chef[t-1]);
			chef->setSteps(steps_file);
			chef->setTool(tools_file);
            rc = pthread_create(&thread[t],&attr, workChef, (void *)chef);
			}
		if (rc) {
            printf("ERROR; return code from pthread_create() is %d\n",rc);
            exit(-1);
        }
    }
	cout << "Open! please order what you want to eat!" << endl;
	/*Join*/
    pthread_attr_destroy(&attr);
    for(t=0;t < NUM_THREADS; t++) {
		if(!flag)
    		pthread_cancel(thread[t]);
        rc = pthread_join(thread[t],&status);
        if(rc) {
            printf("ERROR; return code from pthread_create() is %d\n",rc);
            exit(-1);
        }
    }
	cout << "end" << endl;

    pthread_exit(NULL);
	delete(chef);
	return 0;
}
