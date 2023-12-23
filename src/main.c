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
