#!/bin/bash
filename="$1"

if [ -z "$filename" ]; then
    echo "Usage: $0 <filename>"
    exit 1
fi

if [ ! -f "$filename" ]; then
    echo "File not found: $filename"
    exit 1
fi

filesize=$(stat -c %s "$filename")
echo -e "\e[34mSize of $filename is $filesize bytes\e[0m"