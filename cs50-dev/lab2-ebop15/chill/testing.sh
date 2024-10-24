# no arguments
./chill
# one argument, in range
./chill 32
# one argument, above range
./chill 1001
# one argument, below range
./chill -1000
# two arguments
./chill 32 10
# two arguments, temp above range
./chill 1000 5
# two arguments, temp below range
./chill -500 5
# two arguments, wind below range
./chill 32 -100
# two arguments, wind above range
./chill 32 1000