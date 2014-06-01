#include "shmem.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
unsigned int global_max_size;

int create_shm(const char* path, int size)
{
	int ret=0;
	key_t keytok = ftok(path,0);
	int shmid = shmget( keytok ,  RESERVE_SIZE + size , 0666 | IPC_CREAT | IPC_EXCL );
	if ( shmid == -1)//this share memory already exist
		shmid = shmget( keytok,  RESERVE_SIZE+size, 0666 | IPC_CREAT );
	if( shmid > 0)//first create success
	{
		struct shmid_ds shmds;
		ret = shmctl( shmid, IPC_STAT, &shmds);
		if( ret == 0 )
		{
			printf("size of memory segment is %d\n", shmds.shm_segsz);
			printf("Number of attches %d\n",(int) shmds.shm_nattch);
		}
		else
		{
			printf("shmctl execute failed\n");
		}
	}
	else
	{
		printf("create shm failed\n");
	}
	global_max_size = size;
	return shmid;
}

int close_shm(int shmid)
{
	shmctl(shmid, IPC_RMID, NULL);
}

int get_shmqueue_head(int shmid)
{
	if(shmid <= 0) return -1;
	struct shmqueue_ctl* p = (struct shmqueue_ctl*)shmat(shmid, 0, SHM_RDONLY);
	int head = p->head;
	return head;
}
int get_shmqueue_tail(int shmid)
{
	if(shmid <= 0) return -1;
	struct shmqueue_ctl* p = (struct shmqueue_ctl*)shmat(shmid, 0, SHM_RDONLY);
	int tail = p->tail;
	return tail;
}
int get_shmqueue_length(int shmid)
{
	if(shmid <= 0) return -1;
	struct shmqueue_ctl* p = (struct shmqueue_ctl*)shmat(shmid, 0, SHM_RDONLY);
	int size = p->max_size;
	return size;
}
int set_shmqueue_head(int shmid, int head)
{
	if(shmid <=0 ) return -1;
	struct shmqueue_ctl* p = (struct shmqueue_ctl*)shmat(shmid, 0, SHM_RND);
	p->head = head;
	return head;
}
int set_shmqueue_tail(int shmid, int tail)
{
	if(shmid <=0 ) return -1;
	struct shmqueue_ctl* p = (struct shmqueue_ctl*)shmat(shmid, 0, SHM_RND);
	p->tail = tail;
	return tail;
}
int set_shmqueue_length(int shmid, int length)
{
	if(shmid <=0 ) return -1;
	struct shmqueue_ctl* p = (struct shmqueue_ctl*)shmat(shmid, 0, SHM_RND);
	p->max_size = length;
	return length;
}

void * read_shm(int shmid, void* buff, unsigned int start_pos, unsigned int length)
{
	if( buff == NULL || length == 0 || shmid <= 0)return NULL;
	if( start_pos + length > global_max_size ) return NULL;
	unsigned char *shmem = shmat(shmid, 0, SHM_RDONLY);
	shmem += start_pos + RESERVE_SIZE;
	memcpy(buff, shmem, length);
	return buff;
}

int write_shm(int shmid, void *buff, unsigned int start_pos, unsigned int length)
{
	if( buff == NULL || length == 0 || shmid <= 0)return -1;
	if( start_pos + length > global_max_size ) return -1;
	unsigned char *shmem = shmat( shmid, 0, SHM_RND);
	shmem += start_pos + RESERVE_SIZE;
	memcpy(shmem, buff, length);
	return 0;
}

