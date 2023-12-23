/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jegoh <jegoh@student.42singapore.sg>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/21 21:34:32 by jegoh             #+#    #+#             */
/*   Updated: 2023/12/23 17:06:39 by jegoh            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */
#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft.h"
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/wait.h>
# include <signal.h>
# include <fcntl.h>
# include <dirent.h>
# include <string.h>
# include <errno.h>
# include <termios.h>
# include <sys/stat.h>
# include <limits.h>
# include <readline/readline.h>
# include <readline/history.h>

# define NAME_SIZE 256
# define MAX_INPUT_LENGTH 1024

typedef struct s_env_var
{
	char	*key;
	char	*value;
}	t_env_var;

typedef struct s_env_list
{
	t_env_var			env_var;
	struct s_env_list	*next;
}	t_env_list;

typedef struct s_history
{
	char				*command;
	struct s_history	*next;
	struct s_history	*prev;
}	t_history;

typedef struct s_list
{
	char		*prompt;
	char		*path;
	char		**commandsarr;
	char		**execcmds;
	int			pipefd[2];
	int			stdin;
	int			stdout;
	int			i;
	int			inputfd;
	int			original_stdout;
	int			redirection_active;
	t_history	*history;
	t_env_list	*env_vars;
}	t_list;

extern int	g_exit_code;

void signal_cmd(int sig);
void signal_cmd_pipe(int sig);
t_env_list **get_adress_env(void);
void free_env_var(t_env_var *env_var);
void free_history(t_history *history);
void check_if_list_is_null(t_list *list);
void ft_free_list(t_list *list);
char *join_paths_and_check_access(char **splitpath, t_list *data);
char *handle_special_cases_and_cleanup(char **splitpath, t_list *data);
char *ft_getenv(const char *name);
char *ft_getpath(t_list *data);
int checkforpipe(char *s);
int count_substring(const char *orig, const char *rep);
char *perform_replacement(const char *orig, const char *rep, const char *with, int count);
char *process_env_var(char **p, char *result, char *temp);
size_t calculate_expansion_size(char *s);
char *find_next_variable(char *s, size_t *var_len);
void handle_variable(char **src, char **dest, char *start, size_t var_len);
void process_string(char *src, char *dest);
char *expand_env_vars(char *str);
char *expand_env_variables(char *arg, int expand);
int process_quotes(char *cmd_line);
int process_and_split_command(t_list *data, char ***cmd_parts);
int expand_variables_and_count_pipes(char **cmd_parts, t_list *data, int *numofpipes);
int process_command_parts(t_list *data, char **cmd_parts, int *numofpipes);
char *concatenate(const char *str1, const char *str2);
char *find_command_in_path(char *cmd);
char *str_replace(char *orig, char *rep, char *with);
void replace_exit_status(char **command, char *exit_status);
void process_commands(t_list *data, char **envp, char **strarr, int numofpipes);
int check_if_pipe_is_valid(char **strarr, int numofpipes);
int execute_piped_commands(t_list *data, char **envp, int numofpipes);
int execute_multiple_commands(t_list *data, char **envp, int numofpipes);
int getcmd(t_list *data, char **envp);
void remove_quotes_from_arg(char *arg);
void remove_quotes_from_args(char **args);
int open_temp_file(const char *tmpfile);
char *read_and_process_input(const char *delimiter, t_list *data);
int write_to_file(int tmpfd, const char *input, const char *tmpfile);
int write_user_input_to_file(int tmpfd, const char *delimiter, const char *tmpfile, t_list *data);
int create_and_write_heredoc(char *delimiter, const char *tmpfile, t_list *data);
void setup_heredoc_fd(int tmpfd, char *tmpfile);
void handle_heredoc(char *delimiter, t_list *data);
void handle_append_redirection(char *filename);
char *copy_infile_name(t_list *data, int index, int i);
void open_infile_and_redirect_input(t_list *data, int index);
void move_commands_forward(t_list *data, int index);
void handle_input_redirection(t_list *data, int index);
void move_forward_and_check_for_null(t_list *data, int index);
void reassign_and_handle_heredoc(t_list *data, int index);
void open_output_file(t_list *data, int index);
char **remove_arrow_and_outfile_strings(t_list *data);
void handle_output_redirection(t_list *data, int index);
void move_forward_and_check_for_append(t_list *data, int index);
void reassign_and_handle_append(t_list *data, int index);
void reassign(t_list *data, int flag, int index);
void check_if_redirection_is_last_arg(char *s);
void inputredirection(t_list *data);
void outputredirection(t_list *data);
int check_for_redirection(char **s);
void prepare_command_arrays(t_list *data);
void handle_parent_process(t_list *data, int type, int id);
void execute_buildin2(t_list *data);
void execute_buildin(t_list *data);
void check_for_redirection_and_close_pipe(t_list *data, int type);
void handle_child_process(t_list *data, char **envp, int type, struct stat buff);
void manage_process_and_signals(t_list *data, int type, char **envp);
void executecommands(t_list *data, char **envp, int type);
int checkempty(char *s);
void ft_add_to_history(t_list *data, char *command);
void ft_display_history(t_list *data);
char *parse_env_var(const char *input);
char *resolve_path(char *path);
char *validate_and_resolve_path(char **args);
int change_directory(char *path);
int checkdir(char **args);
int update_quote(int *i, char c, int *in_single_quote, int *in_double_quote);
void check_quotes(int *i, int *output_index, char *output);
void parse_and_print_echo(char *input);
void process_split_temp(char **split_temp, int expand);
void echo_out(char **str, int pos);
int process_flags(char **args, int *n_flag);
int ft_echo(char **args);
int ft_pwd(void);
t_env_list *create_env_node(char *env_str);
int is_valid_identifier(char *str);
int parse_export_argument(char *arg, char **key, char **value);
int update_env_var(char *key, char *value, t_env_list **env_list);
char *create_env_str(char *key, char *value);
void add_new_env_var(char *key, char *value, t_env_list **env_list);
void add_to_env_list(char *key, char *value, t_env_list **env_list);
void ft_export(char *arg);
void free_current(t_env_list *current);
void check_env_list_and_iterate(t_env_list **env_list, t_env_list *current, t_env_list *prev);
void ft_unset(char **args);
void ft_env(void);
int is_valid_number(char *str);
void strip_quotes(char *str);
char *validate_exit_argument(t_list *data);
int process_exit_args(t_list *data);
void ft_exit(t_list *data);
void parse_quotes(const char *prompt, int *in_single_quote, int *in_double_quote, int i);
int calculate_new_length(const char *prompt);
void update_quote_flags(const char *prompt, int *in_single_quote, int *in_double_quote, int index);
void add_spaces(const char *prompt, char *s, int *in_single_quote, int *in_double_quote);
void check_for_quotes_and_add_spaces(const char *prompt, char *s);
char *reassign_prompt(const char *prompt);
void parse_for_comments(char **input);
void handle_external_commands(t_list *data, char **envp);
void execute_specific_command(t_list *data, char **envp);
void expand_command_arguments(char **args);
void handle_redirections(t_list *data, int i);
void update_commands_array(t_list *data, int i);
void setup_redirections(t_list *data);
void restore_stdout(t_list *data);
void execute_command(t_list *data, char **envp);
void handle_command(t_list *data, char **envp);
void ft_display_prompt(t_list *data, char **envp);
void add_env_node(char *env_var, t_env_list **env_list);
void ft_init_t_env(char **envp);
int	ft_cd(char **args);
void	ft_signal_cmd(int sig);
void	ft_signal_cmd_pipe(int sig);
t_env_list	**ft_get_adress_env(void);
void	ft_free_current(t_env_list *current);

#endif
