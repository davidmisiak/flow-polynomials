# gcc:
GCC_SANITIZER_FLAGS=-fsanitize=signed-integer-overflow -fsanitize-trap=all
GCC_COMMON_FLAGS=$(GCC_SANITIZER_FLAGS) -Wall -Wextra -Werror -pedantic -std=c++20 -static
GCC_FAST_FLAGS=$(GCC_COMMON_FLAGS) -O3
GCC_DEBUG_FLAGS=$(GCC_COMMON_FLAGS) -Og -g

# shortcuts:
PLANTRI=./build/plantri -dhc2m2P
MAIN=/bin/time ./build/main
DN=/dev/null

# defaults:
res=0
mod=1

.PHONY: clean run test benchmark_naive benchmark_seq compute mods

clean:
	find build/ -type f ! -name '.gitignore' -delete

build/main: src/*.hpp src/*.cpp
	g++ $(GCC_FAST_FLAGS) -o build/main src/main.cpp

build/main_debug: src/*.hpp src/*.cpp
	g++ $(GCC_DEBUG_FLAGS) -o build/main_debug src/main.cpp

build/random_test: src/*.hpp src/*.cpp
	g++ $(GCC_FAST_FLAGS) -o build/random_test src/random_test.cpp

build/plantri: plantri53/*
	gcc -O3 -static -o build/plantri plantri53/plantri.c

run: build/main
	$(MAIN) $(input) $(solver) $(output)

test: build/plantri build/main build/random_test
	@for file in ./graphs/easy/*; do \
		output1=$$(python scripts/flow_poly.py < $$file); \
		output2=$$(./build/main numeric all fp < $$file); \
		if [ "$$output1" != "$$output2" ]; then echo "Test failed"; exit 1; fi \
	done
	@echo
	@./build/random_test | $(MAIN) numeric all fp >$(DN)
	@echo
	@$(PLANTRI) 14d 2>$(DN) | $(MAIN) plantri all fp >$(DN)
	@echo
	@$(PLANTRI) 20d 0/1000 2>$(DN) | $(MAIN) plantri all fp >$(DN)

# `v` is the number of inner and outer vertices (n+k)
benchmark_naive: build/plantri build/main
	@$(PLANTRI) $$(($(v)-2))d $(res)/$(mod) 2>$(DN) | $(MAIN) plantri naive fp >$(DN)

# `v` is the number of inner and outer vertices (n+k)
benchmark_seq: build/plantri build/main
	@$(PLANTRI) $$(($(v)-2))d $(res)/$(mod) 2>$(DN) | $(MAIN) plantri seq fp >$(DN)

# `maxv` is the maximum number of inner and outer vertices (n+k) up to which to compute the stats
compute: build/plantri build/main
	@mkdir tmp
	@for v in $$(seq 4 2 $(maxv)); do \
		( \
			failed=0; pids=""; \
			for res in $$(seq 0 $$(($(mod)-1))); do \
				$(PLANTRI) $$(($$v-2))d $$res/$(mod) 2>$(DN) | $(MAIN) plantri seq stats >tmp/$$v-$$res & \
				pids="$$pids $$!"; \
			done; \
			for pid in $$pids; do wait $$pid || failed=1; done; \
			if [ $$failed -eq 1 ]; then exit 1; fi; \
		) \
	done
	@cat tmp/* >stats-$(maxv).txt
	@rm -r tmp

mods: build/plantri
	@for mod in 1000000000 100000000 10000000 1000000 100000 10000 1000 100 10 1; do \
		$(PLANTRI) $$(($(v)-2))d 0/$$mod >$(DN); \
	done
