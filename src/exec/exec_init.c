/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_init.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcaratti <vcaratti@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 14:46:02 by vcaratti          #+#    #+#             */
/*   Updated: 2025/01/14 12:09:57 by vcaratti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/exec.h"

int	init_children_pipes(t_executor *exec_head)
{
	int		last_stdout;
	int		created_fds[2];
	t_executor	*current;

	current = exec_head;
	last_stdout = -1;
	while (current)
	{
		if (!current->next)
		{
			current->pipes[0] = last_stdout;
			current->pipes[1] = -1;
			break;
		}	
		if (pipe(created_fds) == -1)
			return (1);//free
		current->pipes[1] = created_fds[1];
		current->pipes[0] = last_stdout;
		last_stdout = created_fds[0];
		current = current->next;
	}
	return (0);
}

int	init_cmd_args(t_executor *exec_head)
{
	t_executor	*current;
	int		cmd_init_ret;

	current = exec_head;
	while (current)
	{	if (!is_builtin(current) && current->exec_args.next)
		{
			cmd_init_ret = treat_cmd(current);
			//if (cmd_init_ret == 3)
			//	return (0);
			if (cmd_init_ret)
				return (1);
		}
		current = current->next;
	}
	return (0);
}

int	create_exec(t_executor **ret, t_executor *p, t_executor *n, char **envp, t_hashmap *env_variables)
{
	*ret = malloc(sizeof(t_executor));
	if (!(*ret))
		return (1);
	(*ret)->prev = p;
	(*ret)->next = n;
	(*ret)->envp = hm_to_array(env_variables);
	(void)envp;
	if (!(*ret)->envp)
		return (free(*ret), 1);
	(*ret)->env_variables = env_variables;
	(*ret)->infiles.next = 0;
	(*ret)->infiles.prev = 0;
	(*ret)->infiles.arg = 0;
	(*ret)->outfiles.next = 0;
	(*ret)->outfiles.prev = 0;
	(*ret)->outfiles.arg = 0;
	(*ret)->exec_args.prev = 0;
	(*ret)->exec_args.next = 0;
	(*ret)->cmd.path = 0;
	(*ret)->cmd.args = 0;
	(*ret)->pipes[0] = -1;
	(*ret)->pipes[1] = -1;
	(*ret)->fds[0] = -1;
	(*ret)->fds[1] = -1;
	(*ret)->heredoc_p[0] = -1;
	(*ret)->heredoc_p[1] = -1;
	(*ret)->bad_command = 0;
	return (0);
}

t_elist *list_pop(t_elist *node)
{
	if (!node->prev)
		return (NULL);
	if (node->prev)
		node->prev->next = node->next;
	if (node->next)
		node->next->prev = node->prev;
	return (node);
}

int	pop_append(t_elist *args_head, t_elist *to_append)
{
	t_elist	*current;

	current = args_head->next;
	if (!current->next)
		return (1);//no infile/outfile specified
	if (has_special_c(current->next->arg))
		return (1);
	free_list_node(list_pop(args_head->next));
	list_append(to_append, list_pop(args_head->next));
	return (0);
}

int	fetch_redirect(t_elist *args_head, t_executor *current_exec)
{
	t_elist	*temp;
	char	mode;

	mode = 0;
	if (!args_head->next->next)
		return (1);//nothing after redirect
	if (args_head->next->arg[0] == '<')
	{
		if (args_head->next->arg[1] == '<')
			mode = 'h';
		temp = list_pop(args_head->next->next);
		temp->mode = mode;
		list_append(&(current_exec->infiles), temp);
	}
	else if (args_head->next->arg[0] == '>')
	{
		if (args_head->next->arg[1] == '>')
			mode = 'a';
		temp = list_pop(args_head->next->next);
		temp->mode = mode;
		list_append(&(current_exec->outfiles), temp);
	}
	free_list_node(list_pop(args_head->next));
	return (0);
}

int	list_init(t_elist *args_head, char **args)
{
	int	i;

	args_head->arg = 0;	
	args_head->next = 0;
	args_head->prev = 0;
	i = 0;
	while (args[i])
	{
		if (list_append_arg(args_head, args[i++]))
			return (1);//
	}
	if (i == 0)
		return (1);
	return (0);
}
