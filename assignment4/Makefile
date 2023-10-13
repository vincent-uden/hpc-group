BINS = diffusion
CFLAGS = -O3 -lOpenCL -lm -Wall -Wextra -Wshadow -std=c11 -g

.PHONY : all
all : $(BINS)

.PHONY : clean
clean :
	rm -rf ./build
	rm -rf ./build_test

.PHONE : bin
bin : diffusion

diffusion : src/main.c src/cli.c
	mkdir -p build
	gcc -o ./build/$@ $^ $(CFLAGS)

