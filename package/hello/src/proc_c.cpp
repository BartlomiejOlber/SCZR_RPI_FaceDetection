#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/time.h>
#include <sys/shm.h>
#include <sys/resource.h>

#include "opencv2/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/opencv.hpp>
#include "cppcodec/base64_rfc4648.hpp"
#include <boost/asio.hpp>


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

uint64_t receive_faces(vector<Mat> &detected_faces, Message& message)
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
	}
	
	secs = message.mesg_text[97];
	usces = message.mesg_text[98];
	uint64_t ret = (message.mesg_text[97]&0xFFFF)*1000000 + message.mesg_text[98];
	msgsnd(send_queue_idx_b,&message , sizeof(message), 0);
	return ret;
}


time_t nowtime;
struct tm *nowtm;
char tmbuf[64], buf[85];

using namespace boost::asio;

class tcp_client {
public:

    tcp_client(const std::string &ip, int port, unsigned long buffer_size = 128) : m_remote_endpoint(
            ip::address::from_string(ip), port), m_socket(m_io_service), m_buffer_size(buffer_size) {
        boost::system::error_code error;
        m_socket.connect(m_remote_endpoint);
    }

    ~tcp_client() {
        m_socket.close();
    }

    void send_faces(const std::vector<cv::Mat> &faces,
                    const std::string &timestamp_taken,
                    const std::string &timestamp_processed) {
        using base64 = cppcodec::base64_rfc4648;

        std::ostringstream message;
        message << "{\"faces\": [";

        for (const cv::Mat &face: faces) {
            std::vector<uchar> buf;
            cv::imencode(".jpg", face, buf);
            auto *image_bytes = reinterpret_cast<unsigned char *>(buf.data());
            std::cout << "Encoding image..." << std::endl;
            std::string image_base64 = base64::encode(image_bytes, buf.size());
            message << R"({"image_src": ")"
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
                << timestamp_processed << "\"}\n";

        std::string message_str = message.str();
        for(unsigned i=0; i < message_str.length(); i += m_buffer_size) {
            m_socket.write_some(buffer(message_str.substr(i, m_buffer_size)));
        }
    }

private:
    io_service m_io_service;
    ip::tcp::socket m_socket;
    ip::tcp::endpoint m_remote_endpoint;
    unsigned long m_buffer_size;
};


int main(int argc, char *argv[])
{
	string server_address = "192.168.150.108";
	if(argc == 6){
		server_address = argv[4];
		setpriority(PRIO_PROCESS,0,-20);	
	}
	if(argc == 5){
		if(!strcmp(argv[4], "-p")){
			setpriority(PRIO_PROCESS,0,-20);	
		}else{
			server_address = argv[4];
		}
	}

	shmid_b = atoi(argv[1]);
	send_queue_idx_b= atoi(argv[3]);
	receive_queue_idx_b = atoi(argv[2]);
	
	struct timeval stop;
	
	vector<Mat> wektor;
	uint64_t starting_time;
	Message message;
	string timestamp_taken,timestamp_processed;

	time_t current_time;
	time(&current_time);
	struct tm *timeinfo = localtime(&current_time);
	int offset = timeinfo->tm_gmtoff;

	tcp_client client(server_address, 9000, 20480);
	frame = (u_char*) shmat(shmid_b,(void*)0,0);
	while (true)
	{
		wektor.clear();
		starting_time = receive_faces(wektor, message);

		gettimeofday(&stop,NULL);

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

		client.send_faces(wektor, timestamp_taken, timestamp_processed);
		
	}
    return 0;
}

