#include <condition_variable>
//written by Alessandro Settimi
#ifndef NAMED_SEMAPHORE_HPP
#define NAMED_SEMAPHORE_HPP
//written by Alessandro Settimi

class named_semaphore
{
    //The current semaphore count.
    unsigned int count_;

    //mutex_ protects count_.
    //Any code that reads or writes the count_ data must hold a lock on
    //the mutex.
    std::mutex mutex_;

    //Code that increments count_ must notify the condition variable.
    std::condition_variable condition_;

public:
    explicit named_semaphore(unsigned int initial_count) 
       : count_(initial_count),
         mutex_(), 
         condition_()
    {
    }

    unsigned int get_count() //for debugging/testing only
    {
        //The "lock" object locks the mutex when it's constructed,
        //and unlocks it when it's destroyed.
        std::unique_lock<std::mutex> lock(mutex_);
        return count_;
    }

    void post() //called "release" in Java
    {
        std::unique_lock<std::mutex> lock(mutex_); //        boost::mutex::scoped_lock lock(mutex_);


        ++count_;

        //Wake up any waiting threads. 
        //Always do this, even if count_ wasn't 0 on entry. 
        //Otherwise, we might not wake up enough waiting threads if we 
        //get a number of signal() calls in a row.
        condition_.notify_one(); 
    }

    void wait() 
    {
        std::unique_lock<std::mutex> lock(mutex_);
        while (count_ == 0)
        {
             condition_.wait(lock);
        }
        --count_;
    }

       bool try_wait() 
    {
   std::unique_lock<std::mutex> lock(mutex_);
        if(count_)
        {
            --count_;
            return true;
        }
        else
        {
            return false;
        }
    }
    
};

//written by Alessandro Settimi
#endif //NAMED_SEMAPHORE_HPP
//written by Alessandro Settimi