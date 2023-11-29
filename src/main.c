/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jegoh <jegoh@student.42singapore.sg>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/21 21:45:15 by jegoh             #+#    #+#             */
/*   Updated: 2023/11/29 20:43:02 by jegoh            ###   ########.fr       */
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
	if (ft_strcmp(data->commandsarr[0], "<") == 0 || (data->commandsarr[0][0] == '<'))
		return (ft_strdup(data->commandsarr[0]));
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
    char *result;
    char *temp;
    char *start;
    char *end;
    char var_name[256];
    char *var_value;
    int in_single_quote = 0;
    int in_double_quote = 0;

    result = malloc(ft_strlen(command) + 1);
    if (!result)
        return (NULL);
    temp = result;
    start = command;
    while (*start)
    {
        if (*start == '\'')
            in_single_quote = !in_single_quote;
        else if (*start == '\"')
            in_double_quote = !in_double_quote;

        if (*start == '$' && !in_single_quote)
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

int process_quotes(char *cmd_line)
{
    int in_single_quote = 0;
    int in_double_quote = 0;

    for (int i = 0; cmd_line[i] != '\0'; ++i) {
        if (cmd_line[i] == '\'' && !in_double_quote)
            in_single_quote = !in_single_quote;
        else if (cmd_line[i] == '\"' && !in_single_quote)
            in_double_quote = !in_double_quote;
        else if (cmd_line[i] == '\\' && !in_single_quote)
		{
            if (cmd_line[i + 1] != '\0')
				i++;
            else
				return (-1);
        }
    }
    return (in_single_quote || in_double_quote) ? -1 : 0;
}

char	**split_command_line(char *cmd_line)
{
    return (ft_split(cmd_line, ' '));
}

void	prepare_execution(char **cmd_parts)
{
	int	i;

	i = 0;
	while (cmd_parts[i] != NULL)
	{
		++i;
        cmd_parts[i] = expand_env_variables(cmd_parts[i]);
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

	temp = NULL;
    if (process_quotes(data->prompt) < 0) {
        fprintf(stderr, "Error: Unmatched quotes in command.\n");
        return -1;
    }
    char **cmd_parts = split_command_line(data->prompt);
    if (!cmd_parts) {
        fprintf(stderr, "Error splitting command input.\n");
        return -1;
    }
    for (int i = 0; cmd_parts[i] != NULL; ++i) {
        char *expanded_cmd = expand_env_variables(cmd_parts[i]);
        if (expanded_cmd) {
            free(cmd_parts[i]);
            cmd_parts[i] = expanded_cmd;
        }
    }
	data->i = 0;
	numofpipes = checkforpipe(data->prompt);
	if (numofpipes)
	{
		type = 1;
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
		dup2(data->stdin, STDIN_FILENO);
		// printf("reset stdin : %i\n", data->stdin);
		dup2(data->stdout, STDOUT_FILENO);
		// printf("reset stdout : %i\n", data->stdout);
		result = 1;
	}
	else
	{
		temp = data->prompt;
		result = 0;
	}
	if (temp)
		data->commandsarr = ft_split(temp, ' ');
	if (data->commandsarr == NULL)
	{
		// printf("Error splitting command input.\n");
		return (0);
	}
	exit_status = getenv("?");
	if (exit_status != NULL)
		replace_exit_status(data->commandsarr, exit_status);
	return (result);
}

void remove_quotes_from_arg(char *arg)
{
    int len = ft_strlen(arg);
    if (len > 1 && ((arg[0] == '"' && arg[len - 1] == '"')
			|| (arg[0] == '\'' && arg[len - 1] == '\'')))
	{
        ft_memmove(arg, arg + 1, len - 2);
        arg[len - 2] = '\0';
    }
}

void remove_quotes_from_args(char **args)
{
    int i = 0;
    while (args[i] != NULL) {
        remove_quotes_from_arg(args[i]);
        i++;
    }
}

void	reassign(t_list *data, int flag)
{
	char	*temp;
	// char	*nameofinfile;
	char	**result;
	int		i;
	int		j;

	i = 0;
	j = 0;
	while (data->execcmds[i])
		i++;
	if (flag == 0) // if "<" "infile"
	{
		result = malloc(sizeof(char *) * i - 2 + 1);
		i = 2;
		while (data->execcmds[i])
		{
			result[j] = data->execcmds[i];
			// printf("%s\n%i\n", result[j], j);
			j++;
			i++;
		}
		data->inputfd = open(data->execcmds[1], O_RDONLY);
		dup2(data->inputfd, 0);
		// printf("< stdin : %i\n", data->inputfd);
		close(data->inputfd);
		free(data->execcmds[0]);
		free(data->execcmds[1]);
		temp = data->commandsarr[0];
		data->commandsarr[0] = ft_strdup(data->execcmds[2]);
		// printf(" cmdarr[0] : %s\n", temp);
		result[0] = ft_getpath(data);
		// printf("path : %s\n", result[0]);
		free(temp);
		free(data->execcmds);
		data->execcmds = result;
	}
	// else if (flag == 1) // if "<infile"
	// {

	// }
}

void	redirection(t_list *data)
{
	//check if <infile is front or back
	//if front remove the < infile and change the input
	if (ft_strcmp(data->execcmds[0], "<") == 0)	//check for "<" "infile"
		reassign(data, 0);
	else if (data->execcmds[0][0] == '<') //check for "<infile"
		reassign(data, 1);
	//if back
	//"grep" "o<" "infile"
	//"grep" "o" "<" "infile"
	//"grep" "o" "<infile"
	//"grep" "o<infile"
}

void executecommands(t_list *data, char **envp, int type)
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
    data->execcmds = malloc(sizeof(char *) * (i + 1));
    if (!data->execcmds)
    {
        perror("malloc error");
        exit(EXIT_FAILURE);
    }
    data->execcmds[0] = data->path;
    data->path = NULL;
    i = 1;
    while (data->commandsarr[i])
    {
        data->execcmds[i] = ft_strdup(data->commandsarr[i]);
        i++;
    }
    data->execcmds[i] = NULL;
    remove_quotes_from_args(data->execcmds);
    id = fork();
    if (id == 0)
    {
        if (type == 1)
		{
            dup2(data->pipefd[1], 1);
			printf("write to pipe1 stdout : %i\n", data->pipefd[1]);
		}
        if (type == 2)
		{
            dup2(data->stdout, 1);
			printf("write to pipe2 stdout : %i\n", data->stdout);
		}
        close(data->pipefd[0]);
        close(data->pipefd[1]);
		// check for redirection
		redirection(data);
        execve(data->execcmds[0], data->execcmds, envp);
        perror(data->execcmds[0]);
        exit(EXIT_FAILURE);
    }
    else
    {
        close(data->pipefd[1]);
        if (type == 1)
        {
			dup2(data->pipefd[0], 0);
			printf("pipe stdin : %i\n", data->pipefd[0]);
        	close(data->pipefd[0]);
		}
        else
		{
			wait(&status);
			if (WIFEXITED(status))
			{
				char exit_status[4];
				int exit_code = WEXITSTATUS(status);
				snprintf(exit_status, sizeof(exit_status), "%d", exit_code);
				setenv("?", exit_status, 1);
			}
		}
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

void	remove_chars(char *str, const char *chars_to_remove)
{
	int	i;
	int	j;
	int	k;
	int	len;
	int	should_remove;
	int	remove_len;

	len = ft_strlen(str);
	remove_len = ft_strlen(chars_to_remove);
	i = 0;
	j = 0;
	while (i < len)
	{
		should_remove = 0;
		k = 0;
		while (k < remove_len)
		{
			if (str[i] == chars_to_remove[k])
			{
				should_remove = 1;
				break ;
			}
			k++;
		}
		if (!should_remove)
			str[j++] = str[i];
		i++;
	}
	str[j] = '\0';
}

void	echo_out(char **str, int pos)
{
	char	*temp;

	temp = ft_strdup(str[pos]);
	if (temp == NULL)
	{
		perror("Failed to allocate memory");
		exit(1);
	}
	remove_chars(temp, "'\"");
	printf("%s", temp);
	free(temp);
}

int	ft_echo(char **args)
{
	int	i;
	int	j;
	int	n_flag;

	n_flag = 0;
	if (!args[0])
	{
		printf("\n");
		return (0);
	}
	i = 0;
	while (args[i] && args[i][0] == '-')
	{
		j = 1;
		while (args[i][j] == 'n')
			j++;
		if (args[i][j] == '\0')
			n_flag = 1;
		else
			break ;
		i++;
	}
	while (args[i])
	{
		echo_out(args, i);
		if (args[i + 1])
			printf(" ");
		i++;
	}
	if (!n_flag)
		printf("\n");
	return (0);
}

int	ft_pwd(void)
{
	char	cwd[4096];

	if (getcwd(cwd, sizeof(cwd)) != NULL)
	{
		printf("%s\n", cwd);
		return (0);
	}
	else
	{
		perror("pwd");
		return (1);
	}
}

void	print_env_vars(t_env_list *env_vars)
{
	t_env_list	*current;

	current = env_vars;
	while (current != NULL)
    {
        printf("declare -x %s=\"%s\"\n", current->env_var.key, current->env_var.value);
		current = current->next;
    }
}

int	valid_export_arg(char *arg)
{
	char	*equal_sign;

	if (!arg)
		return (0);
	equal_sign = ft_strchr(arg, '=');
	if (!equal_sign || equal_sign == arg)
		return (0);
	return (1);
}

void	add_or_update_env_var(char *arg, t_env_list **env_vars)
{
	t_env_list	*new_node;
	t_env_list	*current;
	char		*key;
	char		*value;

	key = ft_strtok(arg, "=");
	value = ft_strtok(NULL, "=");
	if (!key || !value)
		return ;
	current = *env_vars;
	while (current)
	{
		if (ft_strcmp(current->env_var.key, key) == 0)
		{
            free(current->env_var.value);
            current->env_var.value = strdup(value);
            return ;
        }
        current = current->next;
    }
	new_node = malloc(sizeof(t_env_list));
    if (!new_node)
    {
        perror("malloc");
        return ;
	}
	new_node->env_var.key = ft_strdup(key);
	new_node->env_var.value = ft_strdup(value);
	new_node->next = *env_vars;
	*env_vars = new_node;
}

void	custom_export(char **args, t_env_list **env_vars)
{
	int	i;

    if (!args[0])
    {
        print_env_vars(*env_vars);
        return ;
    }
	i = 0;
	while (args[i] != NULL)
    {
        if (valid_export_arg(args[i]))
            add_or_update_env_var(args[i], env_vars);
        else
            fprintf(stderr, "export: invalid argument: %s\n", args[i]);
		i++;
    }
}

// TODO: Refactor shell build in functions into separate functions
void	ft_display_prompt(t_list *data, char **envp)
{
    char	hostname[HOSTNAME_MAX];
    char	cwd[4096];
	char	exit_status_str[4];
    char	*username;
	int		i;
	int		exit_code;

	dup2(data->stdin, STDIN_FILENO);
	dup2(data->stdout, STDOUT_FILENO);
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
        ft_strcpy(dynamic_prompt, username);
        ft_strcat(dynamic_prompt, "@");
        ft_strcat(dynamic_prompt, hostname);
        ft_strcat(dynamic_prompt, ":");
        ft_strcat(dynamic_prompt, cwd);
        ft_strcat(dynamic_prompt, "$ ");
		// printf("current stdin : %i\n current stdout : %i", data->stdin, data->stdout)
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
				if (ft_strcmp(data->commandsarr[0], "echo") == 0)
				{
					exit_code = ft_echo(data->commandsarr + 1);
					snprintf(exit_status_str, sizeof(exit_status_str), "%d", exit_code);
					setenv("?", exit_status_str, 1);
				}
				else if (ft_strcmp(data->commandsarr[0], "cd") == 0)
					checkdir(data->commandsarr[1]);
				else if (ft_strcmp(data->commandsarr[0], "pwd") == 0)
					ft_pwd();
				else if (ft_strcmp(data->commandsarr[0], "export") == 0)
					custom_export(data->commandsarr + 1, &(data->env_vars));
				else if (ft_strcmp(data->commandsarr[0], "exit") == 0)
				{
					free(data->path);
					free(data->prompt);
					break ;
				}
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
						wait(NULL);
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

t_env_list	*create_env_node(char *env_str)
{
	t_env_list	*node;
	char		*separator;
	int			key_len;

	node = malloc(sizeof(t_env_list));
	if (!node)
		return (NULL);
	separator = ft_strchr(env_str, '=');
	if (!separator)
		return (NULL);
	key_len = separator - env_str;
	node->env_var.key = strndup(env_str, key_len);
	node->env_var.value = strdup(separator + 1);
	node->next = NULL;
	return (node);
}

void	ft_init_t_env(char **envp, t_env_list **env_list)
{
	int			i;
	t_env_list	*current;
	t_env_list	*new_node;

    if (!envp || !env_list)
        return;
    *env_list = NULL;
	current = NULL;
	i = 0;
	while (envp[i] != NULL)
    {
		new_node = create_env_node(envp[i]);
        if (!new_node)
            continue ;
        if (current == NULL)
        {
            *env_list = new_node;
            current = *env_list;
        }
        else
        {
            current->next = new_node;
            current = current->next;
        }
		i++;
    }
}

void	ft_free_env_vars(t_env_list *env_vars)
{
	t_env_list	*current;
	t_env_list	*next;

	current = env_vars;
    while (current != NULL)
    {
        next = current->next;
        free(current->env_var.key);
        free(current->env_var.value);
        free(current);
        current = next;
    }
}

int	main(int argc, char **argv, char **envp)
{
	t_list	*data;

	setup_signal_handlers();
	if (!argc && !argv)
		return (0);
	data = malloc(sizeof(t_list));
	data->stdin = dup(STDIN_FILENO);
	// printf("init stdin : %i\n", data->stdin);
	data->stdout = dup(STDOUT_FILENO);
	// printf("init stdout : %i\n", data->stdout);
	if (!data)
	{
		printf("Memory allocation failed\n");
		return (1);
	}
	ft_init_t_env(envp, &(data->env_vars));
	ft_display_prompt(data, envp);
	if (data != NULL)
	{
		ft_free_env_vars(data->env_vars);
		free(data);
	}
	return (0);
}
