CC := cc
CFLAGS := -Wall -Wextra -std=c11
SRC := src
PROGRAMS := 1 2 3 4 5
BINS := $(addprefix $(SRC)/,$(PROGRAMS))

all: $(BINS)

$(SRC)/3: $(SRC)/3.c
	$(CC) $(CFLAGS) -pthread $< -o $@

$(SRC)/%: $(SRC)/%.c
	$(CC) $(CFLAGS) $< -o $@

run: all
	@for p in $(PROGRAMS); do echo "===== $$p ====="; (cd $(SRC) && ./$$p); echo; done

clean:
	rm -f $(BINS) $(SRC)/q.txt

.PHONY: all run clean
