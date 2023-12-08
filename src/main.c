/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jegoh <jegoh@student.42singapore.sg>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/21 21:45:15 by jegoh             #+#    #+#             */
/*   Updated: 2023/12/08 10:30:31 by jegoh            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "minishell.h"

char		*g_prompt = NULL;

extern char	**environ;

int ft_setenv(const char *name, const char *value, int overwrite)
{
    if (name == NULL || value == NULL || strchr(name, '=') != NULL)
        return -1;
    size_t name_len = strlen(name);
    size_t value_len = strlen(value);
    for (int i = 0; environ[i] != NULL; i++)
	{
        if (strncmp(environ[i], name, name_len) == 0 && environ[i][name_len] == '=')
		{
            if (!overwrite)
                return 0;
            size_t new_size = name_len + value_len + 2;
            char *new_env = malloc(new_size);
            if (new_env == NULL)
                return -1;
            strcpy(new_env, name);
            strcat(new_env, "=");
            strcat(new_env, value);
            free(environ[i]);
            environ[i] = new_env;
            return 0;
        }
    }
    int count;
    for (count = 0; environ[count] != NULL; count++);
    char **new_environ = realloc(environ, sizeof(char *) * (count + 2));
    if (new_environ == NULL)
        return -1;
    size_t new_var_size = name_len + value_len + 2;
    char *new_var = malloc(new_var_size);
    if (new_var == NULL)
        return -1;
    strcpy(new_var, name);
    strcat(new_var, "=");
    strcat(new_var, value);
    new_environ[count] = new_var;
    new_environ[count + 1] = NULL;
    environ = new_environ;
    return 0;
}

void	ft_sigint_handler(int sig)
{
	(void)sig;
	if (g_prompt != NULL)
		printf("\n%s", g_prompt);
	else
		printf("\nminishell> ");
}

void	ft_sigquit_handler(int sig)
{
	(void)sig;
}

void	ft_setup_signal_handlers(void)
{
	struct sigaction	sa;

	sa.sa_handler = ft_sigint_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGINT, &sa, NULL);
	sa.sa_handler = ft_sigquit_handler;
	sigaction(SIGQUIT, &sa, NULL);
}

char	**split_and_validate_path(char *path)
{
	char	**splitpath;

	if (path == NULL)
		path = "/usr/local/bin:/usr/bin:/bin";
	splitpath = ft_split(path, ':');
	return (splitpath);
}

char	*join_paths_and_check_access(char **splitpath, t_list *data)
{
	char	*temp;
	char	*joinedpath;
	int		i;

	i = 0;
	while (splitpath[i] != NULL)
	{
		temp = ft_strjoin(splitpath[i], "/");
		joinedpath = ft_strjoin(temp, data->commandsarr[0]);
		free(temp);
		if (access(joinedpath, X_OK) == 0)
			return (joinedpath);
		free(joinedpath);
		i++;
	}
	return (NULL);
}

char	*handle_special_cases_and_cleanup(char **splitpath, t_list *data)
{
	int	j;

	j = 0;
	if (ft_strcmp(data->commandsarr[0], "<") == 0
		|| data->commandsarr[0][0] == '<')
		return (ft_strdup(data->commandsarr[0]));
	while (splitpath[j])
		free(splitpath[j++]);
	free(splitpath);
	return (NULL);
}

void	free_splitpath(char **splitpath)
{
	int	i;

	i = 0;
	if (splitpath != NULL)
	{
		while (splitpath[i])
			free(splitpath[i++]);
		free(splitpath);
	}
}

char	*ft_getpath(t_list *data)
{
	char	**splitpath;
	char	*joinedpath;
	char	*path;

	inputredirection(data);
	path = getenv("PATH");
	splitpath = split_and_validate_path(path);
	if (!splitpath)
		return (NULL);
	joinedpath = join_paths_and_check_access(splitpath, data);
	if (joinedpath)
	{
		free_splitpath(splitpath);
		return (joinedpath);
	}
	return (handle_special_cases_and_cleanup(splitpath, data));
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

int	count_substring(const char *orig, const char *rep)
{
	const char	*current = orig;
	int			count;
	int			length_rep;

	count = 0;
	length_rep = ft_strlen(rep);
	current = ft_strstr(current, rep);
	while (current != NULL)
	{
		count++;
		current += length_rep;
		current = ft_strstr(current, rep);
	}
	return (count);
}

char	*perform_replacement(
	const char *orig, const char *rep, const char *with, int count)
{
	int			length_front;
	char		*result;
	char		*result_tmp;
	const char	*insert_point;

	result = malloc(
			ft_strlen(orig) + (ft_strlen(with) - ft_strlen(rep)) * count + 1);
	if (!result)
		return (NULL);
	result_tmp = result;
	while (count--)
	{
		insert_point = ft_strstr(orig, rep);
		length_front = insert_point - orig;
		result_tmp = ft_strncpy(result_tmp, orig, length_front) + length_front;
		result_tmp = ft_strcpy(result_tmp, with) + ft_strlen(with);
		orig += length_front + ft_strlen(rep);
	}
	ft_strcpy(result_tmp, orig);
	return (result);
}

char	*str_replace(char *orig, char *rep, char *with)
{
	int	count;

	if (!orig || !rep || ft_strlen(rep) == 0)
		return (NULL);
	if (!with)
		with = "";
	count = count_substring(orig, rep);
	return (perform_replacement(orig, rep, with, count));
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
		if (new_command != NULL)
		{
			temp = command[i];
			command[i] = new_command;
			free(temp);
		}
		i++;
	}
}

int	toggle_quote_state(int quote_state, char current_char)
{
	if (current_char == '\'')
		return (!quote_state);
	return (quote_state);
}

char	*process_env_var(char **p, char *result, char *temp)
{
	char	var_name[NAME_SIZE];
	char	*end;
	char	*var_value;
	size_t	len;

	ft_bzero(var_name, NAME_SIZE);
	end = *p + 1;
	while (ft_isalnum((unsigned char)*end) || *end == '_')
		end++;
	ft_strncpy(var_name, *p + 1, ft_min(end - *p - 1, NAME_SIZE - 1));
	var_value = getenv(var_name);
	if (var_value)
	{
		len = ft_strlen(var_value);
		temp = realloc(result, (temp - result) + len + ft_strlen(end) + 1);
		if (!temp)
		{
			free(result);
			return (NULL);
		}
		ft_strcpy(temp, var_value);
		temp += len;
	}
	*p = end - 1;
	return (temp);
}

char	*expand_env_variables(char *command)
{
	char	*result;
	char	*temp;
	int		in_single_quote;

	result = ft_strnew(ft_strlen(command));
	if (!result)
		return (NULL);
	temp = result;
	in_single_quote = 0;
	while (*command)
	{
		in_single_quote = toggle_quote_state(in_single_quote, *command);
		if (*command == '$' && !in_single_quote)
		{
			temp = process_env_var(&command, result, temp);
			if (!temp)
				return (NULL);
		}
		else
			*temp++ = *command;
		command++;
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

int	process_quotes(char *cmd_line)
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

void	freesplit(char **s)
{
	int	i;

	i = 0;
	while (s[i] != NULL)
	{
		free(s[i]);
		s[i] = NULL;
		i++;
	}
	free(s);
	s = NULL;
}

int	getcmd(t_list *data, char **envp)
{
	char	*temp;
	int		numofpipes;
	char	**strarr;
	char	**cmd_parts;
	int 	type;
	int		result;
	char	*exit_status;

	temp = NULL;
    if (process_quotes(data->prompt) < 0)
	{
        ft_putstr_fd("Error: Unmatched quotes in command.\n", 2);
        return (-1);
    }
	cmd_parts = ft_split(data->prompt, ' ');
    if (!cmd_parts)
	{
        ft_putstr_fd("Error splitting command input.\n", 2);
        return (-1);
    }
    for (int i = 0; cmd_parts[i] != NULL; ++i)
	{
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
			data->commandsarr = ft_split(strarr[data->i], ' ');
			data->path = ft_getpath(data);
			if (data->i == numofpipes)
				type = 2;
			executecommands(data, envp, type);
			freesplit(data->execcmds);
			data->execcmds = NULL;
			data->i++;
		}
		dup2(data->stdin, STDIN_FILENO);
		dup2(data->stdout, STDOUT_FILENO);
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
		return (0);
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

void reassign(t_list *data, int flag, int index)
{
	int		i;
	int		j;
	char 	*s;
	char	*temp;
	char	**result;

	i = 0;
	j = 0;
	if (flag == 0) // "<" is present
	{
		while (data->execcmds[index + 1][i] && data->execcmds[index + 1][i] != '>')
			i++;
		s = malloc(sizeof(char) * i + 1);
		while (j < i)
		{
			s[j] = data->execcmds[index + 1][j];
			j++;
		}
		s[j] = '\0';
		data->inputfd = open(s, O_RDONLY);
		free(s);
		dup2(data->inputfd, 0);
		close(data->inputfd);
		if (data->execcmds[index + 1][i])
		{
			j = 0;
			s = malloc(sizeof(char) * ft_strlen(data->execcmds[index + 1] + i));
			while(data->execcmds[index + 1][i])
			{
				s[j] = data->execcmds[index + 1][i];
				i++;
				j++;
			}
			s[j] = '\0';
			free(data->execcmds[index + 1]);
			data->execcmds[index + 1] = s;
			free(data->execcmds[index]);
		}
		else
		{
			free(data->execcmds[index]);
			free(data->execcmds[index + 1]);
			j = index;
			index++;
			while (data->execcmds[index + 1])
			{
				data->execcmds[index] = data->execcmds[index + 1];
				index++;
			}
			data->execcmds[index] = NULL;
			index = j;
		}
		while (data->execcmds[j + 1])
		{
			data->execcmds[j] = data->execcmds[j + 1];
			j++;
		}
		data->execcmds[j] = NULL;
		free(data->commandsarr[0]);
		data->commandsarr[0] = data->execcmds[0];
		temp = data->execcmds[0];
		if (!(access(ft_getpath(data), X_OK))) // if cant access then get path
		{
			data->execcmds[0] = ft_getpath(data);
			free(temp);
		}
	}
	else if (flag == 1)
	{
		while (data->execcmds[index][i] != '<')
			i++;
		// use s to store flags
		s = malloc(sizeof(char) * i + 1);
		while (j < i)
		{
			s[j] = data->execcmds[index][j];
			j++;
		}
		s[j] = '\0';
		if (s[0] == '\0') // if no flags
		{
			free(s);
			i++;
			j = i;
			while (data->execcmds[index][j] && data->execcmds[index][j] != '>')
				j++;
			s = malloc(sizeof(char) * (j - i) + 1);
			j = 0;
			while (data->execcmds[index][i] && data->execcmds[index][i] != '>')
			{
				s[j] = data->execcmds[index][i];
				j++;
				i++;
			}
			s[j] = '\0';
			data->inputfd = open(s, O_RDONLY);
			dup2(data->inputfd, 0);
			close(data->inputfd);
			free(s);
			if (data->execcmds[index][j + 1])
			{
				i = 0;
				while(data->execcmds[index][j + i + 1])
					i++;
				s = malloc(sizeof(char) * i + 1);
				i = 0;
				while(data->execcmds[index][j + i + 1])
				{
					s[i] = data->execcmds[index][j + i + 1];
					i++;
				}
				s[i] = '\0';
				free(data->execcmds[index]);
				data->execcmds[index] = s;
				while(data->execcmds[index])
					index++;
			}
			else
			{
				free(data->execcmds[index]);
				while (data->execcmds[index + 1])
				{
					data->execcmds[index] = data->execcmds[index + 1];
					index++;
				}
			}
			data->execcmds[index] = NULL;
			temp = data->commandsarr[0];
			data->commandsarr[0] = data->execcmds[0];
			free(temp);
			if (!(access(ft_getpath(data), X_OK))) // if cant access then get path
			{
				data->execcmds[0] = ft_getpath(data);
			}
			// printf("execcmds[0] : %s\n", data->execcmds[0]);
			// printf("execcmds[1] : %s\n", data->execcmds[1]);
			// printf("execcmds[2] : %s\n", data->execcmds[2]);
		}
		else
		{
			j = 0;
			while(data->execcmds[index][i + j + 1] && data->execcmds[index][i + j + 1] != '>')
				j++;
			temp = malloc(sizeof(char) * j + 1);
			j = 0;
			while(data->execcmds[index][i + j + 1] && data->execcmds[index][i + j + 1] != '>')
			{
				temp[j] = data->execcmds[index][i + j + 1];
				j++;
			}
			temp[j] = '\0';
			data->inputfd = open(temp, O_RDONLY);
			dup2(data->inputfd, 0);
			close(data->inputfd);
			free(temp);
			if (data->execcmds[index][i + j + 1])
			{
				i = 0;
				while (data->execcmds[i])
					i++;
				result = malloc(sizeof(char *) * i + 1 + 1);
				result[i + 1] = NULL;
				i = 0;
				while (i < index)
				{
					result[i] = data->execcmds[i];
					i++;
				}
				result[i] = s;
				result[i + 1] = ft_strdup(data->execcmds[index] + j + 1 + ft_strlen(s));
				free(data->execcmds[index]);
				free(data->execcmds);
				data->execcmds = result;
			}
			else
			{
				free(data->execcmds[index]);
				data->execcmds[index] = s;
			}
		}
	}
	else if (flag == 2)
	{
		while (data->commandsarr[i])
			i++;
		result = malloc(sizeof(char *) * (i + 1 + 1));
		while (j <= index)
		{
			result[j] = data->commandsarr[j];
			j++;
		}
		i = 0;
		while (data->commandsarr[index][i] != '<')
			i++;
		j = 0;
		while (data->commandsarr[index][i + j])
			j++;
		s = malloc(sizeof(char) * j + 1);
		j = 0;
		while (data->commandsarr[index][i + j])
		{
			s[j] = data->commandsarr[index][i + j];
			j++;
		}
		s[j] = '\0';
		result[index][i] = '\0';
		index++;
		result[index] = s;
		index++;
		while (data->commandsarr[index - 1])
		{
			result[index] = data->commandsarr[index - 1];
			index++;
		}
		result[index] = NULL;
		free(data->commandsarr);
		data->commandsarr = result;
	}
}

void	inputredirection(t_list *data)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	if (data->execcmds == NULL)
	{
		while (data->commandsarr[i])
		{
			while (data->commandsarr[i][j])
			{
				if (data->commandsarr[i][j] == '<' && j != 0)
				{
					reassign(data, 2, i);
					return ;
				}
				j++;
			}
			i++;
		}
	}
	else
	{
		while (data->execcmds[i])
		{
			if (ft_strcmp(data->execcmds[i], "<") == 0) // check for "<" "infile"
			{
				reassign(data, 0, i);
				return ;
			}
			j = 0;
			while (data->execcmds[i][j])
			{
				if (data->execcmds[i][j] == '<') //check for "grep" "o" "<infile" or "grep" "o<" "infile" or "grep" "o" "<infile" or "grep" "o<infile"
				{
					reassign(data, 1, i);
					return ;
				}
				j++;
			}
			i++;
		}
	}
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
			// printf("write to pipe1 stdout : %i\n", data->pipefd[1]);
		}
        if (type == 2)
		{
            dup2(data->stdout, 1);
			// printf("write to pipe2 stdout : %i\n", data->stdout);
		}
		// todo : add in check for > redirect here
        close(data->pipefd[0]);
        close(data->pipefd[1]);
		// check for input redirection
		inputredirection(data);
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
			// printf("pipe stdin : %i\n", data->pipefd[0]);
        	close(data->pipefd[0]);
		}
        else
		{
			wait(&status);
			if (WIFEXITED(status))
				ft_setenv("?", ft_itoa(WEXITSTATUS(status)), 1);
		}
		wait(NULL);
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

int	checkdir(char **args)
{
	char	cwd[4096];
	char	*path;

	if (args && args[0] && args[1])
	{
		ft_putstr_fd("cd: too many arguments\n", 2);
		return (1);
	}
	path = args[0];
	if (path == NULL || ft_strcmp(path, "~") == 0)
	{
		path = getenv("HOME");
		if (path == NULL)
		{
			ft_putstr_fd("cd: HOME not set\n", 2);
			return (1);
		}
	}
	else if (ft_strcmp(path, "-") == 0)
	{
		path = getenv("OLDPWD");
		if (path == NULL)
		{
			ft_putstr_fd("cd: OLDPWD not set\n", 2);
			return (1);
		}
		printf("%s\n", path);
	}
	if (getcwd(cwd, sizeof(cwd)) == NULL)
	{
		perror("cd: getcwd failed");
		return (1);
	}
	if (chdir(path) != 0)
	{
		perror("cd");
		return (1);
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

void echo_out(char **str, int pos)
{
    char *temp;
    char *expanded_cmd;
    int in_single_quote = 0;
    int in_double_quote = 0;

    temp = ft_strdup(str[pos]);
    if (temp == NULL)
    {
        perror("Failed to allocate memory");
        exit(1);
    }
    if (temp[0] == '\'' && temp[ft_strlen(temp) - 1] == '\'')
        in_single_quote = 1;
    if (temp[0] == '\"' && temp[ft_strlen(temp) - 1] == '\"')
        in_double_quote = 1;
    remove_chars(temp, "'\"");
    if (!in_single_quote)
    {
        expanded_cmd = expand_env_variables(temp);
        if (expanded_cmd) {
            printf("%s", expanded_cmd);
            free(expanded_cmd);
        } else {
            printf("%s", temp);
        }
    }
    else
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
	node->env_var.key = ft_strndup(env_str, key_len);
	node->env_var.value = ft_strdup(separator + 1);
	node->next = NULL;
	return (node);
}

int	is_valid_identifier(char *str)
{
	if (!str || !*str || ft_isdigit(*str))
		return (0);
	while (*str)
	{
		if (!ft_isalnum(*str) && *str != '_')
			return (0);
		str++;
	}
	return (1);
}

void	ft_export(char *arg, t_env_list **env_list)
{
    char		*key;
    char		*value;
	char		*separator;
    t_env_list	*current;
	t_env_list	*new_node;

	if (!arg || !env_list)
	{
		ft_setenv("?", "1", 1);
		return ;
	}
	separator = ft_strchr(arg, '=');
	if (separator)
	{
		key = ft_strndup(arg, separator - arg);
		value = ft_strdup(separator + 1);
	}
	else
	{
		key = ft_strdup(arg);
		value = ft_strdup("");
	}
	key = ft_strndup(arg, separator - arg);
	if (!is_valid_identifier(key))
	{
		ft_putstr_fd(" not a valid identifier\n", 2);
		free(key);
		free(value);
		ft_setenv("?", "1", 1);
		return ;
	}
	ft_setenv("?", "0", 1);
    for (current = *env_list; current != NULL; current = current->next)
    {
        if (ft_strcmp(current->env_var.key, key) == 0)
        {
            free(current->env_var.value);
            current->env_var.value = value;
            free(key);
            return ;
        }
    }
	new_node = create_env_node(arg);
    if (!new_node)
    {
        free(key);
        free(value);
        return ;
    }
    current = *env_list;
    if (!current)
        *env_list = new_node;
    else
    {
        while (current->next != NULL)
            current = current->next;
        current->next = new_node;
    }
	free(key);
	free(value);
}

void	ft_unset(char **args, t_env_list **env_list)
{
	t_env_list	*current;
	t_env_list	*prev;
	char		*arg;
	int			i;

	i = 0;
	if (!args || !env_list || !*env_list)
		return ;
	while (args[i] != NULL)
	{
		arg = args[i];
	    current = *env_list;
	    prev = NULL;
	    while (current != NULL)
		{
	        if (ft_strcmp(current->env_var.key, arg) == 0)
			{
	            if (prev == NULL)
	                *env_list = current->next;
	            else
	                prev->next = current->next;
	            free(current->env_var.key);
	            free(current->env_var.value);
	            free(current);
	            break ;
	        }
	        prev = current;
	        current = current->next;
	    }
		i++;
	}
	ft_setenv("?", "0", 1);
}

void	ft_env(t_env_list *env_vars)
{
	t_env_list	*current;

	current = env_vars;
	while (current != NULL)
	{
		printf("%s=%s\n", current->env_var.key, current->env_var.value);
		current = current->next;
	}
}

int	is_valid_number(char *str)
{
	int	i;

	i = 0;
	if (str[i] == '+' || str[i] == '-')
		i++;
	while (str[i] != '\0')
	{
		if (!ft_isdigit(str[i]))
			return (0);
		i++;
	}
	return (i > 0);
}

void	strip_quotes(char *str)
{
	int	i;
	int	j;
	int	len;

	i = 0;
	j = 0;
	len = ft_strlen(str);
	while (i < len)
	{
		if (str[i] != '\"' && str[i] != '\'')
			str[j++] = str[i];
		i++;
	}
	str[j] = '\0';
}

void	ft_exit(char **args)
{
	char	*last_exit_status;
	int		exit_status;
	char	*arg;

	exit_status = 0;
	if (args[1])
	{
		if (args[2])
		{
			ft_putstr_fd("exit: too many arguments\n", 2);
			exit(1);
		}
		arg = ft_strdup(args[1]);
		if (!arg)
		{
			perror("Error allocating memory");
			exit(1);
		}
		strip_quotes(arg);
		if (is_valid_number(arg))
			exit_status = ft_atoi(arg);
		else
		{
			ft_putstr_fd("exit: numeric argument required\n", 2);
			free(arg);
			exit(2);
		}
		free(arg);
	}
	else
	{
		last_exit_status = getenv("?");
		if (last_exit_status)
			exit_status = ft_atoi(last_exit_status);
	}
	exit(exit_status);
}

// TODO: Refactor shell build in functions into separate functions
void	ft_display_prompt(t_list *data, char **envp)
{
    char	hostname[NAME_SIZE];
    char	cwd[4096];
    char	*username;
	int		i;

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
		g_prompt = malloc(ft_strlen(username) + ft_strlen(hostname) + ft_strlen(cwd) + 10);
        if (!g_prompt)
		{
            perror("malloc");
            exit(1);
		}
        ft_strcpy(g_prompt, username);
        ft_strcat(g_prompt, "@");
        ft_strcat(g_prompt, hostname);
        ft_strcat(g_prompt, ":");
        ft_strcat(g_prompt, cwd);
        ft_strcat(g_prompt, "$ ");
        data->prompt = readline(g_prompt);
        if (g_prompt != NULL)
		{
			free(g_prompt);
			g_prompt = NULL;
		}
		if (!data->prompt)
			break ;
		if (checkempty(data->prompt) == 0)
		{
			ft_add_to_history(data, data->prompt);
			add_history(data->prompt);
			if (getcmd(data, envp) == 0)
			{
				if (ft_strcmp(data->commandsarr[0], "echo") == 0)
					ft_setenv("?", ft_itoa(ft_echo(data->commandsarr + 1)), 1);
				else if (ft_strcmp(data->commandsarr[0], "cd") == 0)
				{
					char *expanded_args[3] = {NULL, NULL, NULL};
					for (int i = 1; i <= 2 && data->commandsarr[i] != NULL; i++)
						expanded_args[i-1] = expand_env_variables(data->commandsarr[i]);
					ft_setenv("?", ft_itoa(checkdir(expanded_args)), 1);
					for (int i = 0; i < 2; i++)
					{
						if (expanded_args[i] != NULL)
							free(expanded_args[i]);
					}
				}
				else if (ft_strcmp(data->commandsarr[0], "pwd") == 0)
					ft_setenv("?", ft_itoa(ft_pwd()), 1);
				else if (ft_strcmp(data->commandsarr[0], "export") == 0)
					ft_export(data->commandsarr[1], &(data->env_vars));
				else if (ft_strcmp(data->commandsarr[0], "unset") == 0)
					ft_unset(data->commandsarr + 1, &(data->env_vars));
				else if (ft_strcmp(data->commandsarr[0], "env") == 0)
					ft_env(data->env_vars);
				else if (ft_strcmp(data->commandsarr[0], "exit") == 0)
				{
					free(data->path);
					if (data->prompt != NULL)
					{
						free(data->prompt);
						data->prompt = NULL;
					}
					ft_exit(data->commandsarr);
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
					while (data->commandsarr[i] != NULL)
					{
						free(data->commandsarr[i]);
						data->commandsarr[i] = NULL;
						i++;
					}
					free(data->commandsarr);
					while (data->execcmds[i] != NULL)
					{
						free(data->execcmds[i]);
						data->execcmds[i] = NULL;
						i++;
					}
					free(data->execcmds);
					data->execcmds = NULL;
				}
			}
		}
		else
			ft_setenv("?", "0", 1);
		free(data->path);
		if (data->prompt != NULL)
		{
			free(data->prompt);
			data->prompt = NULL;
		}
	}
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

	ft_setup_signal_handlers();
	if (!argc && !argv)
		return (0);
	data = malloc(sizeof(t_list));
	data->stdin = dup(STDIN_FILENO);
	data->stdout = dup(STDOUT_FILENO);
	if (!data)
		return(printf("Memory allocation failed\n"), 1);
	ft_init_t_env(envp, &(data->env_vars));
	ft_display_prompt(data, envp);
	if (data != NULL)
	{
		ft_free_env_vars(data->env_vars);
		free(data);
	}
	return (0);
}
