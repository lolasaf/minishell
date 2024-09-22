/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   splitting.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wel-safa <wel-safa@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/30 23:39:02 by wel-safa          #+#    #+#             */
/*   Updated: 2024/08/20 23:31:07 by wel-safa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*takes start index s and end index e and
string pointer word and t_list double pointer newwordlist
it creates new word from index s to e of char **word
and adds it to newwordlist*/
void	split_words_create(t_state *state, int s, int e, char **word, t_list **newwordlist)
{
	t_list	*newword;

	if (s > ((int)ft_strlen(*word) - 1) || e < 0)
		// happens when space at beginning or end of word
		return ;
	newword = (t_list *)malloc(sizeof(t_list));
	if (!newword)
		error_exit(state);
	newword->content = (char *)malloc((e - s + 2) * sizeof(char));
	if (!newword->content)
		error_exit(state);
	ft_strlcpy(newword->content, *word + s, e - s + 2);
	newword->next = NULL;
	ft_lstadd_back(newwordlist, newword);
}

/*takes string pointer word and t_list double pointer for newwordlist
if it finds a space not inside quotes, it splits word at that index into two words
adds them to list newwordlist
if space is at beginning or end of word,
it will return a list of one word removing the space
*/
void	split_words(t_state *state, char **word, t_list **newwordlist)
{
	int		sq_flag;
	int		dq_flag;
	int		i;

	sq_flag = 0;
	dq_flag = 0;
	i = 0;
	while ((*word)[i])
	{
		if ((*word)[i] == '\'' && !dq_flag)
			sq_flag = !sq_flag;
		else if ((*word)[i] == '\"' && !sq_flag)
			dq_flag = !dq_flag;
		if ((*word)[i] == ' ' && !sq_flag && !dq_flag)
		{
			split_words_create(state, 0, i - 1, word, newwordlist);
			split_words_create(state, i + 1, ((int)ft_strlen(*word)) - 1, word, newwordlist);
			break ;
		}
		i++;
	}
}

/*splitting words after expansion: it iterates over list of words
if it finds a word with space not inside quotes, it splits into two words,
and inserts new list of two words
in place of the one word in the t_list words in t_node cmd_content,
it then loops again on second replaced word*/
void	splitting_cmd_words(t_state *state, t_node *cmd_content)
{
	//printf("\nsplitting...\n");
	t_list	*word;
	t_list	*before;
	t_list	*after;
	t_list	*newwordlist;

	word = cmd_content->words;
	newwordlist = NULL;
	before = NULL;
	while (word)
	{
		after = word->next;
		newwordlist = NULL;
		split_words(state, (char **) &(word->content), &newwordlist);
		if (newwordlist)
		{
			if (before)
				before->next = newwordlist;
			else
				cmd_content->words = newwordlist;
			if (newwordlist->next)
				newwordlist->next->next = after;
			else
				newwordlist->next = after;
			ft_lstdelone(word, free);
			word = newwordlist->next;
		}
		else
		{
			before = word;
			word = after;
		}
	}
}

/*iterates over list of commans in t_state variable state and calls
splitting_cmd_words on each command node*/
void	splitting(t_state *state)
{
	t_list	*cmd;

	cmd = state->cmds;
	while (cmd)
	{
		splitting_cmd_words(state, (t_node *)(cmd->content));
		cmd = cmd->next;
	}
}
