typedef struct s_exec			t_exec;
typedef struct s_token			t_token;
typedef struct s_commande_line	t_commande_line;
typedef struct s_env			t_env;

struct		s_exec
{
	char	**argv;
	int		fd_in;
	int		fd_out;
	t_exec	*next;
};

struct			s_token
{
	char		*str;
	t_e_token	type;
	bool		expanded;
	t_token		*next;
};

struct				s_commande_line
{
	char			*string;
	t_token			*first_token;
	char			**envp;
	char			**argv;
	int				fd_in;
	int				fd_out;
	char			*name_file;
	t_commande_line	*next;
};

struct		s_env
{
	char	*str;
	int		declare;
	t_env	*next;
};

char	*init_str(char **str, char *duplica, int *cur, char **s1)
{
	*s1 = NULL;
	*cur = 0;
	*str = ft_strdup(duplica);
	free(duplica);
	if (*str == NULL)
		return (NULL);
	return (*str);
}

int	end_modif_two(char *str, t_token **stc, char *s1)
{
	free(str);
	(*stc)->str = s1;
	return (0);
}

int	is_type_file(t_e_token type)
{
	if (type == CREAT_FILE || type == WRITE_FILE
		|| type == OPEN_FILE || type == HERE_DOC)
		return (1);
	return (0);
}

int	ret_file_without_obj(t_e_token type)
{
	write(2, "minishell: erreur de syntaxe",
		ft_strlen("minishell: erreur de syntaxe"));
	if (type == NON)
		write(2, " newline\n", 9);
	if (type == CREAT_FILE)
		write(2, " >\n", 3);
	else if (type == WRITE_FILE)
		write(2, " >>\n", 4);
	else if (type == OPEN_FILE)
		write(2, " <\n", 3);
	else if (type == HERE_DOC)
		write(2, " <<\n", 4);
	return (12);
}

t_e_token	cp_type_change_file(t_e_token type, int *file)
{
	*file = 1;
	return (type);
}

int	ret_error_file_without_file(t_commande_line *cur_b)
{
	write(2, "minishell: erreur de syntaxe",
		ft_strlen("minishell: erreur de syntaxe"));
	if (cur_b)
		write(2, " |\n", 3);
	else
		write(2, " newline\n", 9);
	return (12);
}

t_e_token	change_type_file(t_e_token type, int *file)
{
	*file = 0;
	if (type == OPEN_FILE)
		return (FILE_IN);
	else if (type == CREAT_FILE)
		return (FILE_OUT);
	else if (type == WRITE_FILE)
		return (FILE_OUT_OVER);
	else if (type == HERE_DOC)
		return (LIMITOR);
	return (NON);
}

char	*copy_fake_env(char *str)
{
	int		i;
	char	*cpy;

	i = len_fake_env(str);
	cpy = malloc(sizeof(char) * (i + 1));
	if (cpy == NULL)
		return (NULL);
	cpy = ft_strncpy(cpy, str, i);
	return (cpy);
}

char	*fake_env(int *j, int len_str)
{
	char	*new;

	new = NULL;
	*j = *j + len_str + 1;
	new = malloc(sizeof(char));
	if (new == NULL)
		return (NULL);
	new[0] = '\0';
	return (new);
}

int	len_fake_env(char *str)
{
	int	i;

	i = 0;
	if (str[i] == '$')
		i++;
	while (str && str[i] && str[i] != '$')
	{
		if (str[i] && (ft_is_alpha_num(str[i]) == 1) && str[i] != '_')
			return (i + 1);
		i++;
	}
	if (i == 1)
		return (i + 1);
	return (i);
}

char	*string_env(char *str, char *tmp, int *cur)
{
	char	*tmp2;

	tmp2 = NULL;
	tmp2 = get_env(cur, str);
	if (tmp2 == NULL)
		return (free_str_ret_null(tmp));
	return (tmp2);
}

char	*string_before_env(char *str, char *s1, int *cur, int start)
{
	char	*s2;
	char	*s3;

	s2 = NULL;
	s3 = NULL;
	s2 = malloc(sizeof(char) * ((*cur) - start + 1));
	if (s2 == NULL)
		return (free_str_ret_null(s1));
	s2 = ft_strncpy(s2, str + start, (*cur) - start);
	s3 = ft_strjoin(s1, s2);
	free_both(s1, s2);
	return (s3);
}

char	*end_word_unquote(char *str, char *s1, int *i, int j)
{
	char	*s2;
	char	*s3;

	s2 = NULL;
	s3 = NULL;
	if ((*i) - j == 0)
		return (s1);
	s2 = malloc(sizeof(char) * ((*i) - j + 1));
	if (s2 == NULL)
		return (free_str_ret_null(s1));
	s2 = ft_strncpy(s2, str + j, (*i) - j);
	s3 = ft_strjoin(s1, s2);
	free_both(s1, s2);
	return (s3);
}

char	*free_split_token(char **split, t_token *tok)
{
	free_split(split);
	if (tok)
	{
		if (tok->str)
			free(tok->str);
	}
	return (NULL);
}

static char	*if_no_env(char *str, char *s1, int *cur)
{
	char	*new;
	char	*join;
	int		start;

	start = *cur;
	while (str[*cur] && str[*cur] != '"' && str[*cur] != '$')
		(*cur)++;
	new = malloc(sizeof(char) * ((*cur) - start));
	if (new == NULL)
		return (free_str_ret_null(s1));
	new = ft_strncpy(new, str + start, ((*cur) - start));
	join = ft_strjoin(s1, new);
	free_both(s1, new);
	return (join);
}

static char	*if_env(char *str, char *s1, int *cur)
{
	char	*env;
	char	*join;

	env = string_env(str, s1, cur);
	if (env == NULL)
		return (free_str_ret_null(s1));
	join = ft_strjoin(s1, env);
	free_both(s1, env);
	return (join);
}

char	*word_will_double(char *str, int *cur, char *s1)
{
	while (str && str[*cur] && str[*cur] != '"')
	{
		if (str[*cur] == '$')
		{
			s1 = if_env(str, s1, cur);
			if (s1 == NULL)
				return (NULL);
		}
		else
		{
			s1 = if_no_env(str, s1, cur);
			if (s1 == NULL)
				return (NULL);
		}
	}
	return (s1);
}

char	*word_will_single(char *str, int *i, char *s1)
{
	char	*s2;
	char	*s3;
	int		j;

	s2 = NULL;
	s3 = NULL;
	j = *i;
	while (str && str[(*i)] && str[(*i)] != '\'')
		(*i)++;
	s2 = malloc(sizeof(char) * ((*i) - j + 1));
	if (s2 == NULL)
	{
		if (s1)
			free(s1);
		return (NULL);
	}
	s2 = ft_strncpy(s2, str + j, (*i) - j);
	s3 = ft_strjoin(s1, s2);
	free_both(s1, s2);
	return (s3);
}

char	*free_split_ret_str(char **split, char *str)
{
	free_split(split);
	return (str);
}

char	*new_token_env(t_token **stc, char **split, int i)
{
	t_token	*next;
	t_token	*new;

	if ((*stc))
		next = (*stc)->next;
	while (split[i])
	{
		new = malloc(sizeof(t_token));
		if (new == NULL)
			return (free_split_ret_str(split, NULL));
		init_token(new);
		new->type = ARG;
		new->str = ft_strdup(split[i]);
		if (new->str == NULL)
			if (new)
				return (free_split_token(split, new));
		if ((*stc))
			new->next = next;
		(*stc)->next = new;
		(*stc) = (*stc)->next;
		i++;
	}
	free_split(split);
	return (new->str);
}

static char	*if_no_env(char *str, int *i, char *s1)
{
	char	*new;
	char	*join;
	int		start;

	start = *i;
	while (str[(*i)] && str[(*i)] != '\'' && str[(*i)] != '"'
		&& str[(*i)] != '$')
		(*i)++;
	new = malloc(sizeof(char) * ((*i) - start + 1));
	if (new == NULL)
		return (free_str_ret_null(s1));
	new = ft_strncpy(new, str + start, ((*i) - start));
	join = ft_strjoin(s1, new);
	free_both(s1, new);
	return (join);
}

static char	*if_env(t_token **stc, char *str, char *s1, int *i)
{
	char	*env;
	char	**split;
	char	*join;

	env = string_env(str, s1, i);
	if (env == NULL)
		return (free_str_ret_null(s1));
	split = ft_strsplit(env, ' ');
	free(env);
	if (split == NULL)
		return (free_str_ret_null(s1));
	join = ft_strjoin(s1, split[0]);
	if (s1 == NULL && split[0] == NULL)
	{
		join = malloc(sizeof(char));
		join[0] = '\0';
	}
	free(s1);
	if (split[0] == NULL || split[1] == NULL)
		return (free_split_ret_str(split, join));
	(*stc)->str = join;
	return (new_token_env(stc, split, 1));
}

char	*word_will_unquote(t_token **stc, char *str, int *cur, char *s1)
{
	while (str[*cur] && str[*cur] != '\'' && str[*cur] != '"')
	{
		if (str[*cur] != '$')
		{
			s1 = if_no_env(str, cur, s1);
			if (s1 == NULL)
				return (NULL);
		}
		else if (str[*cur] && str[*cur] == '$')
		{
			s1 = if_env(stc, str, s1, cur);
			if (s1 == NULL)
			{
				return (NULL);
			}
		}
	}
	return (s1);
}

int	move_to(char *str, int *i, t_quote quote)
{
	if (quote == NONE)
	{
		while (str[(*i)] && str[(*i)] != '\'' && str[(*i)] != '"')
			(*i)++;
	}
	else if (quote == SINGLE)
	{
		while (str[(*i)] && str[(*i)] != '\'')
			(*i)++;
	}
	else if (quote == DOUBLE)
	{
		while (str[(*i)] && str[(*i)] != '"')
			(*i)++;
	}
	return (0);
}

char	*limitor_unquote(char *str, int *i, char *s1, t_quote quote)
{
	int		j;
	char	*s2;
	char	*s3;

	s2 = NULL;
	s3 = NULL;
	j = *i;
	move_to(str, i, quote);
	s2 = malloc(sizeof(char) * ((*i) - j + 1));
	if (s2 == NULL)
		return (free_str_ret_null(s1));
	s2 = ft_strncpy(s2, str + j, (*i) - j);
	s3 = ft_strjoin(s1, s2);
	free_both(s1, s2);
	if (s3 == NULL)
		return (NULL);
	return (s3);
}

char	*expand_full_quote_str(void)
{
	char	*new;

	new = NULL;
	new = malloc(sizeof(char));
	if (new == NULL)
		return (NULL);
	new[0] = '\0';
	return (new);
}

char	*devellope_limitor(t_token *stc, char *str, int i)
{
	char	*expand_str;
	t_quote	quote;
	t_quote	prec;

	quote = NONE;
	prec = NONE;
	expand_str = NULL;
	while (str && str[i])
	{
		quote = update_quote_status(str[i], quote);
		if (prec != quote)
			prec = update_quote_succes(stc, &i, quote, &expand_str);
		else
		{
			expand_str = limitor_unquote(str, &i, expand_str, quote);
			if (expand_str == NULL)
				return (NULL);
		}
	}
	if (expand_str == NULL)
		return (expand_full_quote_str());
	return (expand_str);
}

int	limitor(t_token *stc, char *str)
{
	char	*new;

	new = NULL;
	new = devellope_limitor(stc, str, 0);
	if (str)
		free(str);
	if (new == NULL)
		return (50);
	stc->str = new;
	return (0);
}

int	word_modif_two(t_token **stc, char *duplica, t_quote quote, t_quote prec)
{
	char	*s1;
	int		cur;
	char	*str;

	if (init_str(&str, duplica, &cur, &s1) == NULL)
		return (50);
	while (str[cur])
	{
		quote = update_quote_status(str[cur], quote);
		if (prec != quote)
			prec = update_quote_succes(*stc, &cur, quote, &s1);
		else
		{
			if (quote == SINGLE)
				s1 = word_will_single(str, &cur, s1);
			else if (quote == DOUBLE)
				s1 = word_will_double(str, &cur, s1);
			else if (quote == NONE)
				s1 = word_will_unquote(stc, str, &cur, s1);
		}
		if (s1 == NULL)
			return (free_str_ret_malloc_error(str));
	}
	return (end_modif_two(str, stc, s1));
}

int	word_modif(t_token **stc, char *str, t_e_token token)
{
	t_quote	quote;
	t_quote	prec;

	quote = NONE;
	prec = NONE;
	if (token == LIMITOR)
		return (limitor(*stc, str));
	return (word_modif_two(stc, str, quote, prec));
}

int	edit_type(t_commande_line **block, int limiter)
{
	t_commande_line	*cur_b;
	t_token			*cur_t;

	cur_b = *block;
	while (cur_b)
	{
		cur_t = cur_b->first_token;
		while (cur_t)
		{
			if (cur_t->type == HERE_DOC)
				limiter = 1;
			else if (limiter == 1 && cur_t->str && cur_t->str[0] != '\0')
			{
				if (is_type_file(cur_t->type) == 1)
					return (ret_file_without_obj(cur_t->type));
				cur_t->type = LIMITOR;
				limiter = 0;
			}
			cur_t = cur_t->next;
		}
		if (limiter == 1)
			return (ret_file_without_obj(NON));
		cur_b = cur_b->next;
	}
	return (0);
}

int	check_open_fil(t_commande_line **block)
{
	t_commande_line	*cur_b;
	t_token			*cur_t;
	int				file;
	t_e_token		type;

	cur_b = *block;
	file = 0;
	while (cur_b)
	{
		cur_t = cur_b->first_token;
		while (cur_t)
		{
			if (is_type_file(cur_t->type) == 1 && file == 1)
				return (ret_file_without_obj(cur_t->type));
			else if (is_type_file(cur_t->type) == 1)
				type = cp_type_change_file(cur_t->type, &file);
			else if (file == 1 && cur_t->str && (cur_t->str[0] != '\0'))
				cur_t->type = change_type_file(type, &file);
			cur_t = cur_t->next;
		}
		if (file == 1)
			return (ret_error_file_without_file(cur_b->next));
		cur_b = cur_b->next;
	}
	return (0);
}

int	expend_words(t_commande_line **block)
{
	t_commande_line	*cur_b;
	t_token			*cur_t;
	int				res;

	cur_b = *block;
	if (edit_type(block, 0) != 0 || check_open_fil(block) != 0)
		return (12);
	res = 0;
	while (cur_b)
	{
		cur_t = cur_b->first_token;
		while (cur_t)
		{
			if (cur_t->str && cur_t->str[0] != '\0')
			{
				res = word_modif(&cur_t, cur_t->str, cur_t->type);
				if (res != 0)
					return (50);
			}
			cur_t = cur_t->next;
		}
		cur_b = cur_b->next;
	}
	return (0);
}
