#include <functional>
#include <list>
#include <mutex>
#include <thread>
#include <vector>
#include <iostream>
#include <condition_variable>
#include <unistd.h>
#include <atomic>



using namespace std;

/**
 * 
 * The worker class
 * */
class Workers{
    public:

    
    atomic<bool> running;
    list<function<void()>> tasks; //List where all the tasks will be
    mutex tasks_mutex;
    condition_variable cv; //Condition variable to make the threads wait if no tasks is available

    vector<thread> worker_threads; //Threads vector

    int n_threads;


    Workers(int n){
        n_threads = n;
    }

    /**
     * Function to add tasks and notify workers
     * */
    void post(function<void()> task){
        unique_lock<mutex> lock(tasks_mutex);
        tasks.emplace_back(task);
        cv.notify_all();
    }

    /**
     * Function to create the worker threads and execute the tasks if there are any
     * or wait until they get notified that a task is ready to be executed.
     * */
    void start(){
        running = true;
        for(int i = 0; i < n_threads; i ++){
            worker_threads.emplace_back([&] {
                while(running){
                    function<void()> task;
                    {
                        unique_lock<mutex> lock(tasks_mutex);
                        if(tasks.empty()){
                            cv.wait(lock);
                        }else{
                            task = *tasks.begin();
                            tasks.pop_front();
                        }
                    }
                    if(task){
                        task();
                    }
                }
            });
        }
    }


    /**
     * Stops the threads and joins them
    */
    void stop(){
        running = false;
        cv.notify_all();
        for(auto &thread : worker_threads){
            thread.join();
        }
    }

    /**
     * Function that posts tasks, but only executes after n seconds.
    */
    void post_timeout(function<void()> task, int s){
        post([task, s]{
            thread tmp([task, s]{
                sleep(s);
                task();
            });
            tmp.join();  
        });
    }
};
