#ifndef EVENT_H_
#     define EVENT_H_
typedef void (*funcptr)(void *args);
struct event_handlers_queue
{
	struct event_handler_item* next;
};
struct event_handler_item{
	struct event_handler_item* next;
	void* handler;
};

#define define_event_queue(name) struct event_handlers_queue name = {NULL};
#define declare_event_queue(name) extern struct event_handlers_queue name;
#define define_event_queue_list(name,size) struct event_handlers_queue name[size] = {NULL}; 
#define declare_event_queue_list(name,size) extern  struct event_handlers_queue name[size];
int add_event_handler(struct event_handlers_queue* queue, void* handler);
#endif /* EVENT_H_ */
