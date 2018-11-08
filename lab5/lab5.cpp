#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <unistd.h>
#include <algorithm>
#define NUM_THREADS 4

using namespace std;

pthread_mutex_t count_mutex     = PTHREAD_MUTEX_INITIALIZER;
vector<vector<int> > my_order;
int cus = 1;
static bool flag = true;

void * BusyWork(void * t ) {
    //cout << "HI" << endl;
    int time;
    int my_cus;
    vector<int> work;
    bool in = false;
    while(flag) {

        pthread_mutex_lock( &count_mutex );
        if(my_order.size()!=0) {
            my_cus = cus;
            cus++;
            work = my_order.front();
            my_order.erase(my_order.begin(),my_order.begin()+1);
            in = true;
        }
        pthread_mutex_unlock( &count_mutex );
        if(in) {
            reverse(work.begin(),work.end());
            for(int i = 0 ; i < 5; i++) {
                time += (work.back()*(i+1));
                work.pop_back();
            }
            sleep(time);
            cout << "Order [" << my_cus << "] : " << time << " sec" << endl;
            time = 0;
            in = false;
        }
    }
    pthread_exit(NULL);
}

void * Order(void * t) {
    //cout << "Order fucntion ID : " <<(long)t <<endl;
    string ord;
    stringstream ss;
    int a;
    while(getline(cin,ord)){
        vector<int> food;
        ss << ord;
        //for(int i = 0 ; i < 5; i++) {
        //   ss >> a;
        while(ss>>a) {
            if(a == 99){
                flag = false;
                cout << "Order end" << endl;
                pthread_exit(NULL);
            }
            food.push_back(a);
        }
        my_order.push_back(food);
        ss.flush();
        ss.clear();
    }    

    pthread_exit(NULL);
}

int main(int argc, char * argv[]) {

    pthread_t thread[NUM_THREADS];
    pthread_attr_t attr;

    int rc;
    long t;
    void * status;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE);

    cout << "Cook" << endl;
    for( t = 0 ; t < NUM_THREADS; t++) {
        cout << "Main: creating : " << t << endl;
        if ( t == 0 )
            rc = pthread_create(&thread[t],&attr, Order, (void *)t);
        else
            rc = pthread_create(&thread[t],&attr, BusyWork, (void *)t);
        cout << rc << endl;
        if (rc) {
            printf("ERROR; return code from pthread_create() is %d\n",rc);
            exit(-1);
        }
    }

    pthread_attr_destroy(&attr);
    for(t=0;t < NUM_THREADS; t++) {
        rc = pthread_join(thread[t],&status);
        if(rc) {
            printf("ERROR; return code from pthread_create() is %d\n",rc);
            exit(-1);
        }
    }
    pthread_exit(NULL);
    return 0;
}