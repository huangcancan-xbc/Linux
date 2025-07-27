# 目标静态库名称
lib = My_static_library.a

# 构建静态库目标：由目标文件构建 .a 文件
$(lib): My_static_library.o
	ar -rc $@ $^

# 生成 .o 目标文件
My_static_library.o: My_static_library.c
	gcc -c $^

# 清理构建产物
.PHONY: clean
clean:
	rm -rf *.o *.a lib

# 输出头文件和库文件到标准目录
.PHONY: output
output:
	mkdir -p lib/include                        # 创建头文件目录
	mkdir -p lib/My_static_library_lib          # 创建库文件目录
	cp *.h lib/include                          # 复制头文件
	cp *.a lib/My_static_library_lib  	        # 复制静态库文件
