#!/bin/sh
find -H ./core/ -name '*.h' -a \( -type d -o -type f -o -type l \) -exec cp -a '{}' ./include/ \;
