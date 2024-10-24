#!/bin/sh

# File name: Summarize.sh
# Made by Ethan Child

# this checks if there's a file argument at all
if [ $# == 0 ]; then
  echo "Please specify a file."
  exit 1
fi

# iterates over every path argument
for path in "$@"; do

  # checks if the path is readable
  if [[ ! -r "$path" ]]; then
    echo "file is not readable"
    continue
  fi

  # checks if the path ends in .sh, .h, or .c. If it doesn't, skips over the file
  if [[ ! "$path" == *.sh && ! "$path" == *.h && ! "$path" == *.c ]]; then
    echo "file does not end in .sh, .h, or .c"
    continue
  fi

  
  # isolates the basename and echoes it
  basename=$(basename $path)
  echo "## $basename"

  # if the file type is .sh, has bash as the triple tick type
  if [[ "$path" == *.sh ]]; then
    echo '```bash'
    # stream editor that deletes the line that starts with #! and goes until it finds a line that has nothing in it 
    sed -n '/^#!/d; /^$/q; p' "$1"
    echo '```'
  fi
  # same as above, but checks for .c and .h files
  if [[  "$path" == *.c || "path" == *.h ]]; then
    echo '```c'
    sed -n '/^#!/d; /^$/q; p' "$1"
    echo '```'
  fi
done
exit 0