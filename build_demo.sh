#!/bin/sh

# build opencv demo for single file or more
filename="$1"
echo "prompt:you may choose specific file by input the filename as argument of the script before using the script"
if [$1 == ""]; then
    echo "begin to compile all the demo in the current directory"
    for i in *.cpp; do
        # compiling all of the file
        echo "compiling $i"
        g++ -ggdb `pkg-config --cflags opencv` -o `basename $i .cpp` $i `pkg-config --libs opencv`;
    done
else
    for i in *.cpp; do
        if [ "${i}" = "${filename}" ]; then
            # compiling single file
            echo "find file ${i}"
            echo "compiling $i"
            g++ -ggdb `pkg-config --cflags opencv` -o `basename $i .cpp` $i `pkg-config --libs opencv`;
        fi
    done
fi
echo "all compile done!"
exit 0
