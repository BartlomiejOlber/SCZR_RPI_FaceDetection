#include <stdio.h>
#include <string>
#include <iostream>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include "opencv2/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"


#include <time.h>       /* clock_t, clock, CLOCKS_PER_SEC */

using namespace cv;
using namespace std;

int main(int argc, char *argv[])
{
	for(int i = 0; i < argc; i++){
		printf("procb arg%d: %s\n", i, argv[i]);
	}

	int shm_ab_idx = atoi(argv[1]), shm_bc_idx = atoi(argv[2]);
	int send_queue_key_a = atoi(argv[4]), send_queue_key_c = atoi(argv[5]);
	int receive_queue_key_a = atoi(argv[3]);
	int send_queue_idx_a = msgget((key_t)send_queue_key_a, 0666 );
	int send_queue_idx_c = msgget((key_t)send_queue_key_a, 0666 );
	int receive_queue_idx_a = msgget((key_t)receive_queue_key_a, 0666);



    char *str_ab = (char*) shmat(shm_ab_idx,(void*)0,0);
    char *str_bc = (char*) shmat(shm_bc_idx,(void*)0,0);
    int message;

    for(int i = 0; i < 10; i++){
    	cout<<"procb czeka"<< receive_queue_idx_a<<endl;
    	msgrcv(receive_queue_idx_a, &message, sizeof(message), 1, 0);
    	string reply = "received: ";
    	reply += to_string(message);
    	msgsnd(send_queue_idx_a, reply.c_str(), sizeof(reply.c_str()), 0);

    	printf("procb received: %d\n", message);
    }
    return 0;
}
