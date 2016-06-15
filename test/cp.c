#include "syscall.h"

int
main(int argc, char **argv)
{   
  if (argc < 2)
  {
    Write("Faltan argumentos\n", 19, ConsoleOutput);
    Exit(-1);
  }
  
  OpenFileId file1, file2;
  
  if ((file1 = Open(argv[0])) == -1)
  {
    Write("No se pudo abrir el primer archivo\n", 35, ConsoleOutput);
    Exit(-1);
  }
  
  Create(argv[1]);
  
  if ((file2 = Open(argv[1])) == -1)
  {
    Write("No se pudo abrir el segundo archivo\n", 36, ConsoleOutput);
    Exit(-1);
  }
  
  int size_read;
  char c;
  do {
    size_read = Read(&c, 1, file1);
    
    if (size_read == -1)
      Exit(-1);
    
    Write(&c, size_read, file2);
  } while (size_read > 0);
  
  Close(file1);
  Close(file2);
  Exit(0);
    
}
