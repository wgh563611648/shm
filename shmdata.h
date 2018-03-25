#include <unistd.h>  
#include <stdlib.h>  
#include <stdio.h>  
#include <sys/shm.h>
#include <string.h>
#include <sys/types.h>  
#include <sys/stat.h>  
#include <fcntl.h>
#include <string.h>  
#include <sys/sem.h>  
#include <errno.h>


#define MAXSIZE 1024
#define SHM_KEY (1234)
#define SEM_KEY (2345)
typedef struct shared_use_st
{
	unsigned char written;//1:can read 0:can write
	char buff[MAXSIZE];
}SHARED_ST;

union semun
{
	int val;
	struct semid_ds *buf;
	unsigned short *arry;
};

int get_semid(void)
{
	int semid;
	semid = semget((key_t)SEM_KEY, 1, 0666 | IPC_CREAT | IPC_EXCL);
	if(-1 == semid)
	{
		printf("get_semid errno=%d\n", errno);
		if(errno == EEXIST)
		{
			printf("semget already exist==\n");
			return semget((key_t)SEM_KEY, 1, 0666 | IPC_CREAT);
		}
	}

	union semun sem_union;

	sem_union.val = 1;
	if(-1 == semctl(semid, 0, SETVAL, sem_union))
	{
		printf("semctl SETVAL fail\n");
		return 0;
	}
	return semid;
}

int sem_p(int semid)
{
	struct sembuf semp;
	semp.sem_num = 0;
	semp.sem_op = -1;
	semp.sem_flg = SEM_UNDO;

	if(-1 == semop(semid, &semp, 1))
	{
		printf("semop p fail\n");
		return 0;
	}
	return 1;
}

int sem_v(int semid)
{
	struct sembuf semv;
	semv.sem_num = 0;
	semv.sem_flg = SEM_UNDO;
	semv.sem_op = 1;

	if(-1 == semop(semid, &semv, 1))
	{
		printf("semop v fail\n");
		return 0;
	}
	return 1;
}

int sem_del(int semid)
{
	union semun sem_union;

	if(-1 == semctl(semid, 0, IPC_RMID, sem_union))
	{
		printf("semctl IPC_RMID fail\n");
		return 0;
	}
	return 1;
}