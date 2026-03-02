/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adherrer <adherrer@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/04 13:00:04 by adherrer          #+#    #+#             */
/*   Updated: 2026/02/15 21:40:03 by adherrer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Searches for an executable in PATH directories.
 *
 * @param name The command name to search for.
 * @param paths The array of directory paths to search in.
 * @return The full path to the executable, or NULL if not found.
 */
static char	*search_in_paths(char *name, char **paths)
{
	char	*candidate;
	char	*tmp;
	int		i;

	i = 0;
	while (paths[i])
	{
		tmp = ft_strjoin(paths[i], "/");
		candidate = ft_strjoin(tmp, name);
		free(tmp);
		if (access(candidate, X_OK) == 0)
		{
			free_array(paths);
			return (candidate);
		}
		free(candidate);
		i++;
	}
	free_array(paths);
	return (NULL);
}

/**
 * @brief Resolves the full path of an executable command.
 *
 * @param name The command name to resolve.
 * @param env The environment list to look up PATH.
 * @return The full path to the executable, or NULL if not found.
 */
char	*find_executable(char *name, t_env *env)
{
	char	*path_env;
	char	**paths;

	if (ft_strchr(name, '/'))
		return (ft_strdup(name));
	path_env = env_get(env, "PATH");
	if (!path_env)
		return (NULL);
	paths = ft_split(path_env, ':');
	if (!paths)
		return (NULL);
	return (search_in_paths(name, paths));
}

/**
 * @brief Executes a command via execve in a child process.
 *
 * @param path The full path to the executable.
 * @param cmd The command structure containing arguments.
 * @param shell The shell state.
 */
static void	exec_child_run(char *path, t_cmd *cmd, t_shell *shell)
{
	char		**envp;
	struct stat	st;
	int			err;

	if (stat(path, &st) == 0 && S_ISDIR(st.st_mode))
	{
		ft_putstr_fd(path, STDERR_FILENO);
		ft_putstr_fd(": Is a directory\n", STDERR_FILENO);
		exit(126);
	}
	envp = env_to_array(shell->env);
	execve(path, cmd->args, envp);
	err = errno;
	perror(path);
	if (err == ENOENT)
		exit(127);
	exit(126);
}

/**
 * @brief Sets up and executes an external command in a child process.
 *
 * @param cmd The command structure containing arguments and redirections.
 * @param shell The shell state.
 */
void	exec_child(t_cmd *cmd, t_shell *shell)
{
	char	*path;

	setup_signals_child();
	if (apply_redirs(cmd->redirs))
		exit(1);
	path = find_executable(cmd->args[0], shell->env);
	if (!path)
	{
		ft_putstr_fd(cmd->args[0], STDERR_FILENO);
		ft_putstr_fd(": command not found\n", STDERR_FILENO);
		exit(127);
	}
	exec_child_run(path, cmd, shell);
}

/**
 * @brief Executes a builtin command with redirections, saving and restoring fds.
 *
 * @param cmd The command structure containing the builtin and redirections.
 * @param shell The shell state.
 * @param ret Pointer to store the exit status of the builtin.
 */
void	exec_builtin_redir(t_cmd *cmd, t_shell *shell, int *ret)
{
	int	saved_in;
	int	saved_out;

	saved_in = dup(STDIN_FILENO);
	saved_out = dup(STDOUT_FILENO);
	if (apply_redirs(cmd->redirs))
	{
		dup2(saved_in, STDIN_FILENO);
		dup2(saved_out, STDOUT_FILENO);
		close(saved_in);
		close(saved_out);
		*ret = 1;
		return ;
	}
	*ret = exec_builtin(cmd, shell);
	dup2(saved_in, STDIN_FILENO);
	dup2(saved_out, STDOUT_FILENO);
	close(saved_in);
	close(saved_out);
}
