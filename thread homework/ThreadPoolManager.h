#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include "Task.h"
#include <queue>
#include <iostream>


using namespace std;

class ThreadPoolManager{
    public:
    int ThreadPoolInit(int n){
        sizeOfThreads = n;
        threads = new pthread_t[n];
        threadStatus = new int[n];
        for(int i; i<n; i++){
            threadStatus[i] = 0;
        }
        if(!threads)
            return -1;
        else return 0;
    }
    int ThreadPoolInsertTask(Task& task){
        int Qsize = taskList.size();
        taskList.push(task);
        if(taskList.size() > Qsize){
            cout<< "inserted task seccsefuly"<<endl;
            return 0;
        }
        else return -1;
    }
    void ThreadPoolDestroy(){
        if(threads)
			delete[] threads;
    }
    
    int getFreeT(){
        for(int i=0; i<sizeOfThreads;i++){
            if(threadStatus[i]==0){
                return i;
            }
        }
        return -1;
    }

    pthread_t& operator[](int index){
		return threads[index];
	}
	void joinAll(){
		for(int i = 0;  i < sizeOfThreads ; ++i){
			if(threadStatus[i]){
				pthread_join(threads[i],NULL);
			}
		}
	}

    int sizeOfThreads=0;
    int* threadStatus; // 0 = available, -1 = busy
    pthread_t* threads;
    pthread_t master;
    queue<Task> taskList;
};

