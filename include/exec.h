/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcaratti <vcaratti@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 14:37:51 by vcaratti          #+#    #+#             */
/*   Updated: 2025/01/15 14:42:48 by vcaratti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXEC_H
# define EXEC_H

# include "../libft/include/libft.h"
# include "hashmap.h"
# include "minishell.h"
# include "get_next_line.h"
# include <readline/readline.h>
# include <stdlib.h>
# include <stdio.h>
# include <sys/wait.h>
# include <sys/types.h>

typedef struct s_cmd
{
	char	*path;
	char	**args;
}	t_cmd;

typedef struct s_elist
{
	struct s_elist	*prev;
	struct s_elist	*next;
	char	*arg;
	char	mode;
}	t_elist;

typedef	struct s_executor
{
	struct s_executor	*prev;
	struct s_executor	*next;
	t_elist			infiles;
	t_elist			outfiles;
	t_elist			exec_args;
	t_cmd			cmd;
	char			**envp;
	t_hashmap		*env_variables;
	int			fid;
	int			pipes[2];
	int			fds[2];
	int			heredoc_p[2];
	int			bad_command;
}	t_executor;


int	is_builtin(t_executor *exec);


//#===#		executor.c		#===#//

int	exec_routine(t_executor *exec);
int	start_pipes(t_executor **exec_head);
int	executor(char **args, char **envp, t_hashmap *env_variables, int *exec_ret);

//#===#		exec_free.c		#===#//

int		free_and_ret(char **to_free, int ret);
void	free_all(t_executor *head);
void	free_nt_arr(char **arr);
void	free_list(t_elist *head);
void	free_list_node(t_elist *list);

//#===#		cmd_path.c		#===#//

int	treat_cmd(t_executor *exec);
char	*join_path_cmd(char *path, char *cmd);
int	find_path(char *cmd, char **paths, char **ret);
int	cmd_path(char *cmd, t_hashmap *env_variables, char **ret);

//#===#		list_tools.c	#===#//

int	list_len(t_elist *head);
char	**list_to_arr(t_elist *head);
char	**list_to_arr_dup(t_elist *head);
t_elist	*list_pop(t_elist *elem);
int	list_append_arg(t_elist *lst, char *arg);
void	list_append(t_elist *lst, t_elist *node);

//#===#		exec_init.c		#===#//

int	init_children_pipes(t_executor *exec_head);
int	init_cmd_args(t_executor *exec_head);
int	create_exec(t_executor **ret, t_executor *p, t_executor *n, char **envp, t_hashmap *env_variables);
int	fetch_redirect(t_elist *current, t_executor *current_exec);
int	list_init(t_elist *args_head, char **args);

//#===#		exec_tools.c	#===#//

int	has_special_c(char *str);
char	*trim_redirect(char *str);
int	get_nb_exec(t_executor *exec_head);

//#===#		exec_io.c		#===#//

int	open_outfiles(t_executor *exec);
int	open_infiles(t_executor *exec);
void	close_all_except(t_executor *exec);

//#===#		heredoc.c		#===#//

int	heredoc_check(t_elist **node, t_executor *exec);
int	init_heredocs(t_executor *exec_head);
int	heredoc(int fd, char *eof, t_hashmap *env);

//#===#		exec_builtins.c		#===#//

int	is_builtin(t_executor *exec);
int	ft_arr_len(char **arr);
int	route_builtin(t_executor *exec, char **argv);
int	builtin_routine(t_executor *exec);

#endif
