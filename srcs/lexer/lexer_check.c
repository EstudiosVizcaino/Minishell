/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_check.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cvizcain <cvizcain@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 15:20:39 by cvizcain          #+#    #+#             */
/*   Updated: 2026/03/10 15:20:39 by cvizcain         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Checks if an input string has an unclosed quote.
 *
 * Scans through the string tracking whether the current character
 * is inside a single or double quote context.
 *
 * @param input The input string to scan.
 * @return 1 if there is an unclosed quote, 0 otherwise.
 */
int	has_unclosed_quote(char *input)
{
	char	quote;
	int		i;

	quote = 0;
	i = 0;
	while (input[i])
	{
		if (quote)
		{
			if (input[i] == quote)
				quote = 0;
		}
		else if (input[i] == '\'' || input[i] == '"')
			quote = input[i];
		i++;
	}
	return (quote != 0);
}
