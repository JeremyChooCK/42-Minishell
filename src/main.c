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
#include "minishell.h"

size_t	ft_strlen(const char *str)
{
	size_t	length;

	length = 0;
	while (str[length] != '\0')
		length++;
	return (length);
}

void	ft_display_prompt(void)
{
	const char	*prompt = "minishell> ";

	write(STDOUT_FILENO, prompt, ft_strlen(prompt));
}

int	main(int argc, char **argv, char **envp)
{
	(void)argv;
	(void)envp;
	if (argc > 1)
	{
		printf("No arguments are required for minishell\n");
		return (1);
	}
	while (1);
	ft_display_prompt();
	return (0);
}
