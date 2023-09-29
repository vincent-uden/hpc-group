import os
import shutil
import argparse

makefile_template = """
CFLAGS = -O3 -fopenmp -lm -march=native

.PHONY : all
all : TARGET

TARGET : SOURCE_FILES
\tgcc -o ./$@ $^ $(CFLAGS)

.PHONY : clean
clean :
\trm -f ./TARGET
"""

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Create a submission archive.")
    parser.add_argument("target", help="Name of the target binary", default="main")
    args = parser.parse_args()

    src_files = os.listdir("./src")
    src_files = list(filter(lambda x: (not x.startswith("benchmark")) and (not x.startswith("test")),src_files))

    makefile_str = makefile_template.replace("SOURCE_FILES", " ".join(filter(lambda x: x[-1] != "h", src_files)))
    makefile_str = makefile_str.replace("TARGET", args.target)

    if os.path.exists("submit"):
        shutil.rmtree("submit")
    os.makedirs("submit", exist_ok=True)
    os.chdir("submit")

    for file in src_files:
        shutil.copyfile("../src/" + file, "./" + file)

    with open("makefile", "w") as makefile:
        makefile.write(makefile_str)

    os.system("tar czf submit.tar.gz *")
