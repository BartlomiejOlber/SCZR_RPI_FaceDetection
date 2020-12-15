<<<<<<< HEAD
#include <iostream>

using namespace std;

int main(int argc, char *argv[])
{

	int kl;
	while(1){
		kl = 0;
	}

	cout<<"proc c end"<<endl;

=======

#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>

#include "opencv2/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"


#include <time.h>

using namespace cv;
using namespace std;

typedef struct mesg_buffer 
{
    long mesg_type;
    int mesg_text[100];
} Message;


int main(int argc, char *argv[])
{

	int shmid_b = atoi(argv[1]);
	int send_queue_idx_b= atoi(argv[3]);
	int receive_queue_idx_b = atoi(argv[2]);

	
	u_char *frame = (u_char*) shmat(shmid_b,(void*)0,0);

	Message message;
	int timer=0;

	Mat crop;
	Rect face_frame;
	while (timer<100)
	{
		
        msgrcv(receive_queue_idx_b, &message, sizeof(message), 1, 0);
		u_char *frame = (u_char*) shmat(shmid_b,(void*)0,0);

		cout<<"takie wymiary otrzymalismy: "<<message.mesg_text[0]<<", "<<message.mesg_text[1]<<", "<<message.mesg_text[2]<<endl;
		face_frame.x = 0;
		face_frame.y = 0;
		face_frame.width = message.mesg_text[0];
		face_frame.height = message.mesg_text[0];
		
		Mat *tmp_c = new Mat(message.mesg_text[0], message.mesg_text[1], 16);
		//podlaczamy tmp_c do pamieci dizelonej
		tmp_c->data = frame+message.mesg_text[];

		crop = (*tmp_c)(face_frame);
		cout<<"sizeof frame (IN C): "<<crop.rows<<", "<<crop.cols<<", "<<crop.step<<endl;
		crop.step = message.mesg_text[2];
		imwrite("ZAPISANE OD C.jpg",crop);
		//cout<<"ZAPISANO\n";
		msgsnd(send_queue_idx_b,&message , sizeof(message), 0);
		timer++;
		//przeczyta mordy
		//zapisze do wektora
		//do kolejki do_b wysle ze odczytalo
		// wyslanie kolejki 
	}
    return 0;
}
