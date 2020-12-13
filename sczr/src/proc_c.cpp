
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
#include "opencv2/highgui/highgui.hpp"


#include <time.h>       /* clock_t, clock, CLOCKS_PER_SEC */

using namespace cv;
using namespace std;

typedef struct mesg_buffer 
{
    int cols;
    int step;
    int rows;
    // long mesg_type;
    // char mesg_text[100];
} Message;

int main(int argc, char *argv[])
{
	for(int i = 0; i < argc; i++)
	{
		printf("proc_c arg%d: %s\n", i, argv[i]);
	}

	char* shm_bc_name = argv[1];
	char* receive_queue_name_b = argv[2];

	key_t key_shm_bc = ftok(shm_bc_name, 'B');
	key_t key_receive_b = ftok(receive_queue_name_b, 'B');
	//!!!!!!!!!!!!!11
	int receive_queue_idx_b = msgget(key_receive_b, 0666 | IPC_CREAT);
    //!!!!!!!!!!!!!!!!!!!!

    int shmid_b = shmget(key_shm_bc, 640*480*3 ,0666|IPC_CREAT);

	u_char *frame = (u_char*) shmat(shmid_b,(void*)0,0);

	Message message;
	int timer=0;
    cout<<"OK JESTESMY W C";
	while (timer<100)
	{
        msgrcv(receive_queue_idx_b, &message, sizeof(message), 1, 0);
        cout<<"ODEBRALISMY W C\n\n";
        Mat tmp=Mat(message.rows,message.cols,16);
        memcpy((u_char*)(tmp.data),frame,message.step*message.rows);
        imwrite("ZAPISANA TWARZ.jpg",tmp);
		 timer++;
	}
	
    return 0;
}
