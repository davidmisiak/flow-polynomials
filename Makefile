# gcc:
GCC_SANITIZER_FLAGS=-fsanitize=signed-integer-overflow -fsanitize-trap=all
GCC_COMMON_FLAGS=$(GCC_SANITIZER_FLAGS) -Wall -Wextra -Werror -pedantic -std=c++20 -static
GCC_FAST_FLAGS=$(GCC_COMMON_FLAGS) -O3
GCC_DEBUG_FLAGS=$(GCC_COMMON_FLAGS) -Og -g

# shortcuts:
PLANTRI_FLAGS=c2m2dP
PLANTRI=./build/plantri -h$(PLANTRI_FLAGS)
P2N=./build/plantri2num
MAIN=/bin/time ./build/main
DN=/dev/null

# defaults:
input=num
solver=seq
output=none
agg=each
res=0
mod=1

.PHONY: clean run test mods benchmark_naive benchmark_seq compute save_max_star save_k4_max_colorings recompress_unique

clean:
	find build/ -type f ! -name '.gitignore' -delete

build/main: src/*.hpp src/*.cpp
	g++ $(GCC_FAST_FLAGS) -o build/main src/main.cpp

build/main_debug: src/*.hpp src/*.cpp
	g++ $(GCC_DEBUG_FLAGS) -o build/main_debug src/main.cpp

build/random_test: src/*.hpp src/*.cpp
	g++ $(GCC_FAST_FLAGS) -o build/random_test src/random_test.cpp

build/plantri2num: src/*.hpp src/*.cpp
	g++ $(GCC_FAST_FLAGS) -o build/plantri2num src/plantri2num.cpp

build/plantri: plantri53/*
	gcc -O3 -static -o build/plantri plantri53/plantri.c

run: build/main
	$(MAIN) $(input) $(solver) $(output) $(agg)

test: build/plantri build/main build/random_test build/plantri2num
	@set -e; \
	echo "./graphs/easy/*:"; \
	out1=$$(cat ./graphs/easy/* | python scripts/flow_poly.py); \
	out2=$$(cat ./graphs/easy/* | ./build/main num all fp each); \
	if [ "$$out1" != "$$out2" ]; then echo "Test failed"; exit 1; fi; \
	echo; \
	echo "30 random:"; \
	out1=$$(./build/random_test 30 | python scripts/flow_poly.py); \
	out2=$$(./build/random_test 30 | ./build/main num all fp each); \
	if [ "$$out1" != "$$out2" ]; then echo "Test failed"; exit 1; fi; \
	echo; \
	echo "plantri v=12:"; \
	out1=$$(./build/plantri -hc2m2dP 12d 2>$(DN) | $(P2N) | python scripts/flow_poly.py); \
	out2=$$(./build/plantri -hc2m2dP 12d 2>$(DN) | ./build/main plantri all fp each); \
	if [ "$$out1" != "$$out2" ]; then echo "Test failed"; exit 1; fi; \
	echo; \
	echo "3000 random:"; \
	./build/random_test 3000 | $(MAIN) num all none each; \
	echo; \
	echo "plantri v=14:"; \
	./build/plantri -hc2m2dP 14d 2>$(DN) | $(MAIN) plantri all none each; \
	echo; \
	echo "plantri v=20 mod=1000:"; \
	./build/plantri -hc2m2dP 20d 0/1000 2>$(DN) | $(MAIN) plantri all none each

mods: build/plantri
	@for mod in 1000000000 100000000 10000000 1000000 100000 10000 1000 100 10 1; do \
		$(PLANTRI) $$(($(v)-2))d $(res)/$$mod >$(DN); \
	done

# `v` is the number of inner and outer vertices (n+k)
benchmark_naive: build/plantri build/main
	@$(PLANTRI) $$(($(v)-2))d $(res)/$(mod) 2>$(DN) | $(MAIN) plantri naive $(output) $(agg)

# `v` is the number of inner and outer vertices (n+k)
benchmark_seq: build/plantri build/main
	@$(PLANTRI) $$(($(v)-2))d $(res)/$(mod) 2>$(DN) | $(MAIN) plantri seq $(output) $(agg)

# `maxv` is the maximum number of inner and outer vertices (n+k) up to which to compute
# `mod` is the real modulus used by plantri
# `threads` is the number of CPUs to use
compute: build/plantri build/main
	set -e; \
	mkdir tmp; \
	for v in $$(seq 4 2 $(maxv)); do \
		( \
			failed=0; pids=""; \
			for res in $$(seq 0 $$(($(threads)-1))); do \
				$(PLANTRI) $$(($$v-2))d $$res/$$(($(threads)*$(mod))) 2>$(DN) | $(MAIN) plantri seq $(output) $(agg) >tmp/$$v-$$res & \
				pids="$$pids $$!"; \
			done; \
			for pid in $$pids; do wait $$pid || failed=1; done; \
			if [ $$failed -eq 1 ]; then exit 1; fi; \
		) \
	done; \
	mkdir -p computed; \
	cat tmp/* | gzip -c >computed/$(agg)-$(output)-$(PLANTRI_FLAGS)-$(maxv)-$(mod).txt.gz; \
	rm -r tmp

save_max_star: build/plantri build/main
	@echo -n "Did you uncomment the max star condition in output.hpp? Press any key to confirm..."; \
	read -rsn1; \
	for v in 16 18 20 22; do \
		for k in 3 4 5; do \
			./build/plantri -hdP$$k $$(($$v-2))d 2>$(DN) | $(MAIN) plantri seq graph each >graphs/max_star/$$k-$$v.txt; \
		done; \
	done

save_k4_max_colorings: build/plantri build/main
	@echo -n "Did you uncomment the k4 max colorings condition in output.hpp? Press any key to confirm..."; \
	read -rsn1; \
	for v in 16 18 20 22; do \
		./build/plantri -hdP4 $$(($$v-2))d 2>$(DN) | $(MAIN) plantri seq graph each >graphs/k4_max_colorings/$$v.txt; \
	done

recompress_unique:
	@for file in computed/unique-*; do \
		gzip -cd $$file >tmp.txt; \
		cat tmp.txt | sort | uniq | gzip -c >$$file; \
		rm tmp.txt; \
	done
