#include<stdio.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/types.h>

#include<stdlib.h>
#include<time.h>

#include"com.h"


int main() {
	key_t k = ftok(PATHNAME, PROJ_ID);
	if(k == -1) {
		perror(	"ftok error" );
		return 1;
    } else {
		printf("ftok : %d\n", k);
	}


	int shmid = shmget(k, SIZE, 0);
	if(shmid == -1) {
		perror("shmget error");
		return 1;
	}	
	printf("shmid : %d\n", shmid);


	char *str = (char *)shmat(shmid, NULL, 0);


	srand((unsigned)time( NULL ));
	for(int i = 0; i < 100; i++) {
		str[i] = rand()%3 + 1;
		printf("%d\n", str[i]);
		sleep(1);
	}


	shmdt(str);
	return 0;	
}
