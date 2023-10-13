import os
import shutil
import argparse

from itertools import chain

makefile_template = """
CFLAGS = -O3 -lpthread -lm -march=native -std=c11

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
    parser.add_argument("target", help="Name of the target binary", nargs="?", const="main", default="main")
    parser.add_argument("-C", "--concat-inline", action="store_true", default=False, help="Concat all source files into a single file for inlining.")
    args = parser.parse_args()

    src_files = os.listdir("./src")
    src_files = list(filter(lambda x: (not x.startswith("benchmark")) and (not x.startswith("test")),src_files))

    if os.path.exists("submit"):
        shutil.rmtree("submit")
    os.makedirs("submit", exist_ok=True)
    os.chdir("submit")

    if args.concat_inline:
        headers = list(filter(lambda x: x[-1] == "h", src_files))
        c_files = list(filter(lambda x: x[-1] == "c", src_files))
        with open("./src.c", "w") as f:
            for file in c_files:
                with open("../src/" + file, "r") as f2:
                    f.write(f2.read())
                    f.write("\n")
        src_files = ["src.c"] + headers

    makefile_str = makefile_template.replace("SOURCE_FILES", " ".join(filter(lambda x: x[-1] != "h", src_files)))
    makefile_str = makefile_str.replace("TARGET", args.target)

    for file in src_files:
        if file != "src.c":
            shutil.copyfile("../src/" + file, "./" + file)

    with open("makefile", "w") as makefile:
        makefile.write(makefile_str)

    os.system("tar czf submit.tar.gz *")
