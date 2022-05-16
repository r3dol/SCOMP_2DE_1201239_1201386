/********************************************************************************

Optou-se que o valor inicial do semáforo que faria a sincronização entre os dois
processos fosse 0, devido ao processo pai só poder realizar a sua operação depois
do filho.

O filho desbloqueia o semáforo depois de finalizada a sua operação, e só depois é
que o pai realiza o que lhe foi atribuído. 

*********************************************************************************/


#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <semaphore.h>

int main(){

    sem_t *semWriter;
 
    if ((semWriter = sem_open("/ex05", O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED ){
        perror("Error in sem_open()");
        exit(1);
    }

    pid_t pid = fork();

    if(pid == 0){		
		printf("I'm the child!\n");
		if (sem_post(semWriter) == -1) {
            perror("Error at sem_post().");
            exit(2);
        }
        exit(0);
	}else if (pid > 0) {	
		if (sem_wait(semWriter) == -1) {
            perror("Error at sem_wait().");
            exit(3);
        }
		printf("I'm the father!\n");
    } else {
        perror("Fork Falhou");
        exit(4);
    }

    wait(NULL);

    if (sem_unlink("/ex05") == -1 ){
        perror("Error in sem_unlink()");
        exit(5);
    }

    return 0;
}