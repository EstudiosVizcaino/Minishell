NAME = minishell
CC = cc
CFLAGS = -Wall -Wextra -Werror
LIBFT_DIR = libft
LIBFT = $(LIBFT_DIR)/libft.a

# macOS: auto-detect Homebrew readline (works on both Intel and Apple Silicon)
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S), Darwin)
    BREW_PREFIX := $(shell brew --prefix readline 2>/dev/null)
    ifeq ($(BREW_PREFIX),)
        BREW_PREFIX := /usr/local/opt/readline
    endif
    READLINE_INC := -I$(BREW_PREFIX)/include
    READLINE_LIB := -L$(BREW_PREFIX)/lib
else
    READLINE_INC :=
    READLINE_LIB :=
endif

SRCS =	srcs/main.c \
	srcs/signals/signals.c \
	srcs/signals/signals_wait.c \
	srcs/env/env.c \
	srcs/env/env_utils.c \
	srcs/lexer/lexer.c \
	srcs/lexer/lexer_utils.c \
	srcs/lexer/lexer_word.c \
	srcs/parser/parser.c \
	srcs/parser/parser_entry.c \
	srcs/parser/parser_cmd.c \
	srcs/parser/parser_redir.c \
	srcs/expander/expander.c \
	srcs/expander/expander_args.c \
	srcs/expander/expander_utils.c \
	srcs/expander/expander_str.c \
	srcs/expander/expander_word_split.c \
	srcs/executor/executor.c \
	srcs/executor/executor_run.c \
	srcs/executor/executor_pipe.c \
	srcs/executor/executor_redir.c \
	srcs/executor/executor_utils.c \
	srcs/executor/executor_cmd.c \
	srcs/builtins/builtin_echo.c \
	srcs/builtins/builtin_cd.c \
	srcs/builtins/builtin_pwd.c \
	srcs/builtins/builtin_export.c \
	srcs/builtins/builtin_export_print.c \
	srcs/builtins/builtin_unset.c \
	srcs/builtins/builtin_env.c \
	srcs/builtins/builtin_exit.c \
	srcs/utils/utils.c \
	srcs/utils/free.c

# Files replaced by bonus versions
BONUS_REPLACE = srcs/main.c \
	srcs/lexer/lexer_utils.c \
	srcs/parser/parser_entry.c \
	srcs/executor/executor_run.c \
	srcs/expander/expander.c \
	srcs/expander/expander_str.c

BONUS_SRCS = srcs_bonus/main_bonus.c \
	srcs_bonus/lexer_utils_bonus.c \
	srcs_bonus/parser_bonus.c \
	srcs_bonus/parser_entry_bonus.c \
	srcs_bonus/parse_andor_bonus.c \
	srcs_bonus/executor_bonus.c \
	srcs_bonus/expander_bonus.c \
	srcs_bonus/expander_str_bonus.c \
	srcs_bonus/wildcard_match_bonus.c \
	srcs_bonus/wildcard_bonus.c \
	srcs_bonus/wildcard_expand_bonus.c \
	srcs_bonus/wildcard_redir_bonus.c

OBJS = $(SRCS:.c=.o)

INCLUDES = -I includes -I $(LIBFT_DIR) $(READLINE_INC)

all: $(NAME)

$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)

$(NAME): $(LIBFT) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -L$(LIBFT_DIR) -lft $(READLINE_LIB) -lreadline -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

BONUS_CFLAGS = -Wall -Wextra -Werror -DBONUS
BONUS_SRCS_ALL = $(filter-out $(BONUS_REPLACE), $(SRCS)) $(BONUS_SRCS)
BONUS_OBJS_ALL = $(BONUS_SRCS_ALL:.c=.bo)

bonus: fclean $(LIBFT) $(BONUS_OBJS_ALL)
	$(CC) $(BONUS_CFLAGS) $(BONUS_OBJS_ALL) -L$(LIBFT_DIR) -lft $(READLINE_LIB) -lreadline -o $(NAME)

%.bo: %.c
	$(CC) $(BONUS_CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(MAKE) -C $(LIBFT_DIR) clean
	rm -f $(OBJS) $(BONUS_OBJS_ALL)

fclean: clean
	$(MAKE) -C $(LIBFT_DIR) fclean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re bonus
