/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jegoh <jegoh@student.42singapore.sg>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/21 21:45:15 by jegoh             #+#    #+#             */
/*   Updated: 2023/11/21 22:18:02 by jegoh            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "../inc/minishell.h"

#include "../inc/minishell.h"

size_t ft_strlen(const char *str) {
    size_t length = 0;
    while (str[length] != '\0')
        length++;
    return length;
}

int	ft_strcmp(char *s1, char *s2)
{
	int	i;

	i = 0;
	while (s1[i] != '\0')
	{
		if (s1[i] != s2[i])
			return (s1[i] - s2[i]);
		i++;
	}
	if (s1[i] == '\0')
		return (-s2[i]);
	return (0);
}

char	*remove_dotdot_slash_and_goback_one_dir(char *s, char *cwd)
{
	char	*result;
	char	*temp;
	int		i;
	int		j;

	i = 3;
	j = 0;
	result = malloc(sizeof(char) * ft_strlen(s) - 2 + 1);
	while (s[i])
	{
		result[j] = s[i];
		i++;
		j++;
	}
	result[j] = '\0';
	free(s);
	i = ft_strlen(cwd);
	while (cwd[i] != '/')
		i--;
	temp = malloc(sizeof(char) * i + 1);
	j = 0;
	while (j < i)
	{
		temp[j] = cwd[j];
		j++;
	}
	temp[j] = '\0';
	cwd = temp;
	return (result);
}

char	*removedotslash(char *s)
{
	char	*result;
	int		i;
	int		j;

	i = 2;
	j = 0;
	result = malloc(sizeof(char) * ft_strlen(s) - 2 + 1);
	while (s[i])
	{
		result[j] = s[i];
		i++;
		j++;
	}
	result[j] = '\0';
	free(s);
	return (result);
}

void	checkdir(char *s, char *cwd)
{
	char	*changedpath;
	char	*temp;


	if (s[0] == '.')
	{
		if (s[1] == '.')
		{
			if (s[2] == '/')
				s = remove_dotdot_slash_and_goback_one_dir(s, cwd);
		}
		if (s[1] == '/')
		{
			s = removedotslash(s); //remove ./
    	}
	}
	temp = ft_strjoin(cwd, "/");
	changedpath = ft_strjoin(temp, s);
	free(temp);
	if (chdir(changedpath) != 0)
	{
		perror("chdir failed");
		return ;
	}
	free(changedpath);
	return ;
}

char *getpath(t_list *data)
{
	char	*path = getenv("PATH");
	int		i;

	i = 0;
    if (path == NULL)
        path = "/usr/local/bin:/usr/bin:/bin";
    char **splitpath = ft_split(path, ':');
    if (!splitpath)
        return NULL; // Handle error if splitpath is NULL
    while (splitpath[i++] != NULL){
        char *temp = ft_strjoin(splitpath[i], "/");
		char *joinedpath = ft_strjoin(temp, data->commandsarr[0]);
		free(temp);
        if (access(joinedpath, X_OK) == 0)
		{
            while (splitpath[i])
			{
                free(splitpath[i]);
                i++;
            }
            free(splitpath);
            return joinedpath; // Return the joined path
        }
        free(joinedpath); // Free the joined path if not executable
    }
    // Free the split path array
    int j = 0;
    while (splitpath[j])
	{
        free(splitpath[j]);
        j++;
    }
    free(splitpath);
    return NULL; // Return NULL if command not found in paths
}

void getcmd(t_list *data) {
    data->commandsarr = ft_split(data->prompt, ' ');
    if (data->commandsarr == NULL) {
        // Handle error if commandsarr is NULL
        fprintf(stderr, "Error splitting command input.\n");
        return;
    }
}

void	executecommands(t_list *data, char **envp)
{
	int	id;

	data->execcmds[0] = data->path;		data->path = NULL;
	data->execcmds[1] = data->commandsarr[1];
	data->execcmds[2] = NULL;
	id = fork();
	if (id == 0) //child process
		execve(data->execcmds[0], data->execcmds, envp);
	else
		wait(NULL);
}

int checkempty(char *s)
{
	size_t	i;

	i = 0;
	while (s[i] == ' ' || s[i] == '\t')
		i++;
	if (i == ft_strlen(s))
		return (1);
	return (0);
}

void ft_display_prompt(t_list *data, char **envp)
{
	char	buf[1000];
	char	*cwd;

    while (1) {
        data->prompt = readline("minishell> ");
        // Check for EOF first.
        if (!data->prompt) {
            break;
        }

		if (checkempty(data->prompt) == 0)
		{
			getcmd(data);
			cwd = getcwd(buf, sizeof(buf));

			if (cwd == NULL)
			{
				perror("getcwd failed");
				return ;
			}

			if (ft_strcmp(data->commandsarr[0], "cd") == 0)
			{
				checkdir(data->commandsarr[1], cwd);
			}
			else
			{
				data->path = getpath(data);

				// Add input to history if not NULL or empty.
				if (data->prompt && *data->prompt)
					add_history(data->prompt);

				// Do something with the input.
				if (data->path) {
					// call execve() to execute the command
					executecommands(data, envp);
					free(data->path); // Free the path after use
				} else
					printf("Command not found: %s\n", data->commandsarr[0]);
				// Free the malloc'd input and command array.
				int i = 0;
				while (data->commandsarr[i])
				{
					free(data->commandsarr[i]);
					i++;
				}
				free(data->commandsarr); // Free the array itself
			}
		}
		free(data->path);
        free(data->prompt);
        // Assuming ft_free_split is a function to free a null-terminated array of strings

    }
}

int main(int argc, char **argv, char **envp)
{
	(void)argv;
    t_list *data = malloc(sizeof(t_list));
    if (!data) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }
    if (argc > 1) {
        printf("No arguments are required for minishell\n");
        free(data);
        return 1;
    }
    ft_display_prompt(data, envp);
    free(data); // Free the allocated data structure at the end
    return 0;
}

