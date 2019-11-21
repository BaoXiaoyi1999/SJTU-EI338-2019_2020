/*buffer.h*/
typedef int buffer_item;
#define BUFFER_SIZE 5
#define MAX_SLEEP_TIME 3
int insert_item(buffer_item item);
int remove_item(buffer_item *item);
void *producer(void *param);
void *consumer(void *param);
void init_buffer();
void destroy_buffer();
