NAME=slcc
CFLAGS=-std=c11 -g -static
SRCS=main.c codegen.c tokenizer.c parser.c
OBJS=$(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	cc -o $(NAME) $(OBJS)

$(OBJS):


.PHONY: clean fclean re test

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME) tmp*

re: fclean all

test: slcc
	./test.sh
