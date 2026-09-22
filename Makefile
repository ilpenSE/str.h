main: main.c
	clang -Wno-initializer-overrides -ggdb -fsanitize=address -o main main.c
