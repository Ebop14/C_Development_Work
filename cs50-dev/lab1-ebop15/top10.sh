#!/bin/sh

# File name: top10.sh
# Made by Ethan Child

# no file specified
if [ $# == 0 ]; then
  echo "Please specify a file."
  exit 1
fi

# no file found
if [ ! -f "$1" ]; then
  echo "File not found"
  exit 2
fi

# file isn't readable
if [ ! -r "$1" ]; then
  echo "File not readable"
  exit 1
fi

# adds the header and greps the rest
echo "| Area | Doses administered |"
echo "| :--------- | --------: |"
grep "All" "$1" | cut -d ',' -f 2,10 | sort -t "," -k 2,2nr | head -10 | sed 's/,/|/g' | sed 's/^/|/' | sed 's/$/|/'
exit 0