<h1 align = "center"> Operating Systems - Project 2 </h1>
<h2 align = "center"> ECS 150 - Winter Quarter 2023 </h2>
<h3 align = "center"> Chloe Bercy and Jen Galicia </h3>
<h4 align = "center"> February 21, 2023 </h4>

## Table of Contents
1. Summary
2. Implementation
    * Queue
    * Uthread
    * Semaphore
    * Preemption
3. Testing
4. Sources

## Summary
`libuthread` is a basic user-level thread library for Linux. It provides an interface for applications to create and run independent threads concurrently. Like most lightweight user-level thread libraries, this library can create new threads, schedule the execution of threads using a round-robin algorithm, be preemptive, and provide a thread synchronization API, semaphores.

## Implementation
The implementation of `libuthread` is made up of several major phases - queue, uthread, semaphore, and preemption.

#### Queue API
We implemented a FIFO queue using a linked list data structure. Just like any other queue, it has the typical `create`, `destroy`, `enqueue`, `delete` and `dequeue` functions. However, what stands out in our queue API is the `queue_iterate()` function.

The `queue_iterate()` function is a generic way to call a custom function on each item that is enqueued in the queue. How it works is that it takes a queue and a queue callback function type `queue_func_t` as the parameters. 

`queue_func_t func` is the function to be defined by the user. It takes a queue and a void pointer to a data item as parameters. 

Within the `queue_iterate()` function, it first checks to see if `queue` and `func` are `NULL`, if so, it returns -1. We then a run a for-loop to go through each currently enqueued item in the queue. The current item is received as the data parameter in the queue callback function type so the program knows which data item to run the function defined by the user on. After iterating through each item in the queue `queue_iterate()` returns 0 to indicate that it has ran successfully. 

#### Uthread API
The standard interface for threads on a Unix-style operating system is known as the `POSIX thread` or `pthread`. In this project, we only implement the following functions: `uthread_yield()`, `uthread_exit()`, `uthread_create()`, `uthread_run()`, `uthread_block()`, and `uthread_unblock()`.

It's important to note that we have the following global variables: `struct uthread **current` and `queue_t *ready_q`. This is so that all of the functions in `uthread.c` have access to the thread control blocks and the queue. 

The function to first be called by the application is `uthread_run()` which creates the first user thread. We inititalize the `ready_q` using the `queue_create()` function and we also initialize the `current` tcb by `malloc`. We then call the `uthread_create()` function to create and begin the queue and the first thread. 

As the name of the `uthread_create()` function suggests, this creates a new thread. This is done by malloc-ing a `uthread_tcb` called newTCB and its context. The function then calls `uthread_ctx_init()` which initializes the context for a newly created thread. Once context has been created, that TCB is then queued to our `ready_q`.

Going back to the `uthread_run()` function, after `uthread_create()` is called, we enter a while-loop that calls `uthread_yield()` as long as the the `ready_q` is not empty. 

`uthread_yield()` allows for other threads in the queue to execute, not just the currently active and running thread. In this function, we define `struct uthread tcb *uthread_prev` to which we set it to the current tcb. This is to save the context of the current thread that is running. We then define `struct uthread_tcb *uthread_next`, set this pointer to the next item in the queue, and set it to the current tcb to update it. Once this is complete, the function calls `uthread_ctx_switch()`, which basically switches the contexts of the previous thread and the next thread. The application will be able to resume and execute the next thread. 

Both the functions `uthread_block()` and `uthread_unblock()` comes more into play when we head over to the implementation of semaphores. How these two functions work will be explained in the next section.

With that, we are left with the final function in our queue API, which is `uthread_exit()`. This function destroys the current tcb stack and it frees the context of the current thread. If there happens to be more threads in the queue, the current simply gets updated to the next thread in the queue. 

#### Semaphore API + uthread_block() + uthread_unblock()
To control access to a common resource by multiple threads, we implemented the following functions in our semaphore API: `sem_create()`, `sem_destroy()`, `sem_down()`, `sem_up()`.

In the `sem_create()` function, it initializes a new semaphore. Along with that, it also creates a new queue to which we set it to the `queue_t blocked_q` data member as defined in the `struct semaphore`. The reasoning behind this is so that the application can differentiate the threads that are ready for execution and the threads that are blocked from executing. The count of the semaphore is set to the count passed through the `size_t count` parameter. 

The `sem_destroy()` destroys the semaphore, as the name suggests. It checks to see if the semaphore is `NULL` or if there are any threads in the `blocked_q`. If either of those are true, then the function returns `-1`. If not, then `queue_destroy()` gets called to destroy the queue of blocked threads and returns 0 to indicate that the semaphore has been successfully destroyed. 

`sem_down()` is the function that is called when a thread asks to grab a resource. It takes a semaphore as a parameter and checks to see if it is `NULL`. If the count of the semaphore is 0, it enters a while-loop in which the current threads are queued to the `blocked_q`. Afterward, `uthread_block()` is called. Otherwise, the count is decremented.

Similar to the `uthread_yield()` function, `uthread_block()` swaps the contexts of the current tcb with the next tcb. What sets these two functions apart, however, is that `uthread_yield()` queues the current thread back into the ready queue as aforementioned. To unblock the threads that are in the blocked queue--in other words, put the blocked threads back into the ready queue--we simply call `queue_enqueue()` and queue the current thread to the `ready_q`. All of this is defined in the `uthread_unblock()` function. 

To release a semaphore, `sem_up()` is called. Just like the `sem_down()` function, it begins with checking to see if the semaphore is `NULL`. If it isn't, then the count is incremented. If there happens to be any threads in the `blocked_q`, it initializes a pointer to indicate the next thread to be executed, calls the `queue_dequeue()` function to take the oldest item in the `blocked_q`, and calls `uthread_unblock()` to put the item that has been pulled back into the `ready_q`.

#### Preemption
The preemption API exists to prevent uncooperative threads from keeping the processing resource for themselves. We have implemented the following functions: `preempt_start()`, `preempt_stop()`, `preempt_enable`, and `preempt_disable()`. 

In `preempt_start()`, we simply configure the timer interrupt handler. It then takes `preempt`, the boolean value passed through the function's parameter and checks if it is true--if yes, the timer begins by calling the `setitimer()`. If not, then nothing happens. 

`preempt_stop()` stops the thread preemption. This function does this by passing `NULL` into the `setitimer()` and `sigaction()` timers. This restores the previous timer configuration and action associated with the virtual alarm signals.

`preempt_enable()` and `preempt_disable()` utilizes the `sigprocmask()` to block or unblock the signals, respectively. 

#### Testing
In order to ensure the functionality of the APIs, several test programs were used.

Other than the ones already provided, we have created two programs: `queue_tester.c` to test the Queue API and `test_preempt.c` to test the preemption API. 

In `queue_tester.c`, there are a few test cases to test each of the functions in the queue API. 

#### Sources
