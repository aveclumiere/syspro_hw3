#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void shell_mode(int argc, char** argv){
  while(1){
    printf("$potatoshell> ");
    char* cmd;
    char* token;
    char** cmd_list;

    cmd = (char *)malloc(sizeof(char *));
    cmd_list = (char **)malloc(sizeof(char *) * 1024);

    fgets(cmd, 1024, stdin);
    int newline_index = (int) strlen(cmd);
    cmd[newline_index - 1] = '\0';
    token = strtok(cmd, " ");
    int i = 0;
    do {
      printf("token = %s\n", token);
      cmd_list[i++] = token;
    } while(token = strtok(NULL, " "));

    cmd_list[i] = NULL;
    execvp(cmd_list[0], cmd_list);
  }
}

void command_execution_mode(int argc, char** argv){
  char* cmd;
  char* token;
  char** cmd_list = (char **)malloc(sizeof(char *) * 1024);

  cmd = argv[2];
  int i = 0;
  token = strtok(cmd, " ");
  do {
    printf("token = %s\n", token);
    cmd_list[i++] = token;
  } while(token = strtok(NULL, " "));
  cmd_list[i] = NULL;
  execvp(cmd_list[0], cmd_list);
}
