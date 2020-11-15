#include <stdio.h>
#include <string>
#include <iostream>
#include "opencv2/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"


#include <time.h>       /* clock_t, clock, CLOCKS_PER_SEC */

using namespace cv;
using namespace std;

int main()
{
    setUseOptimized(true);
    clock_t t_access, t_proc, t_save, t_total;

    // Access time.
    t_access = clock();
    Mat img0 = imread("image.jpg", IMREAD_COLOR);// takes ~90ms
    t_access = clock() - t_access;

    // Processing time
    t_proc = clock();
    cvtColor(img0, img0, COLOR_BGR2GRAY);
    blur(img0, img0, Size(9,9));// takes ~18ms
    t_proc = clock() - t_proc;

    // Saving time
    t_save = clock();
    imwrite("img1.jpg", img0);
    t_save = clock() - t_save;

    t_total = t_access + t_proc + t_save;

    //printf("CLOCKS_PER_SEC = %d\n\n", CLOCKS_PER_SEC);

    printf("(TEST 0) Total execution time\t %d cycles \t= %f ms!\n", t_total,((float)t_total)*1000./CLOCKS_PER_SEC);
    printf("---->> Accessing  in\t %d cycles \t= %f ms.\n", t_access,((float)t_access)*1000./CLOCKS_PER_SEC);
    printf("---->> Processing in\t %d cycles \t= %f ms.\n", t_proc,((float)t_proc)*1000./CLOCKS_PER_SEC);
    printf("---->> Saving     in\t %d cycles \t= %f ms.\n", t_save,((float)t_save)*1000./CLOCKS_PER_SEC);

    VideoCapture camera(0, CAP_V4L2);
    if (!camera.isOpened()) {
	cout << "ERROR: Could not open camera" << endl;
	return 1;
    }

    for(int i = 0; i<10; ++i){
	Mat frame;
	camera >> frame;
	string fname = "capturedimg";
	fname += to_string(i);
	fname += ".jpg";
	imwrite(fname, frame);
    }
    return 0;
}

