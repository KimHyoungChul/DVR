#!/bin/sh
PATH=.:src:stunt stunt -bftro output.txt -v --debug-file debug.txt 128.84.154.62
echo -n "Press any key to continue . . . "
read -sn1
echo
