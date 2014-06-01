#include <pthread.h>

#define MAX_THREAD 50
#define MAX_POOL 50
typedef void (*func)(void *);

typedef struct Task{
	func runner;
	void *args;
	struct Task *next;
}Task,*Taskp;

typedef struct Pool{
	int size;
	Task *taskqueue;
	Task *tail;
	pthread_t *pqueue;
	int shutdown;// destory the pool
	int status;// 0 init  1 usable
	pthread_cond_t cond;
	pthread_mutex_t mtx;
}Pool,*Poolp;

void *pool_daemon(void *args);
void init_pthread_pool(Pool *pool);
int start_pthread_pool(Pool *pool);
int add_task_to_pool(Pool *pool, func ptr, void *args);
int close_pthread_pool(Pool *pool);

/* thread pool usage */
/*  1 . define a pool 
 *  Pool *pool  = (Pool*)malloc(sizeof(Pool));
 *  2 . init this pool
 *  init_pthread_pool(pool);
 *  3 . start this pool / 4. add task to pool
 *  add_task_to_pool(pool, test, args);
 *  start_pthread_pool(pool);
 *  5 . close this pool
 *  close_pthread_pool(pool);
 *  */
