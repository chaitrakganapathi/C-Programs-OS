#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <signal.h>
#include <sched.h>
#include<sys/prctl.h>



int pCount=0;
int exitrr=0;
int isdefault=1;
int mpid;
int timeQuantum=0;
int burstTime=0;
int execRobinProc;
int execfcfsProc;
int isrr=0;
int isfcfs=0;
int isProcessRunnning=0;
int runAll=0;
int procToRun;
int exitfcfs=0;
int childProcesses [10];
int processStatus [10];
int executingIndexrr=0;
int executingIndexfcfs;
void suspendAllChildren();
void display_shell();
void fifoScheduler();
void customRoundRobin();

void customSigHandler(int sig_num){
exitrr=1;
exitfcfs=1;
isProcessRunnning=0;
display_shell();

}

void ctrlCHandler(int sig_num){

kill(getpid(),SIGUSR1);
//suspendAllChildren();

if(isrr){
kill(execRobinProc,SIGSTOP);
processStatus[executingIndexrr]=0;
}
if(isfcfs){
kill(execfcfsProc,SIGSTOP);
processStatus[executingIndexfcfs]=0;
}
//suspendAllChildren();
signal(SIGINT,ctrlCHandler);
//printf("Cant terminate");
fflush(stdout);

}

void alarmHandler(int sig_num){
if(isrr){
if(processStatus[executingIndexrr]==1){
kill(execRobinProc,SIGSTOP);
processStatus[executingIndexrr]=0;
}
executingIndexrr++;
if(executingIndexrr>= pCount){
executingIndexrr=0;
}
customRoundRobin();
}else{
if(processStatus[executingIndexfcfs]==1){
kill(execfcfsProc,SIGSTOP);
processStatus[executingIndexfcfs]=0;
}
executingIndexfcfs++;
fifoScheduler();
}

}

void createProcesses(int processCount){

int pid[10];


int i;


const char* args2;
const char* args;
pid_t id;
printf("parent group %d",(int)getpgrp());
if((pCount+processCount)>10){
printf("Maximum of 10 processes are allowed.You can now create %d process(es). Or exit and rerun the program.\n",10-pCount);
}else{
for(i=0;i<processCount;i++){

char buffer[4];
snprintf(buffer,4,"p%d",i);
args=buffer;

	if((id=fork())==0){
	//id=getpid();
	prctl(PR_SET_NAME,buffer,NULL,NULL,NULL);
	/*if(setpgid(id,mpid)!=0){
	perror("setpgid error:");
	}*/
	//tcsetpgrp(STDIN_FILENO,getpid());
	//kill(id,SIGSTOP);
	execl("./up",args,NULL);
	}
	kill(id,SIGSTOP);
	childProcesses[pCount]=id;
	processStatus[pCount]=0;
	pCount++;
	
	
	
}
}
//tcsetpgrp(STDIN_FILENO,mpid);
}

void printProcesses(){

char sch[10];
if(isdefault){
//sch="Default";
strncpy(sch,"Default",10);
}else if(isrr){
//sch="rr";
strncpy(sch,"rr",10);
}else if(isfcfs){
strncpy(sch,"fcfs",10);
//sch="fcfs";
}
int k;
printf("Process#     PID    Scheduler    Status\n");
for(k=1;k<=pCount;k++){
int pid=childProcesses[k-1];

char sts[10];
if(processStatus[k-1]==0){
//sts="SUSPENDED";
strncpy(sts,"SUSPENDED",10);
}else if(processStatus[k-1]==1){
//sts="RUNNING";
strncpy(sts,"RUNNING",10);
}else if(processStatus[k-1]==2){
strncpy(sts,"TERMINATED",10);
//sts="TERMINATED";
}
printf("%d     %d    %s    %s\n",k-1,pid,sch,sts);

}
}


void display_shell(){
	static int first_time=1;
	if(first_time){
		const char* clr_ansi= "\e[1;1H\e[2J";
		write(STDOUT_FILENO,clr_ansi,11);
		first_time=0;
	}
	printf("chaitra>>");
}


void fifoScheduler(){
int wStatus;
int k;
printf("inside fifischeduler ");
int cout=1;
if(runAll){
if(burstTime>0){
if(executingIndexfcfs<pCount && !exitfcfs){
printf("inside fifo if");
int pStatus=processStatus[executingIndexfcfs];
while(pStatus==2){
executingIndexfcfs++;
pStatus=processStatus[executingIndexfcfs];
}
int fpid=childProcesses[executingIndexfcfs];
execfcfsProc=fpid;
kill(fpid,SIGCONT);
processStatus[executingIndexfcfs]=1;
alarm(burstTime);
printf("alarm fifo if");
}else{
display_shell();
}



}
else{
executingIndexfcfs=0;
int pStatus=processStatus[executingIndexfcfs];
while(pStatus==2){
executingIndexfcfs++;
pStatus=processStatus[executingIndexfcfs];
}
int fpid=childProcesses[executingIndexfcfs];
execfcfsProc=fpid;
kill(fpid,SIGCONT);
processStatus[executingIndexfcfs]=1;
}

}
else{
int pStatus=processStatus[procToRun];
if(pStatus==2){
printf("\nEntered process cannot be run as it is terminated.\n");
}else{
int procId=childProcesses[procToRun];
execfcfsProc=procId;
executingIndexfcfs=procToRun;
kill(procId,SIGCONT);
processStatus[procToRun]=1;
}

}


}

void customRoundRobin(){

if(runAll){
if(executingIndexrr<pCount && !exitrr){
int pStatus=processStatus[executingIndexrr];
while(pStatus==2){
executingIndexrr++;
pStatus=processStatus[executingIndexrr];
}
int fpid=childProcesses[executingIndexrr];
execRobinProc=fpid;
kill(fpid,SIGCONT);
processStatus[executingIndexrr]=1;
alarm(timeQuantum);


}
}
else{
int pStatus=processStatus[procToRun];
if(pStatus==2){
printf("\nEntered process cannot be run as it is terminated.\n");
}else{
int procId=childProcesses[procToRun];
execRobinProc=procId;
executingIndexrr=procToRun;
kill(procId,SIGCONT);
processStatus[procToRun]=1;

}

}
}



void resumeAllChildren(){
if(isdefault){
int k;
int wStatus;
for(k=1;k<=pCount;k++){
int fpid=childProcesses[k-1];
processStatus[k-1]=1;
kill(fpid,SIGCONT);
}

}

else if (isrr){
if(runAll && timeQuantum==0){
printf("\nSet the time quantum by entering command q #.\n");
} else{
customRoundRobin();
}
}
else if (isfcfs){
fifoScheduler();
}
}

void suspendAllChildren(){

int k;
int wStatus;
exitrr=1;
exitfcfs=1;
for(k=1;k<=pCount;k++){
int fpid=childProcesses[k-1];
int pStatus=processStatus[k-1];
if(pStatus==1){
kill(fpid,SIGSTOP);
processStatus[k-1]=0;
printf("Stopped in s all 3");
}

}
isProcessRunnning=0;

}
int main() {

	char  command[100], *params;
	//char*	envp[]={(char *) "PATH=.", 0};
	char strCmd[10];
	int processNum;
	char algo[10];
	signal(SIGINT,ctrlCHandler);
	signal(SIGALRM,alarmHandler);
	signal(SIGUSR1,customSigHandler);
	while(1){
		command[0]='\0';
		algo[0]='\0';
		strCmd[0]='\0';
		int index=0;
		for(index=0;index<pCount;index++){
		int pStatus=processStatus[index];
		if (pStatus==1){
		isProcessRunnning=1;
		break;
		}
		}
		if(index==pCount){
		isProcessRunnning=0;
		}
		display_shell();
		fgets(command,100, stdin);
		sscanf(command,"%s %s", strCmd, algo);
		//snprintf(algo,10,"%d",processNum);
		processNum= atoi(algo);
		if(strcmp(strCmd,"c")==0){
		createProcesses(processNum);
		} else if(strcmp(strCmd,"l")==0){
		printProcesses();
		} else if (strcmp(strCmd,"b")==0){
		burstTime=processNum;
		}
		else if(strcmp(strCmd,"t")==0){
		
		if(strcmp(algo,"fcfs")==0){
		isfcfs=1;
		isrr=0;
		isdefault=0;
		printf("fifo algo");
		}else if(strcmp(algo,"rr")==0){
		isfcfs=0;
		isdefault=0;
		isrr=1;
		}
		}
		else if (strcmp(strCmd,"r")==0){
		int j;
		for(j=0;j<pCount;j++){
		int pStatus=processStatus[j];
		if (pStatus==0 || pStatus==1){
		break;
		}
		}
		if(j==pCount){
		printf("\nAll processes are terminated. Cannot run any process. Please exit and restart the program.\n");
		}
		else if(isProcessRunnning){
		printf("\nPlease press ctrl+c to enter run command as a process is currently executing\n");
		}else if (!(isrr || isfcfs)){
		printf("\nPlease set the scheduler to run the process using t rr or t fcfs command.\n");
		} else if(pCount==0){
		
		printf("\nPlease create processes to run using c # command.\n");
		}
		else{
		isProcessRunnning=1;
		exitrr=0;
		exitfcfs=0;
		executingIndexfcfs=0;
		executingIndexrr=0;
		if(strcmp(algo,"all")==0){
		runAll=1;
		resumeAllChildren();
		
		}else{
		runAll=0;
		procToRun=processNum;
		resumeAllChildren();
		}
		}
		}
		else if (strcmp(strCmd,"s")==0){
		if(strcmp(algo,"all")==0){
		suspendAllChildren();
		}
		
		}
		else if (strcmp(strCmd,"x")==0){
		//kill(0,SIGKILL);
		exit(0);
		}
		else if (strcmp(strCmd,"k")==0){
		//kill(processNum,SIGQUIT);
		int wStatus;
		int killp=childProcesses[processNum];
		printf("term proc %d \n",killp);
		kill(killp, SIGTERM);
		kill(killp, SIGCONT);
		processStatus[processNum]=2;
		pid_t wp=waitpid(killp, &wStatus, 0);
		printf("term stat %d",wp);
		if(killp==wp){
		processStatus[processNum]=2;
		}
		
		if(isfcfs && runAll && (burstTime==0)){
		fifoScheduler();
		}
		
		}
		else if (strcmp(strCmd,"q")==0){
		timeQuantum=processNum;
		
		}
		else{
		 printf("Invalid command");
		
		}
		
		}
		
		
		
	return 0;
}

