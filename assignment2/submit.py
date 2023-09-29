import os
import shutil

makefile_template = """
CFLAGS = -O3 -fopenmp -lm -march=native

.PHONY : all
all : main

main : SOURCE_FILES
\tgcc -o ./$@ $^ $(CFLAGS)

.PHONY : clean
clean :
\trm -f ./main
"""

if __name__ == "__main__":
    src_files = os.listdir("./src")
    src_files = list(filter(lambda x: (not x.startswith("benchmark")) and (not x.startswith("test")),src_files))

    makefile_str = makefile_template.replace("SOURCE_FILES", " ".join(filter(lambda x: x[-1] != "h", src_files)))

    if os.path.exists("submit"):
        shutil.rmtree("submit")
    os.makedirs("submit", exist_ok=True)
    os.chdir("submit")

    for file in src_files:
        shutil.copyfile("../src/" + file, "./" + file)

    with open("makefile", "w") as makefile:
        makefile.write(makefile_str)

    os.system("tar czf submit.tar.gz *")
