GCC_SANITIZER_FLAGS=-fsanitize=undefined -fno-sanitize-recover=all
GCC_COMMON_FLAGS=$(GCC_SANITIZER_FLAGS) -Wall -Wextra -Werror -pedantic -std=c++20
GCC_FAST_FLAGS=$(GCC_COMMON_FLAGS) -O3
GCC_DEBUG_FLAGS=$(GCC_COMMON_FLAGS) -Og -g

PLANTRI=plantri -dhc2m2P
MAIN=/bin/time build/main
DN=/dev/null

.PHONY: clean run test benchmark mods

clean:
	find build/ -type f ! -name '.gitignore' -delete

build/main: src/*.hpp src/*.cpp
	g++ $(GCC_FAST_FLAGS) -o build/main src/main.cpp

build/main_debug: src/*.hpp src/*.cpp
	g++ $(GCC_DEBUG_FLAGS) -o build/main_debug src/main.cpp

build/random_test: src/*.hpp src/*.cpp
	g++ $(GCC_FAST_FLAGS) -o build/random_test src/random_test.cpp

run: build/main
	$(MAIN) $(input) $(solver)

test: build/main build/random_test
	@for file in ./graphs/easy/*; do \
		output1=$$(python scripts/flow_poly.py < $$file); \
		output2=$$(./build/main numeric all < $$file); \
		if [ "$$output1" != "$$output2" ]; then echo "Test failed"; exit 1; fi \
	done
	@echo
	@./build/random_test | $(MAIN) numeric all >$(DN)
	@echo
	@$(PLANTRI) 14d 2>$(DN) | $(MAIN) plantri all >$(DN)
	@echo
	@$(PLANTRI) 20d 0/1000 2>$(DN) | $(MAIN) plantri all >$(DN)

# `v` is the number of inner and outer vertices (n+k)
benchmark: build/main
	@$(PLANTRI) $$(($(v)-2))d 0/$(mod) 2>$(DN) | $(MAIN) plantri naive >$(DN)
	@echo
	@$(PLANTRI) $$(($(v)-2))d 0/$(mod) 2>$(DN) | $(MAIN) plantri sequential >$(DN)

mods:
	@for mod in 1000000 100000 10000 1000 100 10 1; do \
		$(PLANTRI) $$(($(v)-2))d 0/$$mod >$(DN); \
	done
