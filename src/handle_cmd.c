/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_cmd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wel-safa <wel-safa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/24 14:03:48 by mrodenbu          #+#    #+#             */
/*   Updated: 2024/09/22 21:34:18 by wel-safa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	check_for_builtin(char *cmd, char **builtins)
{
	int	i;

	i = 0;
	while (i < 7)
	{
		if (ft_strncmp(cmd, builtins[i], ft_strlen(builtins[i]) + 1) == 0)
			return (1);
		i++;
	}
	return (0);
}

/*checks is path str is a directory. Returns 1 if true and 0 if false*/
int	check_for_dir(char *str)
{
	struct stat	path_stat;

	if (access(str, F_OK) == 0 && access(str, R_OK) == 0)
	{
		stat(str, &path_stat);
		if (S_ISDIR(path_stat.st_mode))
			return (1);
		else
			return (0);
	}
	return (0);
}

void	handle_path(t_node *curr, char *str)
{
	if (access(str, F_OK) == 0)
	{
		if (check_for_dir(str))
		{
			curr->err_flag = 126;
			errno = EISDIR;
			perror("minishell");
		}
		else if (access(str, X_OK) == 0)
		{
			curr->cmd = ft_strdup(str);
			curr->cmd_flag = PATH;
		}
		else
		{
			curr->err_flag = 126;
			perror("minishell");
		}
	}
	else
	{
		curr->err_flag = 127;
		perror("minishell");
	}
}

void	handle_cmd(t_state *data, t_node *curr, char *str)
{
	if (!str)
		curr->cmd_flag = NO_CMD;
	else if (check_for_builtin(str, data->builtins))
	{
		curr->cmd = ft_strdup(str);
		curr->cmd_flag = BUILTIN;
	}
	else if (ft_strchr(str, '/'))
		handle_path(curr, str);
	else
	{
		curr->cmd = get_path(data, str);
		if (curr->cmd)
			curr->cmd_flag = PATH;
		else
			curr->err_flag = 127;
	}
}

void	cmd_loop(t_state *state)
{
	t_list	*cmd;
	char	*str;

	cmd = state->cmds;
	while (cmd)
	{
		if (((t_node *)cmd->content)->words)
			str = ((t_node *)cmd->content)->words->content;
		else
			str = NULL;
		handle_cmd(state, (t_node *)cmd->content, str);
		wordlist_to_cmdarr(state, (t_node *)cmd->content);
		cmd = cmd->next;
	}
	state->num_of_processes = ft_lstsize(state->cmds);
}
