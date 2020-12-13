
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
	for(int i = 0; i < argc; i++){
		printf("procb arg%d: %s\n", i, argv[i]);
	}

	char* shm_ab_name = argv[1];
	char* shm_bc_name = argv[2];
	char* send_queue_name_a = argv[4];
	char* send_queue_name_c = argv[5];
	char* receive_queue_name_a = argv[3];
//	int send_queue_idx_a = msgget((key_t)send_queue_key_a, 0666 );
//	int send_queue_idx_c = msgget((key_t)send_queue_key_a, 0666 );
//	int receive_queue_idx_a = msgget((key_t)receive_queue_key_a, 0666);

	key_t key_shm_ab = ftok(shm_ab_name, 'B'), key_shm_bc = ftok(shm_bc_name, 'B');
	key_t key_send_a = ftok(send_queue_name_a, 'B'), key_send_c = ftok(send_queue_name_c, 'B');
	key_t key_receive_a = ftok(receive_queue_name_a, 'B');
	//!!!!!!!!!!!!!11
	int send_queue_idx_a = msgget(key_send_a, 0666 | IPC_CREAT), send_queue_idx_c = msgget(key_send_c, 0666 | IPC_CREAT);
	int receive_queue_idx_a = msgget(key_receive_a, 0666 | IPC_CREAT);
//!!!!!!!!!!!!!!!!!!!!

    int shmid_a = shmget(key_shm_ab,1024,0666|IPC_CREAT), shmid_c = shmget(key_shm_bc,1024,0666|IPC_CREAT);





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
