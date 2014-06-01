#include "event.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int add_event_handler(struct event_handlers_queue* queue, void* handler)
{
	assert(queue != NULL);
	assert(handler != NULL);

	struct event_handler_item* item = (struct event_handler_item*)malloc(sizeof(struct event_handler_item));
	if(!item)
		return -1;
	item->handler = handler;
	item->next = queue->next;
	queue->next = item;
	return 0;
}

int traverse_event(struct event_handlers_queue* queue, void *args)
{
    struct event_handler_item *item = NULL;
    for( item = queue->next ; item; item = item->next)
    {
        funcptr f = (funcptr)item->handler;
        f(args);
    }
}
