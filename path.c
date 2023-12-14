#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_BUFFER_SIZE 1500
#define MAX_ARGS 70
#define DELIMITER " \t\n"


void execute_command(char *command, char *args[])
{
	pid_t pid = fork();

	if (pid == -1)
	{
		perror("fork");
		exit(EXIT_FAILURE);
	}
	else if (pid == 0)
	{
		execvp(command, args);

		perror("execvp");
		exit(EXIT_FAILURE);
	}
	else
	{
		int status;
		waitpid(pid, &status, 0);
	}
}

int main(void)
{
	char buffer[MAX_BUFFER_SIZE];
	char *args[MAX_ARGS];
	char *token;

	while (1)
	{
		printf("Shell > ");
		if (fgets(buffer, sizeof(buffer), stdin) == NULL)
		{
			break;
		}

		int arg_count = 0;
		token = strtok(buffer, DELIMITER);
		while (token != NULL && arg_count < MAX_ARGS - 1)
		{
			args[arg_count++] = token;
			token = strtok(NULL, DELIMITER);
		}
		args[arg_count] = NULL;

		if (arg_count > 0)
		{
			char *command = args[0];

			if (access(command, X_OK) == 0)

				execute_command(command, args);
		}
		else
		{
			char *path = getenv("PATH");
			char *path_copy = strdup(path);
			token = strtok(path_copy, DELIMITER);
			while (token != NULL)
			{
				char command_path[MAX_BUFFER_SIZE];
				snprintf(command_path, sizeof(command_path), "%s/%s", token, command);

				if (access(command_path, X_OK) == 0)
				{
					execute_command(command_path, args);
					break;
				}

				token = strtok(NULL, DELIMITER);
			}

			free(path_copy);

			if (token == NULL)
			{
				printf("Command not found: %s\n", command);
			}
		}
	}

	return (0);
}

