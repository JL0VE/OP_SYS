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

static int jobCounter = 0;
static int jobsFinished = 0;
pthread_mutex_t lockCPU = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lockIO = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lockFinal = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lockCounter = PTHREAD_MUTEX_INITIALIZER;
//time_t now;
//struct tm *tm;

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

//function declarations
void initScheduler(SchedulerQueues *theQueues);
Job dequeue(SchedulerQueues *theQueues, int whichQueue);
void enqueue(SchedulerQueues *theQueues, Job theJob, int whichQueue);
void initJob(Job *theJob);
void* cpu(SchedulerQueues *theQueues);
void* io(SchedulerQueues *theQueues);
void* job(SchedulerQueues *theQueues);

//main
int main(int argc, char** argv) {
	setvbuf(stdout, NULL, _IONBF, 0);
	srand(time(NULL));
	pthread_t cpuOne[8], ioOne[4], jobOne[4];
	int i;

	SchedulerQueues theQueues;
	initScheduler(&theQueues);

//	printf("job counter: %d\n", jobCounter);

	for (i = 0; i < 4; i++) {
		pthread_create(&jobOne[i], NULL, job, &theQueues);
	}
	for (i = 0; i < 8; i++) {
		pthread_create(&cpuOne[i], NULL, cpu, &theQueues);
	}
	for (i = 0; i < 4; i++) {
		pthread_create(&ioOne[i], NULL, io, &theQueues);
	}

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
	pthread_join(jobOne[0], NULL);
	pthread_join(jobOne[1], NULL);
	pthread_join(jobOne[2], NULL);
	pthread_join(jobOne[3], NULL);

	return 1;
}

void initScheduler(SchedulerQueues *theQueues) {
	theQueues->cpuSize = 0;
	theQueues->ioSize = 0;
	theQueues->finishedSize = 0;
}

void initJob(Job *theJob) {
	pthread_mutex_lock(&lockCounter);
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

void enqueue(SchedulerQueues *theQueues, Job theJob, int whichQueue) {
	switch (whichQueue) {
	case 0:
		pthread_mutex_lock(&lockCPU);
		theQueues->cpuQueue[theQueues->cpuSize] = theJob;
		theQueues->cpuSize++;
		pthread_mutex_unlock(&lockCPU);
		break;
	case 1:
		pthread_mutex_lock(&lockIO);
		theQueues->cpuQueue[theQueues->cpuSize] = theJob;
		theQueues->ioSize++;
		pthread_mutex_unlock(&lockIO);
		break;
	case 2:
		pthread_mutex_lock(&lockFinal);
		theQueues->cpuQueue[theQueues->cpuSize] = theJob;
		theQueues->finishedSize++;
		pthread_mutex_unlock(&lockFinal);
		break;
	}
}

Job dequeue(SchedulerQueues *theQueues, int whichQueue) {
	Job returnJob;
	int i;
	switch (whichQueue) {
	case 0:
		pthread_mutex_lock(&lockCPU);
		returnJob = theQueues->cpuQueue[0];
		for (i = 0; i < theQueues->cpuSize; i++) {
			theQueues->cpuQueue[i] = theQueues->cpuQueue[i + 1];
		}
		theQueues->cpuSize--;
		pthread_mutex_unlock(&lockCPU);
		break;
	case 1:
		pthread_mutex_lock(&lockIO);
		returnJob = theQueues->ioQueue[0];
		for (i = 0; i < theQueues->cpuSize; i++) {
			theQueues->ioQueue[i] = theQueues->ioQueue[i + 1];
		}
		theQueues->ioSize--;
		pthread_mutex_unlock(&lockIO);
		break;
	case 2:
		pthread_mutex_lock(&lockFinal);
		returnJob = theQueues->finishedQueue[0];
		for (i = 0; i < theQueues->cpuSize; i++) {
			theQueues->finishedQueue[i] = theQueues->finishedQueue[i + 1];
		}
		theQueues->finishedSize--;
		pthread_mutex_unlock(&lockFinal);
		break;
	}
	return returnJob;
}

void* cpu(SchedulerQueues *theQueues) {
	Job job;
	int jobTime;
	while (jobsFinished < NUMBER_OF_JOBS) {
		if (theQueues->cpuSize == 0) {
			sleep(2);
		}
		printf("about to be dequeue'd from CPU: job %d\n", jobCounter);
		//	time(&rawTime);
		//	printf("system time: %s\n", ctime(&rawTime));
		job = dequeue(&theQueues, 0);
		printf("CPU dequeue'd: job %d\n", jobCounter);
		//	time(&rawTime);
		//	printf("system time: %s\n", ctime(&rawTime));
		jobTime = job.cpuDuration;
		//	sleep(jobTime);
		printf("about to be enqueue'd to IO: job %d\n", jobCounter);
		enqueue(&theQueues, job, 1);
		printf("IO enqueue'd: job %d\n", jobCounter);
	}
	return NULL;
}

void* io(SchedulerQueues *theQueues) {
	Job job;
	int jobTime;
	while (jobsFinished < NUMBER_OF_JOBS) {
		if (theQueues->ioSize == 0) {
			sleep(2);
		}
		printf("about to be dequeue'd from IO: job %d\n", jobCounter);
		job = dequeue(&theQueues, 1);
		printf("dequeue'd from IO: job %d\n", jobCounter);
		jobTime = job.ioDuration;
		//	sleep(jobTime);
		printf("about to be enqueue'd to final: job %d\n", jobCounter);
		enqueue(&theQueues, job, 2);
		printf("final enqueue'd: job %d\n", jobCounter);
	}

	return NULL;
}

void* job(SchedulerQueues *theQueues) {
	Job job;
	while (jobsFinished < NUMBER_OF_JOBS) {
		if (theQueues->finishedSize == 0) {
			sleep(2);
		}
		if (jobCounter < NUMBER_OF_JOBS) {
			initJob(&job);
			printf("about to be enqueue'd to CPU: job %d\n", jobCounter);
			enqueue(&theQueues, job, 0);
			printf("CPU enqueue'd: job %d\n", jobCounter);
			//		now = time(0);
			//		tm = localtime(&now);
			//		printf("system time in seconds: %d\n", tm->tm_sec);
			//		sleep(2);
			//		now = time(0);
			//		tm = localtime(&now);
			//		printf("system time in seconds: %d\n", tm->tm_sec);
		} else if (theQueues->finishedSize > 0) {
			printf("about to be dequeue'd from final: job %d\n", jobCounter);
			job = dequeue(&theQueues, 2); //DQ from finished queue
			printf("dequeue'd from final: job %d\n", jobCounter);
			free(&job);
			jobsFinished++;
		}
	}
	pthread_exit(NULL);
	return NULL;
}

int jobsStillLingering() {

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
