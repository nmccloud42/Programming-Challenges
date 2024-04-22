CC = gcc
CFLAGS = -Wall -pthread
SRCS = ThreadedMatrixMul�plica�on.c
OBJS = $(SRCS:.c=.o)
TARGET = ThreadedMatrixMul�plica�on
all: $(TARGET)
$(TARGET): $(OBJS)
$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)
%.o: %.c
$(CC) $(CFLAGS) -c $< -o $@
clean:
rm -f $(OBJS) $(TARGET)
