#include <unistd.h>     /* Symbolic Constants */
#include <sys/types.h>  /* Primitive System Data Types */
#include <errno.h>      /* Errors */
#include <stdio.h>      /* Input/Output */
#include <sys/wait.h>   /* Wait for Process Termination */
#include <stdlib.h>     /* General Utilities */
#include <signal.h>

int main()
{
    pid_t pid , S = -1,s = -1, T = -1, t = -1;
    int i  , temp , x;

    printf("Main Process ID : %d\n\n" , getpid()) ;
    pid = fork() ;

    if(pid == 0)  //child
    {
        S = fork() ;
        if(S > 0 && T == -1) T = fork() ;

        if(S > 0 && T > 0)
        {
            printf("Fork 2. I'm the child %d, my parent is %d.\n", S, getpid());
			printf("Fork 2. I'm the child %d, my parent is %d.\n", T, getpid());
			usleep(90) ;
			wait(0);
			wait(0);
        }
        else if(S == 0 && T == -1)
        {
            s = fork();
            usleep(1000) ;
            temp = s ;
            x = getpid() ;
			if(s < 0)	printf("Error!! \n");
			else if(s > 0)	printf("Fork 3. I'm the child %d, my parent is %d.\n", s , getpid());
        }
        else if(T == 0 && S > 0)
        {
            t = fork();
            usleep(1000) ;
			if(t < 0)	printf("Error!! \n");
			else if(t > 0)	printf("Fork 3. I'm the child %d, my parent is %d.\n", t , getpid());
			//printf("Fork 3. I'm the child %d, my parent is %d.\n", temp , x);

        }
        else
        {
            printf("Error!! \n") ;
        }

    }
    else if(pid > 0)
    {
        printf("Fork 1. I'm the child %d, my parent is %d.\n",pid ,getpid());
		wait(0);
    }
    else
    {
        printf("Error!! \n") ;
    }

    return 0 ;
}
