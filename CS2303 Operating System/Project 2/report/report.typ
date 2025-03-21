#set align(center + horizon)
#set text(size: 40pt)
= Project 2
#set text(size: 24pt)
UNIX Shell Programming\ Linux Kernel Module for Task Information\
#set align(start + top)
#set text(size: 14pt)
#pagebreak(weak: true)

#set page(header: context{
  if counter(page).get().first() > 1 [
  *Project 2*
  #h(1fr)
  _UNIX Shell Programming & Linux Kernel Module for Task Information_
  ]
}, numbering: "1")
#counter(page).update(1)

= Introduction
The objective of this project is to implement a UNIX shell and a Linux kernel module that can be used to display information about the tasks running on the system.\

The shell should be able to:
- Create child processes according to the user's input
- Provide history of the commands entered by the user
- Provide input and output redirection
- Use pipes to connect multiple commands
- Run commands in the background

The kernel module should be able to:
- Write a process ID to the `/proc/pid` file
- Read the command name, pid and state of the process from the `/proc/pid` file.

= Implementation
== Shell
The main structure of the shell is a loop that reads the user's input, parses it, and executes the commands. The data structures and the main function of the shell are as follows:
```c
struct shell_info
{
  char *username;
  char *cwd;
  FILE *history_file;
  char  last_command[BUFFER_SIZE];
  int   should_run;
};

struct command
{
  int   argc;
  char *argv[MAX_ARGS];
  int   in_fd;
  int   out_fd;
};

struct commands
{
  char           input_buffer[BUFFER_SIZE];
  struct command cmds[MAX_COMMANDS];
  int            cmd_cnt;
  char          *redirect_in;
  char          *redirect_out;
  int            background;
};

int main(void)
{
  init_shell();

  while (shell_info.should_run)
  {
    prompt_and_input();
    parse_input();
    exec_commands();
  }

  return 0;
}
```
- The `shell_info` structure holds information about the shell, - such as the username, current working directory, history file, and the last command entered by the user.\
- The `command` structure represents a single command with its arguments and file descriptors for input and output.\
- The `commands` structure holds multiple commands, input buffer, and redirection information.\
- The main function of the shell reads the user's input, parses it, and executes the commands in a loop until the user exits the shell.

The details of the implementation are as follows:
```c

void prompt_and_input(void)
{
  // Print the prompt and read the user's input
  // If the input is empty, return
  // If the input is '!!', copy the last command to the 
  //                       input buffer
  // Copy the input buffer to shell_info.last_command,
  //                          commands.input_buffer,
  //                          and the history file
}

void parse_input(void)
{
  // Initailization
  char           *token;
  struct command *cmd;
  char            parse_buffer[BUFFER_SIZE];

  commands.cmd_cnt      = 1;
  commands.redirect_in  = NULL;
  commands.redirect_out = NULL;
  commands.background   = 0;
  strcpy(parse_buffer, commands.input_buffer);

  // Pick the first command and get the first token
  token     = strtok(parse_buffer, " \n");
  cmd       = commands.cmds;
  cmd->argc = 0;

  // Parse the input by tokenizing it
  while (token != NULL)
  {
    // Input redirection, read the next token as the input file
    if (strcmp(token, "<") == 0)
      commands.redirect_in = strtok(NULL, " \n");
    // Output redirection, read the next token as the output file
    else if (strcmp(token, ">") == 0)
      commands.redirect_out = strtok(NULL, " \n");
    // Pipe, move to the next command
    else if (strcmp(token, "|") == 0)
    {
      cmd->argv[cmd->argc] = NULL;
      ++cmd;
      cmd->argc = 0;
    }
    // Background process, set the flag
    else if (strcmp(token, "&") == 0)
      commands.background = 1;
    // Add the token to the current command's arguments
    else
      cmd->argv[cmd->argc++] = token;
    token = strtok(NULL, " \n");
  }

  // Terminate the last command's argument list
  cmd->argv[cmd->argc] = NULL;
  commands.cmd_cnt     = cmd - commands.cmds + 1;
}

void exec_commands(void)
{
  // If the command is a background process,
  // fork the shell and return
  int background_pid = 1;
  if (commands.background)
  {
    background_pid = fork();
    if (background_pid < 0)
    {
      printf("osh: fork failed\n");
      return;
    }
    // The parent process return to the main loop
    else if (background_pid > 0)
      return;
  }

  // Set the input and output file descriptors for
  // the first and last commands to STDIN and STDOUT
  // The file descriptors in the middle commands will be
  // set to the pipe file descriptors later
  commands.cmds[0].in_fd = 
    commands.redirect_in != NULL ?
      open(commands.redirect_in, O_RDONLY) :
      STDIN_FILENO;
  commands.cmds[commands.cmd_cnt - 1].out_fd =
    commands.redirect_out != NULL ? 
      open(commands.redirect_out,
           O_WRONLY | O_CREAT | O_TRUNC, 0666) :
      STDOUT_FILENO;

  // Execute the commands
  for (int i = 0; i < commands.cmd_cnt; ++i)
  {
    // If the command is a built-in command, execute it
    struct command *cmd = commands.cmds + i;
    if (check_builtin_commands(cmd))
      continue;

    // Create a pipe for the commands except the last one
    int pipe_fd[2];
    if (i != commands.cmd_cnt - 1)
    {
      if (pipe(pipe_fd) == -1)
      {
        printf("osh: pipe failed\n");
        return;
      }
      cmd->out_fd      = pipe_fd[1];
      (cmd + 1)->in_fd = pipe_fd[0];
    }

    // Fork the shell and execute the command
    pid_t pid = fork();
    if (pid < 0)
    {
      printf("osh: fork failed\n");
      return;
    }
    else if (pid == 0)
    {
      // Set the input and output file descriptors and 
      // call execvp to execute the command
      dup2(cmd->in_fd, STDIN_FILENO);
      dup2(cmd->out_fd, STDOUT_FILENO);
      if (execvp(cmd->argv[0], cmd->argv) < 0)
            printf("osh: %s: command not found\n", cmd->argv[0]);
      exit(1);
    }

    // The parent process waits for the child process to finish
    waitpid(pid, NULL, 0);
    if (cmd->in_fd != STDIN_FILENO)
      close(cmd->in_fd);
    if (cmd->out_fd != STDOUT_FILENO)
      close(cmd->out_fd);
  }

  // If the shell is a background process, exit
  if (background_pid == 0)
    exit(0);
}

int check_builtin_commands(struct command *cmd)
{
  // Check if the command is a built-in command
  // If it is, calls the corresponding function
}

// Implementations of the built-in commands cd
void cd(struct command *cmd) {...}
// Implementations of the built-in commands history
void history(struct command *cmd) {...}
```

== Kernel Module
The kernel module is implemented mainly with `proc_write` and `proc_read` functions. 
- The `proc_write` function receives the user input, which should be a process ID, and converts it to an integer. The pid is stored as a global variable.
- The `proc_read` function reads the global pid variable and invokes system calls to get the command name, pid, and state of the process. The information is then written to the user buffer, which will be printed to the console by the system.

= Correctness
== Shell
The shell has been tested with various commands, including input/output redirection, pipes, background processes, and built-in commands. The shell correctly executes the commands and provides the expected output. The history feature also works as expected, storing and displaying the user's command history. The results of the tests are as follows:
#figure(
  image("ls.png", width: 100%),
  caption: [Run a simple ls command]
)
#figure(
  image("pipe.png", width: 100%),
  caption: [Run a command with a pipe]
)
#figure(
  image("background.png", width: 100%),
  caption: [Run a command in the background]
)
#figure(
  image("history.png", width: 100%),
  caption: [Display the command history]
)
#figure(
  image("redirection_1.png", width: 100%),
  caption: [Run a command with input and output redirection]
)
#figure(
  image("redirection_2.png", width: 100%),
  caption: [The output file after the redirection test]
)
#figure(
  image("builtin.png", width: 100%),
  caption: [Run a built-in command]
)
#figure(
  image("complex_1.png", width: 100%),
  caption: [Run a complex command with multiple pipes and redirections]
)
#figure(
  image("complex_2.png", width: 100%),
  caption: [The output of the complex command]
)

== Kernel Module
The test of the kernel module is executed with the shell written in the previous section. The result of the test is as follows:
#figure(
  image("pid.png", width: 100%),
  caption: [The output of the kernel module test]
)

= Bonus
The difference between anonymous pipes and named pipes is:
- Anonymous pipes are used between parent and child processes or between sibling processes. Named pipes can be used between any processes.
- Anonymous pipes provide a one-way communication channel. Named pipes provide a two-way communication channel.
- The lifetime of an anonymous pipe is limited to the lifetime of the processes that use it. Named pipes can be used by multiple processes even after the processes that created them have terminated.
- The data written to an anonymous pipe is lost when the pipe is closed. The data written to a named pipe is stored in the pipe until it is read by another process.
- Anonymous pipes are created using the `pipe` system call, and can be used immediately. Named pipes are created using the `mkfifo` system call, and must be openedbefore they can be used.

= Conclusion
The shell and kernel module have been successfully implemented and tested. The shell can execute various commands, including input/output redirection, pipes, background processes, and built-in commands. The kernel module can write and read the process ID to and from the `/proc/pid` file. The project has provided valuable experience in UNIX shell programming and Linux kernel module development.