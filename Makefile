main: main.c
	clang -Wno-initializer-overrides -ggdb -fsanitize=undefined,address -o main main.c
