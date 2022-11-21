#include<stdio.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/types.h>

#include<stdlib.h>
#include<pthread.h>
#include<semaphore.h>
#include<unistd.h>
#include<time.h>

#include"com.h"
#include"color.h"


//  
sem_t a;
sem_t b;
sem_t c;
sem_t d;

void *judge(void *arg){
	// 1.
	printf("<1> judge create shared memery\n");
	key_t k = ftok(PATHNAME, PROJ_ID);
	if(k == -1) {
		perror(	"ftok error" );
		return NULL;
    	}
	printf("judge.ftok : %d\n", k);



	// 2.
	int shmid = shmget(k, SIZE, IPC_CREAT | IPC_EXCL | 0666);
	if(shmid == -1) {
		perror("shmget error");
		return NULL;
	}
	printf("judge.shmid : %d\n", shmid);
	
	// 3.
	char *str = (char *)shmat(shmid, NULL, 0);
	
	printf("<2> judge has created shared memery, waiting for player_A data...\n");
	sleep(1);
	sem_post(&a);

	// 4.
	sem_wait(&b);
	printf("<7> judge has taken in charge, try to get player_A data\n");
	int player_A = str[0];
	
	printf("<8> player_A data has been successfully gotten, waiting for player_B data...\n");
	sleep(1);
	sem_post(&c);
	
	// 5.
	sem_wait(&d);
	printf("<13> judge has taken in charge, try to get player_B data\n");
	int player_B = str[1];

	printf("<14> player_B data has been successfully gotten, waiting for result...\n");
	sleep(1);


	char *map[] = {"Scissors", "Paper", "Stone"};
	printf("Player_A : %s  v.s. Player_B : %s\n",map[str[0]],map[str[1]]);		
	if((str[0] == 1 && str[1] == 2) || (str[0] == 2 && str[1] == 3) || (str[0] == 3 && str[1] == 0)) {
		printf(YELLOW "Congratulations: Player_A WIN!" NONE);
	} 
	else if((str[1] == 1 && str[0] == 2) || (str[1] == 2 && str[0] == 3) || (str[1] == 3 && str[0] == 0)){
		printf(YELLOW "Congratulations: Player_B WIN!\n" NONE);
	} else {
		printf(L_GREEN "Ops: TIE...\n" NONE);
	}
	
	
	// 6.
	shmdt(str);
	printf("<15> judge detached from shared memery\n");
	sleep(1);
	
	// 7.
	int sh = shmctl(shmid, IPC_RMID, NULL);
	if(sh == -1) {
		perror("shmctl");
		return NULL;
	}
	printf("<16> shared memery destroyed\n");
	sleep(0.5);
	return 0;
}



void *player_A(void *arg) {
	
	// 1.
	sem_wait(&a);
	
	printf("<3> player_A has token in charge of shared memery\n");
	key_t k = ftok(PATHNAME, PROJ_ID);
	if(k == -1) {
		perror(	"ftok error" );
		return NULL;
    	} 
	printf("player_A.ftok : %d\n", k);


	// 2.
	int shmid = shmget(k, SIZE, 0);
	if(shmid == -1) {
		perror("shmget error");
		return NULL;
	}	
	printf("player_A.shmid : %d\n", shmid);
	

	// 3.
	char *str = (char *)shmat(shmid, NULL, 0);
	printf("<4> player_A has connected to shared memery, shmid: %d\n", shmid);


	// 4.
	srand((unsigned)time( NULL ));
	int player_A = rand()%3 + 1;
	str[0] = player_A;
	printf("<5> player_A has writen in shared memery\n"); 
	
	
	char *map[] = {"Scissors", "Paper", "Stone"};
	printf(L_RED "player_A : %s\n" NONE, map[player_A]);
	
	// 5.
	shmdt(str);	
	printf("<6> player_A detached from shared memery, waiting for judge take in charge...\n");
	
	// 6.
	sem_post(&b);
	
	return 0;
}




void *player_B(void *arg) {

	// 1.
	sem_wait(&c);
	printf("<9> player_B has token in charge of shared memery\n");
	
	key_t k = ftok(PATHNAME, PROJ_ID);
	if(k == -1) {
		perror(	"ftok error" );
		return NULL;
    	} 
	printf("player_B.ftok : %d\n", k);


	// 2.
	int shmid = shmget(k, SIZE, 0);
	if(shmid == -1) {
		perror("shmget error");
		return NULL;
	}	
	printf("player_B.shmid : %d\n", shmid);


	// 3.
	char *str = (char *)shmat(shmid, NULL, 0);
	printf("<10> player_B has connected to shared memery, shmid : %d\n", shmid);

	// 4.
	srand((unsigned)time( NULL ));
	int player_B = rand()%3 + 1;
	str[1] = player_B;
	printf("<11> player_B has writen in shared memery\n"); 
	
	char *map[] = {"Scissors", "Paper", "Stone"};
	printf(L_BLUE "player_B : %s\n" NONE, map[player_B]);
	sleep(1);
	

	// 5.
	shmdt(str);
	printf("<12> player_B detached from shared memery, waiting for judge take in charge...\n");
	sleep(1);

	// 6.
	sem_post(&d);
	
	return 0;
}


int main() {
	// 1. 计算用于标识共享内存的唯一KEY值
	// key_t ftok(const char *pathname, int proj_id);
	// ftok():利用两个参数和一套内置的算法，计算出唯一的key并返回。这个key值可以传递给内存参数，作为struct ipc_perm中唯一标识共享内存的key值
	// Paramater: *pathname -> 地址
	// 			   proj_id  -> 项目id(至少8位，不能为0)
	// return: key -> 成功计算出key值
	// 		    -1 -> 计算失败，从新输入参数再次计算   
	
	// 1.
	sem_init(&a, 0, 0);
	sem_init(&b, 0, 0);
	sem_init(&c, 0, 0); 
	sem_init(&d, 0, 0); 
	
	pthread_t jid, aid, bid;
	pthread_create(&jid, NULL, judge, NULL);
	pthread_create(&aid, NULL, player_A, NULL);
	pthread_create(&bid, NULL, player_B, NULL);
	
	pthread_join(jid, NULL);
	pthread_join(aid, NULL);
	pthread_join(bid, NULL);
	
	sem_destroy(&a);
	sem_destroy(&b);
	sem_destroy(&c);
	sem_destroy(&d);		

	return 0;
}

