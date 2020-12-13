/*
 * procd.cpp
 *
 *  Created on: Dec 11, 2020
 *      Author: bartlomiej
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/mman.h>

#include <string>
#include <iostream>
#include <signal.h>
#include <wait.h>
#include <vector>
#include <string>

using namespace std;

pid_t pid;
int counter = 0;
void handler1(int sig)
{
    counter++;
    printf("counter = %d\n", counter);
    /* Flushes the printed string to stdout */
    fflush(stdout);
    kill(pid, SIGUSR1);
}

void sigchld_handler(int signum)
{
    pid_t pid;
    while ((pid = waitpid(-1, NULL, WNOHANG)) != -1)
    {
//        unregister_child(pid);   // Or whatever you need to do with the PID
    }
}

void sigint_handler(int signum)
{

}

struct mesg_buffer {
    long mesg_type;
    char mesg_text[100];
} message;

int SHM_SIZE = 1024;
int main()
{

//    void *ptr_ab = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,0,0);
//    void *ptr_bc = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,0,0);
    int qid_ab, qid_ba, qid_bc;
    key_t key_ab, key_ba, key_bc, shmkey_ab, shmkey_bc;
    char qid_ab_s[16], qid_ba_s[16], qid_bc_s[16], shmid_ab_s[16], shmid_bc_s[16];
    char proc_a_name[8] = "proc_a", proc_b_name[8] = "proc_b", proc_c_name[8] = "proc_c";
    char msgq_ab_fname[16] = "msgqueue_ab", msgq_ba_fname[16] = "msgqueue_ba", msgq_bc_fname[16] = "msgqueue_bc";
    char shm_ab_fname[16] = "shm_ab", shm_bc_fname[16] = "shm_bc";
    FILE * fPtr_1 = fopen(msgq_ab_fname, "w");
    FILE * fPtr_2 = fopen(msgq_ba_fname, "w");
    FILE * fPtr_3 = fopen(msgq_bc_fname, "w");
    FILE * fPtr_4 = fopen(shm_ab_fname, "w");
    FILE * fPtr_5 = fopen(shm_bc_fname, "w");
    key_ab = ftok(msgq_ab_fname, 0);
	key_ba = ftok(msgq_ba_fname, 0);
	key_bc = ftok(msgq_bc_fname, 0);
	shmkey_ab = ftok(shm_ab_fname, 0);
	shmkey_bc = ftok(shm_bc_fname, 0);
	fclose(fPtr_1);
	fclose(fPtr_2);
	fclose(fPtr_3);
	fclose(fPtr_4);
	fclose(fPtr_5);

    int shmid_ab = shmget(shmkey_ab,SHM_SIZE,0666|IPC_CREAT);
    int shmid_bc = shmget(shmkey_bc,SHM_SIZE,0666|IPC_CREAT);
    sprintf(shmid_ab_s, "%d", shmid_ab);
    sprintf(shmid_bc_s, "%d", shmid_bc);
    // shmat to attach to shared memory
//    char *str = (char*) shmat(shmid,(void*)0,0);

	// msgget creates a message queue
	// and returns identifier
	qid_ab = msgget(key_ab, 0666 | IPC_CREAT);
	qid_ba = msgget(key_ba, 0666 | IPC_CREAT);
	qid_bc = msgget(key_bc, 0666 | IPC_CREAT);
    sprintf(qid_ab_s, "%d", qid_ab);
    sprintf(qid_ba_s, "%d", qid_ba);
    sprintf(qid_bc_s, "%d", qid_bc);

	int status;
    cout<<"SYSTEM START"<<endl;
    cout<<key_ab << endl;
//    signal(SIGCHLD, sigchld_handler);
//    signal(SIGINT, sigint_handler);
    if ((pid = fork()) == 0){
            char * args[]={proc_a_name, shmid_ab_s, qid_ab_s, qid_ba_s, NULL};
            execv(args[0],args);
    }
    if ((pid = fork()) == 0){
            char *args[]={proc_b_name, shmid_ab_s, shmid_bc_s, qid_ab_s, qid_ba_s, qid_bc_s, NULL};
            execv(args[0],args);
    }
//    if ((pid = fork()) == 0){
//            char *args[]={proc_c_name, shmid_bc_s, qid_bc_s, NULL};
//            execv(args[0],args);
//    }

    return 0;
}

