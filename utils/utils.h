#ifndef PMA_UTILS_H
#define PMA_UTILS_H

#include "pma_err.h"
#include <pthread.h>

char* PRINTTIME(struct timeval timeinfo);
size_t strtrim(char *str);
int adv_atoi(char* str, int base);

	// This macro call the m_init();
#define USING(m)\
	({\
	 int ret = 0;\
	 if (mod_using(m##_init,#m) != 0) {\
	 ret = -1;\
	 }\
	 ret;\
	 })
// this macro used by USING(m)
#define mod_using(init,m) \
	({\
	 int ret = 0;\
	 if (init() != SUCCESS) {\
	 ret = -1;\
	 }\
	 ret;\
	 })

//Debug Level
enum DBG_LEVEL{
	FATAL = 0,
	ERROR,
	INFO,
	DATA,
	ALL
};

extern pthread_mutex_t mutex_debug;
extern long int pma_start_time;

#define DBG 1

#define DEBUG_LEVEL ALL

// DEBUG(level, "%s",str);
#define DEBUG(level, ...) \
	do{\
		if (DBG == 1){\
			char debug_time[40] = {0};\
			struct timeval detail_time;\
			gettimeofday(&detail_time,NULL);\
			time_t rawtime;\
			struct tm * timeinfo;\
			time(&rawtime);\
			timeinfo = localtime(&(detail_time.tv_sec));\
			sprintf(debug_time,"%4d-%02d-%02d %02d:%02d:%02d.%06d %ld",\
					timeinfo->tm_year+1900,\
					timeinfo->tm_mon+1,\
					timeinfo->tm_mday,\
					timeinfo->tm_hour,\
					timeinfo->tm_min,\
					timeinfo->tm_sec,\
					*(int *)&detail_time.tv_usec,\
					(detail_time.tv_sec - pma_start_time));\
			if (level <= DEBUG_LEVEL) {\
				if (level <= ERROR){\
					pthread_mutex_lock(&mutex_debug);\
					fprintf(stderr, #level" %s [%s][%s][%d]: ",\
							debug_time, __FUNCTION__, __FILE__, __LINE__ );\
					fprintf(stderr, __VA_ARGS__);\
					fprintf(stderr, "\n");\
					fflush(stderr);\
					pthread_mutex_unlock(&mutex_debug);\
				} else {\
					pthread_mutex_lock(&mutex_debug);\
					fprintf(stdout, #level" %s [%s][%s][%d]: ",\
							debug_time, __FUNCTION__, __FILE__, __LINE__ );\
					fprintf(stdout, __VA_ARGS__);\
					fprintf(stdout, "\n");\
					fflush(stdout);\
					pthread_mutex_unlock(&mutex_debug);\
				}\
			}\
		}\
	}while(0)

struct list_head{
	struct list_head *prev;
	struct list_head *next;
};

#define list_add_rcu(head , a) \
	do {\
		(a)->next = head->next;\
		(a)->prev = (&head);\
		head->next->prev = (a);\
		head->next = (a);\
	} while(0)

#define list_del_rcu(a) \
	do{\
		(a)->prev->next=(a)->next;\
		(a)->next->prev=(a)->prev;\
	}while(0)
#define list_add_tail_rcu(head,a) \
	do{\
		(a)->prev=(head)->prev;\
		(a)->next=(head);\
		(head)->prev->next=(a);\
		(head)->prev=(a);\
	}while(0)

#define malloc_z(type) \
	({type* ptr = (type*)malloc(sizeof(type));\
	assert(ptr);\
	memset(ptr, 0, sizeof(type));\
	ptr;})

//this macro is used for simple linked list not using cr_list_head structure
#define FREE_LINK_LIST(head) \
	do{\
		typeof(head) ptr = head; \
		typeof(head) tmp;\
		while(ptr)\
		{\
			tmp = ptr->next;\
			free(ptr);\
			ptr = tmp;\
		}\
		head = NULL;\
	}while(0)

# if __BYTE_ORDER == __LITTLE_ENDIAN
#define pma_ntoh64(x) pma_bswap64(x)
#define pma_hton64(x) pma_bswap64(x)
# else
#define pma_ntoh64(x) (x)
#define pma_hton64(x) (x)
# endif

# define pma_bswap64(x) \
	((((x) & 0xFF00000000000000ull) >> 56)\
	 | (((x) & 0x00FF000000000000ull) >> 40)\
	 | (((x) & 0x0000FF0000000000ull) >> 24)\
	 | (((x) & 0x000000FF00000000ull) >> 8 )\
	 | (((x) & 0x00000000FF000000ull) << 8 )\
	 | (((x) & 0x0000000000FF0000ull) << 24)\
	 | (((x) & 0x000000000000FF00ull) << 40)\
	 | (((x) & 0x00000000000000FFull) << 56))

#define SLEEP(x) \
	do{\
		typeof (x)  _x = (x);\
		while(_x-- >0){\
			printf(".");fflush(stdout);\
			sleep(1);\
		}\
		printf("\n");\
	}while(0)
#endif
