
#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/mman.h>
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

	int shmid_a = atoi(argv[1]);
	int shmid_c = atoi(argv[2]);
	int send_queue_idx_a= atoi(argv[4]);
	int receive_queue_idx_a = atoi(argv[3]);



    char *str_ab = (char*) shmat(shmid_a,(void*)0,0);
    char *str_bc = (char*) shmat(shmid_c,(void*)0,0);
    Message message;
    for(int i = 0; i < 10; i++){
    	msgrcv(receive_queue_idx_a, &message, sizeof(message), 1, 0);

    	printf("procb received: %s\n", message.mesg_text);
    	sprintf(message.mesg_text, "%d", i+i);
    	msgsnd(send_queue_idx_a,&message , sizeof(message), 0);

    }
    return 0;
}
