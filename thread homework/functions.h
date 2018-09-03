#include <iostream>
#include <sys/socket.h>
#include <string.h>


extern pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
extern pthread_cond_t  cond  =	PTHREAD_COND_INITIALIZER;
extern pthread_mutex_t poolLock = PTHREAD_MUTEX_INITIALIZER;
extern pthread_mutex_t dataLock = PTHREAD_MUTEX_INITIALIZER;

typedef struct _digit{
    int data;
    bool ft=false;
}digit;

using namespace std;

    void* executeTasks(void* _pool){

        ThreadPoolManager* pool = (ThreadPoolManager*)_pool;

        int availableT=-1;
        int maxTaskSize = pool->taskList.size();

        while(pool->taskList.size()!=0){
            pthread_mutex_lock(&mutex);

            while(pool->taskList.size() == 0){
			    pthread_cond_wait(&cond,&poolLock);
		    }

            while(availableT == -1){
                availableT = pool->getFreeT();
            }
            pool->threadStatus[availableT]=-1;
            if (pthread_create(&pool->threads[availableT], NULL, pool->taskList.front().taskFunc,pool->taskList.front().arg)){
                 printf("error\n");
                return (void*)1;
            }
            pool->taskList.pop();
            availableT = -1;
            pthread_mutex_unlock(&mutex);

        }
        pool->joinAll();
        return (void*)0;
    }

    void* playGame(void* args){
        int guessingNum,nrecv;
        int* newfd = (int*)args;
        int n = newfd[0];
        int theNumber = (rand() % 100)+1000;
        cout<< theNumber<<endl;
        digit numArray1[4];
        digit numArray2[4];
        
        for(int i=0; i<4; i++){
            int digit = theNumber%10;
            theNumber /= 10;
            numArray2[i].data = digit;
        }
       
        char buffer[256];
        while(1){
            int bulls=0, cows=0;
            memset(buffer,0,sizeof(buffer));
            if ((nrecv = recv(n, &guessingNum, sizeof(guessingNum), 0)) < 0)
            {
                    perror("recv");
                    return (void*)1;
            }
            cout<< guessingNum<< endl;
            for(int i=0; i<4; i++){
                int digit = guessingNum%10;
                guessingNum /= 10;
                numArray1[i].data = digit;
            }
            // server checks bulls
            for(int i=0; i<4; i++){
                    if(numArray1[i].data==numArray2[i].data){
                        numArray1[i].ft = true;
                        numArray2[i].ft = true;
                        bulls++;
                    }
            }
            // server checks cows
            for(int i=0; i<4; i++){
                for(int j=0; j<4; j++){
                    if(i==j)
                        continue;
                    if((numArray1[i].data == numArray2[j].data) && (numArray1[i].ft ==false) && (numArray2[j].ft == false)){
                        cows++;
                        numArray1[i].ft = true;
                        numArray2[i].ft = true;
                    }
                }
            }

            cout<< "bulls: "<<bulls<<" cows: "<<cows<<endl;
            if(bulls==4){
                strcpy(buffer,"you win!");
                if (send(n, buffer, sizeof(buffer), 0) < 0){
			        perror("send");
			        return (void*)1;
		        }
                return (void*)0;
            }
            else{
                string s1 = to_string(bulls);
                string s2 = to_string(cows);
                char const *c1 = s1.c_str();
                char const *c2 = s2.c_str();

                strcpy(buffer,"bulls: ");
                strcat(buffer,c1);
                strcat(buffer, " cows: ");
                strcat(buffer,c2);
                if (send(n, buffer, sizeof(buffer), 0) < 0){
			        perror("send");
			        return (void*)1;
		        }
            }
        }
    }

