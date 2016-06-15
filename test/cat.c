#include "syscall.h"

int
main(int argc, int **argv)
{
  if (argc < 1)
    Exit(0);
    
  OpenFileId currfd;
  int i, size_read;
  char c;
  for (i = 0; i < argc; i++)
  {
    if ((currfd = Open(argv[i])) == -1)
      Exit(-1);
      
    do {
      size_read = Read(&c, 1, currfd);
      Write(&c, size_read, ConsoleOutput);
    } while (size_read > 0);
  }
  
  Exit(0);    
}
