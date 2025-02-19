/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcaratti <vcaratti@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/02 11:37:53 by vcaratti          #+#    #+#             */
/*   Updated: 2025/01/22 12:39:19 by vcaratti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/exec.h"

int	load_prepipes(t_executor **exec_head, int *exec_ret)
{
	int	ret;

	ret = look_for_builtin(*exec_head);
	if (ret == 1)
		return (1);
	if (ret == 2)
		return (2);
	ret = init_heredocs(*exec_head);
	if (ret == 1)
		return (2);
	if (ret == 2)
	{
		*exec_ret = 2;
		return (1);
	}
	ret = init_cmd_args(*exec_head);
	if (ret == 1)
		return (1);
	if (ret == 2)
		return (2);
	if (init_children_pipes(*exec_head))
		return (2);
	return (0);
}

int	start_pipes(t_executor **exec_head, int *exec_ret)
{
	int			ret;
	t_executor	*current;

	signal(SIGINT, ignore_signal);
	signal(SIGKILL, ignore_signal);
	ret = load_prepipes(exec_head, exec_ret);
	if (ret)
		return (ret - 1);
	current = *exec_head;
	while (current)
	{
		current->fid = fork();
		if (current->fid == -1)
			return (3);
		if (current->fid == 0)
		{
			if (exec_routine(current))
				exit(1);
		}
		current = current->next;
	}
	return (0);
}

void	close_exec_fds(t_executor *current)
{
	if (current->fds[0] > 1)
		close(current->fds[0]);
	if (current->fds[1] > 1)
		close(current->fds[1]);
	if (current->pipes[0] > 1)
		close(current->pipes[0]);
	if (current->pipes[1] > 1)
		close(current->pipes[1]);
	if (current->heredoc_p[0] > -1)
		close(current->heredoc_p[0]);
	if (current->heredoc_p[1] > -1)
		close(current->heredoc_p[1]);
}

int	close_wait(t_executor *exec_head)
{
	t_executor	*current;
	int			ret;

	current = exec_head;
	ret = 0;
	while (current)
	{
		close_exec_fds(current);
		current = current->next;
	}
	current = exec_head;
	while (current)
	{
		waitpid(current->fid, &ret, WCONTINUED);
		current = current->next;
	}
	return (ret);
}

int	executor(char **args, t_hashmap *env_variables, int *exec_ret)
{
	t_executor	*exec_head;
	int			tokenise_ret;

	tokenise_ret = tokenise(args, &exec_head, env_variables);
	if (tokenise_ret == 1)
	{
		*exec_ret = 512;
		return (0);
	}
	if (tokenise_ret)
		return (tokenise_ret - 1);
	if (start_pipes(&exec_head, exec_ret))
		return (free_all(&exec_head), 1);
	if (*exec_ret)
		return (free_all(&exec_head), 0);
	*exec_ret = close_wait(exec_head);
	if (check_exit(exec_head, exec_ret))
		return (free_all(&exec_head), 1);
	free_all(&exec_head);
	return (0);
}
