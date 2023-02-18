# Target library
target := libuthread.a 
objs := context.o queue.o uthread.o sem.o

CC := gcc
CFLAGS := -Wall -Wextra -Werror -MMD
CFLAGS += -g
PANDOC += pandoc 

ifneq ($(V),1)
Q = @
endif

all: $(target)

#Dep Tracking
deps := $(patstubt %.o, %.d, $(objs))
-include $(deps)

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
	$(Q)rm -f $(target) $(objs) $(deps)




