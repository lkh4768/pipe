#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>

#define MSGSIZ 16

char *msg = "hello world";

int main()
{
	char buf[MSGSIZ];
	int p[2], j;

	if(pipe(p) == -1)
	{
		perror("pipe call");
		exit(1);
	}

	for(j = 0; j < 3; j++)
		write(p[1], msg, MSGSIZ);

	for(j = 0; j < 3; j++)
	{
		read(p[0], buf, MSGSIZ);
		printf("%s\n", buf);
	}

	exit(0);
}
