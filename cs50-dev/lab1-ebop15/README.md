# Ethan Child 
## CS50 Fall 2023, Lab 1

GitHub username: ebop15

## A

## B
ln -s cs50-dev/shared/COVID-19/data_tables/vaccine_data/us_data/hourly/vaccine_data_us.csv vaccine_data_us.csv
## C
head -1 vaccine_data_us.csv
## D
head -1 vaccine_data_us.csv | sed 's/,/\n/g'
## E
grep "New Hampshire" vaccine_data_us.csv
## F
grep "All" vaccine_data_us.csv | cut -d ',' -f 2,10
## G
grep "All" vaccine_data_us.csv | cut -d ',' -f 2,10 | sort -t "," -k 2,2nr | head -10
## H
grep "All" vaccine_data_us.csv | cut -d ',' -f 2,10 | sort -t "," -k 2,2nr | head -10 | sed 's/,/|/g' | sed 's/^/|/' | sed 's/$/|/'

