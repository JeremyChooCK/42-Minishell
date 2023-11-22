/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jegoh <jegoh@student.42singapore.sg>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/21 21:34:32 by jegoh             #+#    #+#             */
/*   Updated: 2023/11/22 21:41:00 by jegoh            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#ifndef LIBFT_H
# define LIBFT_H

# include <stdlib.h>

size_t	ft_strlen(const char *str);
char	**ft_split(char const *s, char c);
char	*ft_strjoin( char const *s1, char const *s2);
int		ft_strcmp(char *s1, char *s2);
char	*ft_strdup(const char *s1);

#endif
