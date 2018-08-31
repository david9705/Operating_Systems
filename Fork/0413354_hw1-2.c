#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/time.h>

unsigned int* createMatrix(int dim);
//void process(shm_content *);

int main(){
	int dim, shm_id;
	printf("Dimension:");

	while(scanf("%d", &dim)!=EOF){
		//create a shared memory then fork() to create process
		unsigned int check_sum = 0, o = 0;
		unsigned int *m = createMatrix(dim);
		unsigned int *shm_addr;

		struct timeval start, end;
		shm_id = shmget(IPC_PRIVATE, dim*dim*sizeof(unsigned int), IPC_CREAT|0666);
		if(shm_id < 0){ printf("shmget error\n"); return -1; }

		int status = 0;
		pid_t pid,wpid;

		int num_task = 1;
		for(num_task = 1; num_task<5; num_task++){
            if(num_task == 2 || num_task == 3) continue ;
			int i, row, col, n, k;
			gettimeofday(&start, 0);
			for(i=0; i<num_task; i++){
				if((pid = fork())==0){//child process
					//printf("this is child process.\n");
					int begin = i*(dim/num_task);
					int end = begin+(dim/num_task);
					if(i==num_task-1){ end = dim; }
					//attach shared memory
					shm_addr =(unsigned int*) shmat(shm_id, NULL, 0);
					if(shm_addr < 0){ printf("shmat allocate error\n"); exit(EXIT_FAILURE); }

					for(row=begin; row<end; row++){
						for(col=0; col<dim; col++){
							shm_addr[row*dim+col]=0;
							for(n=0; n<dim; n++){
								shm_addr[row*dim+col] += m[row*dim+n]*m[n*dim+col];
							}
						}
					}
					//printf("child ended.\n");
					shmdt(shm_addr);
					exit(0);
				}
			}
			while((wpid = wait(&status))!=-1){;}

			//Parent Process
			gettimeofday(&end, 0);

			int sec=end.tv_sec-start.tv_sec;
			int usec=end.tv_usec=start.tv_usec;

			shm_addr =( unsigned int *) shmat(shm_id, NULL, 0);
			if((int)shm_addr==-1){ printf("shmat error\n"); return -1; }

			for(o=0; o<dim*dim; o++){check_sum+=shm_addr[o];}
			printf("%d-process, ", num_task);
			printf("check sum = %u\n", check_sum);
			printf("elapsed %f ms\n", sec+(usec/1000000.0));

			check_sum = 0;
			shmdt(shm_addr);
		}
		shmctl(shm_id, IPC_RMID, NULL) ;
		printf("Dimension:");
	}

	return 0;
}

unsigned int* createMatrix(int dim){
	unsigned int i, j;
	unsigned int * matrix = malloc(sizeof(unsigned int)*dim*dim);
	for(i=0; i<dim*dim; i++){ matrix[i] = i; }
	return matrix;
}
