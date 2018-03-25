#include "shmdata.h"

int main(int argc, char **argv)
{
	int semid;
	int shmid;
	SHARED_ST *share = NULL;
	void *shm;

	shmid = shmget((key_t)SHM_KEY, sizeof(SHARED_ST), 0666 | IPC_CREAT);
	if(-1 == shmid)
	{
		fprintf(stderr, "shmread shmget fail\n");
		exit(-1);
	}
	shm = shmat(shmid, (void *)0, 0);
	if((void *)-1 == shm)
	{
		fprintf(stderr, "shmread shmget fail\n");
		exit(-1);
	}
	share = (SHARED_ST *)shm;
	share->written = 0;

	if(!(semid = get_semid()))
	{
		exit(-1);
	}
	while(1)
	{
		/*if(0 == share->written)
		{
			printf("please wait write ...\n");
			sleep(1);
		}*/
		if(!sem_p(semid))
		{
			exit(-1);
		}
		printf("you wrote: %s\n",share->buff);
		share->written = 0;

		if(!sem_v(semid))
		{
			break;
		}
		sleep(1);
		if(strncmp(share->buff, "end", 3) == 0) 
			break;
	}
	
	if(!sem_del(semid))
	{
		fprintf(stderr,"Failed to semop del semaphore\n");
		exit(-1);
	}
	if(-1 == shmdt(shm))
	{
		fprintf(stderr, "shmread shmdt fail\n");
		exit(-1);
	}

	if(-1 == shmctl(shmid, IPC_RMID, 0))
	{
		fprintf(stderr, "shmread shmctl fail\n");
		exit(-1);
	}
	return 0;
}
