GCC_SANITIZER_FLAGS=-fsanitize=undefined -fno-sanitize-recover=all
GCC_COMMON_FLAGS=$(GCC_SANITIZER_FLAGS) -Wall -Wextra -Werror -pedantic -std=c++20
GCC_FAST_FLAGS=$(GCC_COMMON_FLAGS) -O3
GCC_DEBUG_FLAGS=$(GCC_COMMON_FLAGS) -Og -g

PLANTRI_FLAGS=-dhc2m2P

DN=/dev/null

.PHONY: clean run test benchmark

clean:
	find build/ -type f ! -name '.gitignore' -delete

build/main: src/*.hpp src/*.cpp
	g++ $(GCC_FAST_FLAGS) -o build/main src/main.cpp

build/main_debug: src/*.hpp src/*.cpp
	g++ $(GCC_DEBUG_FLAGS) -o build/main_debug src/main.cpp

build/random_test: src/*.hpp src/*.cpp
	g++ $(GCC_FAST_FLAGS) -o build/random_test src/random_test.cpp

run: build/main
	/bin/time build/main $(input) $(solver) $(process)

test: build/main build/random_test
	@for file in ./graphs/easy/*; do \
		output1=$$(python scripts/flow_poly.py < $$file); \
		output2=$$(./build/main numeric all 0 < $$file); \
		if [ "$$output1" != "$$output2" ]; then echo "Test failed"; exit 1; fi \
	done
	@echo
	@./build/random_test | /bin/time build/main numeric all 0 >$(DN)
	@echo
	@plantri $(PLANTRI_FLAGS) 14d 2>$(DN) | /bin/time build/main plantri all 0 >$(DN)

# `v` is the number of inner and outer vertices (n+k)
benchmark: build/main
	@plantri $(PLANTRI_FLAGS) $$(($(v)-2))d 2>$(DN) | /bin/time build/main plantri naive $(process) >$(DN)
	@echo
	@plantri $(PLANTRI_FLAGS) $$(($(v)-2))d 2>$(DN) | /bin/time build/main plantri sequential $(process) >$(DN)
