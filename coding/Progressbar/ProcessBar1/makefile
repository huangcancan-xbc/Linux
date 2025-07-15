# 定义目标和源文件
TARGET = processBar
SRCS = processBar.c processBarmain.c
OBJS = $(SRCS:.c=.o)

# 定义编译器和选项
CC = gcc
CFLAGS = -I. -Wall -O2

# 默认目标
all: $(TARGET)

# 生成目标文件
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

# 生成对象文件
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 清理生成的文件
.PHONY: clean
clean:
	rm -f processBar.i processBar.s processBar.o processBar
