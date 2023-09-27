BINS = cell_distances
CFLAGS = -O3 -fopenmp -lm -march=native

.PHONY : all
all : $(BINS)

.PHONY : clean
clean :
	rm -rf ./build

.PHONY : run
run : cell_distances
	./build/cell_distances

cell_distances : src/main.c src/distances.c src/read_file.c
	mkdir -p build
	gcc -o ./build/$@ $^ $(CFLAGS)

test : test/test.c src/distances.c src/read_file.c
	mkdir -p build_test
	gcc -o ./build_test/$@ $^ $(CFLAGS)
	./build_test/test
