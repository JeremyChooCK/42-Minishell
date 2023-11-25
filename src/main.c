/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jegoh <jegoh@student.42singapore.sg>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/21 21:45:15 by jegoh             #+#    #+#             */
/*   Updated: 2023/11/25 21:51:23 by jegoh            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "minishell.h"

char	*dynamic_prompt = NULL;

void	sigint_handler(int sig)
{
	(void)sig;
    if (dynamic_prompt != NULL)
        printf("\n%s", dynamic_prompt);
    else
        printf("\nminishell> ");
}

void	sigquit_handler(int sig)
{
	(void)sig;
}

void	setup_signal_handlers(void)
{
	struct sigaction	sa;

	sa.sa_handler = sigint_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
	sa.sa_handler = sigquit_handler;
	sigaction(SIGQUIT, &sa, NULL);
}

char	*ft_getpath(t_list *data)
{
	char	**splitpath;
	char	*path;
	char	*temp;
	char	*joinedpath;
	int		i;
	int		j;

	i = 0;
	path = getenv("PATH");
	if (path == NULL)
		path = "/usr/local/bin:/usr/bin:/bin";
	splitpath = ft_split(path, ':');
	if (!splitpath)
		return (NULL);
	while (splitpath[i] != NULL)
	{
		temp = ft_strjoin(splitpath[i], "/");
		joinedpath = ft_strjoin(temp, data->commandsarr[0]);
		free(temp);
		if (access(joinedpath, X_OK) == 0)
		{
			while (splitpath[i])
				free(splitpath[i++]);
			free(splitpath);
			return (joinedpath);
		}
		free(joinedpath);
		i++;
	}
	j = 0;
	while (splitpath[j])
		free(splitpath[j++]);
	free(splitpath);
	return (NULL);
}

int	checkforpipe(char *s)
{
	int	i;
	int	num;

	num = 0;
	i = 0;
	while (s[i])
	{
		if (s[i] == '|' && s[i + 1] == '|')
			break ;
		if (s[i] == '|' && s[i + 1] != '|')
			num++;
		i++;
	}
	return (num);
}

char	*str_replace(char *orig, char *rep, char *with)
{
	char	*result;
    char	*ins;
    char	*tmp;
    int		len_rep;
    int		len_with;
    int		len_front;
    int		count;

    if (!orig || !rep)
        return (NULL);
    len_rep = ft_strlen(rep);
    if (len_rep == 0)
        return (NULL);
    if (!with)
        with = "";
    len_with = ft_strlen(with);
    ins = orig;
	count = 0;
	tmp = ft_strstr(ins, rep);
	while (tmp)
	{
		++count;
        ins = tmp + len_rep;
		tmp = ft_strstr(ins, rep);
    }
    tmp = result = malloc(ft_strlen(orig) + (len_with - len_rep) * count + 1);
    if (!result)
        return (NULL);
    while (count--)
	{
        ins = ft_strstr(orig, rep);
        len_front = ins - orig;
        tmp = ft_strncpy(tmp, orig, len_front) + len_front;
        tmp = ft_strcpy(tmp, with) + len_with;
        orig += len_front + len_rep;
    }
    ft_strcpy(tmp, orig);
    return (result);
}

void	replace_exit_status(char **command, char *exit_status)
{
	char	*new_command;
	char	*temp;
	int		i;

	i = 0;
	while (command[i] != NULL)
	{
        new_command = str_replace(command[i], "$?", exit_status);
        if (new_command != NULL) {
            temp = command[i];
            command[i] = new_command;
            free(temp);
        }
        i++;
    }
}

char	*expand_env_variables(char *command)
{
	char	*result;
	char	*temp;
	char	*start;
	char	*end;
	char	var_name[256];
	char	*var_value;

	result = malloc(ft_strlen(command) + 1);
	if (!result)
		return (NULL);
	temp = result;
	start = command;
	while (*start)
	{
		if (*start == '$')
		{
            start++;
            end = start;
            while (ft_isalnum(*end) || *end == '_')
				end++;
            ft_strncpy(var_name, start, end - start);
            var_name[end - start] = '\0';
			var_value = getenv(var_name);
            if (var_value)
            {
                ft_strcpy(temp, var_value);
                temp += ft_strlen(var_value);
            }
            start = end;
        }
        else
            *temp++ = *start++;
    }
    *temp = '\0';
    return (result);
}

void	process_command(char **command)
{
	char	*expanded_cmd;

	expanded_cmd = expand_env_variables(*command);
    if (expanded_cmd)
    {
        free(*command);
        *command = expanded_cmd;
    }
}

int	getcmd(t_list *data, char **envp)
{
	char	*temp;
	int		numofpipes;
	char	**strarr;
	int 	type;
	int		result;
	char	*exit_status;

	data->i = 0;
	numofpipes = checkforpipe(data->prompt);
	if (numofpipes)
	{
		type = 1;
		//strarr split prompt by |
		strarr = ft_split(data->prompt, '|');
		while (data->i < numofpipes + 1)
		{
			process_command(&strarr[data->i]);
			data->commandsarr = ft_split(strarr[data->i], ' ');
			data->path = ft_getpath(data);
			if (data->i == numofpipes)
				type = 2;
			executecommands(data, envp, type);
			data->i++;
		}
		dup2(data->stdin, 0);
		dup2(data->stdout, 1);
		result = 1;
	}
	else
	{
		temp = data->prompt;
		result = 0;
	}
	data->commandsarr = ft_split(temp, ' ');
	if (data->commandsarr == NULL)
	{
		printf("Error splitting command input.\n");
		return (0);
	}
	exit_status = getenv("?");
	if (exit_status != NULL)
		replace_exit_status(data->commandsarr, exit_status);
	return (result);
}

void	executecommands(t_list *data, char **envp, int type)
{
    int id;
    int i;
    int status;

    if (pipe(data->pipefd) == -1)
    {
        perror("pipe error");
        exit(EXIT_FAILURE);
    }
    i = 0;
    while (data->commandsarr[i])
        i++;
    data->execcmds = malloc(sizeof(char *) * i + 1);
    data->execcmds[0] = data->path;
    data->path = NULL;
    i = 1;
    while (data->commandsarr[i])
    {
        data->execcmds[i] = data->commandsarr[i];
        i++;
    }
    data->execcmds[i] = NULL;
    id = fork();
    if (id == 0)
    {
        if (type == 1)
            dup2(data->pipefd[1], 1);
        if (type == 2)
            dup2(data->stdout, 1);
        close(data->pipefd[0]);
        close(data->pipefd[1]);
        execve(data->execcmds[0], data->execcmds, envp);
    }
    else
    {
        wait(&status);
        if (WIFEXITED(status))
        {
            char exit_status[4];
            int exit_code = WEXITSTATUS(status);
            int j = 0;

            if (exit_code == 0) {
                exit_status[j++] = '0';
            } else {
                int reverse_num = 0;
                while (exit_code != 0) {
                    reverse_num = reverse_num * 10 + exit_code % 10;
                    exit_code /= 10;
                }
                while (reverse_num != 0) {
                    exit_status[j++] = (reverse_num % 10) + '0';
                    reverse_num /= 10;
                }
            }
            exit_status[j] = '\0';

            setenv("?", exit_status, 1);
        }
        close(data->pipefd[1]);
        if (type == 1)
            dup2(data->pipefd[0], 0);
        close(data->pipefd[0]);
    }
}

int	checkempty(char *s)
{
	size_t	i;

	i = 0;
	while ((s[i] == ' ' || s[i] == '\t') && s != NULL)
		i++;
	return (i == ft_strlen(s));
}

void	ft_add_to_history(t_list *data, char *command)
{
	t_history	*new_history;
	t_history	*last;

	new_history = malloc(sizeof(t_history));
	if (!new_history)
	{
		perror("Failed to allocate memory for history");
		return ;
	}
	new_history->command = ft_strdup(command);
	new_history->next = NULL;
	new_history->prev = NULL;
	if (data->history == NULL)
		data->history = new_history;
	else
	{
		last = data->history;
		while (last->next != NULL)
			last = last->next;
		last->next = new_history;
		new_history->prev = last;
	}
}

void	ft_display_history(t_list *data)
{
	t_history	*current;
	int			count;

	current = data->history;
	count = 0;
	while (current != NULL)
	{
		printf("%d %s\n", ++count, current->command);
		current = current->next;
	}
}

int	checkdir(char *path)
{
	char	cwd[4096];

    if (path == NULL || ft_strcmp(path, "~") == 0)
	{
        path = getenv("HOME");
        if (path == NULL)
		{
            printf("cd: HOME not set\n");
            exit(1);
        }
    }
	else if (ft_strcmp(path, "-") == 0)
	{
        path = getenv("OLDPWD");
        if (path == NULL)
		{
            printf("cd: OLDPWD not set\n");
            exit(1);
        }
        printf("%s\n", path);
    }
    if (getcwd(cwd, sizeof(cwd)) == NULL)
	{
        perror("cd: getcwd failed");
        exit(1);
    }
    if (chdir(path) != 0)
	{
        perror("cd");
        exit(1);
    }
    return (0);
}

void	execute_echo(char **args)
{
	int	i;

	i = 1;
	while (args[i] != NULL)
	{
		printf("%s", args[i]);
		if (args[i + 1] != NULL)
			printf(" ");
		i++;
	}
	printf("\n");
}

// TODO: Refactor shell build in functions into separate functions
void	ft_display_prompt(t_list *data, char **envp)
{
    char	hostname[HOSTNAME_MAX];
    char	cwd[4096];
    char	*username;
	int		i;

	username = getenv("USER");
	if (!username)
		username = "user";
	while (1)
	{
		if (gethostname(hostname, sizeof(hostname)) != 0)
			ft_strncpy(hostname, "unknown", sizeof(hostname));
        hostname[8] = '\0';
        if (getcwd(cwd, sizeof(cwd)) == NULL)
            ft_strncpy(cwd, "unknown", sizeof(cwd));
		dynamic_prompt = malloc(ft_strlen(username) + ft_strlen(hostname) + ft_strlen(cwd) + 10);
        if (!dynamic_prompt)
		{
            perror("malloc");
            exit(1);
		}
        strcpy(dynamic_prompt, username);
        strcat(dynamic_prompt, "@");
        strcat(dynamic_prompt, hostname);
        strcat(dynamic_prompt, ":");
        strcat(dynamic_prompt, cwd);
        strcat(dynamic_prompt, "$ ");
        data->prompt = readline(dynamic_prompt);
        if (dynamic_prompt != NULL)
            free(dynamic_prompt);
		if (!data->prompt)
			break ;
		if (checkempty(data->prompt) == 0)
		{
			ft_add_to_history(data, data->prompt);
			add_history(data->prompt);
			if (getcmd(data, envp) == 0)
			{
				if (ft_strcmp(data->commandsarr[0], "exit") == 0)
				{
					free(data->path);
					free(data->prompt);
					break ;
				}
				else if (ft_strcmp(data->commandsarr[0], "cd") == 0)
					checkdir(data->commandsarr[1]);
				else if (ft_strcmp(data->commandsarr[0], "history") == 0)
					ft_display_history(data);
				else
				{
					data->path = ft_getpath(data);
					if (data->prompt && *data->prompt)
						add_history(data->prompt);
					if (data->path)
					{
						executecommands(data, envp, 0);
					}
					else
						printf("Command not found: %s\n", data->commandsarr[0]);
					i = 0;
					while (data->commandsarr[i])
						free(data->commandsarr[i++]);
					free(data->commandsarr);
				}
			}
		}
		free(data->path);
		free(data->prompt);
	}
}

int	main(int argc, char **argv, char **envp)
{
	int		i;
	t_list	*data;

	setup_signal_handlers();
	data = malloc(sizeof(t_list));
	data->stdin = dup(0);
	data->stdout = dup(1);
	if (!data)
	{
		printf("Memory allocation failed\n");
		return (1);
	}
	if (argc > 1 && ft_strcmp(argv[1], "-c") == 0)
	{
		if (argv[2] == NULL)
		{
			printf("No command specified for -c option\n");
			free(data);
			return (1);
		}
		else
		{
			data->prompt = strdup(argv[2]);
			if (data->prompt == NULL)
			{
				printf("Memory allocation failed\n");
				free(data);
				return (1);
			}
			getcmd(data, envp);
            data->path = ft_getpath(data);
            if (data->path)
            {
                executecommands(data, envp, 0);
                free(data->path);
            }
            else
                printf("Command not found: %s\n", data->commandsarr[0]);
            i = 0;
            while (data->commandsarr[i])
                free(data->commandsarr[i++]);
            free(data->commandsarr);
            free(data->prompt);
		}
	}
	else
		ft_display_prompt(data, envp);
	if (data != NULL)
		free(data);
	return (0);
}
