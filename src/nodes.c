/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   nodes.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wel-safa <wel-safa@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/07 15:19:55 by wel-safa          #+#    #+#             */
/*   Updated: 2024/09/22 21:29:49 by wel-safa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*mallocs t_node variable and initializes struct variables to default states*/
t_node	*initialize_node(t_state *state)
{
	t_node	*node;

	node = (t_node *) malloc(sizeof(t_node));
	if (!node)
		error_exit(state);
	node->args = NULL;
	node->cmd = NULL;
	node->cmd_flag = NO_CMD;
	node->err_flag = CMD_OK;
	node->fd_in = STDIN_FILENO;
	node->fd_out = STDOUT_FILENO;
	node->hd_content = NULL;
	node->hd_expand_flag = 1;
	node->words = NULL;
	return (node);
}

void	remove_pipe(t_node *node)
{
	t_list	*word;

	word = node->words;
	while (word)
	{
		if (word->next->next == NULL)
		{
			ft_lstdelone(word->next, free);
			word->next = NULL;
			return ;
		}
		word = word->next;
	}
}

/*creates t_list item cmd and adds it to the cmd list in state variable
copies words into the command node
and removes pipe from list of words if pipe = 1
(i.e. if it is not the last command in the input string)*/
void	create_node(t_state *state, int pipe)
{
	t_node	*node;
	t_list	*cmd;

	cmd = (t_list *) malloc(sizeof(t_list));
	if (!cmd)
		error_exit(state);
	cmd->next = NULL;
	node = initialize_node(state);
	node->words = state->words;
	if (pipe)
		remove_pipe(node);
	cmd->content = node;
	ft_lstadd_back(&(state->cmds), cmd);
}

/*takes state and splits word list into multiple lists per cmd
creates cmd struct and sets state->words to null*/
void	nodes(t_state *state)
{
	t_list	*word;
	t_list	*nextword;

	word = state->words;
	while (word)
	{
		if (ft_strncmp((char *) word->content, "|", 1) == 0)
		{
			nextword = word->next;
			word->next = NULL;
			create_node(state, 1);
			state->words = nextword;
			word = state->words;
		}
		else if (word->next == NULL)
		{
			create_node(state, 0);
			word = word->next;
		}
		else
			word = word->next;
	}
	state->words = NULL;
}
