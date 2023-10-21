# TODO enable sanitizers? only sometimes?
# GCC_SANITIZER_FLAGS=-fsanitize=address -fsanitize=undefined -fno-sanitize-recover=all
GCC_COMMON_FLAGS=-Wall -Wextra -Werror -pedantic -std=c++20
PLANTRI_FLAGS=-dhc2m2P

DN=/dev/null

.PHONY: clean run test benchmark

clean:
	find build/ -type f ! -name '.gitignore' -delete

build/main: src/*.hpp src/*.cpp
	g++ $(GCC_COMMON_FLAGS) -O3 -o build/main src/main.cpp

build/main_debug: src/*.hpp src/*.cpp
	g++ $(GCC_COMMON_FLAGS) -g -o build/main_debug src/main.cpp

build/random_test: src/*.hpp src/*.cpp
	g++ $(GCC_COMMON_FLAGS) -O3 -o build/random_test src/random_test.cpp

run: build/main
	/bin/time build/main $(input) $(solver)

test: build/main build/random_test
	@for file in ./graphs/easy/*; do \
		output1=$$(python scripts/flow_poly.py < $$file); \
		output2=$$(./build/main numeric all < $$file); \
		if [ "$$output1" != "$$output2" ]; then echo "Test failed"; exit 1; fi \
	done
	@echo
	@./build/random_test | /bin/time build/main numeric all >$(DN)
	@echo
	@plantri $(PLANTRI_FLAGS) 14d 2>$(DN) | /bin/time build/main plantri all >$(DN)

# `nk` is the number of inner and outer vertices (n+k)
benchmark: build/main
	@plantri $(PLANTRI_FLAGS) $$(($(nk)-2))d 2>$(DN) | /bin/time build/main plantri naive >$(DN)
	@echo
	@plantri $(PLANTRI_FLAGS) $$(($(nk)-2))d 2>$(DN) | /bin/time build/main plantri sequential >$(DN)
