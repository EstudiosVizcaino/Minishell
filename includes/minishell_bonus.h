/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell_bonus.h                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gisidro- <gisidro-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 16:57:37 by gisidro-          #+#    #+#             */
/*   Updated: 2026/02/27 16:23:14 by gisidro-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_BONUS_H
# define MINISHELL_BONUS_H

# include "minishell.h"

t_ast		*parse_and_or(t_token **tokens);
int			exec_and_or(t_ast *ast, t_shell *shell);
char		**expand_wildcard(char *pattern);
int			match_wildcard(char *pattern, char *str);
void		expand_wildcards_cmd(t_cmd *cmd);

#endif
