CFLAGS := -Wall -Wextra -Werror -g -O2

TARGET := gendata

all: $(TARGET)

$(TARGET): gendata.c gen.c 
	gcc $(CFLAGS) $^ -o $@