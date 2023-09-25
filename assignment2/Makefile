BINS = cell_distances test_distances benchmark_distances
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

test_distances : src/distances.c
	mkdir -p build
	gcc -o ./build/$@ $^ -O3 -DTEST_DISTANCES

benchmark_distances : src/distances.c
	mkdir -p build
	gcc -o ./build/$@ $^ -O3 -DBENCHMARK_DISTANCES
