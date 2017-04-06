#!/bin/bash


mkdir -p temp-files
rm -f "temp-files/output space 1.txt" "temp-files/output space 2.txt"

set -e

./runcmds << INPUT
/bin/echo "hello" > "temp-files/output space 1.txt"
/bin/echo "goodbye" > "temp-files/output space 2.txt"
INPUT

if ! [ -e "temp-files/output space 1.txt" ]; then
    echo "Error: temp-files/output space 1.txt should have been created!"
    exit 1
fi;

if ! diff <(echo hello) "temp-files/output space 1.txt"; then
    echo "Error: temp-files/output space 1.txt has wrong contents"
    exit 1
fi;

if ! [ -e "temp-files/output space 2.txt" ]; then
    echo "Error: temp-files/output space 2.txt should have been created!"
    exit 1
fi;

if ! diff <(echo goodbye) "temp-files/output space 2.txt"; then
    echo "Error: temp-files/output space 2.txt has wrong contents"
    exit 1
fi;

./runcmds << INPUT
echo "hello" > "temp-files/output space 1.txt"
echo "goodbye" > "temp-files/output space 2.txt"
INPUT

rm -f "temp-files/output space 1.txt" "temp-files/output space 2.txt"