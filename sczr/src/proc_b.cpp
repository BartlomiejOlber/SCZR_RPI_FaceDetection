#include <stdio.h>
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
	for(int i = 0; i < argc; i++){
		printf("procb arg%d: %s\n", i, argv[i]);
	}
    return 0;
}
