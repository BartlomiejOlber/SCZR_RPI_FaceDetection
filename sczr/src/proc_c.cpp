#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/time.h>
#include <sys/shm.h>

#include "opencv2/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"


#include <time.h>

#include <websocketpp/config/asio_no_tls_client.hpp>

#include <websocketpp/client.hpp>

typedef websocketpp::client<websocketpp::config::asio_client> client;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

// pull out the type of messages sent by our config
typedef websocketpp::config::asio_client::message_type::ptr message_ptr;


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


int secs,usces;
u_char *frame;

Message message;
uint64_t receive_faces(vector<Mat> &detected_faces)
{
	Mat crop;
	Rect face_frame;
	msgrcv(receive_queue_idx_b, &message, sizeof(message), 1, 0);

	for(int i=0; i<message.mesg_text[99]; i++)
	{
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
	//cout<<"ZAPISALO: "<<detected_faces.size()<<" twarzy\n";
	secs = message.mesg_text[97];
	usces = message.mesg_text[98];
	return (message.mesg_text[97]&0xFFFF)*1000000 + message.mesg_text[98];
}


time_t nowtime;
struct tm *nowtm;
char tmbuf[64], buf[85];

int main(int argc, char *argv[])
{

	shmid_b = atoi(argv[1]);
	send_queue_idx_b= atoi(argv[3]);
	receive_queue_idx_b = atoi(argv[2]);
	struct timeval stop;
	
	vector<Mat> wektor;
	uint64_t starting_time;

	string timestamp_taken,timestamp_processed;

	//tutaj naprawiamy timezone
	time_t current_time;
	time(&current_time);
	struct tm *timeinfo = localtime(&current_time);
	int offset = timeinfo->tm_gmtoff;

	frame = (u_char*) shmat(shmid_b,(void*)0,0);
	while (true)
	{
		wektor.clear();
		starting_time = receive_faces(wektor);

		//tutaj mierzymy czas gdy sie skonczylo przetwarzanie, zapisujemy do 'stop'
		gettimeofday(&stop,NULL);
		//stop_time teraz zamieniamy na same mikrosekundy
		//uint64_t stop_time = (stop.tv_sec-offset)*1000000 + stop.tv_usec;// & 0x7FFFFFFF;
		//to samo robimy z start_time
		// uint64_t start_time = secs-offset;
		// start_time*=1000000;
		// start_time+=usces;


		//tutaj wypisujemy o ktorej sie zaczelo przetwarzanie klatki i o ktorej
		//sie ono skonczylo, dodatkowo ile trwala calosc
		// cout<<"START:\t"<<start_time<<endl<<"STOP:\t"<<stop_time<<endl;
		// cout<<"Zajelo: "<<stop_time-start_time<<" us"<<endl;

		nowtime = (time_t)(secs-offset);
		nowtm = localtime(&nowtime);
		strftime(tmbuf, sizeof tmbuf, "%Y-%m-%d %H:%M:%S", nowtm);
		snprintf(buf, sizeof buf, "%s.%06ld", tmbuf, usces);
		timestamp_taken = string(buf);

		nowtime = stop.tv_sec;
		nowtm = localtime(&nowtime);
		strftime(tmbuf, sizeof tmbuf, "%Y-%m-%d %H:%M:%S", nowtm);
		snprintf(buf, sizeof buf, "%s.%06ld", tmbuf, stop.tv_usec);
		timestamp_processed = string(buf);
		// cout<<timestamp_taken<<endl;
		// cout<<timestamp_processed<<endl<<endl;



		//cout<<buf<<endl<<endl;
		
		//tutaj bedzie komunikacja z serwerem
		// buf zawiera informacje o czasie w dokładnie takim formacie jak trzeba
	}
    return 0;
}
