#gcc -Wall -pedantic tests/main.c -o main -Ofast -march=native -fopenmp
#g++ -Wall -pedantic tests/testcpp.cpp -o test -Ofast -march=native -fopenmp

#Compilazione separata

gcc -Wall -pedantic -c lib/logmmap.c -Ofast -march=native -fopenmp
gcc -Wall -pedantic tests/main.c logmmap.o -o main -Ofast -march=native -fopenmp

