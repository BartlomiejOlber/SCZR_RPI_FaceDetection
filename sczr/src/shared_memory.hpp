#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <iostream>
#include "memfile.hpp"
#include "opencv2/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#define IPC_RESULT_ERROR (-1)

static int get_shared_block(char *filename,int size)
{
    key_t key;

    key = ftok(filename,'B');
    if (key == IPC_RESULT_ERROR)
    {
        return IPC_RESULT_ERROR;
    }
    return shmget(key, size, 0666 | IPC_CREAT);

}

char * attach_memory_block(char *filename, int size)
{
    int shared_block_id = get_shared_block(filename,size);
    char *result;

    // if (get_shared_block == IPC_RESULT_ERROR)
    // {
    //     return NULL;
    // }

    result = (char* )shmat(shared_block_id, NULL, 0);
    if(result == (char*)IPC_RESULT_ERROR)
    {
        return NULL;
    }
    return result;
}

bool detach_memory_block(char *block)
{
    return (shmdt(block) != IPC_RESULT_ERROR);
}

bool destroy_memory_block(char *filename)
{
    int shared_block_id = get_shared_block(filename,0);

    if(shared_block_id == IPC_RESULT_ERROR)
    {
        return NULL;
    }
    return (shmctl(shared_block_id, IPC_RMID, NULL) != IPC_RESULT_ERROR);
}