
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/types.h>
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
		usleep(1000);
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
	kill_children();
	free_ipc_objs();

}

void sigchld_handler_ignore(int signum)
{

}

void sigint_handler(int signum)
{
	signal(SIGCHLD, sigchld_handler_ignore);
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
	vector<int> ab= {0,2,3};
	vector<int> bc= {1,4,5};
	for(auto it = proc_names.begin(); it!=proc_names.end(); ++it){
		v.push_back(const_cast<char*>(it->c_str()));
		if( *it == proc_names[0] || *it == proc_names[1]){
			for(auto i = ab.begin(); i!=ab.end(); ++i){
				v.push_back(const_cast<char*>(ipc_ids[*i].c_str()));
			}
		}
		if( *it == proc_names[1] || *it == proc_names[2]){
			for(auto i = bc.begin(); i!=bc.end(); ++i){
				v.push_back(const_cast<char*>(ipc_ids[*i].c_str()));
			}
		}
		v.push_back(NULL);
		args.push_back(v);
		v.clear();
	}

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

void wait_for_children()
{
	for(uint i =0; i < children.size(); ++i){
		wait(NULL);
	}
}

int main()
{
	int i = 0;
	while(i<5){
		cout<<"d1"<<endl;
		i++;
		vector<string>mq_names = {"msgqueue_ab", "msgqueue_ba", "msgqueue_bc", "msgqueue_cb"};
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
		cout<<"d2"<<endl;
		signal(SIGCHLD, sigchld_handler);
//		signal(SIGINT, sigint_handler);
//		signal(SIGTERM, sigint_handler);
		make_children_args(children_args, ipc_ids, proc_names);
		cout<<"d3"<<endl;
		start_children(children_args);
		cout<<"d4"<<endl;
		wait_for_children();
		cout<<"d5"<<endl;
	}
    return 0;
}

