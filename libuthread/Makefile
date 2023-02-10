# Target library
lib := libuthread.a 
objs:= queue.o 

CC := gcc
CFLAGS := -Wall -Wextra -Werror -MMD
CFLAGS += -g
PANDOC += pandoc 

ifneq ($(V),1)
Q = @
endif

all: $(lib)

## TODO: Phase 1

#Dep Tracking
deps := $(patstubt %.o, %.d, $(objs))
-include $(deps)

queue: $(objs)
	@echo "CC $@"
	$(Q)$(CC) $(CFLAGS) -o $@ $^

# https://stackoverflow.com/questions/31421616/c-creating-static-library-and-linking-using-a-makefile

libuthread.a: $(objs)
	ar rcs $@ $^

%.o: %.c 
	@echo "CC $@"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

%.html: %.md 
	@echo "MD $@"
	$(Q)$(PANDOC) -o $@ $<

clean:
	@echo "clean"
	$(Q)rm -f $(targets) $(objs) $(deps)




