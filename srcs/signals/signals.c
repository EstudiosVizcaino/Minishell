/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cvizcain <cvizcain@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/07 16:46:21 by cvizcain          #+#    #+#             */
/*   Updated: 2026/02/21 11:15:12 by cvizcain         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Signal handling overview
** ========================
** The shell uses three different signal configurations:
**
**  1. Interactive mode (main loop, waiting for user input):
**       SIGINT  → sig_handler   : print a newline and redraw the prompt.
**       SIGQUIT → SIG_IGN       : Ctrl-\ is ignored at the prompt.
**
**  2. Child-execution mode (waiting for a foreground command):
**       SIGINT  → sig_handler_wait : record the signal but do NOT touch
**                                    readline (prevents prompt corruption).
**       SIGQUIT → SIG_IGN          : still ignored in the parent.
**       (children get SIG_DFL via setup_signals_child so they respond
**        normally to Ctrl-C and Ctrl-\.)
**
**  3. Heredoc mode (reading heredoc lines from the user):
**       SIGINT  → sig_heredoc  : close stdin so the readline loop exits.
**       SIGQUIT → SIG_IGN      : ignored here too.
**
** g_signal is reset to 0 by the caller after it has been inspected.
*/

/**
 * @brief SIGINT handler for interactive (prompt) mode.
 *
 * When the user presses Ctrl-C at the prompt:
 *   1. Record the signal number in g_signal so the main loop can
 *      set last_exit = 130 after readline returns.
 *   2. Print a newline so the cursor is on a fresh line.
 *   3. Tell readline the cursor is on a new line (rl_on_new_line).
 *   4. Clear any partially-typed text (rl_replace_line("", 0)).
 *   5. Redraw the prompt (rl_redisplay).
 *
 * @param sig The signal number (always SIGINT here).
 */
void	sig_handler(int sig)
{
	if (sig == SIGINT)
	{
		/* Save the signal so the main loop can update $?. */
		g_signal = sig;

		/*
		** write() is async-signal-safe (unlike printf/puts), so it is
		** the correct way to do I/O inside a signal handler.
		*/
		write(STDOUT_FILENO, "\n", 1);

		/*
		** These three readline calls together produce the effect of
		** cancelling the current input line and showing a clean prompt:
		**   rl_on_new_line()       – tell readline we moved to a new line
		**   rl_replace_line("", 0) – erase the buffered partial input
		**   rl_redisplay()         – redraw the prompt on the new line
		*/
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
}

/**
 * @brief SIGINT handler for heredoc input mode.
 *
 * When the user presses Ctrl-C while typing a heredoc:
 *   1. Record the signal in g_signal.
 *   2. Close STDIN_FILENO so the readline call inside the heredoc reader
 *      gets EOF and the loop terminates cleanly.
 *
 * We do NOT call rl_* functions here because the heredoc reader uses
 * its own readline context and we just want it to stop.
 *
 * @param sig The signal number (always SIGINT here).
 */
void	sig_heredoc(int sig)
{
	if (sig == SIGINT)
	{
		g_signal = sig;

		/*
		** Closing stdin causes the next readline() call inside the
		** heredoc reader to return NULL (EOF), which signals it to stop.
		*/
		close(STDIN_FILENO);
	}
}

/**
 * @brief Installs signal handlers for the interactive (prompt) mode.
 *
 * Called at the top of every main-loop iteration so we are always in
 * the correct signal state when waiting for user input.
 *
 * We use sigaction() instead of signal() for the SIGINT handler because:
 *   - SA_RESTART automatically restarts interrupted system calls, which
 *     prevents spurious EINTR errors in our code.
 *   - sigaction() behaviour is more portable and predictable than signal().
 */
void	setup_signals(void)
{
	struct sigaction	sa;

	/*
	** Zero out the entire struct first to avoid any garbage in the
	** sa_flags or sa_mask fields from uninitialized memory.
	*/
	ft_memset(&sa, 0, sizeof(sa));

	/* Set our custom handler for SIGINT. */
	sa.sa_handler = sig_handler;

	/* Empty signal mask: do not block any additional signals while
	** the handler is running. */
	sigemptyset(&sa.sa_mask);

	/* SA_RESTART: if a signal arrives while we are in a blocking call
	** (like read()), the call is restarted rather than returning EINTR. */
	sa.sa_flags = SA_RESTART;

	/* Install for SIGINT; NULL means we do not save the old handler. */
	sigaction(SIGINT, &sa, NULL);

	/*
	** SIGQUIT (Ctrl-\) is ignored at the prompt, just like bash.
	** SIG_IGN is inherited across fork(), so child processes would
	** also ignore SIGQUIT unless they reset it – which is done in
	** setup_signals_child().
	*/
	signal(SIGQUIT, SIG_IGN);
}

/**
 * @brief Resets signal handlers to their defaults for child processes.
 *
 * After fork(), the child inherits the parent's signal dispositions.
 * We reset SIGINT and SIGQUIT to SIG_DFL so that the child's program
 * can be interrupted/quit normally (e.g. Ctrl-C kills cat, not the shell).
 *
 * This function is called in exec_child() just before execve().
 */
void	setup_signals_child(void)
{
	/*
	** SIG_DFL restores the kernel's default behaviour:
	**   SIGINT  → terminate the process.
	**   SIGQUIT → terminate the process and dump core.
	*/
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}

/**
 * @brief Installs signal handlers for heredoc input mode.
 *
 * Called before entering the heredoc reading loop so that Ctrl-C
 * aborts the heredoc instead of trying to redraw the prompt.
 *
 * Note: SA_RESTART is deliberately NOT set here because we want the
 * close(STDIN_FILENO) in sig_heredoc to cause readline to return NULL
 * rather than being restarted.
 */
void	setup_signals_heredoc(void)
{
	struct sigaction	sa;

	ft_memset(&sa, 0, sizeof(sa));
	sa.sa_handler = sig_heredoc;
	sigemptyset(&sa.sa_mask);

	/* No SA_RESTART: we want the interrupted readline to surface the EOF. */
	sigaction(SIGINT, &sa, NULL);
	signal(SIGQUIT, SIG_IGN);
}
