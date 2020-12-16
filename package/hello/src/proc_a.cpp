#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/time.h>
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
    int mesg_text[100];
} Message;

int main(int argc, char *argv[])
{


	int shmid = atoi(argv[1]);
	int send_queue_idx= atoi(argv[2]);
	int receive_queue_idx = atoi(argv[3]);

	size_t size = 640*480*3;

	
	
	if(shmid<0)
	{
		printf("%s\n",strerror(errno));
		return -1;
	}

    Message message;
    u_char *frame = (u_char*) shmat(shmid,(void*)0,0);

	VideoCapture capture;
	capture.open(0);
	if ( ! capture.isOpened() )
    {
        cout << "--(!)Error opening video capture\n";
		return -1;
    }
	// else
	// {
	// 	cout<<"OTWARTO\n";
	// }

	Mat curr_frame;

	struct timeval start, stop;
	Mat tmp = Mat(480,640,curr_frame.type());
	while ( capture.read(curr_frame))
    {

        if( (curr_frame).empty() )
        {
            std::cout << "--(!) No captured frame -- Break!\n";
            break;
        }
		else
		{
			//pobraz timestamp
			tmp.data = frame;
			memcpy((u_char*)(tmp.data), curr_frame.data, curr_frame.step*curr_frame.rows);
			message.mesg_type = 1;
			//wrzuc timestampa
			gettimeofday(&start,NULL);
			//w wysylanej wiadomosci wyslij aktualny czas w mikrosekundach

			//informacja o sekundach
			message.mesg_text[0] = start.tv_sec;
			//informacja o mikrosekundach
			message.mesg_text[1] = start.tv_usec;

			msgsnd(send_queue_idx, &message, sizeof(message), 0);
    		msgrcv(receive_queue_idx, &message, sizeof(message), 1, 0);
		}

    }


    return 0;
}


