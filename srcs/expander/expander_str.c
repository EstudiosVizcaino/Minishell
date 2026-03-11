/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_str.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cvizcain <cvizcain@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/29 17:52:04 by cvizcain          #+#    #+#             */
/*   Updated: 2026/02/13 18:49:08 by cvizcain         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Handles a single-quoted segment of a string.
 *
 * In POSIX shells, single quotes preserve the literal value of every
 * character inside them.  No variable expansion, no escape sequences –
 * everything is taken as-is.
 *
 * This function advances *i past the opening quote, copies everything
 * until the closing quote, then advances past the closing quote.
 *
 * @param str The full string being processed.
 * @param i   Index pointer; on entry points at the opening '\'' character.
 *            On return points to the character after the closing '\''.
 * @return A new string containing the raw text inside the quotes (no quotes).
 */
static char	*expand_single_quote(char *str, int *i)
{
	char	*result;
	int		start;

	(*i)++;         /* skip the opening single-quote character */
	start = *i;     /* record where the content starts */

	/* Advance until we find the matching closing quote or end-of-string. */
	while (str[*i] && str[*i] != '\'')
		(*i)++;

	/*
	** ft_substr copies the characters from start to *i (exclusive).
	** This gives us the raw text between the quotes, with no expansion.
	*/
	result = ft_substr(str, start, *i - start);

	/* Skip the closing single-quote if present. */
	if (str[*i] == '\'')
		(*i)++;
	return (result);
}

/**
 * @brief Handles a double-quoted segment of a string.
 *
 * Inside double quotes:
 *   - $VAR references ARE expanded.
 *   - Everything else (spaces, special chars, etc.) is kept literal.
 *   - The quote characters themselves are consumed and not included
 *     in the output.
 *
 * We build the result character by character (or variable by variable)
 * using join_free() which joins two strings and frees both originals.
 *
 * @param str   The full string being processed.
 * @param i     Index pointer; on entry points at the opening '"'.
 *              On return points to the character after the closing '"'.
 * @param shell Shell context for variable lookup.
 * @return The expanded content of the double-quoted section.
 */
static char	*expand_double_quote(char *str, int *i, t_shell *shell)
{
	char	*result;
	char	*part;   /* one piece to append (a char or an expanded variable) */
	char	buf[2];  /* small buffer for copying single literal characters */

	result = ft_strdup("");  /* start with an empty string */
	(*i)++;                  /* skip the opening '"' character */

	while (str[*i] && str[*i] != '"')
	{
		if (str[*i] == '$')
		{
			/*
			** Variable reference inside double quotes.
			** expand_var() reads the variable name that follows '$',
			** looks it up in the environment, and returns its value.
			** It also advances *i past the variable name.
			*/
			part = expand_var(str, i, shell);
		}
		else
		{
			/*
			** Ordinary character inside double quotes.
			** Copy it into a tiny 2-char buffer (char + '\0') and
			** duplicate it so we can safely pass it to join_free().
			*/
			buf[0] = str[(*i)++];
			buf[1] = '\0';
			part = ft_strdup(buf);
		}

		/*
		** join_free(result, part) is equivalent to:
		**   tmp = result + part;  free(result);  free(part);  result = tmp;
		** This avoids accumulating intermediate allocations.
		*/
		result = join_free(result, part);
	}

	/* Skip the closing '"' if present. */
	if (str[*i] == '"')
		(*i)++;
	return (result);
}

/**
 * @brief Expands exactly one element at position *i in str.
 *
 * Dispatches to the appropriate handler:
 *   '\'' → single-quote handler (no expansion)
 *   '"'  → double-quote handler ($VAR expanded)
 *   '$'  → bare variable expansion
 *   else → copy the literal character
 *
 * @param str   The full string being processed.
 * @param i     Index pointer (advanced past the element that was processed).
 * @param shell Shell context for variable lookup.
 * @return The expanded or copied element as a new string.
 */
static char	*expand_one(char *str, int *i, t_shell *shell)
{
	char	buf[2];

	/* Single-quoted string: literal, no expansion. */
	if (str[*i] == '\'')
		return (expand_single_quote(str, i));

	/* Double-quoted string: expand $VAR but keep everything else literal. */
	if (str[*i] == '"')
		return (expand_double_quote(str, i, shell));

	/* Bare $ outside quotes: expand the variable. */
	if (str[*i] == '$')
		return (expand_var(str, i, shell));

	/*
	** Any other character: copy it verbatim into a single-char string.
	** We advance *i here before returning.
	*/
	buf[0] = str[(*i)++];
	buf[1] = '\0';
	return (ft_strdup(buf));
}

/**
 * @brief Fully expands a string: processes all quotes and variable refs.
 *
 * This is the top-level expander for one token string.  It walks the
 * string from left to right, calling expand_one() at each position,
 * and joins all the pieces into a single result string.
 *
 * Examples:
 *   "hello $USER!"  → "hello alice!" (if USER=alice)
 *   "'no $expand'"  → "no $expand"
 *   "$HOME/file"    → "/home/alice/file"
 *
 * @param str   The raw (unexpanded) string from the token/redir.
 * @param shell Shell context for variable lookup.
 * @return A newly-allocated expanded string.  Caller must free it.
 */
char	*expand_str(char *str, t_shell *shell)
{
	char	*result;
	char	*part;
	int		i;

	/* Handle NULL gracefully: return an empty string, not a crash. */
	if (!str)
		return (ft_strdup(""));

	result = ft_strdup("");  /* accumulator for the final result */
	i = 0;
	while (str[i])
	{
		/*
		** expand_one() advances i and returns the next "chunk" of the
		** expanded string.  join_free() appends it to result and frees
		** both the old result and the chunk.
		*/
		part = expand_one(str, &i, shell);
		result = join_free(result, part);
	}
	return (result);
}
