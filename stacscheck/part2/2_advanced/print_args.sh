#!/bin/bash

echo "Starting"
for i in "$@"; do
    echo "Argument: '"$i"'"
done
echo "Finishing"