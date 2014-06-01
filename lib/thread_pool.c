#include <stdio.h>
#include <stdlib.h>
#include "thread_pool.h"

void *pool_daemon(void *args)
{
	if (args == NULL) return NULL;
	Pool *p = (Pool*)args;
	printf( "Starting Thread 0x%x\n", pthread_self() );
	while( 1 )
	{
		pthread_mutex_lock( &p->mtx );
		while( p->size == 0 && p->shutdown == 0)
		{
			printf( "thread 0x%x is waiting\n", pthread_self() );
			pthread_cond_wait( &p->cond, &p->mtx );
		}
		if( p->shutdown == 1 )
		{
			pthread_mutex_unlock( &p->mtx );
			printf ( "thread 0x%x will exit\n", pthread_self() );
			pthread_exit( NULL );
		}
		printf ("thread 0x%x is starting to work\n", pthread_self() );

		p->size--;
		Taskp task = p->taskqueue;
		p->taskqueue = task->next;

		pthread_mutex_unlock( &p->mtx );

		if( task->runner == NULL ) continue;
		task->runner(task->args);
		free(task);
		task = NULL;
	}
}


//init the pthread pool
void init_pthread_pool(Pool *pool)
{
	pool->size = 0;
	pool->taskqueue = NULL;
	pool->tail = NULL;
	pool->pqueue = NULL;
	pool->shutdown = 0;// 1 present destory the pool
	pool->status = 0;
	pthread_cond_init( &pool->cond, NULL);
	pthread_mutex_init( &pool->mtx, NULL);
}

int start_pthread_pool(Pool *pool)
{
	pool->pqueue = (pthread_t*)malloc(MAX_THREAD * sizeof( pthread_t) );
	int i = 0;
	for( ; i < MAX_THREAD ; i++ )
	{
		pthread_create( &(pool->pqueue[i]), NULL, (void*)pool_daemon, (void*)pool);
	}
	pool->status = 1;
	return 0;
}

int add_task_to_pool(Pool *pool, func ptr, void *args)
{
	if( pool == NULL || ptr == NULL
			|| pool->shutdown == 1 || pool->status != 1 )
		return -1;
	Task *task = (Task *)malloc( sizeof(Task) );
	task->next =NULL;
	task->runner = ptr;
	task->args = args;
	pthread_mutex_lock( &pool->mtx );
	printf( "add as task begin\n" );
	if ( pool->taskqueue == NULL )
	{
		pool->taskqueue = task;
		pool->tail = pool->taskqueue;
	}
	else 
	{
		pool->tail->next = task;
		pool->tail = pool->tail->next;
	}
	pool->size++;
	printf("add a task end\n" );
	pthread_mutex_unlock( &pool->mtx );
	pthread_cond_signal ( &pool->cond );
}

int close_pthread_pool(Pool *pool)
{
	if( pool->shutdown == 0) return -1;
	pthread_cond_broadcast( &pool->cond );

	int i = 0;
	for ( ; i < MAX_THREAD; i++ )
	{
		pthread_join( pool->pqueue[i], NULL);
	}
	free(pool->pqueue);

	Task *head = NULL;
	while( pool->taskqueue != NULL)
	{
		head = pool->taskqueue;
		pool->taskqueue = pool->taskqueue->next;
		free(head);
	}
}
