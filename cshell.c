#include <sys/wait.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define stack_max_buff 32
#define history_max_buff 32
#define alias_max_buff 32
#define wild_card_buff 100

char *stack[stack_max_buff];
int stack_number = 0;
int history_number = 0;  
int alias_number = 0;
char history_command[history_max_buff][10];
char prompt_name[15] = "command";
struct Alias{
	int index;
	char new_name[10];
	char original_name[10];
}alias[alias_max_buff];


//char *history[history_max_buff];    // declare storage for history
//int history_number = 0;		// history counter

/* 
  Function Declarations for builtin shell commands:
 */
int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);
int lsh_pushd(char **args);
int lsh_popd(char **args);
int lsh_dirs(char **args);
int lsh_history(char **args);
int lsh_prompt(char **args);
int lsh_alias(char **args);
int lsh_unalias(char **args);
int lsh_push(char **args);

/*
  List of builtin commands, followed by their corresponding functions.
 */
char *builtin_str[] = {
  "cd",
  "help",
  "exit",
  "dirs",
  "history",
  "prompt",
  "pushd",
  "popd",
  "alias",
  "unalias",
  "push"
};

int (*builtin_func[]) (char **) = {
  &lsh_cd,
  &lsh_help,
  &lsh_exit,
  &lsh_dirs,
  &lsh_history,
  &lsh_prompt,
  &lsh_pushd,
  &lsh_popd,
  &lsh_alias,
  &lsh_unalias,
  &lsh_push
  
};

int lsh_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

/*
  Builtin function implementations.
*/

/**
   @brief Bultin command: change directory.
   @param args List of args.  args[0] is "cd".  args[1] is the directory.
   @return Always returns 1, to continue executing.
 */
 

int lsh_cd(char **args)
{
  if (args[1] == NULL) {
  	//printf("NULL\n");
  	chdir("/home/");
    //fprintf(stderr, "lsh: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("lsh");
    }
  }
  return 1;
}

/**
   @brief Builtin command: print help.
   @param args List of args.  Not examined.
   @return Always returns 1, to continue executing.
 */
int lsh_help(char **args)
{
  int i;
  printf("Stephen Brennan's LSH\n");
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built in:\n");

  for (i = 0; i < lsh_num_builtins(); i++) {
    printf("  %s\n", builtin_str[i]);
  }

  printf("Use the man command for information on other programs.\n");
  return 1;
}

  /* dirs functopn */
int lsh_dirs(char **args){
	int i=0;
	while(stack[i]!=NULL){
	printf("%d  %s\n",i,stack[i]);
	i++;
	}
	printf("\n");
	return 1;
}

  /* pushd functopn */
int lsh_pushd(char **args){
	//printf("***** pushd function *****\n");
	stack[stack_number] = getcwd(NULL,0);
	//printf("dir has been saved, stack[%d] = %s\n",stack_number,stack[stack_number]);
	stack_number ++;
	return 1;
}
 
  /* popd functopn */
int lsh_popd(char **args){
	//printf("***** popd function *****\n");
	stack_number--;
	stack[stack_number] = NULL;
	//printf("the dir has been removed\n");
}

int lsh_push(char **args){
	//printf("push func!\n");
	//printf("push to %s\n",stack[atoi(args[1])]);
	chdir(stack[atoi(args[1])]);
	return 1;
}

	/* alias function*/
int lsh_alias(char **args){
	//printf("before: args[1] = %s, args[2] = %s\n",args[1],args[2]);
	if(args[1]==NULL){
			//printf("All alias commands : \n");	
			for(int i=0;i<alias_number;i++){
				//printf("%s  %s\n",alias[i].new_name,alias[i].original_name);
			}	
			return 1;
	}
	//printf("***** alias function ****\n");
	alias[alias_number].index = alias_number;
	strcpy(alias[alias_number].new_name,args[1]);
	strcpy(alias[alias_number].original_name,args[2]);
	//printf("new command name has been saved!\n");
	//printf("alias[%d]: %s = %s\n",alias[alias_number].index,alias[alias_number].new_name,alias[alias_number].original_name);
	alias_number++;
	//printf("after free : args[1] = %d, args[2] = %d\n",args[1],args[2]);
	//printf("alias number = %d\n",alias_number);
	return 1;
}

	/* unalias function */
int lsh_unalias(char **args){
	for(int i=0;i<alias_number;i++){
		if(strcmp(args[1],alias[i].new_name)==0){
			strcpy(alias[i].new_name,"");
			//printf("alias command %s has been romoved\n",args[1]);
		}
	}
}


 /* prompt function */
int lsh_prompt(char **args){
	char *default_prompt;
	default_prompt = "command";
	if(*(args+1)=='\0'){*(args+1)=default_prompt;}//strcpy(args[1],"command");
	strcpy(prompt_name,args[1]);
	return 1;
}


int lsh_history(char **args){
	print_history();
	return 1;
}

int print_history(){
	for(int i=0;i<history_number;i++) printf("%s\n",history_command[i]);
}


/**
   @brief Builtin command: exit.
   @param args List of args.  Not examined.
   @return Always returns 0, to terminate execution.
 */
int lsh_exit(char **args)
{
  return 0;
}

/**
  @brief Launch a program and wait for it to terminate.
  @param args Null terminated list of arguments (including program).
  @return Always returns 1, to continue execution.
 */
int lsh_launch(char **args)
{
  pid_t pid;
  int status;

  pid = fork();
  if (pid == 0) {
    // Child process
    if (execvp(args[0], args) == -1) {
      perror("lsh");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    // Error forking
    perror("lsh");
  } else {
    // Parent process
    do {
      waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

/**
   @brief Execute shell built-in or launch program.
   @param args Null terminated list of arguments.
   @return 1 if the shell should continue running, 0 if it should terminate
 */
int lsh_execute(char **args)
{
  int i;

  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }

  for (i = 0; i < lsh_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
    	//printf("find builtin function : %d\n",i);
      return (*builtin_func[i])(args);
    }
  }

  return lsh_launch(args);
}

#define LSH_RL_BUFSIZE 1024
/**
   @brief Read a line of input from stdin.
   @return The line from stdin.
 */
char *lsh_read_line(void)
{
  int bufsize = LSH_RL_BUFSIZE;
  int position = 0;
  char *buffer = malloc(sizeof(char) * bufsize);
  int c;

  if (!buffer) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  while (1) {
    // Read a character
    c = getchar();

    if (c == EOF) {
      exit(EXIT_SUCCESS);
    } else if (c == '\n') {
      buffer[position] = '\0';
      return buffer;
    } else {
      buffer[position] = c;
    }
    position++;

    // If we have exceeded the buffer, reallocate.
    if (position >= bufsize) {
      bufsize += LSH_RL_BUFSIZE;
      buffer = realloc(buffer, bufsize);
      if (!buffer) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}



#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"
/**
   @brief Split a line into tokens (very naively).
   @param line The line.
   @return Null-terminated array of tokens.
 */
char **lsh_split_line(char *line)
{
  int bufsize = LSH_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token, **tokens_backup;

  if (!tokens) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, LSH_TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize += LSH_TOK_BUFSIZE;
      tokens_backup = tokens;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
		free(tokens_backup);
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, LSH_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

char *get_wildcard(char **args)
{
  struct stat    filestat;
  struct dirent *directory;
  DIR           *dp;
  static char str[wild_card_buff];

  dp = opendir(".");

  while((directory=readdir(dp))!=NULL){
    if(!strcmp(directory->d_name, ".") ||
       !strcmp(directory->d_name, ".."))
      continue;
    if(stat(directory->d_name,&filestat)==-1){
      perror("main");
      exit(1);
    }else{
      strcat(str,directory->d_name);
      strcat(str," ");
    }
  }
	//printf("%s\n",str);
  closedir(dp);
  return str;
   exit(0);
}



char *search_command_from_history(char *line){
	int i;
	char str[10];
	char *command;
	for(i=0;i<strlen(line);i++) str[i] = *(line+i+1);
	//printf("history command matching...\n");
	//printf("input line = %s\nstr = %s\n",line,str);
	//printf("history number = %d\n",history_number);
	for(i=history_number-1;i>=0;i--){
		//printf("history[%d] = %s\n",i,history_command[i]);
		if(strncmp(str,history_command[i],strlen(str))==0){
			 //strcpy(command,history_command[i]);
			 command = history_command[i];
			 //printf("matched to history[%d] = %s,command = %s\n",i,history_command[i],command);
			 break;
		}
	}
	if(command == NULL) printf("Error : can't find command in history!!\n");
	
	
	return command;

}

int get_args_number(char **args){
		int i=0;
		int command_number=0;
		while(*(args+i)!='\0'){
			i++;
			command_number++;
		}
		return command_number;
}

/**
   @brief Loop getting input and executing it.
 */
void lsh_loop(void)
{	char *tmp;
  char *line;
  char **args;
  char args_tmp[10];
  int status;
  int args_number;
  int i;


  //stack[0] = getcwd(NULL,0);
  //printf("path = %s\n",stack[0]);

  do {
  	/* prompt */
    printf("%s: ",prompt_name);
    
    /* read command */
    line = lsh_read_line();
    //printf("size = %d\n",strlen(line));
    
    /* history*/
		strcpy(history_command[history_number],line);
		//		printf("history[%d] = %s\n",history_number,history_command[history_number]);
		history_number++;
		
		/* check alias*/
		for(int i=0;i<alias_number;i++){
			if(strcmp(line,alias[i].new_name)==0){
				//printf("found command in alias : %s\n",alias[i].original_name);
				strcpy(line,alias[i].original_name);
			}
		}
		
		/* check the wildcard and !string */
		if(*line=='!'){
    	if(*(line+1)=='!'){
    		if(history_number==0) printf("Error, no previous command\n");
    		else args = lsh_split_line(history_command[history_number-2]);
    	
    	}else{
    		char *command;
    		command = search_command_from_history(line);
    		args = lsh_split_line(command);    	
    	}
    
    }else{
    		//printf("case 3\n");
				args = lsh_split_line(line);
				//print_command(args);
				
    }
    
    /*get args number */
		args_number = get_args_number(args);
		//printf("args number = %d\n",args_number);
		
		
		/* copy the last args*/
		//printf("args[%d] = %s\n",args_number-1,args[args_number-1]);
		strcpy(args_tmp,args[args_number-1]);
		//printf("args[%d] = %s\n",args_number-1,args_tmp);
		
		/* check wild card */
		for(i=0;i<args_number;i++){
			if(strcmp(args[i],"*")==0){
				//printf("find star symble in args[%d] = %s\n",i,args[i]);
				char str[100];
				
				strcpy(str,args[0]);
				strcat(str," ");
				strcat(str,get_wildcard(args));
				strcat(str," ");
				strcat(str,args_tmp);
				//printf("string = %s\n",str);
				
				free(line);
				free(args);
				strcpy(line,str);
				//line = test();
				args = lsh_split_line(line);
				//printf("line = %s\nargs = %s\n",line,args);
				break;
			}
		}
		
		//print_command(args);
    status = lsh_execute(args);
    free(line);
    free(args);
  } while (status);

}

int print_command(char **args){
		int i=0;
		int command_number=0;
		while(*(args+i)!='\0'){
			printf("args[%d] = %s\n",i,*(args+i));
			i++;
			command_number++;
		}
		//printf("number of command = %d\n",command_number);
}

/**
   @brief Main entry point.
   @param argc Argument count.
   @param argv Argument vector.
   @return status code
 */
int main(int argc, char **argv)
{
  // Load config files, if any.

  // Run command loop.
  lsh_loop();
  //get_wildcard();

  

  // Perform any shutdown/cleanup.

  return EXIT_SUCCESS;
}

