NAME=slcc
CFLAGS=-std=c11 -g -static -Wall -Wextra
SRCS=main.c codegen.c tokenizer.c parser.c var.c
OBJS=$(SRCS:.c=.o)
CC=clang

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) -o $(NAME) $(OBJS)

$(OBJS):


.PHONY: clean fclean re test

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME) tmp*

re: fclean all

test: slcc
	./test.sh
