#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/inotify.h>

#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )

int main( int argc, char **argv ) {
  int length, c, i = 0;
  int fd;
  FILE* in;
  int wd;
  char buffer[BUF_LEN];

  if(argc<2) {
	printf("syntax: pusher <absolute-pathname>\n");
	exit(0);
  }
  fd = inotify_init();

  if ( fd < 0 ) {
    perror( "inotify_init" );
  }

  in = fopen(argv[1], "r");
  if(in == NULL) {
     printf("file %s doesn't exist\n");
     exit(1);
  }
  fseek(in, 0, SEEK_END);

  while(1) {
    wd = inotify_add_watch( fd, argv[1], IN_CLOSE_WRITE);
    length = read( fd, buffer, BUF_LEN );  

    if ( length < 0 ) {
      perror( "read" );
    }  
    while((c = getc(in)) != EOF) {
      putchar(c);
    }
    fflush(stdout);
  }
}
