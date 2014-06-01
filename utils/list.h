#ifndef LIST_H
#define LIST_H
#define TRUE 1
#define FALSE 0

#define CR_OFFSETOF(type, member) ((size_t)&((type*)0)->member)
//these macro & functions below is used for complex linked list using cr_list_head structure
struct cr_list_head{
	struct cr_list_head* next;
	struct cr_list_head* prev;
};
#define CR_LIST_ENTRY(ptr, type, member) ({\
		type* result = NULL;\
		if(ptr)\
		result = (type*)((char*)(ptr) - CR_OFFSETOF(type, member));\
		result;})
#define cr_init_list_head(ptr) do{\
	(ptr)->next = (ptr); (ptr)->prev = (ptr);\
}while(0)
void cr_list_add(struct cr_list_head* new, struct cr_list_head *head);
void cr_list_add_tail(struct cr_list_head* new, struct cr_list_head *head);
typedef int (*fetch_keyword)(struct cr_list_head* ptr);
void cr_list_add_in_order(struct cr_list_head* new, struct cr_list_head* head, int key, fetch_keyword v);
int cr_list_empty(const struct cr_list_head* head);
int cr_list_del(struct cr_list_head* entry);
#define cr_list_entrys_free(head, type) \
	do{\
		struct cr_list_head* ptr, *tmp;\
		for(ptr = (head)->next; ptr != (head); ptr = tmp)\
		{\
			type* entry = CR_LIST_ENTRY(ptr, type, ptrs);\
			tmp = ptr->next;\
			cr_list_del(ptr);\
			free(entry);\
		}\
		assert(cr_list_empty(head));\
	}while(0)

#define cr_list_entrys_free2(head, type, ptrs) \
	do{\
		struct cr_list_head* ptr, *tmp;\
		for(ptr = (head)->next; ptr != (head); ptr = tmp)\
		{\
			type* entry = CR_LIST_ENTRY(ptr, type, ptrs);\
			tmp = ptr->next;\
			cr_list_del(ptr);\
			free(entry);\
		}\
		assert(cr_list_empty(head));\
	}while(0)

#define cr_stack_init(stack) cr_init_list_head(stack)
void cr_stack_push(void* item, struct cr_list_head* stack);
void* cr_stack_pop(struct cr_list_head* stack);
#define cr_stack_empty(stack) cr_list_empty(stack)
#endif
