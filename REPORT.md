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
3. Testing
4. Sources

## Summary
`libuthread` is a basic user-level thread library for Linux. It provides an interface for applications to create and run independent threads concurrently. Like most lightweight user-level thread libraries, this library can create new threads, schedule the execution of threads using a round-robin algorithm, be preemptive, and provide a thread synchronization API, semaphores.

## Implementation
The implementation of `libuthread` is made up of several major phases - queue, uthread, semaphore, and preemption.

#### Queue API
We implemented a FIFO queue using a linked list data structure. Just like any other queue, it has the typical `create`, `destroy`, `enqueue`, `delete` and `dequeue` functions. However, what stands out in our queue API is the `queue_iterate` function.

The `queue_iterate` function is a generic way to call a custom function on each item that is enqueued in the queue. How it works is that it takes a queue and a queue callback function type `queue_func_t` as the parameters. 

`queue_func_t func` is the function to be defined by the user. It takes a queue and a void pointer to a data item as parameters. 

Within the `queue_iterate` function, it first checks to see if `queue` and `func` are `NULL`, if so, it returns -1. We then a run a for-loop to go through each currently enqueued item in the queue. The current item is received as the data parameter in the queue callback function type so the program knows which data item to run the function defined by the user on. After iterating through each item in the queue `queue_iterate` returns 0 to indicate that it has ran successfully. 

#### Uthread API
The standard interface for threads on a Unix-style operating system is known as the `POSIX thread` or `pthread`. In this project, we only implement the following functions: `uthread_yield`, `uthread_exit`, `uthread_create`, `uthread_run`, `uthread_block`, and `uthread_unblock`.

It's important to note that we have the following global variables: `struct uthread **current` and `queue_t *ready_q`. This is so that all of the functions in `uthread.c` have access to the thread control blocks and the queue. 

The function to first be called by the application is `uthread_run` which creates the first user thread. We inititalize the `ready_q` using the `queue_create` function and we also initialize the `current` tcb by `malloc`. We then call the `uthread_create` function to create and begin the queue and the first thread. 

As the name of the `uthread_create` function suggests, this creates a new thread. This is done by malloc-ing a `uthread_tcb` called newTCB and its context. The function then calls `uthread_ctx_init` which initializes the context for a newly created thread. Once context has been created, that TCB is then queued to our `ready_q`.

Going back to the `uthread_run` function, after `uthread_create` is called, we enter a while-loop that calls `uthread_yield` as long as the the `ready_q` is not empty. 

`uthread_yield` allows for other threads in the queue to execute, not just the currently active and running thread. In this function, we define `struct uthread tcb *uthread_prev` to which we set it to the current tcb. This is to save the context of the current thread that is running. We then define `struct uthread_tcb *uthread_next`, set this pointer to the next item in the queue, and set it to the current tcb to update it. Once this is complete, the function calls `uthread_ctx_switch`, which basically switches the contexts of the previous thread and the next thread. The application will be able to resume and execute the next thread. 

Both the functions `uthread_block` and `uthread_unblock` comes more into play when we head over to the implementation of semaphores. How these two functions work will be explained in the next section.

With that, we are left with the final function in our queue API, which is `uthread_exit`. This function destroys the current tcb stack and it frees the context of the current thread. If there happens to be more threads in the queue, the current simply gets updated to the next thread in the queue. 

#### Semaphore API