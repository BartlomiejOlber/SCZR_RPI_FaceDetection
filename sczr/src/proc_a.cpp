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

typedef struct mesg_buffer 
{
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


	size_t size = 640*480*3;
    int shmid = shmget(key_shm, 640*480*3, 0666|IPC_CREAT);
	
	if(shmid<0)
	{
		printf("%s\n",strerror(errno));
		return -1;
	}

    Message message;
	cout<<shmid<<endl;
    u_char *frame = (u_char*) shmat(shmid,(void*)0,0);

	//char *frame = (char*) shmat(shmid,(void*)0,0);
	// cout<<"ZAPISANIE LOREM\n";
	// strcpy(str,"LOREM IPSUM");
	// cout<<"ZAPISANO\n";

	VideoCapture capture;
	capture.open(0);
	if ( ! capture.isOpened() )
    {
        cout << "--(!)Error opening video capture\n";
    }
	else
	{
		cout<<"OTWARTO\n";
	}
	

	
	// printf("TO SIE ZNAJDUJE W FRAME = %s\n", frame);

	// for(int row = 0; row<curr_frame.rows; row++)
	// {
	// 	cout<<row<<" przejscie\n";
	// 	const uchar* ptr = (const uchar*) (curr_frame.data + row * curr_frame.step);
	// 	cout<<"chce skopiowac\n"<<curr_frame.step<<endl;
	// 	memcpy((uchar*)(tmp->data + row * tmp->step), ptr, curr_frame.step);
	// }
	Mat curr_frame;
	// capture.read(curr_frame);
	// imwrite("takie zapisane od PRZED.jpg",curr_frame);

	Mat *tmp = new Mat(480,640,curr_frame.type());
	int timer= 0;
	while ( capture.read(curr_frame)  && timer <100)
    {
        if( (curr_frame).empty() )
        {
            std::cout << "--(!) No captured frame -- Break!\n";
            break;
        }
		else
		{
			tmp->data = frame;
			memcpy((u_char*)(tmp->data), curr_frame.data, curr_frame.step*curr_frame.rows);	
			message.mesg_type = 1;
			sprintf(message.mesg_text, "%s", "wyslano klatke");
			cout<<"proc_a wysyla: "<< message.mesg_text<< endl;
			msgsnd(send_queue_idx, &message, sizeof(message), 0);
    		msgrcv(receive_queue_idx, &message, sizeof(message), 1, 0);
			if( cv::waitKey(10) == 27 )
			{
				break; // escape
				return 0;
			}
		}
		timer++;
		
        //-- 3. Apply the classifier to the frame
        // detectFaces( frame, face_cascade);
        // if( cv::waitKey(10) == 27 )
        // {
        //     break; // escape
        // }
    }
	

    // for(int i = 0; i < 10; i++)
	// {
    // 	message.mesg_type = 1;
    // 	sprintf(message.mesg_text, "%d", i*i);
    // 	cout<<"proca wysyla: "<< message.mesg_text<< endl;
    // 	msgsnd(send_queue_idx, &message, sizeof(message), 0);
    // 	msgrcv(receive_queue_idx, &message, sizeof(message), 1, 0);
    // 	printf("proca received: %s\n", message.mesg_text );
    // }




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


