/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cvizcain <cvizcain@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/30 12:42:30 by cvizcain          #+#    #+#             */
/*   Updated: 2026/02/10 18:10:16 by cvizcain         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Looks up a variable name and returns its current value.
 *
 * This is the core lookup used by the expander.  Two special cases:
 *   - A NULL name returns an empty string (defensive, should not happen).
 *   - The name "?" returns the last exit status as a decimal string
 *     because $? must expand to the exit code of the last command.
 *
 * For all other names, we consult the shell's env linked list.
 * If the variable is not set, we return "" (empty string) rather than
 * NULL, because unexpanded but unset variables produce the empty string.
 *
 * @param name  The variable name (e.g. "PATH", "USER", "?").
 * @param shell The shell context that holds the env list and last_exit.
 * @return A newly-allocated string containing the value.
 *         The caller must free() this string.
 */
char	*get_var_value(char *name, t_shell *shell)
{
	char	*val;

	if (!name)
		return (ft_strdup(""));

	/*
	** $? is special: it expands to the exit code of the last command.
	** ft_itoa converts the integer to a heap-allocated decimal string.
	*/
	if (ft_strcmp(name, "?") == 0)
		return (ft_itoa(shell->last_exit));

	/*
	** env_get() walks the linked list to find the variable.
	** It returns NULL if the variable is not set (not exported).
	*/
	val = env_get(shell->env, name);
	if (!val)
		return (ft_strdup(""));  /* unset variable → empty string */

	/*
	** We return a fresh copy because the caller will eventually free()
	** this string, and we do not want them to accidentally free the
	** pointer that is stored inside the t_env node.
	*/
	return (ft_strdup(val));
}

/**
 * @brief Parses and expands a single $VARIABLE reference in a string.
 *
 * On entry, *i points at the '$' character.
 * On return, *i points to the first character AFTER the variable name.
 *
 * Handles three cases:
 *   1. $?   – the special last-exit-status variable.
 *   2. $VAR – a normal identifier (letter/underscore then alnum/_).
 *   3. $    – a bare dollar with no valid identifier following it;
 *             returned literally as "$" (matches bash behaviour).
 *
 * @param str   The full string being processed.
 * @param i     Current index pointer (must point at '$' on entry).
 * @param shell Shell context for variable lookup.
 * @return The variable's value as a new string.  Caller must free().
 */
char	*expand_var(char *str, int *i, t_shell *shell)
{
	int		start;  /* index of the first character of the variable name */
	char	*name;  /* extracted variable name */
	char	*val;

	(*i)++;  /* skip past the '$' character */

	/*
	** Special case: $? expands to the last exit status.
	** We handle it here (before the general identifier loop) because
	** '?' is not a valid identifier character.
	*/
	if (str[*i] == '?')
	{
		(*i)++;  /* consume the '?' */
		return (ft_itoa(shell->last_exit));
	}

	/*
	** For a normal variable name, the identifier must start with a
	** letter or underscore and continue with letters, digits, or
	** underscores – the same rules as C identifiers.
	*/
	start = *i;
	while (str[*i] && (ft_isalnum(str[*i]) || str[*i] == '_'))
		(*i)++;

	/*
	** If *i did not advance past 'start', the character after '$' is
	** neither '?' nor a valid identifier start.  This means we have a
	** lone '$' (e.g. "price is $5") – return it literally.
	*/
	if (*i == start)
		return (ft_strdup("$"));

	/* Extract just the name part (e.g. "HOME" from "$HOME/foo"). */
	name = ft_substr(str, start, *i - start);
	if (!name)
		return (ft_strdup(""));

	/*
	** Look up the value in the environment.
	** get_var_value() handles the $? case too, but we already handled
	** it above, so here it is always a plain variable lookup.
	*/
	val = get_var_value(name, shell);
	free(name);   /* we no longer need the extracted name string */
	return (val);
}

/**
 * @brief Joins two strings, frees both, and returns the concatenation.
 *
 * This helper is used throughout the expander to accumulate a result
 * string piece by piece without leaking memory.
 *
 * Pattern of use:
 *   result = join_free(result, new_piece);
 * After this call, both 'result' (old) and 'new_piece' are freed and
 * 'result' is reassigned to the new concatenated string.
 *
 * If ft_strjoin fails (malloc returns NULL), we call fatal_error() to
 * abort the shell cleanly rather than silently producing wrong output.
 *
 * @param s1 First string (will be freed).
 * @param s2 Second string (will be freed).
 * @return Newly-allocated string containing s1 followed by s2.
 */
char	*join_free(char *s1, char *s2)
{
	char	*tmp;

	/* ft_strjoin allocates a new string = s1 + s2. */
	tmp = ft_strjoin(s1, s2);

	/* Free both inputs regardless of whether the join succeeded. */
	free(s1);
	free(s2);

	/*
	** If the join failed (malloc returned NULL), we cannot continue
	** safely.  fatal_error() prints a message to stderr and exits 1.
	*/
	if (!tmp)
		fatal_error("malloc");
	return (tmp);
}

/**
 * @brief Expands $VAR references inside heredoc body lines.
 *
 * This differs from expand_str() in one important way: single and
 * double quote characters are NOT treated as quoting constructs here.
 * Inside a heredoc body, a single quote is just a literal "'" character,
 * not the start of a quoted region.  Only '$' triggers expansion.
 *
 * This matches the behaviour of bash:
 *   cat << EOF
 *   $HOME is my home, 'literally'
 *   EOF
 *   → /home/alice is my home, 'literally'
 *
 * @param str   One line from the heredoc body.
 * @param shell Shell context for variable lookup.
 * @return The expanded line as a new string.  Caller must free().
 */
char	*expand_heredoc_str(char *str, t_shell *shell)
{
	char	*result;
	char	*part;
	char	buf[2];
	int		i;

	if (!str)
		return (ft_strdup(""));
	result = ft_strdup("");
	i = 0;
	while (str[i])
	{
		if (str[i] == '$')
		{
			/*
			** Dollar sign → expand the variable that follows.
			** expand_var() advances i past the variable name.
			*/
			part = expand_var(str, &i, shell);
		}
		else
		{
			/*
			** All other characters (including ' and ") are copied
			** literally into the output.
			*/
			buf[0] = str[i++];
			buf[1] = '\0';
			part = ft_strdup(buf);
		}
		result = join_free(result, part);
	}
	return (result);
}
