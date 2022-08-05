CC=gcc
CFLAGS=-Wall -Wextra -Iinclude
LDFLAGS=-lm -pthread

GAME=byzantium
GAME_SRCS=$(wildcard src/core/*.c src/game/*.c src/ui/*.c)
GAME_OBJS=$(GAME_SRCS:.c=.o)
GAME_DEPS=$(GAME_SRCS:.c=.d)
GAME_MAIN=src/main.o

GAME_TEST=byzantium-test
TEST_SRCS=$(wildcard src/test/*.c src/test/core/*.c)
TEST_OBJS=$(TEST_SRCS:.c=.o)
TEST_DEPS=$(TEST_SRCS:.c=.d)

.PHONY: clean fullclean debug fast test profile $(GAME)

%.o: %.c
	$(CC) -MMD $(CFLAGS) -c $< -o $@

$(GAME): $(GAME_OBJS) $(GAME_MAIN)
	$(CC) $(CFLAGS) $(GAME_OBJS) $(GAME_MAIN) -o $(GAME) $(LDFLAGS)

fast: CFLAGS += -Ofast
fast: $(GAME)

debug: CFLAGS += -DDEBUG -g
debug: $(GAME)

test-debug: CFLAGS += -DDEBUG -g
test-debug: test

profile: CFLAGS += -g -pg
profile: $(GAME)

test: $(GAME_OBJS) $(TEST_OBJS)
	$(CC) $(CFLAGS) $(GAME_OBJS) $(TEST_OBJS) -o $(GAME_TEST) $(LDFLAGS)

clean:
	@rm -f $(GAME_OBJS)
	@rm -f $(GAME_DEPS)
	@rm -f $(TEST_OBJS)
	@rm -f $(TEST_DEPS)

fullclean: clean
	@rm -f $(GAME)
	@rm -f $(GAME_TEST)

-include $(DEPS)
