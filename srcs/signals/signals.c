#include "minishell.h"

void	sig_handler(int sig)
{
	if (sig == SIGINT)
	{
		g_signal = sig;
		write(STDOUT_FILENO, "\n", 1);
		rl_on_new_line();
		rl_replace_line("", 0);
		rl_redisplay();
	}
}

void	sig_heredoc(int sig)
{
	if (sig == SIGINT)
	{
		g_signal = sig;
		write(STDOUT_FILENO, "\n", 1);
		close(STDIN_FILENO);
	}
}

void	setup_signals(void)
{
	struct sigaction	sa;

	ft_memset(&sa, 0, sizeof(sa));
	sa.sa_handler = sig_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sigaction(SIGINT, &sa, NULL);
	signal(SIGQUIT, SIG_IGN);
}

void	setup_signals_child(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}
