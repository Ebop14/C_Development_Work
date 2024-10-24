# testing input with prompt from the user
./words

# testing file opening
./words sentences.txt

# testing with bad file
./words 123

# testing with -
./words -

# testing with -, then file
./words - sentences.txt

# testing with file, then -
./words sentences.txt -

# testing with echo
echo Hello there! This is my lab submission. I deleted my file in git last time | ./words
