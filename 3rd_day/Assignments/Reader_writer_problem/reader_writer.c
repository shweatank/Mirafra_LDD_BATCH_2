#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>

int shared_data =0;
pthread_mutex_t mutex1;
pthread_mutex_t  mutex2;

void *readers(void *args)
{
	int id = *(int *)args;

	pthread_mutex_lock(&mutex1);
	printf("Reader thread %d is reading the shared data value %d\n", id, shared_data);
	pthread_mutex_unlock(&mutex1);
}

void *writers(void *args)
{
	int id = *(int *)args;

	pthread_mutex_lock(&mutex2);
	
	shared_data += 1;
	printf("Writer thred %d is writing and modifying the shared data value %d\n", id, shared_data);
	pthread_mutex_unlock(&mutex2);
}
int main()
{
	pthread_t reader[3],writer[2];
	int reader_id[3] = {1,2,3};
	int writer_id[2] = {1,2};
	
	pthread_mutex_init(&mutex1, NULL);
	pthread_mutex_init(&mutex2, NULL);

	for(int i=0;i<3;i++)
	{
		reader_id[i] = i+1;
		pthread_create(&reader[i], NULL, readers,&reader_id[i]);

	}
	for(int i=0;i<2;i++)
	{
		writer_id[i] = i+1;
		pthread_create(&writer[i], NULL, writers,&writer_id[i]);
	}

	for(int i=0;i<3;i++)
	{
		pthread_join(reader[i], NULL);
	}
	for(int i=0;i<2;i++)
	{
		pthread_join(writer[i], NULL);
	}
	return 0;
}

