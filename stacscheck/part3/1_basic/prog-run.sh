#!/bin/bash

mkdir -p temp-files
cp "${TESTDIR}/print_args.sh" temp-files/print_args.sh

(cd temp-files; ../runparallelcmds)

rm temp-files/print_args.sh
