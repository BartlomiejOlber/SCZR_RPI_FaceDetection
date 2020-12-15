
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
#include <opencv2/objdetect.hpp>


#include <time.h>       /* clock_t, clock, CLOCKS_PER_SEC */

using namespace cv;
using namespace std;

typedef struct mesg_buffer 
{
    long mesg_type;
    int mesg_text[100];
} Message;

int min(int a, int b)
{
	if(a<b)
	return a;

	else
	return b;
	
}

int main(int argc, char *argv[])
{

	int shmid_a = atoi(argv[1]);
	int shmid_c = atoi(argv[4]);
	int send_queue_idx_a= atoi(argv[3]);
	int receive_queue_idx_a = atoi(argv[2]);
	int send_queue_idx_c= atoi(argv[5]);
	int receive_queue_idx_c = atoi(argv[6]);

	Mat *tmp_a = new Mat(480,640,16);
	Message message;
	Message msg_to_send;
	CascadeClassifier face_cascade;
	bool candetect=true;
	if( !face_cascade.load( "/root/haarcascades/haarcascade_frontalface_alt.xml" ) )
    {
        std::cout << "--(!)Error loading face cascade\n";
		candetect=false;
    }

	u_char *frame_a = (u_char*) shmat(shmid_a,(void*)0,0);
	u_char *frame_c = (u_char*) shmat(shmid_c,(void*)0,0);
	while (true)
	{
		msgrcv(receive_queue_idx_a, &message, sizeof(message), 1, 0);
		tmp_a->data = frame_a;
		
		if(candetect)
		{
			Mat frame_gray;
			Mat crop;
    		Mat res;
			cvtColor( *tmp_a, frame_gray, COLOR_BGR2GRAY );
    		equalizeHist( frame_gray, frame_gray );
			vector<Rect> faces;
			face_cascade.detectMultiScale( frame_gray, faces );
			
			if(faces.size()!=0)
			{
				Rect face_frame;
				int offset=0;
				//mesg_type = zawsze 1
				msg_to_send.mesg_type = 1;
				//liczba twarzy
				msg_to_send.mesg_text[99]=faces.size();
				//przekazanie timestampa
				//sekundy
				msg_to_send.mesg_text[97]=message.mesg_text[0];
				//mikrosekundy
				msg_to_send.mesg_text[98]=message.mesg_text[1];


				//cout<<"B - wykryto "<<faces.size()<<" twarzy\n";

				for ( size_t i = 0; i < min(faces.size(),30); i++ )
				{
					face_frame.x = faces[i].x;
					face_frame.y = faces[i].y;
					face_frame.width = (faces[i].width);
					face_frame.height = (faces[i].height);
					Mat *tmp_c = new Mat(faces[i].height,faces[i].width,16);
					crop = (*tmp_a)(face_frame);
					tmp_c->data = frame_c;
					resize(crop, res, Size(128, 128), 0, 0, INTER_LINEAR); // This will be needed later while saving images
					memcpy((u_char*)(tmp_c->data+offset), crop.data, crop.step*crop.rows);
					msg_to_send.mesg_text[3*i+0]=crop.rows;
					msg_to_send.mesg_text[3*i+1]=crop.step;
					msg_to_send.mesg_text[3*i+2]=offset;
					offset+=crop.step*crop.rows;
					//imwrite("plik_od_b.jpg",crop);
				}
				msgsnd(send_queue_idx_c, &msg_to_send, sizeof(msg_to_send),0);
				msgrcv(receive_queue_idx_c, &message, sizeof(message), 1, 0);
			}

		}

		msgsnd(send_queue_idx_a,&message , sizeof(message), 0);
		
	}

    return 0;
}
