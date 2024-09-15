/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections_utils.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wel-safa <wel-safa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/18 19:53:40 by wel-safa          #+#    #+#             */
/*   Updated: 2024/08/19 20:54:16 by wel-safa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* sets the file descriptor for the infile "file" */
int	set_fd_in(t_state *state, t_node *curr, char *file)
{
	if (access(file, F_OK) == -1)
	{
		// ERROR FILE DOES NOT EXIST
		curr->err_flag = 1;
		perror(" ");
		// MARC START
		// added t_state *state as argument to have acces to exit status
		state->exit_status = 1;
		// added return value to be able to break redirection loop in case of failure
		return (1);
		// MARC END
	}
	else
	{
		if (access(file, R_OK) == -1)
		{
			// ERROR NOT READABLE
			curr->err_flag = 1;
			perror(" ");
		}
		else
		{
			if (curr->fd_in > 0)
				close(curr->fd_in);
			curr->fd_in = open(file, O_RDONLY);
		}
	}
	// MARC START
	return (0);
	// MARC END
}

/* set the file descriptor for the outfile "file" */
int	set_fd_out(t_node *curr, char *file, int append)
{
	if (access(file, F_OK) == -1)
	{
		curr->fd_out = open(file, O_WRONLY | O_CREAT, 0644);
	}
	else
	{
		if (access(file, W_OK) == -1)
		{
			// ERROR FILE NOT WRITABLE
			curr->err_flag = 1;
			perror(" ");
			// MARC START
			return (1);
			// MARC END
		}
		else
		{
			if (curr->fd_out != STDOUT_FILENO)
				close(curr->fd_out);
			if (append)
				curr->fd_out = open(file, O_WRONLY | O_APPEND);
			else
				curr->fd_out = open(file, O_WRONLY | O_TRUNC);
		}
	}
	// MARC START
	return (0);
	// MARC END
}

/*takes string filename and cuts out spaces in beginning and at end
returns 0 on sucess and 1 on error if what's left is an empty string*/
int	filename_cut_spaces(char **filename)
{
	// need to test with filename that is one letter and with empty string filename, etc.
	int	i;
	int	len;

	i = 0;
	while((*filename)[i] == ' ')
		i++;
	*filename = strreplace(filename, NULL, 0, i - 1);
	len = (int) ft_strlen(*filename);
	if (len == 0)
		return (1); // empty string
	i = len - 1;
	while((*filename)[i] == ' ')
	{
		if (i == 0)
			return (1); // empty string
		i--;
	}
	*filename = strreplace(filename, NULL, i + 1, len - 1);
	return (0);
}

/*takes string filename, cuts out spaces in beginning and end and
returns 1 on error and 0 on no error
errors:
1- empty string after expansion
2- more than one word after expansion*/
int		filename_expansion_error(char **filename)
{
	int	i;
	int	sq_flag;
	int	dq_flag;

	sq_flag = 0;
	dq_flag = 0;
	if (!filename)
		return (1);
	if (!(*filename))
		return (1);
	if (!ft_strlen(*filename))
		return (1);
	if (filename_cut_spaces(filename))
		return (1);
	i = 0;
	while((*filename)[i])
	{
		if ((*filename)[i] == '\'' && !dq_flag)
			sq_flag = !sq_flag;
		else if ((*filename)[i] == '\"' && !sq_flag)
			dq_flag = !dq_flag;
		if ((*filename)[i] == ' ' && !sq_flag && !dq_flag)
			return (1);
		i++;
	}
	return (0);
}
