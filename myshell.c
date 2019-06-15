#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "myshell.h"

void shell_mode(int argc, char** argv);
void command_execution_mode(int argc, char** argv);

int main(int argc, char** argv){
  if (argc != 1){
    command_execution_mode(argc, argv);
  }
  else{
    shell_mode(argc, argv);
  }
  return 0;
}
