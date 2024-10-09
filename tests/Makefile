CC = gcc
CFLAGS = -I../src/
SRCS = test.c ../src/main.c ../src/log.c
LIBS = -lcriterion
TARGET = test

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) $(LIBS) -o $(TARGET)

clean:
	rm -f $(TARGET)
