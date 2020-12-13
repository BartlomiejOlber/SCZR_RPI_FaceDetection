#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <string>
#include <iostream>
#include "opencv2/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"


#include <time.h>       /* clock_t, clock, CLOCKS_PER_SEC */

using namespace cv;
using namespace std;

int main(int argc, char *argv[])
{
//    setUseOptimized(true);
//    clock_t t_access, t_proc, t_save, t_total;
//
//    // Access time.
//    t_access = clock();
//    Mat img0 = imread("image.jpg", IMREAD_COLOR);// takes ~90ms
//    t_access = clock() - t_access;
//    cout << "SIZE" << endl;
//    cout << img0.size << endl;
//
//    // Processing time
//    t_proc = clock();
//    cvtColor(img0, img0, COLOR_BGR2GRAY);
//    blur(img0, img0, Size(9,9));// takes ~18ms
//    t_proc = clock() - t_proc;
//
//    // Saving time
//    t_save = clock();
//    imwrite("img1.jpg", img0);
//    t_save = clock() - t_save;
//
//    t_total = t_access + t_proc + t_save;
//
//    //printf("CLOCKS_PER_SEC = %d\n\n", CLOCKS_PER_SEC);
//
//    printf("(TEST 0) Total execution time\t %d cycles \t= %f ms!\n", t_total,((float)t_total)*1000./CLOCKS_PER_SEC);
//    printf("---->> Accessing  in\t %d cycles \t= %f ms.\n", t_access,((float)t_access)*1000./CLOCKS_PER_SEC);
//    printf("---->> Processing in\t %d cycles \t= %f ms.\n", t_proc,((float)t_proc)*1000./CLOCKS_PER_SEC);
//    printf("---->> Saving     in\t %d cycles \t= %f ms.\n", t_save,((float)t_save)*1000./CLOCKS_PER_SEC);
//    for(int i = 0; i<10; ++i){
//		string fname = "chuuujapturedimg";
//		fname+= to_string(i);
//		fname+= ".jpg";
//		// show the image on the window
//		cout << fname << endl;
//
//		imwrite(fname, img0);
//    }
//
//	VideoCapture camera(0, CAP_V4L2);
//	if (!camera.isOpened()) {
//		cout << "ERROR: Could not open camera" << endl;
//		return 1;
//	}
//	cout << "OPENED" << endl;
//
//    for(int i = 0; i<10; ++i){
//		// open the first webcam plugged in the computer
//
//		// this will contain the image from the webcam
//		Mat frame;
//
//		// capture the next frame from the webcam
//		camera >> frame;
//		cout << "CAPTURED" << endl;
//		cout << frame.size << endl;
//
//
//		string fname = "capturedimg";
//		fname+= i;
//		fname+= ".jpg";
//		// show the image on the window
//		imwrite(fname, frame);
//
//		// wait (10ms) for a key to be pressed
//    }

	for(int i = 0; i < argc; i++){
		printf("proca arg%d: %s\n",i , argv[i]);
	}
	int shm_idx = atoi(argv[1]);
//    char *str = (char*) shmat(shmid,(void*)0,0);
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
//    //detach from shared memory
//    shmdt(str);
//
//    // destroy the shared memory
//    shmctl(shmid,IPC_RMID,NULL);


    return 0;
}
