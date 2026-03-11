/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_args.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gisidro- <gisidro-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/28 13:56:36 by gisidro-          #+#    #+#             */
/*   Updated: 2026/03/05 19:00:00 by gisidro-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Returns 1 if the string consists entirely of bare variable references.
 *
 * A "bare variable reference" is a token like $FOO or $? with no
 * surrounding text, quotes, or literals.
 *
 * This predicate is used to implement one POSIX rule: when an unquoted
 * variable expands to an empty string, that argument must be removed
 * entirely from the command.  But if a variable is mixed with other
 * text (e.g. "prefix_$FOO"), the argument is always kept even if the
 * variable is empty.
 *
 * Examples:
 *   "$FOO"      → 1 (only vars, no quotes or literals)
 *   "$FOO$BAR"  → 1
 *   "hello$FOO" → 0 (has a literal prefix)
 *   "'$FOO'"    → 0 (quoted: the $ is literal)
 *
 * @param str The original (unexpanded) string to inspect.
 * @return 1 if it is only variable references, 0 otherwise.
 */
static int	is_only_vars(char *str)
{
	int	i;

	i = 0;
	if (!str || !*str)
		return (0);
	while (str[i])
	{
		/*
		** If we see a quote, this is not a "bare" variable reference
		** because quotes suppress or alter expansion semantics.
		*/
		if (str[i] == '\'' || str[i] == '"')
			return (0);

		/*
		** Every segment must start with '$'.  If not, there are literal
		** characters mixed with the variables.
		*/
		if (str[i] != '$')
			return (0);
		i++;

		/*
		** After the '$' we allow either:
		**   '?' (special variable $?)
		**   a valid identifier (letter/underscore start, then alnum/_)
		** Anything else (e.g. just "$" alone) is not a valid var ref.
		*/
		if (str[i] == '?')
			i++;
		else if (ft_isalpha(str[i]) || str[i] == '_')
		{
			while (str[i] && (ft_isalnum(str[i]) || str[i] == '_'))
				i++;
		}
		else
			return (0);
	}
	return (1);
}

/**
 * @brief Expands one argument and places the result into dst[].
 *
 * If the original token was a bare variable reference AND the expansion
 * produced a non-empty string, we perform word splitting: the value is
 * split on whitespace and each word becomes a separate argument.
 *
 * If the expansion produced an empty string AND the original was only
 * variables, the argument is silently dropped (nothing written to dst).
 *
 * @param dst   Destination array of string pointers.
 * @param j     Current write index into dst (incremented for each word added).
 * @param orig  The original (unexpanded) argument string.
 * @param shell Shell context for variable lookup.
 */
static void	fill_one_arg(char **dst, int *j, char *orig, t_shell *shell)
{
	char	*exp;    /* the expanded version of orig */
	char	**words; /* word-split result (if applicable) */
	int		k;

	/*
	** expand_str() processes orig character by character, handling
	** single quotes (literal), double quotes (expand $), and bare $VAR.
	** The result is a newly-allocated string.
	*/
	exp = expand_str(orig, shell);

	/*
	** Word splitting: only applies when:
	**   1. The original was a bare variable reference (no quotes), AND
	**   2. The expanded string is non-empty.
	** In that case we split exp on IFS (whitespace here) so that e.g.
	** `set -- $IFS_VAR` works correctly.
	*/
	if (is_only_vars(orig) && *exp)
	{
		words = word_split(exp);  /* returns NULL if no words */
		free(exp);                /* word_split copies the words, so free exp */
		if (!words)
			return ;
		k = 0;
		/* Each word from the split becomes a separate argument. */
		while (words[k])
			dst[(*j)++] = words[k++];
		free(words);  /* free the array of pointers (NOT the strings inside) */
		return ;
	}

	/*
	** No word splitting needed.
	** Keep the expanded string as-is if:
	**   - it is non-empty, OR
	**   - the original had literal text (not only vars), which means
	**     even an empty result should be kept as an empty-string argument.
	*/
	if (*exp || !is_only_vars(orig))
		dst[(*j)++] = exp;
	else
		free(exp);  /* empty expansion of a bare variable → drop the arg */
}

/**
 * @brief Pre-counts how many arguments the command will have after expansion.
 *
 * We need the final count BEFORE building the new array so we can
 * allocate it with the right size in one shot.
 * This function mirrors the logic of fill_one_arg() but only counts,
 * it does not store anything.
 *
 * @param args  The original NULL-terminated argument array.
 * @param shell Shell context for variable expansion.
 * @return Total number of argument strings after expansion and splitting.
 */
static int	count_new_args(char **args, t_shell *shell)
{
	char	*exp;
	char	**words;
	int		i;
	int		total;

	i = 0;
	total = 0;
	while (args[i])
	{
		/*
		** Expand each original argument to see how many words it produces.
		** We call expand_str here (instead of in fill_one_arg) which means
		** we expand twice overall, but this two-pass approach avoids a
		** realloc loop and keeps the code simple.
		*/
		exp = expand_str(args[i], shell);
		if (is_only_vars(args[i]) && *exp)
		{
			/* Bare variable: count the words it splits into. */
			words = word_split(exp);
			total += array_len(words);   /* array_len handles NULL safely */
			free_array(words);
		}
		else if (*exp || !is_only_vars(args[i]))
			total++;  /* kept as a single argument */
		free(exp);
		i++;
	}
	return (total);
}

/**
 * @brief Expands all arguments in a command, applying variable expansion
 *        and word splitting, then replaces cmd->args with the result.
 *
 * After this function returns, cmd->args is a fresh array containing
 * the fully-expanded arguments.  The old array and its strings are freed.
 *
 * @param cmd   The command whose args are to be expanded.
 * @param shell Shell context for variable lookup.
 */
void	expand_args(t_cmd *cmd, t_shell *shell)
{
	char	**new;  /* the new args array */
	int		n;      /* total args after expansion */
	int		i;      /* index into the old args array */
	int		j;      /* index into the new args array */

	/*
	** First pass: count how many args the new array must hold.
	** Allocate it (+1 for the NULL sentinel).
	*/
	n = count_new_args(cmd->args, shell);
	new = malloc((n + 1) * sizeof(char *));
	if (!new)
		return ;

	i = 0;
	j = 0;
	/*
	** Second pass: expand each original argument and fill new[].
	** We free each original string as soon as it has been processed
	** because fill_one_arg has already consumed it.
	*/
	while (cmd->args[i])
	{
		fill_one_arg(new, &j, cmd->args[i], shell);
		free(cmd->args[i]);
		i++;
	}

	/* Terminate the new array with NULL. */
	new[j] = NULL;

	/* Swap in the new array and free the old pointer. */
	free(cmd->args);
	cmd->args = new;

	/*
	** Special case: if all arguments expanded to empty strings and were
	** dropped, j == 0.  In that case free the array and set args to NULL
	** so the executor can detect that there is no command to run.
	*/
	if (j == 0)
	{
		free(new);
		cmd->args = NULL;
	}
}

/**
 * @brief Expands variables in all redirection filenames.
 *
 * For each redirection in the list:
 *   - If it is a heredoc (<<), we first scan the delimiter for any
 *     quote characters and set redir->quoted = 1 if found.  The
 *     executor uses this flag to decide whether to expand $VARs inside
 *     the heredoc body or treat them literally.
 *   - Then we call expand_str() to expand the filename (or delimiter)
 *     and replace the old string.
 *
 * @param redir Head of the redirection list.
 * @param shell Shell context for variable lookup.
 */
void	expand_redirs(t_redir *redir, t_shell *shell)
{
	char	*exp;
	int		i;

	while (redir)
	{
		i = 0;
		redir->quoted = 0;  /* reset before each redir */

		/*
		** For heredoc redirs: scan the delimiter for quote characters.
		** If the delimiter is quoted (e.g. << 'EOF' or << "EOF"),
		** the heredoc body should NOT have variables expanded.
		** We set redir->quoted here; the executor checks it later.
		*/
		if (redir->type == TOKEN_HEREDOC && redir->file)
		{
			while (redir->file[i])
			{
				if (redir->file[i] == '\'' || redir->file[i] == '"'
					|| redir->file[i] == '\\')
					redir->quoted = 1;
				i++;
			}
		}

		/*
		** Expand any $VAR references in the filename / delimiter.
		** For heredocs this also strips any quote characters from
		** the delimiter so it can be matched cleanly.
		*/
		if (redir->file)
		{
			exp = expand_str(redir->file, shell);
			free(redir->file);    /* free the old (unexpanded) string */
			redir->file = exp;    /* replace with the expanded version */
		}

		redir = redir->next;
	}
}
