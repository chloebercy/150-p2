#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include <unistd.h>
#include <stdio.h> // delete
#include <string.h>

#include "private.h"
#include "uthread.h"

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 100

struct sigaction preempt_sa, previous_sa;
struct itimerval previous_timer;
sigset_t block_alarm;

void timer_handler(int signum)
{
	int check = write(1, "ALARM\n", 6);
	if (signum != SIGVTALRM || check == -1){
		return;
	}
	uthread_yield();
}

void preempt_disable(void)
{
	sigprocmask (SIG_BLOCK, &block_alarm, NULL);
	printf("preempt_disable\n");
}

void preempt_enable(void)
{
	printf("preempt_enable\n");
	sigprocmask (SIG_UNBLOCK, &block_alarm, NULL);
}

void preempt_start(bool preempt)
{
	printf("preempt gonna start!\n");
	memset (&preempt_sa, 0, sizeof(preempt_sa));
	//sigemptyset(&preempt_sa.sa_mask);
    //preempt_sa.sa_flags = 0;
    preempt_sa.sa_handler = timer_handler;
	sigaction(SIGVTALRM, &preempt_sa, &previous_sa);

	sigemptyset (&block_alarm);
  	sigaddset (&block_alarm, SIGVTALRM);

	struct itimerval timer;
  	timer.it_interval.tv_usec = 100; //1000000 / HZ;
  	timer.it_interval.tv_sec = 0;
  	timer.it_value.tv_usec = 100; //1000000 / HZ;
  	timer.it_value.tv_sec = 0;

	if (preempt){
		int check = setitimer(ITIMER_VIRTUAL, &timer, &previous_timer);
		fprintf(stderr, "started timer? %d\n", check);
	} else {
		getitimer(ITIMER_VIRTUAL, &previous_timer);
	}		
}

void preempt_stop(void)
{
	setitimer(ITIMER_VIRTUAL, &previous_timer, NULL);
	sigaction(SIGVTALRM, &previous_sa, NULL);
}

