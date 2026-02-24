NAME = minishell
CC = cc
CFLAGS = -Wall -Wextra -Werror
LIBFT_DIR = libft
LIBFT = $(LIBFT_DIR)/libft.a

SRCS =	srcs/main.c \
	srcs/signals/signals.c \
	srcs/env/env.c \
	srcs/env/env_utils.c \
	srcs/lexer/lexer.c \
	srcs/lexer/lexer_utils.c \
	srcs/parser/parser.c \
	srcs/parser/parser_cmd.c \
	srcs/parser/parser_redir.c \
	srcs/expander/expander.c \
	srcs/expander/expander_utils.c \
	srcs/executor/executor.c \
	srcs/executor/executor_pipe.c \
	srcs/executor/executor_redir.c \
	srcs/executor/executor_utils.c \
	srcs/builtins/builtin_echo.c \
	srcs/builtins/builtin_cd.c \
	srcs/builtins/builtin_pwd.c \
	srcs/builtins/builtin_export.c \
	srcs/builtins/builtin_unset.c \
	srcs/builtins/builtin_env.c \
	srcs/builtins/builtin_exit.c \
	srcs/utils/utils.c \
	srcs/utils/free.c

BONUS_SRCS = srcs_bonus/parser_bonus.c \
	srcs_bonus/executor_bonus.c \
	srcs_bonus/wildcard_bonus.c

OBJS = $(SRCS:.c=.o)
BONUS_OBJS = $(BONUS_SRCS:.c=.o)

INCLUDES = -I includes -I $(LIBFT_DIR)

all: $(NAME)

$(LIBFT):
	$(MAKE) -C $(LIBFT_DIR)

$(NAME): $(LIBFT) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -L$(LIBFT_DIR) -lft -lreadline -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

bonus: $(LIBFT) $(OBJS) $(BONUS_OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(BONUS_OBJS) -L$(LIBFT_DIR) -lft -lreadline -o $(NAME)

clean:
	$(MAKE) -C $(LIBFT_DIR) clean
	rm -f $(OBJS) $(BONUS_OBJS)

fclean: clean
	$(MAKE) -C $(LIBFT_DIR) fclean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re bonus
