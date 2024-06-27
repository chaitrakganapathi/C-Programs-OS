#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/time.h>
#include<signal.h>
void ctrlCHandler(int sig_num){

//kill(getpid(),SIGSTOP);
//printf("Cant terminate");
//fflush(stdout);

}

void termHandler(int sig_num){
printf("in handler");
kill(getpid(),SIGSTOP);
}

int main() {

int countIteration=0;
//signal(SIGINT,ctrlCHandler);
//signal(SIGTERM, termHandler);
signal(SIGINT, SIG_IGN);

/*if(setpgid(getpid(),getpid())!=0){
	perror("setpgid error:");
	}
tcsetpgrp(STDIN_FILENO,getpid());
*/

while(1){
/*if(countIteration==5){
break;
}*/

printf("\nProcess %d at iteration %d group %d \n", getpid(),countIteration, (int)getpgrp());
countIteration++;

sleep(1);


}

return 0;

}
