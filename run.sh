#!/bin/bash

# Compile the code
g++.exe -g -std=c++17 -I./include -L./lib ./src/*.c ./src/*.cpp -lglfw3dll -o ./out/program.exe

# Check if the compilation was successful
if [ $? -eq 0 ]; then
    # Run the program
    ./out/program.exe
else
    echo "Compilation failed."
fi