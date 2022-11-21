#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdio.h>
#include "color.h"

typedef int que_t[10];
que_t q;
sem_t p;				//可生产的数量
sem_t c;				//可消费的数量

//打印缓冲区
void printBuf() {
	printf(YELLOW "Buffer:" NONE);
	for(int i = 0; i < 10; i++) {
		printf(YELLOW " %d" NONE, q[i]);
	}
	printf("\n");
}
//生产者线程
void *p_func(void *arg){
	int t = 0;
	while(1){
		sem_wait(&p);

		printf(RED "Producer is producing...\n" NONE);
		q[t] = rand() % 1000 + 1;
		printf(RED "p:%d\n" NONE, q[t]);
		t = (t + 1) % 10;
		printBuf();

		sem_post(&c);
		sleep(rand()%4+1);
	}
	return NULL;
}

//消费者线程
void *c_func(void *arg){
	int h = 0;
	int tmp;
	while(1){
		sem_wait(&c);

		printf(L_GREEN "Consumer is consuming...\n" NONE);
		tmp = q[h];
		q[h] = -1;
		h = (h + 1) % 10;
		sem_post(&p);
		printf(L_GREEN "c:%d\n" NONE, tmp);
		printBuf();
		
		sleep(rand()%4+1);
	}
	return 0;
}

int main(void){
	//初始化信号量的值
	sem_init(&p, 0, 10);
	sem_init(&c, 0, 0);
	srand(time(NULL));
	//创建两个线程，一个用于生产者，一个用于消费者
	pthread_t tid, cid;
	pthread_create(&tid, NULL, p_func, NULL);
	pthread_create(&cid, NULL, c_func, NULL);
	//阻塞等待线程汇合
	pthread_join(tid, NULL);
	pthread_join(cid, NULL);
	//销毁信号量
	sem_destroy(&p);
	sem_destroy(&c);
	return 0;
}

