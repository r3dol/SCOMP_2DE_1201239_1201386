#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include <string.h>

#define WORD_SIZE 20
#define ARRAY_SIZE 100000

struct info{
    int numero;
    char frase[WORD_SIZE];
};

typedef struct{
    struct info arr[ARRAY_SIZE];
} enviar;

int main(int argc, char* argv[]) {

    int size = sizeof(struct info); 
	enviar *infoPartilhada;

    time_t start = time(NULL);

	int fd = shm_open("/ex3",O_CREAT|O_EXCL|O_RDWR,S_IRUSR|S_IWUSR); 

    if(fd == -1){
		perror("Creating or opening shared memory failure");
		shm_unlink("/ex3");
		return 1;
	}

    if(ftruncate(fd,size) == -1) {
		perror("Truncating shared memory failure");
		return 2;
	}

    infoPartilhada = (enviar*) mmap(NULL, size,PROT_READ|PROT_WRITE,MAP_SHARED, fd, 0);

    for(int i = 0; i < ARRAY_SIZE; i++){
        struct info informacao;
        informacao.numero = i;
        strcpy(informacao.frase, "ISEP-SCOMP 2020");
        infoPartilhada->arr[i] = informacao;
    }

    pid_t pid = fork();

    if(pid == -1){
        perror("Fork Falhou");
        return 3;

    } else if (pid == 0){

        enviar *infoRecebida;
        infoRecebida = (enviar*) mmap(NULL, size,PROT_READ|PROT_WRITE,MAP_SHARED, fd, 0);

        if (munmap(infoPartilhada, size) < 0) {
            printf("Error at munmap()!\n");
            return 4;
        }

        if (close(fd) < 0) {
            printf("Error at close()!\n");
            return 5;
        }

    
        if (shm_unlink("/ex3") < 0) {
            printf("Error at shm_unlink()!\n");
            return 6;
        }

        exit(1);
        
    }

    wait(NULL);    

    time_t end = time(NULL);
    int total = end - start;
    printf("Tempo de execução memória partilhada: %ds\n", total);

    if (munmap((void *)infoPartilhada, size) < 0) {
        printf("Error at munmap()!\n");
        return 3;
    }
    
    if (close(fd) < 0) {
        printf("Error at close()!\n");
        return 4;
    }


    time_t inicio = time(NULL);

    int pp[2];

    if(pipe(pp) == -1){
        perror("There was an error when creating the Pipe.\n");
        return -1;
    }

    pid_t p = fork();

    if(p == -1){
        perror("Fork Falhou");
        return 3;

    } else if (p == 0){

        close(pp[1]);

        enviar *infoRecebida;

        read(pp[0], &infoRecebida, sizeof(infoRecebida));

        close(pp[0]);
        exit(1);
    }

    time_t stop = time(NULL);
    int dif = inicio - stop;
    printf("Tempo de execução pipes: %ds\n", dif);



    return 0;
}
