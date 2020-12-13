
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
    char mesg_text[100];
} Message;

typedef struct mesg_buffer_c 
{
    int cols;
	int step;
	int rows;
    // long mesg_type;
    // char mesg_text[100];
} Message_c;

int main(int argc, char *argv[])
{
	for(int i = 0; i < argc; i++)
	{
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

    int shmid_a = shmget(key_shm_ab, 640*480*3 ,0666|IPC_CREAT), shmid_c = shmget(key_shm_bc,640*480*3,0666|IPC_CREAT);

	Mat *tmp_a = new Mat(480,640,16);
	Message message;
	int timer=0;
	CascadeClassifier face_cascade;
	bool candetect=true;
	if( !face_cascade.load( "haarcascades/haarcascade_frontalface_alt.xml" ) )
    {
        std::cout << "--(!)Error loading face cascade\n";
		candetect=false;
    };
	Message_c message_c;

	while (timer<100)
	{
		msgrcv(receive_queue_idx_a, &message, sizeof(message), 1, 0);
		u_char *frame_a = (u_char*) shmat(shmid_a,(void*)0,0);
		u_char *frame_c = (u_char*) shmat(shmid_c,(void*)0,0);
		tmp_a->data = frame_a;
		//odebralismy klatke, rozpoznajemy i wycinamy twarz
		if(candetect)
		{
			cout<<"WYSYLAMY DO C\n\n";
			Mat frame_gray;
			Mat crop;
    		Mat res;
			cvtColor( *tmp_a, frame_gray, COLOR_BGR2GRAY );
    		equalizeHist( frame_gray, frame_gray );
			vector<Rect> faces;
			face_cascade.detectMultiScale( frame_gray, faces );
			//jezeli nie wykrylo twarzy to nic nie rob z ta klatka
			
			if(faces.size!=0)
			{
				Rect face_frame;
				for ( size_t i = 0; i < faces.size(); i++ )
				{
					face_frame.x = faces[i].x;
					face_frame.y = faces[i].y;
					face_frame.width = (faces[i].width);
					face_frame.height = (faces[i].height);
					Mat *tmp_c = new Mat(faces[i].height,faces[i].width,16);
					crop = (*tmp_a)(face_frame);
					tmp_c->data = frame_c;
					resize(crop, res, Size(128, 128), 0, 0, INTER_LINEAR); // This will be needed later while saving images
					imwrite("plik od b.jpg",crop);
					memcpy((u_char*)(tmp_c->data), crop.data, crop.step*crop.rows);
					message_c.cols=crop.cols;
					message_c.step=crop.step;
					message_c.rows=crop.rows;
					msgsnd(send_queue_idx_c, &message_c, sizeof(message_c),0);
					// to tylko do wyswietlania zielonego prostokata wokol twarzy
					// printf("rozmiar = %ld\n",frame.type());
					// Mat *s = new Mat(480,640,frame.type());
					// Point pt1(faces[i].x-2, faces[i].y-2); // Display detected faces on main window
					// Point pt2((faces[i].x + faces[i].height+1), (faces[i].y + faces[i].width+1));
					// rectangle(frame, pt1, pt2, Scalar(0, 255, 0), 2, 8, 0); 
				}
			}

		}

		printf("proc_b received: %s\n", message.mesg_text);
		sprintf(message.mesg_text, "%d", 4);
		msgsnd(send_queue_idx_a,&message , sizeof(message), 0);
		timer++;
	}
	
    // for(int i = 0; i < 10; i++)
	// {
    // 	msgrcv(receive_queue_idx_a, &message, sizeof(message), 1, 0);

    // 	printf("procb received: %s\n", message.mesg_text);
    // 	sprintf(message.mesg_text, "%d", i+i);
    // 	msgsnd(send_queue_idx_a,&message , sizeof(message), 0);
    // }
	// cout<<"ZAPISUJE\n";
	// imwrite("OTRZYMANE.jpg",*str_ab);
    return 0;
}
