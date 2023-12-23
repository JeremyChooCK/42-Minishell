/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jegoh <jegoh@student.42singapore.sg>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/21 21:45:15 by jegoh             #+#    #+#             */
/*   Updated: 2023/12/23 17:30:49 by jegoh            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "minishell.h"

int	g_exit_code;

void	ft_signal_cmd(int sig)
{
	g_exit_code += sig;
	if (sig == 2)
	{
		g_exit_code = 130;
		printf("\n");
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
	if (sig == SIGQUIT)
	{
		ft_putstr_fd("Quit (core dumped)\n", 2);
		exit(1);
	}
}

void	ft_signal_cmd_pipe(int sig)
{
	g_exit_code += sig;
	if (sig == 2)
	{
		g_exit_code = 130;
		printf("\n");
		rl_replace_line("", 0);
		rl_redisplay();
	}
}

t_env_list	**ft_get_adress_env(void)
{
	static t_env_list	*new = NULL;

	return (&new);
}

void	ft_free_env_var(t_env_var *env_var)
{
	if (env_var)
	{
		free(env_var->key);
		free(env_var->value);
	}
}

void	ft_free_history(t_history *history)
{
	t_history	*tmp;

	while (history)
	{
		tmp = history;
		history = history->next;
		free(tmp->command);
		free(tmp);
	}
}

void	check_if_list_is_null(t_list *list)
{
	if (list)
	{
		if (list->prompt)
			free(list->prompt);
		if (list->path)
			free(list->path);
		ft_freesplit(list->commandsarr);
		ft_freesplit(list->execcmds);
		ft_free_history(list->history);
		free(list);
	}
}

void	ft_free_list(t_list *list)
{
	t_env_list	**env_list;
	t_env_list	*env_current;
	t_env_list	*env_temp;

	env_list = ft_get_adress_env();
	env_current = *env_list;
	while (env_current != NULL)
	{
		env_temp = env_current;
		env_current = env_current->next;
		free(env_temp->env_var.key);
		free(env_temp->env_var.value);
		free(env_temp);
	}
	*env_list = NULL;
	check_if_list_is_null(list);
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
	ft_freesplit(splitpath);
	if (ft_strcmp(data->commandsarr[0], "<") == 0
		|| ft_strcmp(data->commandsarr[0], "<<") == 0
		|| ft_strcmp(data->commandsarr[0], ">") == 0
		|| ft_strcmp(data->commandsarr[0], ">>") == 0)
		return (ft_strdup(data->commandsarr[0]));
	return (NULL);
}

char	*ft_getenv(const char *name)
{
	t_env_list	**env_list;
	t_env_list	*current;

	env_list = ft_get_adress_env();
	if (!env_list || !*env_list || !name)
		return (NULL);
	current = *env_list;
	while (current)
	{
		if (ft_strcmp(current->env_var.key, name) == 0)
			return (current->env_var.value);
		current = current->next;
	}
	return (NULL);
}

char	*ft_getpath(t_list *data)
{
	char	**splitpath;
	char	*joinedpath;

	splitpath = ft_split(ft_getenv("PATH"), ':');
	if (!splitpath)
		return (NULL);
	joinedpath = join_paths_and_check_access(splitpath, data);
	if (joinedpath)
	{
		ft_freesplit(splitpath);
		return (joinedpath);
	}
	return (handle_special_cases_and_cleanup(splitpath, data));
}

int	checkforpipe(char *s)
{
	int	i;
	int	num;
	int	in_single_quote;
	int	in_double_quote;

	num = 0;
	i = 0;
	in_single_quote = 0;
	in_double_quote = 0;
	while (s[i])
	{
		if (s[i] == '\'' && (i == 0 || s[i - 1] != '\\'))
			in_single_quote = !in_single_quote;
		else if (s[i] == '\"' && (i == 0 || s[i - 1] != '\\'))
			in_double_quote = !in_double_quote;
		else if (s[i] == '|' && !in_single_quote && !in_double_quote)
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
	var_value = ft_getenv(var_name);
	if (var_value)
	{
		len = ft_strlen(var_value);
		temp = ft_realloc(result, (temp - result) + len + ft_strlen(end) + 1);
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

size_t	calculate_expansion_size(char *s)
{
	char	*start;
	char	*var_name;
	size_t	total_size;
	size_t	var_len;

	total_size = 0;
	while (*s)
	{
		if (*s == '$' && (ft_isalnum(*(s + 1)) || *(s + 1) == '_'))
		{
			start = ++s;
			while (ft_isalnum(*s) || *s == '_')
				s++;
			var_len = s - start;
			var_name = ft_strndup(start, var_len);
			total_size += ft_strlen(ft_getenv(var_name));
			free(var_name);
		}
		else
		{
			total_size++;
			s++;
		}
	}
	return (total_size + 1);
}

char	*find_next_variable(char *s, size_t *var_len)
{
	char	*start;

	while (*s)
	{
		if (*s == '$' && (ft_isalnum(*(s + 1)) || *(s + 1) == '_'))
		{
			start = ++s;
			while (ft_isalnum(*s) || *s == '_')
				s++;
			*var_len = s - start;
			return (start);
		}
		s++;
	}
	return (NULL);
}

void	handle_variable(char **src, char **dest, char *start, size_t var_len)
{
	char	*var_name;
	char	*var_value;

	var_name = ft_strndup(start, var_len);
	if (ft_getenv(var_name))
		var_value = ft_strdup(ft_getenv(var_name));
	else
		var_value = ft_strdup("");
	ft_strcpy(*dest, var_value);
	*dest += ft_strlen(var_value);
	free(var_name);
	free(var_value);
	*src = start + var_len;
}

void	process_string(char *src, char *dest)
{
	char	*start;
	size_t	var_len;

	while (*src)
	{
		var_len = 0;
		start = find_next_variable(src, &var_len);
		if (start)
			handle_variable(&src, &dest, start, var_len);
		else
			*dest++ = *src++;
	}
	*dest = '\0';
}

char	*expand_env_vars(char *str)
{
	char	*buffer;
	size_t	buf_size;

	if (str == NULL)
		return (NULL);
	buf_size = calculate_expansion_size(str);
	buffer = malloc(buf_size);
	if (!buffer)
		return (NULL);
	process_string(str, buffer);
	return (buffer);
}

char	*expand_env_variables(char *arg, int expand)
{
	char	*result;
	int		in_single_quote;

	in_single_quote = 0;
	if (arg == NULL)
		return (NULL);
	if (arg[0] == '\'' && arg[strlen(arg) - 1] == '\'')
		in_single_quote = 1;
	if (expand && !in_single_quote)
	{
		if (ft_strcmp(arg, "$?") == 0)
			result = ft_itoa(g_exit_code);
		else
			result = expand_env_vars(arg);
		free(arg);
		return (result);
	}
	return (arg);
}

int	process_quotes(char *cmd_line)
{
	int	i;
	int	in_single_quote;
	int	in_double_quote;

	i = 0;
	in_single_quote = 0;
	in_double_quote = 0;
	while (cmd_line[i] != '\0')
	{
		i++;
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
	return ((in_single_quote || in_double_quote) * -1);
}

int	process_and_split_command(t_list *data, char ***cmd_parts)
{
	if (process_quotes(data->prompt) < 0)
	{
		ft_putstr_fd("Error: Unmatched quotes in command.\n", 2);
		return (-1);
	}
	*cmd_parts = ft_split_quote(data->prompt, ' ');
	if (!*cmd_parts)
	{
		ft_putstr_fd("Error splitting command input.\n", 2);
		return (-1);
	}
	return (0);
}

int	expand_variables_and_count_pipes(
	char **cmd_parts, t_list *data, int *numofpipes)
{
	char	*expanded_cmd;
	int		i;

	i = 0;
	while (cmd_parts[i] != NULL)
	{
		expanded_cmd = expand_env_variables(cmd_parts[i], 1);
		if (expanded_cmd)
			cmd_parts[i] = expanded_cmd;
		free(expanded_cmd);
		i++;
	}
	*numofpipes = checkforpipe(data->prompt);
	return (0);
}

int	process_command_parts(t_list *data, char **cmd_parts, int *numofpipes)
{
	if (process_and_split_command(data, &cmd_parts) < 0)
		return (-1);
	if (expand_variables_and_count_pipes(cmd_parts, data, numofpipes) < 0)
		return (-1);
	free(cmd_parts);
	return (0);
}

char	*concatenate(const char *str1, const char *str2)
{
	char	*result;

	result = malloc(ft_strlen(str1) + ft_strlen(str2) + 1);
	ft_strcpy(result, str1);
	ft_strcat(result, "/");
	ft_strcat(result, str2);
	return (result);
}

char	*find_command_in_path(char *cmd)
{
	char	*path;
	char	*path_copy;
	char	*token;
	char	*full_path;

	path = ft_getenv("PATH");
	path_copy = ft_strdup(path);
	token = ft_strtok(path_copy, ':');
	while (token != NULL)
	{
		full_path = concatenate(token, cmd);
		if (access(full_path, X_OK) == 0)
		{
			free(path_copy);
			return (full_path);
		}
		free(full_path);
		token = ft_strtok(NULL, ':');
	}
	free(path_copy);
	return (NULL);
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

void	process_commands(
	t_list *data, char **envp, char **strarr, int numofpipes)
{
	int	type;

	type = 1;
	while (data->i < numofpipes + 1)
	{
		data->commandsarr = ft_split_quote(strarr[data->i], ' ');
		data->path = ft_getpath(data);
		if (data->i == numofpipes)
			type = 2;
		executecommands(data, envp, type);
		ft_freesplit(data->execcmds);
		if (data->i < numofpipes)
			ft_freesplit(data->commandsarr);
		data->execcmds = NULL;
		data->i++;
	}
}

int	check_if_pipe_is_valid(char **strarr, int numofpipes)
{
	int	i;

	i = 0;
	while (strarr[i])
		i++;
	if (i != numofpipes + 1)
	{
		ft_putstr_fd("syntax error near unexpected token `|'\n", 2);
		return (0);
	}
	return (1);
}

int	execute_piped_commands(t_list *data, char **envp, int numofpipes)
{
	char	*temp;
	char	**strarr;

	temp = reassign_prompt(data->prompt);
	strarr = ft_split_quote(temp, '|');
	free(temp);
	if (check_if_pipe_is_valid(strarr, numofpipes))
		process_commands(data, envp, strarr, numofpipes);
	ft_freesplit(strarr);
	data->i = 0;
	dup2(data->stdin, STDIN_FILENO);
	dup2(data->stdout, STDOUT_FILENO);
	return (1);
}

int	execute_multiple_commands(t_list *data, char **envp, int numofpipes)
{
	char	*temp;
	int		result;

	result = 0;
	if (numofpipes)
		result = execute_piped_commands(data, envp, numofpipes);
	else
	{
		temp = reassign_prompt(data->prompt);
		ft_freesplit(data->commandsarr);
		data->commandsarr = ft_split_quote(temp, ' ');
		free(temp);
		if (data->commandsarr == NULL)
			return (0);
	}
	return (result);
}

int	getcmd(t_list *data, char **envp)
{
	char	**cmd_parts;
	int		numofpipes;
	int		result;

	cmd_parts = NULL;
	result = process_command_parts(data, cmd_parts, &numofpipes);
	if (result < 0)
		return (-1);
	return (execute_multiple_commands(data, envp, numofpipes));
}

void	remove_quotes_from_arg(char *arg)
{
	int	len;

	len = ft_strlen(arg);
	if (len > 1 && ((arg[0] == '"' && arg[len - 1] == '"')
			|| (arg[0] == '\'' && arg[len - 1] == '\'')))
	{
		ft_memmove(arg, arg + 1, len - 2);
		arg[len - 2] = '\0';
	}
}

void	remove_quotes_from_args(char **args)
{
	int	i;

	i = 0;
	while (args[i] != NULL)
	{
		remove_quotes_from_arg(args[i]);
		i++;
	}
}

int	open_temp_file(const char *tmpfile)
{
	int	tmpfd;

	tmpfd = open(tmpfile, O_RDWR | O_CREAT, 0644);
	if (tmpfd == -1)
	{
		perror("Error creating temporary file");
		return (-1);
	}
	return (tmpfd);
}

char	*read_and_process_input(const char *delimiter, t_list *data)
{
	char	*input;

	dup2(data->stdin, 0);
	input = readline("> ");
	if (!input || ft_strcmp(input, delimiter) == 0)
	{
		if (input)
			free(input);
		return (NULL);
	}
	return (input);
}

int	write_to_file(int tmpfd, const char *input, const char *tmpfile)
{
	if (write(tmpfd, input, ft_strlen(input)) == -1
		|| write(tmpfd, "\n", 1) == -1)
	{
		perror("Error writing to temporary file");
		close(tmpfd);
		unlink(tmpfile);
		return (-1);
	}
	return (0);
}

int	write_user_input_to_file(
	int tmpfd, const char *delimiter, const char *tmpfile, t_list *data)
{
	char	*input;

	while (1)
	{
		input = read_and_process_input(delimiter, data);
		if (!input)
			break ;
		if (write_to_file(tmpfd, input, tmpfile) == -1)
		{
			free(input);
			return (-1);
		}
		free(input);
	}
	return (0);
}

int	create_and_write_heredoc(char *delimiter, const char *tmpfile, t_list *data)
{
	int	tmpfd;

	tmpfd = open_temp_file(tmpfile);
	if (tmpfd == -1)
		return (-1);
	if (write_user_input_to_file(tmpfd, delimiter, tmpfile, data) == -1)
	{
		close(tmpfd);
		return (-1);
	}
	return (tmpfd);
}

void	setup_heredoc_fd(int tmpfd, char *tmpfile)
{
	int	heredocfd;

	if (close(tmpfd) == -1)
	{
		perror("Error closing temporary file");
		unlink(tmpfile);
		exit(EXIT_FAILURE);
	}
	heredocfd = open(tmpfile, O_RDONLY);
	if (heredocfd == -1)
	{
		perror("Error opening temporary file for heredoc");
		unlink(tmpfile);
		exit(EXIT_FAILURE);
	}
	if (dup2(heredocfd, STDIN_FILENO) == -1)
	{
		perror("Error redirecting stdin");
		close(heredocfd);
		unlink(tmpfile);
		exit(EXIT_FAILURE);
	}
	close(heredocfd);
	unlink(tmpfile);
}

void	handle_heredoc(char *delimiter, t_list *data)
{
	char	*tmpfile;
	int		tmpfd;

	tmpfile = "/tmp/heredocXXXXXX";
	tmpfd = create_and_write_heredoc(delimiter, tmpfile, data);
	if (tmpfd == -1)
		exit(EXIT_FAILURE);
	setup_heredoc_fd(tmpfd, tmpfile);
}

void	handle_append_redirection(char *filename)
{
	int	fd;

	fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (fd == -1)
	{
		perror("Error opening file for append");
		exit(EXIT_FAILURE);
	}
	if (dup2(fd, STDOUT_FILENO) == -1)
	{
		perror("Error redirecting stdout");
		close(fd);
		exit(EXIT_FAILURE);
	}
	close(fd);
}

char	*copy_infile_name(t_list *data, int index, int i)
{
	char	*s;
	int		j;

	j = 0;
	s = malloc(sizeof(char) * i + 1);
	while (j < i)
	{
		s[j] = data->execcmds[index + 1][j];
		j++;
	}
	s[j] = '\0';
	return (s);
}

void	open_infile_and_redirect_input(t_list *data, int index)
{
	int		i;
	char	*s;

	i = 0;
	while (data->execcmds[index + 1][i] && data->execcmds[index + 1][i] != '>')
		i++;
	s = copy_infile_name(data, index, i);
	data->inputfd = open(s, O_RDONLY);
	if (data->inputfd == -1)
	{
		perror(s);
		g_exit_code = 1;
		exit(EXIT_FAILURE);
	}
	free(s);
	dup2(data->inputfd, 0);
	close(data->inputfd);
	free(data->execcmds[index]);
	free(data->execcmds[index + 1]);
}

void	move_commands_forward(t_list *data, int index)
{
	int	j;

	j = index;
	index++;
	while (data->execcmds[index + 1])
	{
		data->execcmds[index] = data->execcmds[index + 1];
		index++;
	}
	data->execcmds[index] = NULL;
	index = j;
	while (data->execcmds[j + 1])
	{
		data->execcmds[j] = data->execcmds[j + 1];
		j++;
	}
	data->execcmds[j] = NULL;
}

void	handle_input_redirection(t_list *data, int index)
{
	char	*temp;
	char	*temp2;

	open_infile_and_redirect_input(data, index);
	move_commands_forward(data, index);
	free(data->commandsarr[0]);
	data->commandsarr[0] = NULL;
	data->commandsarr[0] = ft_strdup(data->execcmds[0]);
	temp = data->execcmds[0];
	temp2 = ft_getpath(data);
	if (temp2)
	{
		if (!(access(temp2, X_OK)))
		{
			data->execcmds[0] = temp2;
			free(temp);
		}
	}
	free(data->commandsarr[0]);
	data->commandsarr[0] = NULL;
}

void	move_forward_and_check_for_null(t_list *data, int index)
{
	int	i;

	i = 2;
	while (data->execcmds[index + i])
	{
		data->execcmds[index] = data->execcmds[index + i];
		index++;
	}
	data->execcmds[index] = NULL;
	if (data->commandsarr[0] == NULL
		|| ft_strcmp(data->commandsarr[0], "<<") == 0)
	{
		if (data->commandsarr[0] != NULL
			&& ft_strcmp(data->commandsarr[0], "<<") == 0)
			free(data->commandsarr[0]);
		data->commandsarr[0] = ft_strdup(data->execcmds[0]);
	}
}

void	reassign_and_handle_heredoc(t_list *data, int index)
{
	char	*temp;
	char	*s;

	if (data->execcmds[index + 1] == NULL)
		return ;
	handle_heredoc(data->execcmds[index + 1], data);
	free(data->execcmds[index]);
	free(data->execcmds[index + 1]);
	move_forward_and_check_for_null(data, index);
	temp = ft_getpath(data);
	s = data->execcmds[0];
	if (!(access(temp, X_OK)))
	{
		data->execcmds[0] = ft_getpath(data);
		free(temp);
		free(s);
	}
	else
		free(temp);
}

void	open_output_file(t_list *data, int index)
{
	data->inputfd = open(data->execcmds[index + 1],
			O_RDWR | O_CREAT | O_TRUNC, 0644);
	if (data->inputfd == -1)
	{
		perror(data->execcmds[index + 1]);
		exit(EXIT_FAILURE);
	}
	dup2(data->inputfd, 1);
	close(data->inputfd);
	if (data->inputfd == -1)
	{
		perror(data->execcmds[index + 1]);
		exit(EXIT_FAILURE);
	}
}

char	**remove_arrow_and_outfile_strings(t_list *data)
{
	int		i;
	int		j;
	char	**result;

	i = 0;
	j = 0;
	while (data->execcmds[i])
		i++;
	result = malloc(sizeof(char *) * (i + 1 - 2));
	i = 0;
	while (data->execcmds[i])
	{
		if (ft_strcmp(data->execcmds[i], ">") == 0)
			i += 2;
		if (data->execcmds[i] == NULL)
			break ;
		result[j] = ft_strdup(data->execcmds[i]);
		i++;
		j++;
	}
	result[j] = NULL;
	return (result);
}

void	handle_output_redirection(t_list *data, int index)
{
	char	*temp;
	char	*s;
	char	**result;

	open_output_file(data, index);
	result = remove_arrow_and_outfile_strings(data);
	ft_freesplit(data->execcmds);
	data->execcmds = result;
	if (data->commandsarr[0] == NULL
		|| ft_strcmp(data->commandsarr[0], ">") == 0)
	{
		if (data->commandsarr[0] != NULL
			&& ft_strcmp(data->commandsarr[0], ">") == 0)
			free(data->commandsarr[0]);
		data->commandsarr[0] = ft_strdup(data->execcmds[0]);
	}
	temp = ft_getpath(data);
	s = data->execcmds[0];
	if (!(access(temp, X_OK)))
	{
		data->execcmds[0] = ft_getpath(data);
		free(s);
	}
	free(temp);
}

void	move_forward_and_check_for_append(t_list *data, int index)
{
	int	i;

	i = 2;
	while (data->execcmds[index + i])
	{
		data->execcmds[index] = data->execcmds[index + i];
		index++;
	}
	data->execcmds[index] = NULL;
	if (data->commandsarr[0] == NULL
		|| ft_strcmp(data->commandsarr[0], ">>") == 0)
	{
		if (data->commandsarr[0] != NULL
			&& ft_strcmp(data->commandsarr[0], ">>") == 0)
			free(data->commandsarr[0]);
		data->commandsarr[0] = ft_strdup(data->execcmds[0]);
	}
}

void	reassign_and_handle_append(t_list *data, int index)
{
	char	*s;
	char	*temp;

	if (data->execcmds[index + 1] == NULL)
		return ;
	handle_append_redirection(data->execcmds[index + 1]);
	free(data->execcmds[index]);
	free(data->execcmds[index + 1]);
	move_forward_and_check_for_append(data, index);
	temp = ft_getpath(data);
	s = data->execcmds[0];
	if (temp)
	{
		if (!(access(temp, X_OK)))
		{
			data->execcmds[0] = ft_getpath(data);
			free(temp);
			free(s);
		}
		else
			free(temp);
	}
}

void	reassign(t_list *data, int flag, int index)
{
	if (flag == 0)
		handle_input_redirection(data, index);
	else if (flag == 1)
		reassign_and_handle_heredoc(data, index);
	else if (flag == 2)
		handle_output_redirection(data, index);
	else if (flag == 3)
		reassign_and_handle_append(data, index);
}

void	check_if_redirection_is_last_arg(char *s)
{
	if (s == NULL)
	{
		perror("syntax error near unexpected token `newline'");
		exit(1);
	}
}

void	inputredirection(t_list *data)
{
	int	i;

	i = 0;
	while (data->execcmds[i])
	{
		if (ft_strcmp(data->execcmds[i], "<") == 0)
		{
			check_if_redirection_is_last_arg(data->execcmds[i + 1]);
			reassign(data, 0, i);
			i = -1;
		}
		else if (ft_strcmp(data->execcmds[i], "<<") == 0)
		{
			check_if_redirection_is_last_arg(data->execcmds[i + 1]);
			reassign(data, 1, i);
			i = -1;
		}
		i++;
	}
}

void	outputredirection(t_list *data)
{
	int	i;

	i = 0;
	while (data->execcmds[i])
	{
		if (ft_strcmp(data->execcmds[i], ">") == 0)
		{
			check_if_redirection_is_last_arg(data->execcmds[i + 1]);
			reassign(data, 2, i);
			i = -1;
		}
		else if (ft_strcmp(data->execcmds[i], ">>") == 0)
		{
			check_if_redirection_is_last_arg(data->execcmds[i + 1]);
			reassign(data, 3, i);
			i = -1;
		}
		i++;
	}
}

int	check_for_redirection(char	**s)
{
	int	i;

	i = 0;
	while (s[i])
	{
		if (!ft_strcmp(s[i], "'<'") || !ft_strcmp(s[i], "'<<'")
			|| !ft_strcmp(s[i], "'>'") || !ft_strcmp(s[i], "'>>'")
			|| !ft_strcmp(s[i], "\"<\"") || !ft_strcmp(s[i], "\"<<\"")
			|| !ft_strcmp(s[i], "\">\"") || !ft_strcmp(s[i], "\">>\""))
			return (1);
		i++;
	}
	return (0);
}

void	prepare_command_arrays(t_list *data)
{
	int	i;

	i = 0;
	while (data->commandsarr[i])
		i++;
	data->execcmds = malloc(sizeof(char *) * (i + 1));
	if (!data->execcmds)
		exit(EXIT_FAILURE);
	if (data->commandsarr[0] && (data->commandsarr[0][0] == '/'
		|| !ft_strncmp(data->commandsarr[0], "./", 2)))
		data->execcmds[0] = ft_strdup(data->commandsarr[0]);
	else
		data->execcmds[0] = data->path;
	if (data->execcmds[0] == NULL)
	{
		ft_putstr_fd(data->commandsarr[0], 2);
		ft_putstr_fd(": command not found\n", 2);
		return ;
	}
	data->path = NULL;
	i = 0;
	while (data->commandsarr[++i])
		data->execcmds[i] = ft_strdup(data->commandsarr[i]);
	data->execcmds[i] = NULL;
}

void	handle_parent_process(t_list *data, int type, int id)
{
	int	status;

	signal(SIGINT, ft_signal_cmd_pipe);
	signal(SIGQUIT, SIG_IGN);
	close(data->pipefd[1]);
	if (type == 1)
	{
		dup2(data->pipefd[0], 0);
		close(data->pipefd[0]);
	}
	else
	{
		waitpid(id, &status, 0);
		if (WIFEXITED(status))
			g_exit_code = WEXITSTATUS(status);
	}
	wait(NULL);
	signal(SIGINT, ft_signal_cmd);
	signal(SIGQUIT, SIG_IGN);
}

void	execute_buildin2(t_list *data)
{
	if (ft_strcmp(data->commandsarr[0], "unset") == 0)
	{
		ft_unset(data->commandsarr + 1);
		exit(1);
	}
	else if (ft_strcmp(data->commandsarr[0], "env") == 0)
	{
		ft_env();
		exit(1);
	}
	else if (ft_strcmp(data->commandsarr[0], "history") == 0)
	{
		ft_display_history(data);
		exit(1);
	}
	else if (ft_strcmp(data->commandsarr[0], "exit") == 0)
		ft_exit(data);
}

// data->execcmds[0] for echo fix the test cases, do not touch that
void	execute_buildin(t_list *data)
{
	if (ft_strcmp(data->execcmds[0], "echo") == 0)
	{
		g_exit_code = ft_echo(data->commandsarr + 1);
		exit(1);
	}
	else if (ft_strcmp(data->commandsarr[0], "cd") == 0)
	{
		g_exit_code = ft_cd(data->commandsarr + 1);
		exit(1);
	}
	else if (ft_strcmp(data->commandsarr[0], "pwd") == 0)
	{
		g_exit_code = ft_pwd();
		exit(1);
	}
	else if (ft_strcmp(data->commandsarr[0], "export") == 0)
	{
		ft_export(data->commandsarr[1]);
		exit(1);
	}
	else
		execute_buildin2(data);
}

void	check_for_redirection_and_close_pipe(t_list *data, int type)
{
	inputredirection(data);
	if (type == 1)
		dup2(data->pipefd[1], 1);
	if (type == 2)
		dup2(data->stdout, 1);
	outputredirection(data);
	close(data->pipefd[0]);
	close(data->pipefd[1]);
}

void	handle_child_process(
	t_list *data, char **envp, int type, struct stat buff)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	check_for_redirection_and_close_pipe(data, type);
	if (data->commandsarr[0])
		execute_buildin(data);
	if (data->execcmds[0] != NULL)
	{
		remove_quotes_from_args(data->execcmds);
		execve(data->execcmds[0], data->execcmds, envp);
		if (stat(data->execcmds[0], &buff) == 0)
		{
			ft_putstr_fd("minishell: ", 2);
			ft_putstr_fd(data->execcmds[0], 2);
			ft_putstr_fd(": Permission denied\n", 2);
			exit(126);
		}
	}
	g_exit_code = EXIT_FAILURE;
	exit(EXIT_FAILURE);
}

void	manage_process_and_signals(t_list *data, int type, char **envp)
{
	int			id;
	struct stat	buff;

	ft_memset(&buff, 0, sizeof(buff));
	if (pipe(data->pipefd) == -1)
		exit(EXIT_FAILURE);
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	id = fork();
	if (id == -1)
		exit(1);
	if (id == 0)
		handle_child_process(data, envp, type, buff);
	else
		handle_parent_process(data, type, id);
}

void	executecommands(t_list *data, char **envp, int type)
{
	prepare_command_arrays(data);
	if (!check_for_redirection(data->execcmds))
		remove_quotes_from_args(data->execcmds);
	if (ft_strcmp(data->commandsarr[0], "cd") == 0)
	{
		g_exit_code = ft_cd(data->commandsarr + 1);
		return ;
	}
	manage_process_and_signals(data, type, envp);
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

char	*parse_env_var(const char *input)
{
	char	*env_value;

	if (input[0] == '$')
	{
		env_value = ft_getenv(input + 1);
		if (env_value != NULL)
			return (strdup(env_value));
		else
			return (NULL);
	}
	else
		return (ft_strdup(input));
}

char	*resolve_path(char *path)
{
	if (path == NULL || ft_strcmp(path, "~") == 0)
	{
		path = ft_getenv("HOME");
		if (path == NULL)
		{
			ft_putstr_fd("cd: HOME not set\n", 2);
			return (NULL);
		}
	}
	else if (ft_strcmp(path, "-") == 0)
	{
		path = ft_getenv("OLDPWD");
		if (path == NULL)
		{
			ft_putstr_fd("cd: OLDPWD not set\n", 2);
			return (NULL);
		}
		printf("%s\n", path);
	}
	return (parse_env_var(path));
}

char	*validate_and_resolve_path(char **args)
{
	char	*path;

	if (args && args[0] && args[1])
	{
		ft_putstr_fd("cd: too many arguments\n", 2);
		path = NULL;
	}
	else
		path = args[0];
	return (resolve_path(path));
}

int	change_directory(char *path)
{
	char	cwd[4096];

	if (path == NULL)
		return (1);
	if (getcwd(cwd, sizeof(cwd)) == NULL)
	{
		perror("cd: getcwd failed");
		free(path);
		return (1);
	}
	if (chdir(path) != 0)
	{
		perror("cd");
		free(path);
		return (1);
	}
	free(path);
	return (0);
}

int	ft_cd(char **args)
{
	char	*resolved_path;

	resolved_path = validate_and_resolve_path(args);
	return (change_directory(resolved_path));
}

int	update_quote(int *i, char c, int *in_single_quote, int *in_double_quote)
{
	int	check;

	check = 0;
	if (c == '\'' && !*in_double_quote)
	{
		*in_single_quote = !*in_single_quote;
		(*i)++;
		check = 1;
	}
	if (c == '\"' && !*in_single_quote)
	{
		*in_double_quote = !*in_double_quote;
		(*i)++;
		check = 1;
	}
	return (check);
}

void	check_quotes(int *i, int *output_index, char *output)
{
	if (*output_index > 0 && output[(*output_index) - 1] != ' ')
	{
		output[(*output_index)++] = ' ';
		(*i)++;
	}
}

void	parse_and_print_echo(char *input)
{
	int		i;
	int		in_single_quote;
	int		in_double_quote;
	char	output[MAX_INPUT_LENGTH];
	int		output_index;

	ft_memset(output, 0, sizeof(output));
	output_index = 0;
	in_single_quote = 0;
	in_double_quote = 0;
	i = 0;
	while (i < (int)ft_strlen(input))
	{
		if (update_quote(&i, input[i], &in_single_quote, &in_double_quote))
			continue ;
		if (input[i] == ' ' && !in_single_quote && !in_double_quote)
		{
			if (output_index > 0 && output[output_index - 1] != ' ')
				check_quotes(&i, &output_index, output);
			continue ;
		}
		output[output_index++] = input[i++];
	}
	printf("%s", output);
}

void	process_split_temp(char **split_temp, int expand)
{
	int	i;

	i = -1;
	while (split_temp[++i] != NULL)
		split_temp[i] = expand_env_variables(split_temp[i], expand);
}

void	echo_out(char **str, int pos)
{
	char	*temp;
	char	**split_temp;
	char	*joined_cmd;
	int		expand;
	int		length;

	expand = 1;
	temp = ft_strdup(str[pos]);
	if (temp == NULL)
		exit(1);
	length = ft_strlen(temp);
	if (length >= 2 && temp[0] == '\'' && temp[length - 1] == '\'')
		expand = 0;
	split_temp = ft_split(temp, ' ');
	if (split_temp == NULL)
		exit(1);
	process_split_temp(split_temp, expand);
	joined_cmd = ft_strjoin_space(split_temp);
	if (joined_cmd)
	{
		parse_and_print_echo(joined_cmd);
		free(joined_cmd);
	}
	ft_freesplit(split_temp);
	free(temp);
}

int	process_flags(char **args, int *n_flag)
{
	int	i;
	int	j;

	*n_flag = 0;
	i = 0;
	while (args[i] && args[i][0] == '-')
	{
		j = 1;
		while (args[i][j] == 'n')
			j++;
		if (args[i][j] == '\0')
			*n_flag = 1;
		else
			break ;
		i++;
	}
	return (i);
}

int	ft_echo(char **args)
{
	int	i;
	int	n_flag;

	if (!args[0])
	{
		printf("\n");
		return (0);
	}
	i = process_flags(args, &n_flag);
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

int	parse_export_argument(char *arg, char **key, char **value)
{
	char	*separator;

	if (!arg)
		return (0);
	separator = ft_strchr(arg, '=');
	if (separator)
	{
		*key = ft_strndup(arg, separator - arg);
		*value = ft_strdup(separator + 1);
	}
	else
	{
		*key = ft_strdup(arg);
		*value = ft_strdup("");
	}
	if (!is_valid_identifier(*key))
	{
		ft_putstr_fd(" not a valid identifier\n", 2);
		free(*key);
		free(*value);
		return (0);
	}
	return (1);
}

int	update_env_var(char *key, char *value, t_env_list **env_list)
{
	t_env_list	*current;

	current = *env_list;
	while (current != NULL)
	{
		if (ft_strcmp(current->env_var.key, key) == 0)
		{
			free(current->env_var.value);
			current->env_var.value = value;
			free(key);
			return (1);
		}
		current = current->next;
	}
	return (0);
}

char	*create_env_str(char *key, char *value)
{
	char	*env_str;
	char	*temp;

	env_str = ft_strjoin(key, "=");
	temp = env_str;
	env_str = ft_strjoin(env_str, value);
	free(temp);
	return (env_str);
}

void	add_new_env_var(char *key, char *value, t_env_list **env_list)
{
	t_env_list	*new_node;
	t_env_list	*current;
	char		*env_str;

	env_str = create_env_str(key, value);
	new_node = create_env_node(env_str);
	free(env_str);
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

void	add_to_env_list(char *key, char *value, t_env_list **env_list)
{
	if (!update_env_var(key, value, env_list))
		add_new_env_var(key, value, env_list);
}

void	ft_export(char *arg)
{
	t_env_list	**env_list;
	char		*key;
	char		*value;

	env_list = ft_get_adress_env();
	if (!parse_export_argument(arg, &key, &value))
	{
		g_exit_code = 1;
		return ;
	}
	add_to_env_list(key, value, env_list);
	g_exit_code = 0;
}

void	ft_free_current(t_env_list *current)
{
	free(current->env_var.key);
	free(current->env_var.value);
	free(current);
}

void	check_env_list_and_iterate(t_env_list	**env_list,
			t_env_list	*current, t_env_list	*prev)
{
	if (prev == NULL)
		*env_list = current->next;
	else
		prev->next = current->next;
	ft_free_current(current);
}

void	ft_unset(char **args)
{
	t_env_list	**env_list;
	t_env_list	*current;
	t_env_list	*prev;
	int			i;

	env_list = ft_get_adress_env();
	if (!args || !*args || !env_list || !*env_list)
		return ;
	i = 0;
	while (args[i] != NULL)
	{
		prev = NULL;
		current = *env_list;
		while (current != NULL)
		{
			if (ft_strcmp(current->env_var.key, args[i]) == 0)
			{
				check_env_list_and_iterate(env_list, current, prev);
				break ;
			}
			prev = current;
			current = current->next;
		}
		i++;
	}
}

void	ft_env(void)
{
	t_env_list	**env_list;
	t_env_list	*current;

	env_list = ft_get_adress_env();
	if (!env_list || !*env_list)
		return ;
	current = *env_list;
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

char	*validate_exit_argument(t_list *data)
{
	char	*arg;

	if (data->commandsarr[1])
	{
		if (data->commandsarr[2])
		{
			ft_putstr_fd("exit: too many arguments\n", 2);
			return (ft_strdup("-1"));
		}
		arg = ft_strdup(data->commandsarr[1]);
		if (!arg)
			return (NULL);
		strip_quotes(arg);
		if (!is_valid_number(arg))
		{
			ft_putstr_fd("exit: numeric argument required\n", 2);
			free(arg);
			return (ft_strdup("-2"));
		}
	}
	else
		arg = NULL;
	return (arg);
}

int	process_exit_args(t_list *data)
{
	int		i;
	char	*validated_arg;

	validated_arg = validate_exit_argument(data);
	if (validated_arg == NULL && data->commandsarr[1])
		return (1);
	if (validated_arg && strcmp(validated_arg, "-1") == 0)
	{
		free(validated_arg);
		g_exit_code = 1;
		return (1111);
	}
	if (validated_arg && strcmp(validated_arg, "-2") == 0)
	{
		free(validated_arg);
		return (2);
	}
	if (validated_arg)
	{
		i = ft_atoi(validated_arg);
		free(validated_arg);
		return (i);
	}
	return (0);
}

void	ft_exit(t_list *data)
{
	int	exit_status;

	exit_status = process_exit_args(data);
	if (exit_status == 1111)
		return ;
	else if (exit_status == 2)
	{
		ft_free_list(data);
		exit(2);
	}
	else
	{
		ft_free_list(data);
		exit(exit_status);
	}
}

void	parse_quotes(
	const char *prompt, int *in_single_quote, int *in_double_quote, int i)
{
	if (prompt[i] == '\'' && (i == 0 || prompt[i - 1] != '\\'))
		*in_single_quote = !*in_single_quote;
	if (prompt[i] == '\"' && (i == 0 || prompt[i - 1] != '\\'))
		*in_double_quote = !*in_double_quote;
}

int	calculate_new_length(const char *prompt)
{
	int	i;
	int	len;
	int	in_single_quote;
	int	in_double_quote;

	len = 0;
	in_single_quote = 0;
	in_double_quote = 0;
	i = 0;
	while (prompt[i])
	{
		parse_quotes(prompt, &in_single_quote, &in_double_quote, i);
		if (!in_single_quote && !in_double_quote)
		{
			if (prompt[i] == '<' || prompt[i] == '>')
				len = len + 2 + (2 * (prompt[i + 1] == prompt[i]));
		}
		len++;
		i++;
	}
	return (len);
}

void	update_quote_flags(
	const char *prompt, int *in_single_quote, int *in_double_quote, int index)
{
	if (prompt[index] == '\'' && (index == 0 || prompt[index - 1] != '\\'))
		*in_single_quote = !*in_single_quote;
	if (prompt[index] == '\"' && (index == 0 || prompt[index - 1] != '\\'))
		*in_double_quote = !*in_double_quote;
}

void	add_spaces(
	const char *prompt, char *s, int *in_single_quote, int *in_double_quote)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	while (prompt[i])
	{
		update_quote_flags(prompt, in_single_quote, in_double_quote, i);
		if (!*in_single_quote && !*in_double_quote
			&& (prompt[i] == '<' || prompt[i] == '>'))
		{
			s[j++] = ' ';
			s[j++] = prompt[i];
			if (prompt[i + 1] == prompt[i])
				s[j++] = prompt[i++];
			s[j++] = ' ';
		}
		else
			s[j++] = prompt[i];
		i++;
	}
	s[j] = '\0';
}

void	check_for_quotes_and_add_spaces(const char *prompt, char *s)
{
	int	in_single_quote;
	int	in_double_quote;

	in_single_quote = 0;
	in_double_quote = 0;
	add_spaces(prompt, s, &in_single_quote, &in_double_quote);
}

char	*reassign_prompt(const char *prompt)
{
	int		new_len;
	char	*s;

	if (prompt == NULL)
		return (NULL);
	new_len = calculate_new_length(prompt);
	s = malloc(new_len + 1);
	if (!s)
		return (NULL);
	check_for_quotes_and_add_spaces(prompt, s);
	return (s);
}

void	parse_for_comments(char **input)
{
	char	*hash_pos;

	hash_pos = ft_strchr(*input, '#');
	if (hash_pos != NULL)
		*hash_pos = '\0';
}

void	handle_external_commands(t_list *data, char **envp)
{
	char	*cmd;

	data->path = ft_getpath(data);
	cmd = data->commandsarr[0];
	if (data->path || (cmd && (*cmd == '/' || !ft_strncmp(cmd, "./", 2))))
	{
		executecommands(data, envp, 0);
		wait(NULL);
	}
	else
	{
		ft_putstr_fd(cmd, 2);
		ft_putstr_fd(": command not found\n", 2);
		g_exit_code = 127;
	}
	ft_freesplit(data->commandsarr);
	data->commandsarr = NULL;
	if (data->execcmds)
	{
		ft_freesplit(data->execcmds);
		data->execcmds = NULL;
	}
}

void	execute_specific_command(t_list *data, char **envp)
{
	if (ft_strcmp(data->commandsarr[0], "echo") == 0)
		g_exit_code = ft_echo(data->commandsarr + 1);
	else if (ft_strcmp(data->commandsarr[0], "cd") == 0)
		g_exit_code = ft_cd(data->commandsarr + 1);
	else if (ft_strcmp(data->commandsarr[0], "pwd") == 0)
		g_exit_code = ft_pwd();
	else if (ft_strcmp(data->commandsarr[0], "export") == 0)
		ft_export(data->commandsarr[1]);
	else if (ft_strcmp(data->commandsarr[0], "unset") == 0)
		ft_unset(data->commandsarr + 1);
	else if (ft_strcmp(data->commandsarr[0], "env") == 0)
		ft_env();
	else if (ft_strcmp(data->commandsarr[0], "history") == 0)
		ft_display_history(data);
	else
		handle_external_commands(data, envp);
}

void	expand_command_arguments(char **args)
{
	int	i;

	i = 0;
	while (args[i] != NULL)
	{
		args[i] = expand_env_variables(args[i], 1);
		i++;
	}
}

void	handle_redirections(t_list *data, int i)
{
	int	flags;
	int	fd;

	data->original_stdout = dup(STDOUT_FILENO);
	flags = (O_WRONLY | O_CREAT | O_APPEND);
	if (ft_strcmp(data->commandsarr[i], ">") == 0)
		flags = (O_WRONLY | O_CREAT | O_TRUNC);
	fd = open(data->commandsarr[i + 1], flags, 0644);
	if (fd == -1)
	{
		perror("open");
		exit(EXIT_FAILURE);
	}
	dup2(fd, STDOUT_FILENO);
	close(fd);
	data->redirection_active = 1;
}

void	update_commands_array(t_list *data, int i)
{
	int	j;

	j = i;
	while (data->commandsarr[j + 2] != NULL)
	{
		data->commandsarr[j] = data->commandsarr[j + 2];
		j++;
	}
	data->commandsarr[j] = NULL;
}

void	setup_redirections(t_list *data)
{
	int	i;

	i = 0;
	data->redirection_active = 0;
	while (data->commandsarr[i] != NULL)
	{
		if (ft_strcmp(data->commandsarr[i], ">") == 0
			|| ft_strcmp(data->commandsarr[i], ">>") == 0)
		{
			handle_redirections(data, i);
			update_commands_array(data, i);
			break ;
		}
		i++;
	}
}

void	restore_stdout(t_list *data)
{
	if (data->redirection_active)
	{
		dup2(data->original_stdout, STDOUT_FILENO);
		close(data->original_stdout);
		data->redirection_active = 0;
	}
}

// This function may break, revert it quickly if so
void	execute_command(t_list *data, char **envp)
{
	setup_redirections(data);
	if (strcmp(data->commandsarr[0], "echo") == 0)
		ft_echo(data->commandsarr + 1);
	else if (ft_strcmp(data->commandsarr[0], "exit") == 0)
		ft_exit(data);
	else
	{
		expand_command_arguments(data->commandsarr);
		execute_specific_command(data, envp);
	}
	restore_stdout(data);
}

void	handle_command(t_list *data, char **envp)
{
	parse_for_comments(&(data->prompt));
	if (checkempty(data->prompt) == 0)
	{
		ft_add_to_history(data, data->prompt);
		add_history(data->prompt);
		if (getcmd(data, envp) == 0)
			execute_command(data, envp);
	}
	else
		g_exit_code = 0;
	if (data->path)
		free(data->path);
	if (data->prompt != NULL)
		free(data->prompt);
}

void	ft_display_prompt(t_list *data, char **envp)
{
	dup2(data->stdin, STDIN_FILENO);
	dup2(data->stdout, STDOUT_FILENO);
	while (1)
	{
		data->prompt = readline("minishell$> ");
		signal(SIGINT, ft_signal_cmd);
		signal(SIGQUIT, SIG_IGN);
		if (!data->prompt)
			return ;
		handle_command(data, envp);
	}
}

void	add_env_node(char *env_var, t_env_list **env_list)
{
	t_env_list	*new_node;
	t_env_list	*current;

	new_node = create_env_node(env_var);
	if (new_node == NULL)
		return ;
	if (*env_list == NULL)
		*env_list = new_node;
	else
	{
		current = *env_list;
		while (current->next != NULL)
			current = current->next;
		current->next = new_node;
	}
}

void	ft_init_t_env(char **envp)
{
	int			i;
	t_env_list	**env_list;

	env_list = ft_get_adress_env();
	if (env_list != NULL)
		*env_list = NULL;
	if (envp == NULL || env_list == NULL)
		return ;
	i = 0;
	while (envp[i] != NULL)
	{
		add_env_node(envp[i], env_list);
		i++;
	}
}

int	main(int argc, char **argv, char **envp)
{
	t_list	*data;

	if (!argc && !argv)
		return (0);
	g_exit_code = 0;
	data = ft_calloc(1, sizeof(t_list));
	if (!data)
		return (1);
	data->stdin = dup(STDIN_FILENO);
	data->stdout = dup(STDOUT_FILENO);
	ft_init_t_env(envp);
	signal(SIGINT, ft_signal_cmd);
	signal(SIGQUIT, SIG_IGN);
	ft_display_prompt(data, envp);
	if (data)
		ft_free_list(data);
	return (0);
}
