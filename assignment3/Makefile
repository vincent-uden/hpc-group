BINS = newton
CFLAGS = -O3 -lpthreads -lm -march=native -Wall -Wextra -Wshadow -std=c99 -g

.PHONY : all
all : $(BINS)

.PHONY : clean
clean :
	rm -rf ./build
	rm -rf ./build_test

.PHONY : run
run : newton
	./build/newton --input_file ./test/small_sample.txt --threads 4

.PHONE : bin
bin : newton

newton : src/main.c src/newton.c src/cli.c src/slave.c
	mkdir -p build
	gcc -o ./build/$@ $^ $(CFLAGS)
