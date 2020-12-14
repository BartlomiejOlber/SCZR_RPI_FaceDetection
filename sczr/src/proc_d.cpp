///*
// * procd.cpp
// *
// *  Created on: Dec 11, 2020
// *      Author: bartlomiej
// */
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <errno.h>
//#include <unistd.h>
//#include <sys/ipc.h>
//#include <sys/msg.h>
//#include <sys/shm.h>
//#include <sys/mman.h>
//
//#include <string>
//#include <iostream>
//#include <signal.h>
//#include <wait.h>
//#include <vector>
//#include <string>
//
//using namespace std;
//
//vector<pid_t> children;
//pid_t pid;
//
//
//void sigchld_handler(int signum)
//{
//	cout<<"handler"<<endl;
////    pid_t pid;
////    while ((pid = waitpid(-1, NULL, WNOHANG)) != -1)
////    {
//////        unregister_child(pid);   // Or whatever you need to do with the PID
////    }
//}
//
//void sigint_handler(int signum)
//{
//
//}
//
//struct mesg_buffer {
//    long mesg_type;
//    char mesg_text[100];
//} message;
//
//int SHM_SIZE = 640*480*3;
//int main()
//{
//
//    int qid_ab, qid_ba, qid_bc;
//    key_t key_ab, key_ba, key_bc, shmkey_ab, shmkey_bc;
//    char qid_ab_s[16], qid_ba_s[16], qid_bc_s[16], shmid_ab_s[16], shmid_bc_s[16];
//    char proc_a_name[8] = "proc_a", proc_b_name[8] = "proc_b", proc_c_name[8] = "proc_c";
//    char msgq_ab_fname[16] = "msgqueue_ab", msgq_ba_fname[16] = "msgqueue_ba", msgq_bc_fname[16] = "msgqueue_bc";
//    char shm_ab_fname[16] = "shm_ab", shm_bc_fname[16] = "shm_bc";
//    FILE * fPtr_1 = fopen(msgq_ab_fname, "w");
//    FILE * fPtr_2 = fopen(msgq_ba_fname, "w");
//    FILE * fPtr_3 = fopen(msgq_bc_fname, "w");
//    FILE * fPtr_4 = fopen(shm_ab_fname, "w");
//    FILE * fPtr_5 = fopen(shm_bc_fname, "w");
//    key_ab = ftok(msgq_ab_fname, 'B');
//	key_ba = ftok(msgq_ba_fname, 'B');
//	key_bc = ftok(msgq_bc_fname, 'B');
//	shmkey_ab = ftok(shm_ab_fname, 'B');
//	shmkey_bc = ftok(shm_bc_fname, 'B');
//	fclose(fPtr_1);
//	fclose(fPtr_2);
//	fclose(fPtr_3);
//	fclose(fPtr_4);
//	fclose(fPtr_5);
//
//    int shmid_ab = shmget(shmkey_ab,SHM_SIZE,0666|IPC_CREAT);
//    int shmid_bc = shmget(shmkey_bc,SHM_SIZE,0666|IPC_CREAT);
//    if(shmid_ab < 0 || shmid_bc < 0){
//    	 printf("Oh dear, something went wrong with read()! %s\n", strerror(errno));
//    }
//    sprintf(shmid_ab_s, "%d", shmid_ab);
//    sprintf(shmid_bc_s, "%d", shmid_bc);
//
//
//	qid_ab = msgget(key_ab, 0666 | IPC_CREAT);
//	qid_ba = msgget(key_ba, 0666 | IPC_CREAT);
//	qid_bc = msgget(key_bc, 0666 | IPC_CREAT);
//    sprintf(qid_ab_s, "%d", (int)key_ab);
//    sprintf(qid_ba_s, "%d", (int)key_ba);
//    sprintf(qid_bc_s, "%d", (int)key_bc);
//
//	int status;
//    cout<<"SYSTEM START"<<endl;
////    cout<<qid_ab << "   "<<qid_ba<< endl;
//    signal(SIGCHLD, sigchld_handler);
////    signal(SIGINT, sigint_handler);
//    if ((pid = fork()) == 0){
////            char * args[]={proc_a_name, shm_ab_fname, msgq_ab_fname, msgq_ba_fname, NULL};
//            char * args[]={proc_a_name, shmid_ab_s, qid_ab_s, qid_ba_s, NULL};
//            execv(args[0],args);
//    }
//    if ((pid = fork()) == 0){
////            char *args[]={proc_b_name, shm_ab_fname, shm_bc_fname, msgq_ab_fname, msgq_ba_fname, msgq_bc_fname, NULL};
//            char *args[]={proc_b_name, shmid_ab_s, shmid_bc_s, qid_ab_s, qid_ba_s, qid_bc_s, NULL};
//            execv(args[0],args);
//    }
//
////    if ((pid = fork()) == 0){
////            char *args[]={proc_c_name, shmid_bc_s, qid_bc_s, NULL};
////            execv(args[0],args);
////    }
//    wait(NULL);
//    wait(NULL);
//    cout<<"procd koniec" <<endl;
//    return 0;
//}

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
#include <sys/types.h>

#include <string>
#include <iostream>
#include <signal.h>
#include <wait.h>
#include <vector>
#include <string>

using namespace std;

vector<pid_t> children;
vector<int> shm_ids;
vector<int> mq_ids;
int SHM_SIZE = 640*480*3;

void kill_children()
{
	int status;
	for(auto i = children.begin(); i!= children.end(); ++i){
		kill(*i, SIGTERM);
		sleep(1000);
		waitpid(*i, &status, WNOHANG);
		if(!WIFSIGNALED(status)){
			kill(*i, SIGKILL);
		}
	}
	children.clear();
}

void free_ipc_objs()
{
	for(auto i = shm_ids.begin(); i!= shm_ids.end(); ++i){
		shmctl(*i,IPC_RMID,NULL);
	}
	shm_ids.clear();
	for(auto i = mq_ids.begin(); i!= mq_ids.end(); ++i){
		msgctl(*i, IPC_RMID, NULL);
	}
	mq_ids.clear();
}

void sigchld_handler(int signum)
{
	cout<<"handler"<<endl;
//	kill_children();
//	free_ipc_objs();

}

void sigint_handler(int signum)
{
	kill_children();
	free_ipc_objs();
	exit(-1);
}

void create_ipc_files(const vector<string>& v)
{
	for(auto it = v.begin(); it != v.end(); ++it){
		FILE * fPtr = fopen(it->c_str(), "w");
		fclose(fPtr);
	}
}

void create_ipc_ftoks(const vector<string>& fnames, vector<key_t>& ftoks)
{
	for(auto it = fnames.begin(); it != fnames.end(); ++it){
		key_t key = ftok(it->c_str(), 'B');
		ftoks.push_back(key);
	}
}

void get_shared_memory_segments(const vector<key_t>& ftoks)
{
	int id;
	for(auto it = ftoks.begin(); it != ftoks.end(); ++it){
		id = shmget(*it,SHM_SIZE,0666|IPC_CREAT);
		shm_ids.push_back(id);
	}
}

void get_message_queues(const vector<key_t>& ftoks)
{
	int id;
	for(auto it = ftoks.begin(); it != ftoks.end(); ++it){
		id = msgget(*it, 0666 | IPC_CREAT);
		mq_ids.push_back(id);
	}
}

void ipc_ids_to_string(vector<string>& args)
{
	for(auto it = shm_ids.begin(); it != shm_ids.end(); ++it){
		args.push_back(to_string(*it));
	}
	for(auto it = mq_ids.begin(); it != mq_ids.end(); ++it){
		args.push_back(to_string(*it));
	}

}

void make_children_args(vector<vector<char*> >& args, const vector<string>& ipc_ids, const vector<string>& proc_names)
{
	vector<char*> v;
	v.push_back(const_cast<char*>(proc_names[0].c_str()));
	v.push_back(const_cast<char*>(ipc_ids[0].c_str()));
	v.push_back(const_cast<char*>(ipc_ids[2].c_str()));
	v.push_back(const_cast<char*>(ipc_ids[3].c_str()));
	v.push_back(NULL);
	args.push_back(v);
	v.clear();
	v.push_back(const_cast<char*>(proc_names[1].c_str()));
	v.push_back(const_cast<char*>(ipc_ids[0].c_str()));
	v.push_back(const_cast<char*>(ipc_ids[1].c_str()));
	v.push_back(const_cast<char*>(ipc_ids[2].c_str()));
	v.push_back(const_cast<char*>(ipc_ids[3].c_str()));
	v.push_back(const_cast<char*>(ipc_ids[4].c_str()));
	v.push_back(NULL);


	args.push_back(v);
	v.clear();
	v.push_back(const_cast<char*>(proc_names[2].c_str()));
	v.push_back(const_cast<char*>(ipc_ids[1].c_str()));
	v.push_back(const_cast<char*>(ipc_ids[4].c_str()));
	v.push_back(NULL);
	args.push_back(v);

}

void start_children(vector<vector<char*> >& args)
{
	for(auto it = args.begin(); it != args.end(); ++it){
		pid_t pid;
		if ((pid = fork()) == 0){
			char **command = it->data();
			execv(command[0], &command[0]);
		}
		children.push_back(pid);
	}
}


int main()
{
//	while(1){
		vector<string>mq_names = {"msgqueue_ab", "msgqueue_ba", "msgqueue_bc"};
		vector<string>proc_names = {"proc_a", "proc_b", "proc_c"};
		vector<string>shm_names = {"shm_ab", "shm_bc"};
		vector<string>ipc_ids;
		vector<key_t>shm_ftoks, mq_ftoks;
		vector<vector<char*> >children_args;
		create_ipc_files(mq_names);
		create_ipc_files(shm_names);
		create_ipc_ftoks(mq_names, mq_ftoks);
		create_ipc_ftoks(shm_names, shm_ftoks);
		get_shared_memory_segments(shm_ftoks);
		get_message_queues(mq_ftoks);
		ipc_ids_to_string(ipc_ids);
		signal(SIGCHLD, sigchld_handler);
//		signal(SIGINT, sigint_handler);
		signal(SIGTERM, sigint_handler);
		make_children_args(children_args, ipc_ids, proc_names);
		start_children(children_args);
		for(uint i =0; i < children.size(); ++i){
			wait(NULL);
		}

    cout<<"procd koniec" <<endl;
    return 0;
}

