/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cvizcain <cvizcain@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/07 12:32:31 by cvizcain          #+#    #+#             */
/*   Updated: 2026/02/26 11:48:03 by cvizcain         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** g_signal is the only global variable allowed by the project rules.
** Signal handlers cannot receive extra arguments, so this is the only
** reliable way to pass signal information from a handler to the main loop.
** When a SIGINT (Ctrl-C) arrives, the handler writes the signal number
** here so the shell can check it after readline returns.
*/
int	g_signal = 0;

/**
 * @brief Checks if a string contains only whitespace characters.
 *
 * @param line The string to check.
 * @return 1 if the string is blank (all whitespace), 0 otherwise.
 */
static int	is_blank_line(char *line)
{
	int	i;

	i = 0;
	while (line[i])
	{
		if (!ft_isspace(line[i]))
			return (0);
		i++;
	}
	return (1);
}

/**
 * @brief Processes one line of user input through the full pipeline.
 *
 * Takes the line through lexer → parser → expander → executor.
 * The exit code goes into shell->last_exit so $? works correctly.
 *
 * @param line  The raw input line.
 * @param shell The shell context.
 */
static void	run_line(char *line, t_shell *shell)
{
	t_token	*tokens;

	/*
	** STEP 1 – LEXING
	** lexer() scans 'line' character by character and breaks it into a
	** linked list of tokens (words, pipes, redirections, etc.).
	** If lexer() returns NULL it means the input was empty or a malloc
	** failed, so there is nothing to execute.
	*/
	tokens = lexer(line);
	if (!tokens)
		return ;

	/*
	** STEP 2 – PARSING
	** parser() consumes the token list and builds an Abstract Syntax Tree
	** (AST).  Each node is either a command (NODE_CMD) or a pipe
	** (NODE_PIPE).  The AST is stored directly in shell->ast so that
	** cleanup code can reach it even if something crashes later.
	** We free the token list immediately because the AST does not keep
	** any pointers into it – the parser already copied what it needed.
	*/
	shell->ast = parser(tokens);
	free_tokens(tokens);
	if (!shell->ast)
		return ;

	/*
	** STEP 3 – EXPANSION
	** expand_ast() walks the AST and replaces every $VAR, $?, and quoted
	** string in command arguments and redirection filenames with their
	** actual values.  This must happen after parsing so the AST shape is
	** already known, and before execution so the executor sees final values.
	*/
	expand_ast(shell->ast, shell);

	/*
	** STEP 4 – EXECUTION
	** execute() runs the AST.  For a single command it forks + execs (or
	** calls a builtin directly).  For a pipeline it chains children via
	** pipes.  The return value is the exit status of the last command,
	** which we save so that $? gives the correct answer next time.
	*/
	shell->last_exit = execute(shell->ast, shell);

	/*
	** Clean up the AST after execution.  We set ast to NULL so the shell
	** struct is always consistent – a NULL ast means 'nothing pending'.
	*/
	free_ast(shell->ast);
	shell->ast = NULL;
}

/**
 * @brief Reads a full (possibly multi-line) input from the user.
 *
 * If the initial line has unclosed quotes, additional lines are read
 * with a continuation prompt until all quotes are balanced.
 *
 * @return The assembled input string, or NULL on EOF.
 */
static char	*read_full_input(void)
{
	char	*line;
	char	*cont;
	char	*tmp;

	/*
	** readline() shows the prompt, reads one line (without the trailing
	** newline), and returns it as a malloc'd string.
	** It returns NULL when the user presses Ctrl-D (EOF), which signals
	** that the shell should exit.
	*/
	line = readline("minishell> ");
	if (!line)
		return (NULL);

	/*
	** Multi-line input support: if the user typed something like
	**   echo "hello
	** the opening " is still unclosed.  has_unclosed_quote() detects that
	** and we keep reading with a secondary prompt ("> ") until every quote
	** is closed.  Each extra line is joined to 'line' with a literal '\n'
	** in between, so the final string is exactly what bash would pass to
	** the parser (newlines inside quotes are preserved as-is).
	*/
	while (has_unclosed_quote(line))
	{
		cont = readline("> ");
		if (!cont)
			break ;
		/* ft_strjoin3 concatenates three strings: line + "\n" + cont */
		tmp = ft_strjoin3(line, "\n", cont);
		free(line);
		free(cont);
		if (!tmp)
			return (NULL);
		line = tmp;
	}
	return (line);
}

/**
 * @brief Main read-evaluate-print loop of the shell.
 *
 * Sets up signals for interactive use, reads input, and sends each
 * non-blank line to run_line().  NULL from readline means EOF (Ctrl-D),
 * so we print "exit" (matching bash behaviour) and break.
 *
 * @param shell The shell context.
 */
static void	main_loop(t_shell *shell)
{
	while (1)
	{
		/*
		** Re-install signal handlers at the top of every iteration so
		** that they are always in 'interactive' mode here (SIGINT prints
		** a new line and redraws the prompt; SIGQUIT is ignored).
		** Signals are changed to different handlers during child execution
		** and heredoc reading, so we must reset them on each loop.
		*/
		setup_signals();

		shell->input = read_full_input();

		/*
		** readline returns NULL on Ctrl-D (EOF).  We print "exit" to
		** match the output of a real bash session, then break out of
		** the loop to reach cleanup code.
		*/
		if (!shell->input)
		{
			ft_putstr_fd("exit\n", STDOUT_FILENO);
			break ;
		}

		/*
		** Ignore lines that are nothing but spaces/tabs – no need to
		** add them to history or waste time parsing them.
		*/
		if (is_blank_line(shell->input))
		{
			free(shell->input);
			shell->input = NULL;
			continue ;
		}

		/*
		** add_history() stores the line in readline's history buffer so
		** the user can scroll back to it with the up-arrow key.
		** We only add non-blank lines (checked above).
		*/
		add_history(shell->input);

		/* Run the full lexer/parser/expander/executor pipeline. */
		run_line(shell->input, shell);

		/* Free the input string and reset the pointer for the next cycle. */
		free(shell->input);
		shell->input = NULL;
	}
}

/**
 * @brief Entry point of the minishell program.
 *
 * Initialises the shell struct and the environment list, runs the
 * main loop, then performs final cleanup before returning.
 *
 * @param argc The argument count (unused – minishell takes no arguments).
 * @param argv The argument vector (unused).
 * @param envp The environment strings passed by the OS ("KEY=VALUE" pairs).
 * @return The exit status of the last command that was executed.
 */
int	main(int argc, char **argv, char **envp)
{
	t_shell	shell;

	/*
	** argc and argv are silenced because this shell intentionally
	** ignores any command-line arguments (like the real minishell subject).
	*/
	(void)argc;
	(void)argv;

	/*
	** env_init() converts the raw 'envp' array (e.g. "PATH=/usr/bin")
	** into an internal linked list of t_env nodes so the rest of the
	** shell can look up, set, and unset variables efficiently.
	*/
	shell.env = env_init(envp);

	/* last_exit tracks the exit code of the most-recently run command.
	** Starts at 0 (= "success") so that $? is 0 before any command runs. */
	shell.last_exit = 0;

	/* input and ast are set to NULL so free() on them before first use
	** is always safe (free(NULL) is a no-op in C). */
	shell.input = NULL;
	shell.in_heredoc = 0;
	shell.ast = NULL;

	/* Enter the interactive read-evaluate-print loop. */
	main_loop(&shell);

	/*
	** Cleanup: free the environment list and clear readline's internal
	** history buffer to avoid memory leaks that would show in valgrind.
	*/
	env_free(shell.env);
	rl_clear_history();

	/* Return the last exit code so the parent process (your terminal)
	** can see what status the shell exited with. */
	return (shell.last_exit);
}
