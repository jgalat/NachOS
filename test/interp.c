#include "syscall.h"

int str_len (char *str)
{
  int len = 0;
  while (str[len] != '\0')
    len++;
  return len;
}


int copy_until (char *dst, char *src)
{
  int ptr = 0;
  while ( src[ptr] != ' ' && src[ptr] != '\n')
  {
    dst[ptr] = src[ptr];
    ptr++;
  }
  dst[ptr] = '\0';
  return ptr;
}

int drop_until (char *src)
{
  int ptr = 0;
  while (src[ptr] == ' ' || src[ptr] == '\n')
    ptr++;
  return ptr;
}

int explode (char *buffer, char **args)
{
  int ptr = 0;
  int i = 0;
  do
  {
    ptr += copy_until(args[i], buffer + ptr);
    ptr += drop_until(buffer + ptr);
    i++;
  } while(buffer[ptr-1] != '\n');
  return i;
}

void mem_set(char *buff, char ch, int size)
{
  int i;
  for(i=0; i < size; i++)
    buff[i] = ch;
}

int main (int argc, char **argv)
{
  SpaceId child;
  char buffer[352];
  char *args[MAX_ARGS+1];
  char superblock[352];
  char prompt = '>';
  int i, argcount;
  

  
  for (;;)
  {
    Write(&prompt, 1, ConsoleOutput);
    i = 0;
    
    mem_set(superblock, '0', 352);

    do {
      Read(&buffer[i], 1, ConsoleInput);
    } while (buffer[i++] != '\n');
    
    for (i = 0; i < MAX_ARGS + 1; i++)
      args[i] = &superblock[32 * i]; /* Se me quejaba el compilador sino con args[MAX_ARGS][32]; */
    
    argcount = explode(buffer, args);
    
    args[argcount] = 0;
    
    if(*args[0] == '&')
    {
      Exec((args[0]+1), &args[1]);
    } else {
      child = Exec(*args, &args[1]);
      Join(child);
    }
  }
}
