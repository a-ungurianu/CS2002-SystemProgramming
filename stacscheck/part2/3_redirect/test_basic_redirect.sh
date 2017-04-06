#!/bin/bash


mkdir -p temp-files
rm -f temp-files/output1.txt temp-files/output2.txt

set -e

./runcmds << INPUT
/bin/echo "hello" > temp-files/output1.txt
/bin/echo "goodbye" > temp-files/output2.txt
INPUT

if ! [ -e temp-files/output1.txt ]; then
    echo "Error: temp-files/output1.txt should have been created!"
    exit 1
fi;

if ! diff <(echo hello) temp-files/output1.txt; then
    echo "Error: temp-files/output1.txt has wrong contents"
    exit 1
fi;

if ! [ -e temp-files/output2.txt ]; then
    echo "Error: temp-files/output2.txt should have been created!"
    exit 1
fi;

if ! diff <(echo goodbye) temp-files/output2.txt; then
    echo "Error: temp-files/output2.txt has wrong contents"
    exit 1
fi;

rm -f temp-files/output1.txt temp-files/output2.txt