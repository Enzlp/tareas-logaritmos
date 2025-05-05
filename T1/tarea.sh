#!/bin/bash
g++ -O2 -o main_tests main.cpp mergesort/mergesort_externo.cpp file_generator/input_generator.cpp quicksort/quicksort_externo.cpp
./main_tests 50
