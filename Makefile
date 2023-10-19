COMMON_FLAGS=-Wall -Wextra -Werror -pedantic -std=c++20

build/main: src/*.hpp src/*.cpp
	g++ $(COMMON_FLAGS) -O3 -o build/main src/main.cpp

build/main_debug: src/*.hpp src/*.cpp
	g++ $(COMMON_FLAGS) -g -o build/main_debug src/main.cpp

run: build/main
	/bin/time build/main
