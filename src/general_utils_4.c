/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   general_utils_4.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jechoo <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/23 20:17:38 by jechoo            #+#    #+#             */
/*   Updated: 2023/12/23 20:18:19 by jechoo           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
