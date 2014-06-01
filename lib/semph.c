#include "semph.h"
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>

int global_sems = 0;
//-1 error 
//0 already exist
//>0 first create
int init_sems(const char * path, int sum)
{
	int ret=0;
	key_t keytok = ftok(path, 1);
	int semids = semget( keytok, sum, 0666 | IPC_CREAT | IPC_EXCL );
	if( semids == -1)
	{
		semids = semget( keytok, sum, 0666 | IPC_STAT );
		if (semids <= 0 )
		{
			printf("create semphore failed\n");
			return -1;
		}
		else
		{
			global_sems = semids;
			return 0;
		}
	}
	else if(semids <= 0)
	{
		printf("Create semphore failed\n");
		return -1;
	}
	global_sems = semids;
	return global_sems;
}

int close_sems()
{
	if(global_sems <= 0) return -1;
	int ret = semctl(global_sems, 0, IPC_RMID);
	return ret;
}

int set_sem_value(int i, int value)
{
	if(global_sems <= 0) return -1;
	int ret = semctl(global_sems, i, SETVAL,value);
	printf("set semphore[%d] value[%d] %s\n", i, value, strerror(errno));
	return ret;
}

int get_sem_value(int i)
{
	if(global_sems <= 0) return -1;
	int ret = semctl(global_sems, i, GETVAL);
	printf("set semphore[%d] value[%d] %s\n", i, ret, strerror(errno));
	return ret;
}
void p_sem(int i)
{
	if(global_sems <= 0) return;
	struct sembuf semb;
	semb.sem_num = i;
	semb.sem_op = -1;
	semb.sem_flg = SEM_UNDO;
	semop(global_sems, &semb, 1);
}

void v_sem(int i)
{
	if(global_sems <= 0) return;
	struct sembuf semb;
	semb.sem_num = i;
	semb.sem_op = 1;
	semb.sem_flg = SEM_UNDO;
	semop(global_sems, &semb, 1);
}

