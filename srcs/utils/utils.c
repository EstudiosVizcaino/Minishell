/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cvizcain <cvizcain@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/08 17:33:16 by cvizcain          #+#    #+#             */
/*   Updated: 2026/02/14 18:55:00 by cvizcain         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * @brief Allocates memory and exits on failure.
 *
 * @param size The number of bytes to allocate.
 * @return A pointer to the allocated memory.
 */
void	*ft_malloc(size_t size)
{
	void	*ptr;

	ptr = malloc(size);
	if (!ptr)
		fatal_error("minishell: malloc failed");
	return (ptr);
}

/**
 * @brief Checks if a character is a whitespace character.
 *
 * @param c The character to check.
 * @return Non-zero if the character is whitespace, 0 otherwise.
 */
int	ft_isspace(int c)
{
	return (c == ' ' || c == '\t' || c == '\n'
		|| c == '\r' || c == '\f' || c == '\v');
}

/**
 * @brief Frees a NULL-terminated array of strings.
 *
 * @param arr The array of strings to free.
 */
void	free_array(char **arr)
{
	int	i;

	if (!arr)
		return ;
	i = 0;
	while (arr[i])
	{
		free(arr[i]);
		i++;
	}
	free(arr);
}

/**
 * @brief Returns the length of a NULL-terminated string array.
 *
 * @param arr The array of strings to measure.
 * @return The number of elements in the array.
 */
int	array_len(char **arr)
{
	int	i;

	if (!arr)
		return (0);
	i = 0;
	while (arr[i])
		i++;
	return (i);
}

/**
 * @brief Prints an error and exits.
 *
 * Writes msg to stderr, then exits 1.
 *
 * @param msg The error message.
 */
void	fatal_error(char *msg)
{
	if (msg)
		ft_putstr_fd(msg, STDERR_FILENO);
	ft_putchar_fd('\n', STDERR_FILENO);
	exit(1);
}
