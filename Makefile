CC = gcc
CFLAGS = -Wall -fPIC -g
LDFLAGS = -shared -ldl -Wl,-soname,libmy_secmalloc.so
TARGET = build/libmy_secmalloc.so
SRC = src/main.c src/log.c
OBJ = build/main.o build/log.o

all: $(TARGET)

dynamic: CFLAGS += -D_DYNAMIC
dynamic: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) -o $(TARGET) $(OBJ)

build/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJ)
