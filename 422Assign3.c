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

#define NUMBER_OF_JOBS 15
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
FILE *outFile;
static int jobCounter = 1;
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
	outFile = fopen("threadfunoutput.txt", "w");


	initScheduler();

//	printf("job counter: %d\n", jobCounter);
	pthread_mutex_init(&lockCPU, NULL);
	pthread_mutex_init(&lockIO, NULL);
	pthread_mutex_init(&lockFinal, NULL);
	pthread_mutex_init(&lockCounter, NULL);

	for (i = 0; i < 4; i++) {
		printf("current job thread: %d\n", i);
		rc = pthread_create(&(jobOne[i]), NULL, job, NULL);
		fprintf(outFile, "Job %d started.\n", i );
//		printf("\n\nCPU Size: %d\n", theQueues.cpuSize);
//		printf("\n\nIO Size: %d\n", theQueues.ioSize);
//		printf("\n\nFinished Size: %d\n", theQueues.finishedSize);

	}
	for (i = 0; i < 8; i++) {
		printf("current cpu thread: %d\n", i);
		fprintf(outFile, "CPU %d started.\n", i );
		rc = pthread_create(&(cpuOne[i]), NULL, cpu, NULL);

	}
	for (i = 0; i < 4; i++) {
		printf("current io thread: %d\n", i);
		fprintf(outFile, "IO %d started.\n", i );
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
	fclose(outFile);
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
//			sleep(2);
		} else {
			fprintf(outFile, "About to be dequeue'd from CPU: job %d\n", job.job_id);
			//	time(&rawTime);
			//	printf("system time: %s\n", ctime(&rawTime));
			job = dequeue(0);
			fprintf(outFile, "CPU dequeue'd: job %d\n", job.job_id);
			//	time(&rawTime);
			//	printf("system time: %s\n", ctime(&rawTime));
			jobTime = job.cpuDuration;
				sleep(jobTime);
			fprintf(outFile, "About to be enqueue'd to IO: job %d;\n", job.job_id);
			printf("iosize: %d\n", theQueues.ioSize);
			enqueue(job, 1);
			printf("iosize after enqueue: %d\n", theQueues.ioSize);
			fprintf(outFile, "IO enqueue'd: job %d\n", job.job_id);
		}

	}
	return NULL;
}

void* io() {
	Job job;
	int jobTime;
	while (jobsFinished < NUMBER_OF_JOBS) {
		if (theQueues.ioSize == 0) {
//			sleep(2);
		} else {
			fprintf(outFile, "About to be dequeue'd from IO: job %d\n", job.job_id);
			job = dequeue(1);
			fprintf(outFile, "Dequeue'd from IO: job %d\n", job.job_id);
			jobTime = job.ioDuration;
				sleep(jobTime);
			fprintf(outFile, "About to be enqueue'd to final: job %d\n", job.job_id);
			enqueue(job, 2);
			fprintf(outFile, "Final enqueue'd: job %d\n", job.job_id);
		}

	}

	return NULL;
}

void* job() {
	Job job;
	while (jobsFinished < NUMBER_OF_JOBS) {
		if (jobCounter < NUMBER_OF_JOBS) {
			initJob(&job);
			fprintf(outFile, "About to be enqueue'd to CPU: job %d\n", job.job_id);
			enqueue(job, 0);
//			sleep(2);
//			printf("cpu size after enqueue: %d\n", theQueues.cpuSize);
			fprintf(outFile, "CPU enqueue'd: job %d\n", job.job_id);


			printf("\n\nCPU Size: %d\n", theQueues.cpuSize);
			printf("\n\nIO Size: %d\n", theQueues.ioSize);
			printf("\n\nFinished Size: %d\n", theQueues.finishedSize);
		} else if (theQueues.finishedSize > 0) {
			job = dequeue(2); //DQ from finished queue
			fprintf(outFile, "Dequeue'd from final: job %d\n", job.job_id);
			free(&job);
			jobsFinished++;
		}
	}
	pthread_exit(NULL);
	return NULL;
}
