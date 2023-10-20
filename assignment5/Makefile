BINS = diffusion
LIBRARIES = -L/usr/lib64/openmpi/lib -lm -lmpi -Wl,-rpath,/usr/lib64/openmpi/lib -Wl,--enable-new-dtags
CFLAGS  = -O3 -I. -I/usr/include/openmpi-x86_64 -pthread $(LIBRARIES)

.PHONY : all
all : $(BINS)

.PHONY : clean
clean :
	rm -rf ./build
	rm -rf ./build_test

.PHONE : bin
bin : diffusion

diffusion : src/main.c src/cli.c src/read_input.c src/diffusion.c
	mkdir -p build
	gcc -o ./build/$@ $^ $(CFLAGS)

