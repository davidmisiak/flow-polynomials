COMMON_FLAGS=-Wall -Wextra -Werror -pedantic -std=c++20

clean:
	find build/ -type f ! -name '.gitignore' -delete

build/main: src/*.hpp src/*.cpp
	g++ $(COMMON_FLAGS) -O3 -o build/main src/main.cpp

build/main_debug: src/*.hpp src/*.cpp
	g++ $(COMMON_FLAGS) -g -o build/main_debug src/main.cpp

build/random_test: src/*.hpp src/*.cpp
	g++ $(COMMON_FLAGS) -O3 -o build/random_test src/random_test.cpp

run: build/main
	/bin/time build/main $(input) $(solver)

test: build/main build/random_test
	for file in ./graphs/easy/*; do \
		output1=$$(python scripts/flow_poly.py < $$file); \
		output2=$$(./build/main numeric all < $$file); \
		if [ "$$output1" != "$$output2" ]; then \
			echo "Test failed"; \
			exit 1; \
		fi \
	done
	./build/random_test | build/main numeric all > /dev/null
	# TODO plantri
