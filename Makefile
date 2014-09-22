TARGET           = e1gen 
VERSION          = "0.0.1"

SRCS = e1.c e1gen.c e1ts.c params.c
OBJS = $(SRCS:.c=.o)
CFLAGS += -DVERSION=\"$(VERSION)\"
CFLAGS += -Wall -Werror --std=gnu99
#CFLAGS += -DDEBUG

e1gen: $(OBJS) 

all: $(TARGET) 

clean:
	rm -f $(OBJS)

distclean: clean
	rm -f $(TARGET)
