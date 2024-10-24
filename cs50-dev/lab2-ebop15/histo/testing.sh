# testing echo
echo {1..16} 150 | ./histo
# negative numbers
echo -5 23 | ./histo
# up to 128
echo 128 | ./histo
# testing expanding range
echo 1 2 4 8 16 32 64 128 256 | ./histo
# testing 0
echo 0 1 2 3 | ./histo