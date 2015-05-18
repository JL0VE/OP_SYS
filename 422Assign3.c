/*
 // * ThreadingFun.c
 *
 *  Created on: May 15, 2015
 *  Author: Dennis Kenyon, Jordan Love, Sean Robbins, Luke Lamberson, Ridwan Abdilahi
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/queue.h>

#define NUMBER_OF_JOBS 10
#define CLOCK_CYCLE 2

typedef struct job {
	int job_id;
	int current_phase; // 0 = cpu, 1 = io, 2 = finished
	// Phase types: 1 = CPU phase; 2 = IO phase
	int cpuDuration;
	int ioDuration;
	int cpuCurrentTiming;
	int ioCurrentTiming;

	int is_completed;
} Job;

typedef struct queues {
	Job cpuQueue[NUMBER_OF_JOBS];
	Job ioQueue[NUMBER_OF_JOBS];
	Job finishedQueue[NUMBER_OF_JOBS];
	int cpuSize;
	int ioSize;
	int finishedSize;
} SchedulerQueues;

SchedulerQueues theQueues;
static int jobCounter = 0;
static int jobsFinished = 0;
pthread_mutex_t lockCPU;
pthread_mutex_t lockIO;
pthread_mutex_t lockFinal;
pthread_mutex_t lockCounter;
//time_t now;
//struct tm *tm;



//function declarations
void initScheduler();
Job dequeue(int whichQueue);
void enqueue(Job theJob, int whichQueue);
void initJob(Job *theJob);
void* cpu();
void* io();
void* job();

//main
int main(int argc, char** argv) {
	setvbuf(stdout, NULL, _IONBF, 0);
	srand(time(NULL));
	pthread_t cpuOne[8], ioOne[4], jobOne[4];
	int i, rc;


	initScheduler();

//	printf("job counter: %d\n", jobCounter);
	pthread_mutex_init(&lockCPU, NULL);
	pthread_mutex_init(&lockIO, NULL);
	pthread_mutex_init(&lockFinal, NULL);
	pthread_mutex_init(&lockCounter, NULL);

	for (i = 0; i < 4; i++) {
		printf("current job thread: %d\n", i);
		rc = pthread_create(&(jobOne[i]), NULL, job, NULL);
		printf("\n\nCPU Size: %d\n", theQueues.cpuSize);
		printf("\n\nIO Size: %d\n", theQueues.ioSize);
		printf("\n\nFinished Size: %d\n", theQueues.finishedSize);

	}
	for (i = 0; i < 8; i++) {
		printf("current cpu thread: %d\n", i);
		rc = pthread_create(&(cpuOne[i]), NULL, cpu, NULL);

	}
	for (i = 0; i < 4; i++) {
		printf("current io thread: %d\n", i);
		rc = pthread_create(&(ioOne[i]), NULL, io, NULL);
	}

	pthread_join(jobOne[0], NULL);
	pthread_join(jobOne[1], NULL);
	pthread_join(jobOne[2], NULL);
	pthread_join(jobOne[3], NULL);
	pthread_join(cpuOne[0], NULL);
	pthread_join(cpuOne[1], NULL);
	pthread_join(cpuOne[2], NULL);
	pthread_join(cpuOne[3], NULL);
	pthread_join(cpuOne[4], NULL);
	pthread_join(cpuOne[5], NULL);
	pthread_join(cpuOne[6], NULL);
	pthread_join(cpuOne[7], NULL);
	pthread_join(ioOne[0], NULL);
	pthread_join(ioOne[1], NULL);
	pthread_join(ioOne[2], NULL);
	pthread_join(ioOne[3], NULL);
	pthread_mutex_destroy(&lockCPU);
	pthread_mutex_destroy(&lockIO);
	pthread_mutex_destroy(&lockFinal);
	pthread_mutex_destroy(&lockCounter);
	return 1;
}

void initScheduler() {
	theQueues.cpuSize = 0;
	theQueues.ioSize = 0;
	theQueues.finishedSize = 0;
}

void initJob(Job *theJob) {
	pthread_mutex_trylock(&lockCounter);
	theJob->job_id = jobCounter++;
	theJob->current_phase = 0;
	theJob->cpuDuration = (rand() % 5) + 1;
	theJob->ioDuration = 5 + (rand() % 15);
	theJob->cpuCurrentTiming = 0;
	theJob->ioCurrentTiming = 0;
	theJob->is_completed = 0;
	pthread_mutex_unlock(&lockCounter);
//	sleep(2);
}

void enqueue(Job theJob, int whichQueue) {
	printf("queue: %d\n", whichQueue);
	switch (whichQueue) {
	case 0:
		pthread_mutex_trylock(&lockCPU);
		printf("cpu size before increment: %d\n", theQueues.cpuSize);
		theQueues.cpuQueue[theQueues.cpuSize] = theJob;
		theQueues.cpuSize++;
		printf("cpu size after increment: %d\n", theQueues.cpuSize);
		pthread_mutex_unlock(&lockCPU);
		break;
	case 1:
		pthread_mutex_trylock(&lockIO);
		theQueues.ioQueue[theQueues.ioSize] = theJob;
		printf("Should increment iosize from %d to %d\n", theQueues.ioSize,
				theQueues.ioSize + 1);
		theQueues.ioSize++;
		printf("iosize is now: %d\n", theQueues.ioSize);
		pthread_mutex_unlock(&lockIO);
		break;
	case 2:
		pthread_mutex_trylock(&lockFinal);
		theQueues.finishedQueue[theQueues.finishedSize] = theJob;
		theQueues.finishedSize++;
		pthread_mutex_unlock(&lockFinal);
		break;
	}
}

Job dequeue(int whichQueue) {
	Job returnJob;
	int i;
	switch (whichQueue) {
	case 0:
		pthread_mutex_trylock(&lockCPU);
		returnJob = theQueues.cpuQueue[0];
		for (i = 0; i < theQueues.cpuSize; i++) {
			theQueues.cpuQueue[i] = theQueues.cpuQueue[i + 1];
		}
		theQueues.cpuSize--;
		pthread_mutex_unlock(&lockCPU);
		break;
	case 1:
		pthread_mutex_trylock(&lockIO);
		returnJob = theQueues.ioQueue[0];
		for (i = 0; i < theQueues.ioSize; i++) {
			theQueues.ioQueue[i] = theQueues.ioQueue[i + 1];
		}
		theQueues.ioSize--;
		pthread_mutex_unlock(&lockIO);
		break;
	case 2:
		pthread_mutex_trylock(&lockFinal);
		returnJob = theQueues.finishedQueue[0];
		for (i = 0; i < theQueues.finishedSize; i++) {
			theQueues.finishedQueue[i] = theQueues.finishedQueue[i + 1];
		}
		theQueues.finishedSize--;
		pthread_mutex_unlock(&lockFinal);
		break;
	}
	return returnJob;
}

void* cpu() {
	Job job;
	int jobTime;
	while (jobsFinished < NUMBER_OF_JOBS) {
		if (theQueues.cpuSize == 0) {
			sleep(2);
		} else {
			printf("about to be dequeue'd from CPU: job %d\n", jobCounter);
			//	time(&rawTime);
			//	printf("system time: %s\n", ctime(&rawTime));
			job = dequeue(0);
			printf("CPU dequeue'd: job %d\n", jobCounter);
			//	time(&rawTime);
			//	printf("system time: %s\n", ctime(&rawTime));
			jobTime = job.cpuDuration;
			//	sleep(jobTime);
			printf("about to be enqueue'd to IO: job %d;\n", jobCounter);
			printf("iosize: %d\n", theQueues.ioSize);
			enqueue(job, 1);
			printf("iosize after enqueue: %d\n", theQueues.ioSize);
			printf("IO enqueue'd: job %d\n", jobCounter);
		}

	}
	return NULL;
}

void* io() {
	Job job;
	int jobTime;
	while (jobsFinished < NUMBER_OF_JOBS) {
		if (theQueues.ioSize == 0) {
			sleep(2);
		} else {
			printf("about to be dequeue'd from IO: job %d\n", jobCounter);
			job = dequeue(1);
			printf("dequeue'd from IO: job %d\n", jobCounter);
			jobTime = job.ioDuration;
			//	sleep(jobTime);
			printf("about to be enqueue'd to final: job %d\n", jobCounter);
			enqueue(job, 2);
			printf("final enqueue'd: job %d\n", jobCounter);
		}

	}

	return NULL;
}

void* job() {
	Job job;
	while (jobsFinished < NUMBER_OF_JOBS) {
//		if (theQueues.finishedSize == 0) {
//			sleep(2);
//		} else
		if (jobCounter < NUMBER_OF_JOBS) {
			initJob(&job);
			printf("about to be enqueue'd to CPU: job %d\n"
					"current thread: %d\n", jobCounter, pthread_self);
			enqueue(job, 0);
//			printf("cpu size after enqueue: %d\n", theQueues.cpuSize);
			printf("CPU enqueue'd: job %d\n", jobCounter);
			//		now = time(0);
			//		tm = localtime(&now);
			//		printf("system time in seconds: %d\n", tm.tm_sec);
			//		sleep(2);
			//		now = time(0);
			//		tm = localtime(&now);
			//		printf("system time in seconds: %d\n", tm.tm_sec);

			printf("\n\nCPU Size: %d\n", theQueues.cpuSize);
			printf("\n\nIO Size: %d\n", theQueues.ioSize);
			printf("\n\nFinished Size: %d\n", theQueues.finishedSize);
		} else if (theQueues.finishedSize > 0) {
			printf("about to be dequeue'd from final: job %d\n", jobCounter);
			job = dequeue(2); //DQ from finished queue
			printf("dequeue'd from final: job %d\n", jobCounter);
			free(&job);
			jobsFinished++;
		}
	}
	pthread_exit(NULL);
	return NULL;
}

//	Job job1;
//	Job job2;
//	Job job3;
//	Job job4;
//	initJob(&job1);
//	initJob(&job2);
//	initJob(&job3);
//	initJob(&job4);

//	enqueue(&theQueues, &job1, 0);
//	enqueue(&theQueues, &job2, 0);
//	enqueue(&theQueues, &job3, 0);
//	enqueue(&theQueues, &job4, 0);

//	printf("job1 jobid: %d\n", job1.job_id);
//	printf("job1 current_phase: %d\n", job1.current_phase);
//	printf("job1 cpuDuration: %d\n", job1.cpuDuration);
//	printf("job1 ioDuration: %d\n", job1.ioDuration);
//	printf("job1 cpuCurrentTiming: %d\n", job1.cpuCurrentTiming);
//	printf("job1 ioCurrentTiming: %d\n", job1.ioCurrentTiming);
//	printf("job1 is_completed: %d\n\n", job1.is_completed);
//
//	printf("job2 jobid: %d\n", job2.job_id);
//	printf("job2 current_phase: %d\n", job2.current_phase);
//	printf("job2 cpuDuration: %d\n", job2.cpuDuration);
//	printf("job2 ioDuration: %d\n", job2.ioDuration);
//	printf("job2 cpuCurrentTiming: %d\n", job2.cpuCurrentTiming);
//	printf("job2 ioCurrentTiming: %d\n", job2.ioCurrentTiming);
//	printf("job2 is_completed: %d\n\n", job2.is_completed);
//
//	printf("job3 jobid: %d\n", job3.job_id);
//	printf("job3 current_phase: %d\n", job3.current_phase);
//	printf("job3 cpuDuration: %d\n", job3.cpuDuration);
//	printf("job3 ioDuration: %d\n", job3.ioDuration);
//	printf("job3 cpuCurrentTiming: %d\n", job3.cpuCurrentTiming);
//	printf("job3 ioCurrentTiming: %d\n", job3.ioCurrentTiming);
//	printf("job3 is_completed: %d\n", job3.is_completed);

//	enqueue(&theQueues, job3, 0);
//	enqueue(&theQueues, job1, 0);
//	Job pulledJob = dequeue(&theQueues, 0);
//	printf("CPU Size: %d\n", theQueues.cpuSize);
//	printf("IO Size: %d\n", theQueues.ioSize);
//	printf("Final Size: %d\n", theQueues.finishedSize);
//	printf("pulled job ID: %d\n", pulledJob.job_id);

//	createJobs(NUMBER_OF_JOBS);
