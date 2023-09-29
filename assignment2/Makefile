BINS = cell_distances
CFLAGS = -O3 -fopenmp -lm -march=native

.PHONY : all
all : $(BINS)

.PHONY : clean
clean :
	rm -rf ./build
	rm -rf ./build_test

.PHONY : run
run : cell_distances
	./build/cell_distances --input_file ./test/small_sample.txt --threads 4

.PHONE : bin
bin : cell_distances

cell_distances : src/main.c src/distances.c src/read_file.c src/cli.c
	mkdir -p build
	gcc -o ./build/$@ $^ $(CFLAGS)

test : test/test.c src/distances.c src/read_file.c
	mkdir -p build_test
	gcc -o ./build_test/$@ $^ $(CFLAGS)
	./build_test/test

test_distances : src/distances.c
	mkdir -p build_test
	gcc -o ./build_test/$@ $^ -O3 -DTEST_DISTANCES
	./build_test/$@

benchmark_distances : src/distances.c src/benchmark-distances.c
	mkdir -p build_test
	gcc -o ./build_test/$@ $^ -O3
	./build_test/$@
