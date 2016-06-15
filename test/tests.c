#include "syscall.h"

int main()
{
  
  Write("---tests---\n", 12, ConsoleOutput);
  
  Write("filetest...", 11, ConsoleOutput);
  Exec("test/filetest", 0);
  Write("done\n", 5, ConsoleOutput);
  
  Write("sort...", 7, ConsoleOutput);
  Exec("test/sort", 0);
  Write("done\n", 5, ConsoleOutput);
  
  Write("matmult...", 10, ConsoleOutput);
  Exec("test/matmult", 0);
  Write("done\n", 5, ConsoleOutput);
  
  Write("halt...", 7, ConsoleOutput);
  Exec("test/halt", 0);
  
}
