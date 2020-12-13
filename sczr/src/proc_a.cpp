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

//	for(int i = 0; i < argc; i++){
//		printf("proca arg%d: %s\n",i , argv[i]);
//	}
	char* shm_name = argv[1];
	char* send_queue_name = argv[2];
	char* receive_queue_name = argv[3];
	key_t key_shm = ftok(shm_name, 'B');
	key_t key_send = ftok(send_queue_name, 'B');
	key_t key_receive = ftok(receive_queue_name, 'B');

	int send_queue_idx = msgget(key_send, 0666 | IPC_CREAT);
	int receive_queue_idx = msgget(key_receive, 0666 | IPC_CREAT);


    int shmid = shmget(key_shm,1024,0666|IPC_CREAT);
    Message message;
    char *str = (char*) shmat(shmid,(void*)0,0);
    for(int i = 0; i < 10; i++){
    	message.mesg_type = 1;
    	sprintf(message.mesg_text, "%d", i*i);
    	cout<<"proca wysyla"<< message.mesg_text<< endl;
    	msgsnd(send_queue_idx, &message, sizeof(message), 0);
    	msgrcv(receive_queue_idx, &message, sizeof(message), 1, 0);
    	printf("proca received: %s\n", message.mesg_text );
    }




//
//    cout<<"Write Data : ";
//    gets(str);
//
//    printf("Data written in memory: %s\n",str);
//
//    char *str = (char*) shmat(shmid,(void*)0,0);
//
//    printf("Data read from memory: %s\n",str);
//


    return 0;
}
/*
 * proc_a.cpp
 *
 *  Created on: Dec 12, 2020
 *      Author: bartlomiej
 */


