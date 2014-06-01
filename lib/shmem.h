#ifndef SHMEM_H_
#define SHMEM_H_


struct shmqueue_ctl{
	unsigned int max_size;
	int head;
	int tail;
};

#define RESERVE_SIZE sizeof(struct shmqueue_ctl) 

int create_shm(const char* path, int size);

void *read_shm(int shmid, void *buff, unsigned int start_pos, unsigned int length);

int write_shm(int shmid, void *buff, unsigned int start_pos, unsigned int length);

int close_shm(int shmid);

#endif
