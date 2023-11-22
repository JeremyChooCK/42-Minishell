/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jegoh <jegoh@student.42singapore.sg>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/21 21:45:15 by jegoh             #+#    #+#             */
/*   Updated: 2023/11/22 21:11:51 by jegoh            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#include "minishell.h"

// TODO fix segmentation error when invalid command is provided
// TODO cd recursively does not work need to fix it
char	*remove_dotdot_slash_and_goback_one_dir(char *s, char *cwd)
{
	char	*result;
	char	*temp;
	int		i;
	int		j;

	i = 3;
	j = 0;
	result = malloc(ft_strlen(s) - 1);
	while (s[i])
		result[j++] = s[i++];
	result[j] = '\0';
	free(s);
	i = ft_strlen(cwd);
	while (cwd[i] != '/')
		i--;
	temp = malloc(i + 1);
	j = -1;
	while (++j < i)
		temp[j] = cwd[j];
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
	result = malloc(ft_strlen(s) - 1);
	while (s[i])
		result[j++] = s[i++];
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
			s = removedotslash(s);
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
}

char	*getpath(t_list *data)
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
	while (splitpath[i++] != NULL)
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
	}
	j = 0;
	while (splitpath[j])
		free(splitpath[j++]);
	free(splitpath);
	return (NULL);
}

void	getcmd(t_list *data)
{
	data->commandsarr = ft_split(data->prompt, ' ');
	if (data->commandsarr == NULL)
	{
		fprintf(stderr, "Error splitting command input.\n");
		return ;
	}
}

void	executecommands(t_list *data, char **envp)
{
	int	id;

	data->execcmds[0] = data->path;
	data->path = NULL;
	data->execcmds[1] = data->commandsarr[1];
	data->execcmds[2] = NULL;
	id = fork();
	if (id == 0)
		execve(data->execcmds[0], data->execcmds, envp);
	else
		wait(NULL);
}

int	checkempty(char *s)
{
	size_t	i;

	i = 0;
	while (s[i] == ' ' || s[i] == '\t')
		i++;
	return (i == ft_strlen(s));
}

void	ft_display_prompt(t_list *data, char **envp)
{
	char	buf[1000];
	char	*cwd;
	int		i;

	while (1)
	{
		data->prompt = readline("minishell> ");
		if (!data->prompt)
			break ;
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
				checkdir(data->commandsarr[1], cwd);
			else
			{
				data->path = getpath(data);
				if (data->prompt && *data->prompt)
					add_history(data->prompt);
				if (data->path)
				{
					executecommands(data, envp);
					free(data->path);
				}
				else
					printf("Command not found: %s\n", data->commandsarr[0]);
				i = 0;
				while (data->commandsarr[i])
					free(data->commandsarr[i++]);
				free(data->commandsarr);
			}
		}
		free(data->path);
		free(data->prompt);
	}
}

int	main(int argc, char **argv, char **envp)
{
	t_list	*data;

	(void)argv;
	if (argc > 1)
	{
		printf("No arguments are required for minishell\n");
		return (1);
	}
	data = malloc(sizeof(t_list));
	if (!data)
	{
		printf("Memory allocation failed\n");
		return (1);
	}
	ft_display_prompt(data, envp);
	free(data);
	return (0);
}
