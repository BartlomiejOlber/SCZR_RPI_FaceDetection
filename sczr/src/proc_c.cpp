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

int shmid_b;
int send_queue_idx_b;
int receive_queue_idx_b;

typedef struct mesg_buffer 
{
    long mesg_type;
    int mesg_text[100];
} Message;

Message message;
int receive_faces(vector<Mat> &detected_faces)
	{
		Mat crop;
		Rect face_frame;
		//vector<Mat> detected_faces;
		msgrcv(receive_queue_idx_b, &message, sizeof(message), 1, 0);
		u_char *frame = (u_char*) shmat(shmid_b,(void*)0,0);
		cout<<"\tC - odebrano: "<<message.mesg_text[99]<<" twarzy\n";
		
		stringstream ss;
		for(int i=0; i<message.mesg_text[99]; i++)
		{
			ss.str(string());
			face_frame.x = 0;
			face_frame.y = 0;
			face_frame.width = message.mesg_text[3*i];
			face_frame.height = message.mesg_text[3*i];
			//stworz nowy Mat z wymiarami takimi jak odebrana klatka
			Mat *tmp_c = new Mat(message.mesg_text[3*i], message.mesg_text[3*i], 16);
			//podlacz dane Mata do pamieci wspoldzielonej
			tmp_c->data = (frame+message.mesg_text[3*i+2]);
			//stworz na podstawie
			crop = (*tmp_c)(face_frame);
			//zmieniamy step w crop zeby dalo sie zapisac do zdjecia
			crop.step = message.mesg_text[3*i+1];
			detected_faces.push_back(crop);
		}
		msgsnd(send_queue_idx_b,&message , sizeof(message), 0);

		return time(NULL);
	}



int main(int argc, char *argv[])
{

	shmid_b = atoi(argv[1]);
	send_queue_idx_b= atoi(argv[3]);
	receive_queue_idx_b = atoi(argv[2]);


	vector<Mat> wektor;
	int curr_time;
	while (true)
	{
		wektor.clear();
        curr_time = receive_faces(wektor);
		
		//tutaj bedize komunikacja z serwerem
	}
    return 0;
}
