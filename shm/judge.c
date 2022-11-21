#include<stdio.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/types.h>

#include<unistd.h>

#include"com.h"

int main() {
	// 1. 计算用于标识共享内存的唯一KEY值
	// key_t ftok(const char *pathname, int proj_id);
	// ftok():利用两个参数和一套内置的算法，计算出唯一的key并返回。这个key值可以传递给内存参数，作为struct ipc_perm中唯一标识共享内存的key值
	// Paramater: *pathname -> 地址
	// 			   proj_id  -> 项目id(至少8位，不能为0)
	// return: key -> 成功计算出key值
	// 		    -1 -> 计算失败，从新输入参数再次计算   
	key_t k = ftok(PATHNAME, PROJ_ID);
	if(k == -1) {
		perror(	"ftok error" );
		return 1;
    } else {
		printf("ftok : %d\n", k);
	}


	int shmid = shmget(k, SIZE, IPC_CREAT | IPC_EXCL | 0666);
	if(shmid == -1) {
		perror("shmget error");
		return 1;
	}
	printf("shmid : %d\n", shmid);
	

	char *str = (char *)shmat(shmid, NULL, 0);

	sleep(20);
	int tmp = 0;
	while(tmp <= 60) {
		printf("%d\n", str[tmp]);
		tmp ++;
		sleep(1);	
	}

	shmdt(str);
	int sh = shmctl(shmid, IPC_RMID, NULL);
	if(sh == -1) {
		perror("shmctl");
		return 1;
	}
	return 0;
}
