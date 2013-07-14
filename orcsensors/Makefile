CFLAGS = -g -Wall 
LDFLAGS = -g

INCLUDES := . ./src
LIBPATH := 
LIBS := 

CFLAGS += $(addprefix -I,$(INCLUDES))
VPATH = src
SOURCES = $(wildcard $(VPATH)/*.c)
OBJS = $(patsubst %.c, %.o, $(SOURCES))
TARGET := $(shell pwd | xargs basename)
LDFLAGS := $(addprefix -L,$(LIBPATH)) $(addprefix -l,$(LIBS))

all:	$(OBJS)
	$(CC) -o $(TARGET)  $(OBJS) $(LDFLAGS)

clean:
	rm -f $(OBJS) $(DEPS) $(TARGET) 
