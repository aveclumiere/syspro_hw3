#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void shell_mode(int argc, char** argv){
  while(1){
    printf("$ ");
    char* cmd;
    char* token;
    char* filename;
    char** cmd_list;

    cmd = (char *)malloc(sizeof(char *));
    cmd_list = (char **)malloc(sizeof(char *) * 1024);

    fgets(cmd, 1024, stdin);
    int newline_index = (int) strlen(cmd);
    cmd[newline_index - 1] = '\0';

    if(has_output(cmd)){ // output redirection
      char* temp[2];
      int i = 0;
      char* tmp_tk = strtok(cmd, ">");
      do {
        temp[i++] = tmp_tk;
      } while(tmp_tk = strtok(NULL, ">"));
      temp[0][(int) strlen(temp[0]) - 1] = '\0';
      temp[1]++;

      token = strtok(temp[0], " ");
      i = 0;
      do {
        cmd_list[i++] = token;
      } while(token = strtok(NULL, " "));

      cmd_list[i] = NULL;

      filename = malloc(strlen(temp[1]) + 3);
      strcpy(filename, "./");
      strcat(filename, temp[1]);

      int child_pid = fork();
      int* status;
      if(child_pid == 0){
        int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 777);
        close(1); dup(fd); close(fd);
        if(execvp(cmd_list[0], cmd_list) < 0){
          panic("exec failed!");
        }
        close(fd);
        exit(0);
      }
      else{
        waitpid(child_pid, &status, WUNTRACED);
      }
    }
    else if(has_input(cmd)){ // input redirection
      char* temp[2];
      int i = 0;
      char* tmp_tk = strtok(cmd, "<");
      do {
        temp[i++] = tmp_tk;
      } while(tmp_tk = strtok(NULL, "<"));
      temp[0][(int) strlen(temp[0]) - 1] = '\0';
      temp[1]++;

      token = strtok(temp[0], " ");
      i = 0;
      do {
        cmd_list[i++] = token;
      } while(token = strtok(NULL, " "));

      cmd_list[i] = NULL;

      filename = malloc(strlen(temp[1]) + 3);
      strcpy(filename, "./");
      strcat(filename, temp[1]);

      int child_pid = fork();
      int* status;
      if(child_pid == 0){
        int fd = open(filename, O_RDONLY, 777);
        close(0); dup(fd); close(fd);
        if(execvp(cmd_list[0], cmd_list) < 0){
          panic("exec failed!");
        }
        close(fd);
        exit(0);
      }
      else{
        waitpid(child_pid, &status, WUNTRACED);
      }
    }
    else if(has_pipe(cmd)){ // pipeline execution
      char** cmd_list_1;
      char** cmd_list_2;
      cmd_list_1 = (char **)malloc(sizeof(char *) * 1024);
      cmd_list_2 = (char **)malloc(sizeof(char *) * 1024);

      char* temp[2];
      int i = 0;
      char* tmp_tk = strtok(cmd, "|");
      do {
        temp[i++] = tmp_tk;
      } while(tmp_tk = strtok(NULL, "|"));
      temp[0][(int) strlen(temp[0]) - 1] = '\0';
      temp[1]++;

      token = strtok(temp[0], " ");
      i = 0;
      do {
        cmd_list_1[i++] = token;
      } while(token = strtok(NULL, " "));

      cmd_list_1[i] = NULL;

      token = strtok(temp[1], " ");
      i = 0;
      do {
        cmd_list_2[i++] = token;
      } while(token = strtok(NULL, " "));

      cmd_list_2[i] = NULL;

      int fd[2];
      pipe(fd);

      int *status;

      int child_pid0 = fork();
      if (child_pid0 == 0){
        close(STDOUT_FILENO); dup(fd[1]);
        close(fd[1]); close(fd[0]);
        if(execvp(cmd_list_1[0], cmd_list_1) < 0){
          panic("exec failed!");
        }
      }
      else{
        int child_pid1 = fork();
        if (child_pid1 == 0){
          close(STDIN_FILENO);  dup(fd[0]);
          close(fd[0]); close(fd[1]);
          if(execvp(cmd_list_2[0], cmd_list_2) < 0){
            panic("exec failed!");
          }
          waitpid(child_pid0, &status, WUNTRACED);
          waitpid(child_pid1, &status, WUNTRACED);
        }
      }
    }
    else{ // normal single command
      token = strtok(cmd, " ");
      int i = 0;
      do {
        cmd_list[i++] = token;
      } while(token = strtok(NULL, " "));

      cmd_list[i] = NULL;

      int child_pid = fork();
      int* status;
      if(child_pid == 0){
        if(execvp(cmd_list[0], cmd_list) < 0){
          panic("exec failed!");
        }
        exit(0);
      }
      else{
        waitpid(child_pid, &status, WUNTRACED);
      }
    }
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
    cmd_list[i++] = token;
  } while(token = strtok(NULL, " "));
  cmd_list[i] = NULL;
  execvp(cmd_list[0], cmd_list);
}

int has_semicolon(char* cmd){
  int i;
  for(i = 0; i < strlen(cmd); i++){
    if (cmd[i] == ';'){
      return 1;
    }
  }
  return 0;
}

int has_output(char* cmd){
  int i;
  for(i = 0; i < strlen(cmd); i++){
    if (cmd[i] == '>'){
      return 1;
    }
  }
  return 0;
}

int has_input(char* cmd){
  int i;
  for(i = 0; i < strlen(cmd); i++){
    if (cmd[i] == '<'){
      return 1;
    }
  }
  return 0;
}

int has_pipe(char* cmd){
  int i;
  for(i = 0; i < strlen(cmd); i++){
    if (cmd[i] == '|'){
      return 1;
    }
  }
  return 0;
}
