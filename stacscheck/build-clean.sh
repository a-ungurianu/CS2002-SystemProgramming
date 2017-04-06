#!/bin/bash

make clean
rm -f temp-files/print_args.sh temp-files/output1.txt temp-files/output2.txt "temp-files/output space 1.txt" "temp-files/output space 2.txt"
rm temp-files 2> /dev/null

# The 'true' makes sure we still mark as passing if any of these things failed
true