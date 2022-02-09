CC=gcc
CFLAGS=-Wall -Wextra -Iinclude
LDFLAGS=-lm -pthread
GAME=byzantium

INCL=$(wildcard include/*.h)
SRCS=$(wildcard src/*.c src/core/*.c src/game/*.c src/ui/*.c)
#SRCS_CORE=$(wildcard src/core/*.c)
#SRCS_GAME=$(wildcard src/game/*.c)
#SRCS_UI=$(wildcard src/ui/*.c)

OBJS=$(SRCS:.c=.o)
DEPS=$(SRCS:.c=.d)

.PHONY: clean fullclean debug fast $(GAME)

$(GAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(GAME) $(LDFLAGS)

%.o: %.c
	$(CC) -MMD $(CFLAGS) -c $< -o $@

fast: CFLAGS += -Ofast
fast: $(GAME)

debug: CFLAGS += -DDEBUG -g
debug: $(GAME)

clean:
	@rm -f $(OBJS)
	@rm -f $(DEPS)

fullclean: clean
	@rm -f $(GAME)

-include $(DEPS)
