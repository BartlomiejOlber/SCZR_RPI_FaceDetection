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
	}
	
	//cout<<"ZAPISALO: "<<detected_faces.size()<<" twarzy\n";
	secs = message.mesg_text[97];
	usces = message.mesg_text[98];
	return (message.mesg_text[97]&0xFFFF)*1000000 + message.mesg_text[98];
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

	tcp_client client("192.168.150.108", 9000);
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

		if(wektor.size())
			client.send_faces(wektor, timestamp_taken, timestamp_processed);
		msgsnd(send_queue_idx_b,&message , sizeof(message), 0);
	}
    return 0;
}

