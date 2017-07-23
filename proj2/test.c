#include <stdio.h> //for i/o
#include <stdlib.h>
#include <string.h> //for strcat, strcpy and for memcpy
#include <limits.h> //int_max, int_min
#include <math.h> //pow function, for file_size
#include <time.h> //for timing
#include <unistd.h> //interact w/ processes
#include <sys/types.h> //for pid_t

#include <signal.h> // for sigaction



void sighandler( int signum, siginfo_t *info, void *ptr){
	printf("Received signal %d\n", signum);
  printf("Signal originates from process %lu\n", (unsigned long)info->si_pid);
  printf("Signal value is %d\n", info->si_value.sival_int);
  
}

int main(){
  struct sigaction act;
  union sigval value;

  act.sa_sigaction = sighandler;
  act.sa_flags = SA_SIGINFO;
  sigset_t mask;
  sigemptyset(&mask);
  sigaddset(&mask, SIGRTMIN);
  sigaddset(&mask, SIGRTMIN+1);
  sigaddset(&mask, SIGRTMIN+2);
  sigprocmask(SIG_BLOCK, &mask, NULL);
  sigemptyset(&act.sa_mask);
  sigaction(SIGRTMIN, &act, NULL);
  sigaction(SIGRTMIN+1, &act, NULL);
  sigaction(SIGRTMIN+2, &act, NULL);

  siginfo_t info;
  
	int pid = fork();
  if(pid==0){
    int sum = 10;
    int min = -20;
    int max = 30;
    value.sival_int = max;
    sigqueue(getppid(),SIGRTMIN,value);

    //sleep(2);
    value.sival_int = min;
    sigqueue(getppid(),SIGRTMIN+1,value);
    
    //sleep(2);
    value.sival_int = sum;
    sigqueue(getppid(),SIGRTMIN+2,value);
    exit(1);
  }
  else{
    //read in signal
      //printf("got sum of %d, min of %d, max of %d",sum,min,max);
    //sleep(10);
    //sleep(10);
    //sigwaitinfo()
    for (int i = 0; i<3; i++){
        if (sigwaitinfo(&mask, &info) == -1) {
          perror("sigwaitinfo() failed");
        }else{
          int num = info.si_signo;
          int val = info.si_value.sival_int;
          printf("Signal value is %d with signo %d\n", val, num);
        }
    }

    printf("I handled the signal!\n");
  }

  
  
  return 0;
}