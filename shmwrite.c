#include "shmdata.h"

int main(int argc, char **argv)
{
	int semid;
	int shmid;
	SHARED_ST *share = NULL;
	void *shm;
	char buffer[MAXSIZE] = {0};
	
	if(-1 == (shmid = shmget((key_t)SHM_KEY, sizeof(SHARED_ST), 0666 | IPC_CREAT)))
	{
		fprintf(stderr, "shmget error\n");
		exit(-1);
	}
	shm = shmat(shmid, (void *)0, 0);
	if((void *)-1 == shm)
	{
		fprintf(stderr, "shmat error\n");
		exit(-1);
	}
	share = (SHARED_ST *)shm;

	if(!(semid = get_semid()))
	{
		exit(-1);
	}
	while(1)
	{
		/*if(0 != share->written)
		{
			sleep(1);
			printf("please wait read...\n");
		}*/
		if(!sem_p(semid))
		{
			exit(-1);
		}
		printf("please input:");
		fgets(buffer, MAXSIZE, stdin);
		share->written = 1;
		strncpy(share->buff, buffer, strlen(buffer));

		if(!sem_v(semid))
		{
			exit(-1);
		}
		//sleep(rand() % 2);
		if(strncmp(buffer, "end", 3) == 0) 
		{
			break;
		}
	}
	if(-1 == shmdt(shm))
	{
		fprintf(stderr, "shdt error\n");
		exit(-1);
	}
	
	return 0;
}
