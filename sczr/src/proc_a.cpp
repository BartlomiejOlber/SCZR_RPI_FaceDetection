#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <string>
#include <iostream>
#include "opencv2/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"


#include <time.h>       /* clock_t, clock, CLOCKS_PER_SEC */

using namespace cv;
using namespace std;

typedef struct mesg_buffer {
    long mesg_type;
    char mesg_text[100];
} Message;

int main(int argc, char *argv[])
{

	int shm_id = atoi(argv[1]);
	int send_queue_idx= atoi(argv[2]);
	int receive_queue_idx = atoi(argv[3]);

	Message message;
    char *str = (char*) shmat(shm_id,(void*)0,0);

    for(int i = 0; i < 10; i++){
    	message.mesg_type = 1;
    	sprintf(message.mesg_text, "%d", i*i);
    	cout<<"proca wysyla"<< message.mesg_text<< endl;
    	msgsnd(send_queue_idx, &message, sizeof(message), 0);
    	msgrcv(receive_queue_idx, &message, sizeof(message), 1, 0);
    	printf("proca received: %s\n", message.mesg_text );
    }

    return 0;
}


