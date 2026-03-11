/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cvizcain <cvizcain@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/27 15:56:53 by cvizcain          #+#    #+#             */
/*   Updated: 2026/02/12 14:15:03 by cvizcain         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Expands variables in a single command's arguments and redirections.
 *
 * The expander is the stage between parsing and execution.  At this
 * point the AST already has the right shape; we just need to replace
 * placeholder strings like "$HOME" with their actual values.
 *
 * @param cmd   The command whose args and redirs need to be expanded.
 * @param shell The shell context (provides the env list and last_exit).
 */
static void	expand_cmd(t_cmd *cmd, t_shell *shell)
{
	if (!cmd)
		return ;

	/*
	** Expand variables in every argument string.
	** After expansion, cmd->args may be longer than before if a variable
	** expanded to multiple words (word splitting).
	*/
	if (cmd->args)
		expand_args(cmd, shell);

	/*
	** Expand variables in redirection filenames.
	** e.g. "> $OUTFILE" becomes "> actual_file_name".
	** Heredoc delimiters are handled specially inside expand_redirs.
	*/
	expand_redirs(cmd->redirs, shell);
}

/**
 * @brief Recursively traverses the AST and expands every command node.
 *
 * The AST can be a tree of pipelines and commands.  We walk it in
 * pre-order: if the current node is a command, expand it directly.
 * If it is a pipe (or any other compound node), recurse into both
 * children so every leaf command eventually gets expanded.
 *
 * @param ast   The current AST node (may be NULL at the leaves).
 * @param shell The shell context for variable lookup.
 */
void	expand_ast(t_ast *ast, t_shell *shell)
{
	if (!ast)
		return ;

	if (ast->type == NODE_CMD)
		/* Leaf node: expand the command directly. */
		expand_cmd(ast->cmd, shell);
	else
	{
		/*
		** Compound node (pipe, &&, ||, subshell, etc.):
		** recurse into both subtrees.  The actual expansion happens
		** when we reach a NODE_CMD leaf.
		*/
		expand_ast(ast->left, shell);
		expand_ast(ast->right, shell);
	}
}
