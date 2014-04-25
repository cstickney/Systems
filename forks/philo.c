/*
 ============================================================================
 Name        : philo.c
 Author      : Chris Stickney
 Description : 5 philosopher problem program
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <errno.h>
#include <math.h>
#include <time.h>
#define PHILO_COUNT 5


int randomGaussian(int mean, int stddev);
void philosopher(int philoID);

int semid;
int *chopstick;
struct sembuf semWait[1] = {{0,-1,0}},
			  semSignal[1] = {{0,1,0}};


int main() {
	int shmid, *shmem;
//	int semget(key, nsems, semflg)
//	returns set identifier for key
//	set identifier, number of semaphores, semaphore flags with | separators

	if((semid = semget(IPC_PRIVATE, 1, S_IRUSR | S_IWUSR))< 0){
		fprintf(stderr, "semget failure:%s\n",strerror(errno));
		return 1;
	}
//	int shmget(key, size, shmflg)
//	returns identifier of shared memory segment
//	memory identifier, size of memory, shared memory flags with |s
	if((shmid = shmget(IPC_PRIVATE, sizeof(int *) * PHILO_COUNT,  S_IRUSR | S_IWUSR)) < 0){
		fprintf(stderr, "shmget failure:%s\n",strerror(errno));
		return 1;
	}
//	int semop(int semid, struct sembuf *sops, unsigned nsops)
//	returns -1 if error
//	semaphore id from semget, sembuf, size of array
//		struct sembuf{ ushort sem_num; short sem_op; short sem_flg)
//		semaphore number, semaphore operation, semaphore flags
//	if sem_op is positive then its value is added to the semaphore
//	if negative, then its value is subtracted from the semaphore
//		trying to decrement below zero waits until it can decrement to zero or higher
//		blocks remove when operations can all finish, the process gets a signal, or the semaphore set is removed
	if(semop(semid, semSignal, 1) < 0){//init to 1
		fprintf(stderr, "semop failure:%s\n",strerror(errno));
		return 1;
	}
//	void* shmat ( int shmid, const void* shmaddr, int shmflg);
//	returns address of shared memory segment or -1 for error
//	id of shared memory,memory segment to attach to, null = system pick  ,shared memory flags
	shmem = (int*) shmat(shmid, 0, 0);
	if(shmem < 0){
		fprintf(stderr, "shmat failure:%s\n",strerror(errno));
		return 1;
	}
	int i;
	chopstick = (int*) shmem;
	for (i = 0; i < PHILO_COUNT; i++) {//place chopsticks
		chopstick[i] = 1;
	}
	i=0;
	while(i<PHILO_COUNT) {//create philosophers
		if(fork() == 0) {
			philosopher(i);
			return 0;
		}
		++i;
	}
	i=0;
	while(i<PHILO_COUNT) {//wait for philosophers to leave
		wait(NULL);//waits for child processes to end
		++i;
		printf("%d Philosophers remain\n", PHILO_COUNT-i);
	}
//	int shmctl ( int shmqid, int cmd, struct shmid_ds *buf ); //removes a shared memory segment
//	returns -1 for error, or other things for non-RMID
//	shared memory id, an IPC_ command,
	shmctl(shmid, IPC_RMID, NULL);//remove shares memory segment
	return 0;
}
void philosopher(int philoID){
	int totalEatTime=0, eatTime=0, totalThinkTime=0, thinkTime=0, leftChopstick;
	int rightChopstick = philoID;
	if(philoID == 0) leftChopstick = PHILO_COUNT-1;//-1 is not a valid index, so loop back to the highest index
	else leftChopstick = philoID-1;

	srand(time(NULL) * philoID);//seed random for the random gaussian function
	while(totalEatTime < 100){
			if(semop(semid, semWait, 1) < 0) {//waits for a turn to move
				fprintf(stderr, "semop failure(philosopher wait):%s\n",strerror(errno));
				exit(1);
			}

			if(chopstick[rightChopstick] == 1 && chopstick[leftChopstick] == 1) {//both chopsticks are available
				printf("Philosopher %d picks up chopsticks %d and %d.\n", philoID, leftChopstick, rightChopstick);
				chopstick[rightChopstick] = 0;//pick up right chopstick
				chopstick[leftChopstick] = 0;//pick up left chopstick
				if (semop(semid, semSignal, 1) < 0) {//lets other philosophers move again
					fprintf(stderr, "semop failure(philosopher if):%s\n",strerror(errno));
					exit(1);
				}

				if ((eatTime = randomGaussian(9, 3)) < 0) eatTime = 0;// eats for a random amount of time > 0
				printf("Philosopher %d will be eating for %d seconds. %d seconds total eat time so far.\n", philoID, eatTime, totalEatTime);
				totalEatTime += eatTime;
				sleep(eatTime);
				printf("Philosopher %d sets down chopsticks %d and %d.\n", philoID, leftChopstick, rightChopstick);//drops chopsticks
				chopstick[rightChopstick] = 1;
				chopstick[leftChopstick] = 1;

				if ((thinkTime = randomGaussian(11, 7)) < 0) thinkTime = 0;//thinks for a random amount of time > 0
				printf("Philosopher %d will be thinking for %d seconds. %d seconds total think time so far.\n",philoID, thinkTime, totalThinkTime);
				totalThinkTime += thinkTime;
				sleep(thinkTime);
				printf("Philosopher %d has finished thinking.\n", philoID);
			}

			else{
				if (semop(semid, semSignal, 1) < 0) {//lets other philosophers move
					fprintf(stderr, "semop failure (philosopher else):%s\n",strerror(errno));
					exit(1);
				}
			}
	}
		printf("Philosopher %d has left the table. Total time eating: %d Total time thinking: %d\n", philoID, totalEatTime, totalThinkTime);
//		int shmdt(const void *shmaddr)// detaches from a shared memory segment
//		returns -1 for error
//		takes shared memory address as argument
		shmdt(chopstick);
		return;
}
int randomGaussian(int mean, int stddev) {
	double mu = 0.5 + (double) mean;
	double sigma = fabs((double) stddev);
	double f1 = sqrt(-2.0 * log((double) rand() / (double) RAND_MAX));
	double f2 = 2.0 * 3.14159265359 * (double) rand() / (double) RAND_MAX;
	if (rand() & (1 << 5)) return (int) floor(mu + sigma * cos(f2) * f1);
	else return (int) floor(mu + sigma * sin(f2) * f1);
}
