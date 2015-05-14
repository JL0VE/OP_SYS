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

#define NUMBER_OF_JOBS 3
#define CLOCK_CYCLE 2

static int jobCounter = 1;

typedef struct job {
	int job_id;
	int current_phase; // 0 = cpu, 1 = io, 2 = final
	// Phase types: 1 = CPU phase; 2 = IO phase
	int cpuDuration;
	int ioDuration;
	int cpuCurrentTiming;
	int ioCurrentTiming;

	int is_completed;
}Job;

typedef struct queues {
	Job cpuQueue[NUMBER_OF_JOBS];
	Job ioQueue[NUMBER_OF_JOBS];
	Job finalQueue[NUMBER_OF_JOBS];
	int cpuSize;
	int ioSize;
	int finalSize;
}SchedulerQueues;

void initScheduler(SchedulerQueues *theQueues);
Job dequeue(SchedulerQueues *theQueues, int whichQueue);
void enqueue(SchedulerQueues *theQueues, Job theJob, int whichQueue);
void initJob(Job *theJob);

int main(int argc, char** argv) {
	setvbuf (stdout, NULL, _IONBF, 0);
	srand (time(NULL));
	pthread_t cpuOne, ioOne, jobOne;
	SchedulerQueues theQueues;
	initScheduler(&theQueues);

	Job job1;
	Job job2;
	Job job3;
	Job job4;
	initJob(&job1);
	initJob(&job2);
	initJob(&job3);
	initJob(&job4);

	enqueue(&theQueues, &job1, 0);
	enqueue(&theQueues, &job2, 0);
	enqueue(&theQueues, &job3, 0);
	enqueue(&theQueues, &job4, 0);

	pthread_create(&cpuOne, NULL, cpuController, &theQueues);
	pthread_create(&ioOne, NULL, ioController, &theQueues);
	pthread_create(&jobOne, NULL, jobController, &theQueues);
	pthread_join(cpuOne, NULL);
	pthread_join(ioOne, NULL);
	pthread_join(jobOne, NULL);


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
//	printf("Final Size: %d\n", theQueues.finalSize);
//	printf("pulled job ID: %d\n", pulledJob.job_id);

//	createJobs(NUMBER_OF_JOBS);
	return 1;
}


void initScheduler(SchedulerQueues *theQueues) {
	theQueues->cpuSize = 0;
	theQueues->ioSize = 0;
	theQueues->finalSize = 0;
}


void initJob(Job *theJob) {
	theJob->job_id = jobCounter++;
	theJob->current_phase = 0;
	theJob->cpuDuration = (rand() % 5) + 1;
	theJob->ioDuration = 5 + (rand() % 15);
	theJob->cpuCurrentTiming = 0;
	theJob->ioCurrentTiming = 0;
	theJob->is_completed = 0;
}

void enqueue(SchedulerQueues *theQueues, Job theJob, int whichQueue) {
	switch(whichQueue) {
	case 0:
		theQueues->cpuQueue[theQueues->cpuSize] = theJob;
		theQueues->cpuSize++;
		break;
	case 1:
		theQueues->cpuQueue[theQueues->cpuSize] = theJob;
		theQueues->ioSize++;
		break;
	case 2:
		theQueues->cpuQueue[theQueues->cpuSize] = theJob;
		theQueues->finalSize++;
		break;
	}
}

Job dequeue(SchedulerQueues *theQueues, int whichQueue) {
	Job returnJob;
	int i;
	switch(whichQueue) {
	case 0:
		returnJob = theQueues->cpuQueue[0];
		for (i = 0; i < theQueues->cpuSize; i++) {
			theQueues->cpuQueue[i] = theQueues->cpuQueue[i + 1];
		}
		theQueues->cpuSize--;
		break;
	case 1:
		returnJob = theQueues->ioQueue[0];
		for (i = 0; i < theQueues->cpuSize; i++) {
			theQueues->ioQueue[i] = theQueues->ioQueue[i + 1];
		}
		theQueues->ioSize--;
		break;
	case 2:
		returnJob = theQueues->finalQueue[0];
		for (i = 0; i < theQueues->cpuSize; i++) {
			theQueues->finalQueue[i] = theQueues->finalQueue[i + 1];
		}
		theQueues->finalSize--;
		break;
	}
	return returnJob;
}

void cpuController(SchedulerQueues theQueue) {
	pthread_t cpuTwo, cpuThree, cpuFour, cpuFive, cpuSix, cpuSeven, cpuEight;

}

void* cpu(Job cpuQueue[NUMBER_OF_JOBS]) {

}


void ioController(SchedulerQueues theQueue) {
	pthread_t ioTwo, ioThree, ioFour;


}

void* io(Job ioQueue[NUMBER_OF_JOBS]) {

}

void jobController(SchedulerQueues theQueue) {
	pthread_t jobTwo, jobThree, jobFour;


}

void* job() {

}
