#include <string>

typedef struct 
{
    std::string msg;
    int liczba;
    /* data */
} Memfile;


typedef struct mesg_buffer 
{
    long mesg_type;
    char mesg_text[100];
    char * point; //wskaznik do pamieci dzielonej
} Message;