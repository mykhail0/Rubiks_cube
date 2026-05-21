#!/bin/bash

for n in $(seq 1 10)
do
    filename=test${n}
    < tests/"${filename}".in valgrind --leak-check=full -q bin/test_executable > tests/"${filename}".t
    if diff --color=auto tests/"${filename}".t tests/"${filename}".out
    then
        echo "${filename}: Ok"
        rm tests/"${filename}".t
    else
        echo "${filename}: Failed"
    fi
done
