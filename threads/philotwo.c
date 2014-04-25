/*
 ============================================================================
 Name        : philotwo.c
 Author      : Chris Stickney
 Description : the dining philosophers problem with posix threads
 ============================================================================
 */
#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <time.h>
#define PHILO_COUNT 5

void* philosopher();
int randomGaussian_r(int mean, int stddev, unsigned int* state);

int chopstick[PHILO_COUNT];
pthread_mutex_t mutex;

int main() {
	int i;
	pthread_t threads[PHILO_COUNT];//the philosopher threads

	int* ids = malloc(PHILO_COUNT * sizeof(int));
	for(i=0; i<PHILO_COUNT; i++){
		ids[i]=i;
	}
	pthread_mutex_init(&mutex, NULL);//init mutex var

	for(i=0; i < PHILO_COUNT; i++) {//place chopsticks
		chopstick[i] = 1;
	}
	for(i=1; i <= PHILO_COUNT; i++){//create posix threads
		pthread_create(&threads[PHILO_COUNT - i], NULL, philosopher, (void*) &ids[i-1] );
	}
	for(i=1; i <= PHILO_COUNT; i++){//waits for each thread to terminate
		pthread_join(threads[PHILO_COUNT - i], NULL);
	}

	pthread_mutex_destroy(&mutex);//destroy mutex var
	return 0;
}
void* philosopher(void* arg){
	int* id = (int*) arg;

	int totalEatTime=0, eatTime=0, totalThinkTime=0, thinkTime=0, leftChopstick;

	int rightChopstick = *id;
	if(*id == 0) leftChopstick = PHILO_COUNT-1;//-1 is not a valid index, so loop back to the highest index
	else leftChopstick = *id-1;

	unsigned int* state = malloc(sizeof(unsigned int));
	*state = time(NULL) * (13 -*id);//seed for gaussian fn

	while(totalEatTime < 100){
		if(pthread_mutex_trylock(&chop_hold) == 0){//move and prevent other philosophers from moving
			//pthread_mutex_lock(&mutex);//prevent other philosophers from moving, or wait to do so
			if(chopstick[rightChopstick] == 1 && chopstick[leftChopstick] == 1) {//both chopsticks are available
				printf("Philosopher %d picks up chopsticks %d and %d.\n", *id, leftChopstick, rightChopstick);
				chopstick[rightChopstick] = 0;//pick up right chopstick
				chopstick[leftChopstick] = 0;//pick up left chopstick
	
				pthread_mutex_unlock(&mutex);//unlock now that chopsticks are safely taken
	
				if ((eatTime = randomGaussian_r(9, 3,state)) < 0) eatTime = 0;// eats for a random amount of time >= 0
				printf("Philosopher %d will be eating for %d seconds. %d seconds total eat time so far.\n", *id, eatTime, totalEatTime);
				totalEatTime += eatTime;
				sleep(eatTime);
				printf("Philosopher %d sets down chopsticks %d and %d.\n", *id, leftChopstick, rightChopstick);//drops chopsticks
				chopstick[rightChopstick] = 1;
				chopstick[leftChopstick] = 1;
	
				if ((thinkTime = randomGaussian_r(11, 7,state)) < 0) thinkTime = 0;//thinks for a random amount of time >= 0
				printf("Philosopher %d will be thinking for %d seconds. %d seconds total think time so far.\n",*id, thinkTime, totalThinkTime);
				totalThinkTime += thinkTime;
				sleep(thinkTime);
				printf("Philosopher %d has finished thinking.\n", *id);
			}
	
			else{//let someone else move since both chopsticks are unavailable
				pthread_mutex_unlock(&mutex);
				sleep(1);
			}
		else sleep(1); //wait to try and move again
			
	}
	printf("Philosopher %d has left the table. Total time eating: %d Total time thinking: %d\n", *id, totalEatTime, totalThinkTime);
	return;
}
/* successive calls to randomGaussian produce integer return values */
/* having a gaussian distribution with the given mean and standard  */
/* deviation.  Return values may be negative.                       */
/* This routine is re-entrant and thread safe, but requires the     */
/* caller to pass a pointer to an unsigned integer to be used as    */
/* state for the pseudo-random sequence.                            */
int randomGaussian_r(int mean, int stddev, unsigned int* state) {
        double mu = 0.5 + (double) mean;
        double sigma = fabs((double) stddev);
        double f1 = sqrt(-2.0 * log((double) rand_r(state) / (double) RAND_MAX));
        double f2 = 2.0 * 3.14159265359 * (double) rand_r(state) / (double) RAND_MAX;
        if (rand_r(state) & (1 << 5))
                return (int) floor(mu + sigma * cos(f2) * f1);
        else
                return (int) floor(mu + sigma * sin(f2) * f1);
}
