/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_builtins.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vcaratti <vcaratti@student.s19.be>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/20 13:14:50 by vcaratti          #+#    #+#             */
/*   Updated: 2025/01/20 13:31:57 by vcaratti         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/exec.h"

int	is_builtin(t_executor *exec)
{
	if (!exec->exec_args.next)
		return (0);
	if (!exec->exec_args.next->arg)
		return (0);
	if (!ft_strcmp(exec->exec_args.next->arg, "cd")
		|| !ft_strcmp(exec->exec_args.next->arg, "echo")
		|| !ft_strcmp(exec->exec_args.next->arg, "env")
		|| !ft_strcmp(exec->exec_args.next->arg, "export")
		|| !ft_strcmp(exec->exec_args.next->arg, "unset")
		|| !ft_strcmp(exec->exec_args.next->arg, "pwd")
		|| !ft_strcmp(exec->exec_args.next->arg, "exit"))
		return (1);
	return (0);
}

int	ft_arr_len(char **arr)
{
	int	i;

	i = 0;
	if (!arr)
		return (0);
	while (arr[i])
		i++;
	return (i);
}

int	route_builtin(t_executor *exec, char **argv)
{
	char	*arg;

	arg = exec->exec_args.next->arg;
	if (!ft_strcmp(arg, "echo"))
		return (ft_echo(ft_arr_len(argv), argv));
	else if (!ft_strcmp(arg, "env"))
		return (ft_env(exec->env_variables));
	else if (!ft_strcmp(arg, "pwd"))
		return (ft_pwd(exec->env_variables));
	return (0);
}

int	builtin_routine(t_executor *exec)
{
	char	**argv;
	char	*arg;
	int		ret;

	arg = exec->exec_args.next->arg;
	argv = list_to_arr_dup(exec->exec_args.next);
	if (!argv)
		return (1);
	if ((!ft_strcmp(arg, "cd") || !ft_strcmp(arg, "export")
			|| !ft_strcmp(arg, "unset") || !ft_strcmp(arg, "exit"))
		&& (exec->prev || exec->next))
		return (free_nt_arr(argv), 0);
	ret = route_builtin(exec, argv);
	free_nt_arr(argv);
	return (ret);
}
