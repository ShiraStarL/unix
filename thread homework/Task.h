

typedef  void* (*f)(void*);

class Task{
    public:
    f taskFunc;
    void* arg;
    Task(f func,void* args){
        taskFunc=func;
        arg=args;
    }
};