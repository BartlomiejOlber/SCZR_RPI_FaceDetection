#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/shm.h>
#include <sys/resource.h>

// #include "cppcodec/base64_rfc4648.hpp"

// #include <websocketpp/config/asio_no_tls_client.hpp>
// #include <websocketpp/client.hpp>
// #include <websocketpp/common/thread.hpp>
// #include <websocketpp/common/memory.hpp>

#include <base64_rfc4648.hpp>
#include <boost/asio.hpp>



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


using namespace boost::asio;

void send_faces(ip::udp::socket &socket,
                ip::udp::endpoint &endpoint,
                const std::vector<cv::Mat> &faces,
                const std::string &timestamp_taken,
                const std::string &timestamp_processed) {
    using base64 = cppcodec::base64_rfc4648;

    std::ostringstream message;
    message << "{\"faces\": [";

    for (const cv::Mat &face: faces) {
        std::vector<uchar> buf;
        cv::imencode(".jpg", face, buf);
        auto *image_bytes = reinterpret_cast<unsigned char *>(buf.data());
        std::string image_base64 = base64::encode(image_bytes, buf.size());
        std::cout << "Encoded image: " << image_base64 << std::endl;
        message << R"({"image_src": "data:image/png;base64,)"
                << image_base64
                << "\"}";
        if (&faces.back() != &face) {
            message << ", ";
        }
    }
    message << R"(], "timestamp_taken": ")"
            << timestamp_taken
            << "\", "
            << R"("timestamp_processed": ")"
            << timestamp_processed << "\"}";


    boost::system::error_code err;

    socket.send_to(buffer(message.str(), 131072), endpoint, 0, err);
}



// int main() {

//     io_service io_service;
//     ip::udp::socket socket(io_service);
//     ip::udp::endpoint remote_endpoint;

//     socket.open(ip::udp::v4());

//     remote_endpoint = ip::udp::endpoint(ip::address::from_string("192.168.0.4"), 9000);

//     cv::Mat img0 = imread("../img0.jpg", cv::IMREAD_COLOR);
//     cv::cvtColor(img0, img0, cv::COLOR_BGR2GRAY);
//     std::vector<cv::Mat> faces;
//     faces.push_back(img0);
//     int i = 0;
//     while (++i) {
//         send_faces(socket, remote_endpoint, faces, std::to_string(i), "");
//         usleep(20000);
//         std::cout << i << std::endl;
//     }

//     socket.close();
// }


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
		Mat tmp_c = Mat(message.mesg_text[3*i], message.mesg_text[3*i], 16);
		//podlacz dane Mata do pamieci wspoldzielonej
		tmp_c.data = (frame+message.mesg_text[3*i+2]);
		//stworz na podstawie
		crop = tmp_c(face_frame);
		//zmieniamy step w crop zeby dalo sie zapisac do zdjecia
		crop.step = message.mesg_text[3*i+1];
		detected_faces.push_back(crop);
		//imwrite("OD C.jpg",crop);
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
	setpriority(PRIO_USER,0,-20);
	
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
		uint64_t stop_time = (stop.tv_sec-offset)*1000000 + stop.tv_usec;// & 0x7FFFFFFF;
		// to samo robimy z start_time
		uint64_t start_time = secs-offset;
		start_time*=1000000;
		start_time+=usces;


		// tutaj wypisujemy o ktorej sie zaczelo przetwarzanie klatki i o ktorej
		// sie ono skonczylo, dodatkowo ile trwala calosc
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
