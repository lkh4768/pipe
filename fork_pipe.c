#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/wait.h>

#define MSGSIZ 16

char *msg = "hello world";

int main()
{
	int p[2], childPid;
	char buf[MSGSIZ];

	if(pipe(p) == -1)
	{
		perror("pipe error");
		exit(1);
	}

	childPid = fork();

	if(childPid == 0)
	{
		close(p[1]);
		read(p[0], buf, MSGSIZ);
		printf("child rev : %s\n", buf);

		exit(0);
	}
	else if(childPid > 0)
	{
		close(p[0]);
		write(p[1], msg, MSGSIZ);

		wait(NULL);
	}
	else{
		perror("fork error");
		exit(1);
	}

	exit(0);
}
