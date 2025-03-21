// osh.c
// Simple shell implementation in C.

#include <fcntl.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUFFER_SIZE  2048
#define MAX_ARGS     64
#define MAX_COMMANDS 16

#define FONT_BOLD   "\033[1m"
#define FONT_RESET  "\033[0m"
#define COLOR_GREEN "\033[32m"
#define COLOR_BLUE  "\033[34m"
#define COLOR_RESET "\033[0m"

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

struct shell_info shell_info;
struct commands   commands;

void init_shell(void);
void prompt_and_input(void);
void parse_input(void);
void exec_commands(void);

int  check_builtin_commands(struct command *cmd);
void cd(struct command *cmd);
void history(struct command *cmd);

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

void init_shell()
{
    shell_info.username     = getpwuid(getuid())->pw_name;
    shell_info.cwd          = getcwd(NULL, 0);
    shell_info.history_file = fopen(".osh-history", "a+");
    shell_info.should_run   = 1;

    commands.input_buffer[0] = '\n';
}

void prompt_and_input(void)
{
    printf(FONT_BOLD COLOR_GREEN "%s@osh> " COLOR_BLUE "%s" COLOR_RESET "$ " FONT_RESET, shell_info.username, shell_info.cwd);
    fflush(stdout);

    strcpy(shell_info.last_command, commands.input_buffer);
    fgets(commands.input_buffer, BUFFER_SIZE, stdin);

    if (commands.input_buffer[0] == '\n')
        return;
    if (strcmp(strtok(commands.input_buffer, "\n"), "!!") == 0)
    {
        if (strcmp(shell_info.last_command, "\n") == 0)
        {
            printf("osh: No commands in history\n");
            commands.input_buffer[0] = '\n';
            commands.input_buffer[1] = '\0';
            return;
        }
        strcpy(commands.input_buffer, shell_info.last_command);
        printf("%s\n", commands.input_buffer);
        fflush(stdout);
    }
    fprintf(shell_info.history_file, "%s\n", commands.input_buffer);
}

void parse_input(void)
{
    if (commands.input_buffer[0] == '\n')
        return;

    char           *token;
    struct command *cmd;
    char            parse_buffer[BUFFER_SIZE];

    commands.cmd_cnt      = 1;
    commands.redirect_in  = NULL;
    commands.redirect_out = NULL;
    commands.background   = 0;
    strcpy(parse_buffer, commands.input_buffer);

    token     = strtok(parse_buffer, " \n");
    cmd       = commands.cmds;
    cmd->argc = 0;

    while (token != NULL)
    {
        if (strcmp(token, "<") == 0)
            commands.redirect_in = strtok(NULL, " \n");
        else if (strcmp(token, ">") == 0)
            commands.redirect_out = strtok(NULL, " \n");
        else if (strcmp(token, "|") == 0)
        {
            cmd->argv[cmd->argc] = NULL;
            ++cmd;
            cmd->argc = 0;
        }
        else if (strcmp(token, "&") == 0)
            commands.background = 1;
        else
            cmd->argv[cmd->argc++] = token;
        token = strtok(NULL, " \n");
    }

    cmd->argv[cmd->argc] = NULL;
    commands.cmd_cnt     = cmd - commands.cmds + 1;
}

void exec_commands(void)
{
    if (commands.input_buffer[0] == '\n')
        return;

    int background_pid = 1;
    if (commands.background)
    {
        background_pid = fork();
        if (background_pid < 0)
        {
            printf("osh: fork failed\n");
            return;
        }
        else if (background_pid > 0)
            return;
    }

    /* clang-format off */
    commands.cmds[0].in_fd = 
        commands.redirect_in  != NULL ? open(commands.redirect_in,  O_RDONLY)                           : STDIN_FILENO;
    commands.cmds[commands.cmd_cnt - 1].out_fd =
        commands.redirect_out != NULL ? open(commands.redirect_out, O_WRONLY | O_CREAT | O_TRUNC, 0666) : STDOUT_FILENO;
    /* clang-format on */

    for (int i = 0; i < commands.cmd_cnt; ++i)
    {
        struct command *cmd = commands.cmds + i;
        if (check_builtin_commands(cmd))
            continue;

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

        pid_t pid = fork();
        if (pid < 0)
        {
            printf("osh: fork failed\n");
            return;
        }
        else if (pid == 0)
        {
            dup2(cmd->in_fd, STDIN_FILENO);
            dup2(cmd->out_fd, STDOUT_FILENO);
            if (execvp(cmd->argv[0], cmd->argv) < 0)
                printf("osh: %s: command not found\n", cmd->argv[0]);
            exit(1);
        }

        waitpid(pid, NULL, 0);
        if (cmd->in_fd != STDIN_FILENO)
            close(cmd->in_fd);
        if (cmd->out_fd != STDOUT_FILENO)
            close(cmd->out_fd);
    }

    if (background_pid == 0)
        exit(0);
}

int check_builtin_commands(struct command *cmd)
{
    if (strcmp(cmd->argv[0], "cd") == 0)
        cd(cmd);
    else if (strcmp(cmd->argv[0], "history") == 0)
        history(cmd);
    else if (strcmp(cmd->argv[0], "exit") == 0)
        shell_info.should_run = 0;
    else
        return 0;
    return 1;
}

void cd(struct command *cmd)
{
    char *path;

    if (cmd->argc == 1)
    {
        struct passwd *pwd;
        pwd  = getpwuid(getuid());
        path = pwd->pw_dir;
    }
    else if (cmd->argc == 2)
        path = cmd->argv[1];
    else
    {
        printf("cd: too many arguments\n");
        return;
    }

    if (chdir(path) < 0)
        printf("cd: %s: No such file or directory\n", path);
    else
        shell_info.cwd = getcwd(NULL, 0);
}

void history(struct command *cmd)
{
    if (cmd->argc == 1)
    {
        int  n = 1;
        char line[BUFFER_SIZE];

        fseek(shell_info.history_file, 0, SEEK_SET);
        while (fgets(line, BUFFER_SIZE, shell_info.history_file) != NULL)
        {
            printf("    %d  %s", n, line);
            ++n;
        }
    }
    else if (cmd->argc == 2)
        if (strcmp(cmd->argv[1], "-c") == 0)
        {
            fclose(shell_info.history_file);
            shell_info.history_file = fopen(".osh-history", "w+");
        }
        else
        {
            int  n        = 0;
            int  output_n = atoi(cmd->argv[1]);
            char line[BUFFER_SIZE];

            fseek(shell_info.history_file, 0, SEEK_SET);
            while (fgets(line, BUFFER_SIZE, shell_info.history_file) != NULL)
                ++n;

            int i;
            fseek(shell_info.history_file, 0, SEEK_SET);
            for (i = 1; i <= n - output_n; ++i)
                fgets(line, BUFFER_SIZE, shell_info.history_file);
            while (fgets(line, BUFFER_SIZE, shell_info.history_file) != NULL)
            {
                printf("    %d  %s", i - 1, line);
                ++i;
            }
        }
    else
        printf("history: too many arguments\n");
}