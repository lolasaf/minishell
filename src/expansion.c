/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wel-safa <wel-safa@student.42berlin.de>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/28 19:26:09 by wel-safa          #+#    #+#             */
/*   Updated: 2024/08/20 18:24:29 by wel-safa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* takes word and two string pointers which it assigns substrings from word
from index 0 to i and from index j+1 to len,
disregarding chars between i and j*/
void	substr_words(char **first, char **second, char *word, int i, int j)
{
	int	len;

	len = ft_strlen(word);
	*first = ft_substr(word, 0, i);
	*second = ft_substr(word, j + 1, len); // len? or len-j should be okay but check with other situations
}

/* takes a string: word, cuts out the part from i to j 
and replaces it with another string: rep
if rep = "" or NULL, the characters between i and j are cut out
the new string is returned*/
char	*strreplace(char **word, char *rep, int i, int j)
{
	char	*first;
	char	*second;
	char	*tmp;
	char	*res;

	// change to strjoin free function instead
	if (j < i)
		return(*word);
	substr_words(&first, &second, *word, i, j);
	if (!rep)
	{
		res = ft_strjoin(first, second);
		free(first);
		free(second);
		free(*word);
		return (res);
	}
	tmp = ft_strjoin(first, rep);
	res = ft_strjoin(tmp, second);
	free(first);
	free(second);
	free(tmp);
	free(*word);
	return (res);
}

/*returns 1 if c is alphanumeric or underscore, returns 0 otherwise*/
int	var_letter(char c)
{
	if (ft_isalnum(c) || c == '_')
		return (1);
	return (0);
}

/*takes t_state struct and string pointer word and index i of the dollar sign found
it finds variable after $ sign. if variable is not found, no expansions, 
except in such cases: $"hello" or $'hello' where the dollar sign is removed.
if var is found, it searches env for var and replaces $VAR from string with its value
otherwise it removes $VAR from the string.
it returns index last character (replacement or no replacement)
*/
int	expand(t_state *state, char **word, int i)
{
	int		j;
	char	*var;
	int		len;
	char	*rep;

	j = i + 1;
	var = NULL;
	while((*word)[j])
	{
		if (ft_isdigit((*word)[j]) && (j - i == 1))
		{
			*word = strreplace(word, NULL, i, i + 1);
			return (i - 1);
		}
		if (var_letter((*word)[j]) == 0)
			break ; // end of var and j is index after var
		j++;
	}
	len = j - i - 1; // length of variable name
	if (len > 0)
	{
		var = (char *)malloc((len + 1) * sizeof(char));
		if (var == NULL)
		{
			// malloc error
			cleanup_shell(state);
			exit(1);
		}
		// copy var from word.
		ft_memcpy(var, (*word) + i + 1, len);
		var[len] = '\0';
		rep = find_var_value(state, var);
		if (!rep)
			rep = ""; // handle when var is not found
		*word = strreplace(word, rep, i, i + len);
		// index change to after replacement
		//  - 1, it will iterate to the next character after this function.
		i += ft_strlen(rep) - 1;
		free(var);
	}
	else if ((*word)[j] == '\"' || (*word)[j] == '\'')
	{
		// $"hello" or $'hello' removes the dollar sign
		// it will not detect "hello $" as this is taken care of in toexpand function if statement
		*word = strreplace(word, NULL, i, i);
		i -= 1;
	}
	return (i);
}

/*takes t_state struct and a string pointer of the word,
it detects whether $ is inside double or single quotes
it expands $VAR for normal state or if it is inside double quote
and there is a variable. If there is no variable or
it is inside the single quote it will not expand
*/
void	toexpand(t_state *state, char **word)
{
	int sq_flag;
	int dq_flag;
	int	i;

	sq_flag = 0;
	dq_flag = 0;

	i = 0;
	while ((*word)[i])
	{
		if ((*word)[i] == '\'' && !dq_flag)
			sq_flag = !sq_flag;
		else if ((*word)[i] == '\"' && !sq_flag)
			dq_flag = !dq_flag;
		else if ((*word)[i] == '$')
		{
			if (!sq_flag && !dq_flag && (*word)[i + 1] != 0)
				// not inside any quotes
				// find variable to expand
				// Variable cuts off at char that is not alphanumric or not an underscore.
				// if quote is right after $ sign there will be no var
				// and nothing to expand, just remove $ sign
				i = expand(state, word, i);
			else if (dq_flag && (*word)[i + 1] != '\"')
				// inside double quotes AND there is a VAR assigned after $
				// for example "hello $ " or "hello $", 
				// will be ignored and kept in the word
				i = expand(state, word, i);
		}
		i++;
	}
}

/*takes state struct and iterates over list of words in each cmd 
and calls toexpand function on each*/
void	expansion(t_state *state)
{
	t_list	*cmd;
	t_list	*word;

	cmd = state->cmds;
	word = ((t_node *)cmd->content)->words;
	while(cmd)
	{
		while(word)
		{
			toexpand(state, (char **) &(word->content));
			word = word->next;
		}
		cmd = cmd->next;
	}
}
