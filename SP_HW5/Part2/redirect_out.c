/*
 * redirect_out.c   :   check for >
 */
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "shell.h"
#define STD_OUTPUT 1
#define STD_INPUT  0

/*
 * Look for ">" in myArgv, then redirect output to the file.
 * Returns 0 on success, sets errno and returns -1 on error.
 */
int redirect_out(char ** myArgv) {
	int i = 0;
  	int fd;

  	/* search forward for >
  	 * Fill in code. */
	while(myArgv[i]!=NULL){
	  	if ( strcmp(myArgv[i],">")==0 ) {	/* found ">" in vector. */
			/* 1) Open file.
			* 2) Redirect to use it for output.
			* 3) Cleanup / close unneeded file descriptors.
			* 4) Remove the ">" and the filename from myArgv.
			*
			* Fill in code. */
			if( (fd = open(myArgv[i+1],O_CREAT|O_WRONLY|O_SYNC))==-1){
				printf("open\n");
				exit(1);
			}
			dup2(fd,STD_OUTPUT);
			close(fd);
			while(myArgv[i]!=NULL){
				myArgv[i++]=NULL;
			}
			return 0;
		}else	i++;
	}
	printf("redirect_in, errno: %d",errno);
  	return -1;
}
