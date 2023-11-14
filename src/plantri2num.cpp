#include <bits/stdc++.h>

#include "base.hpp"
#include "multipole.hpp"


int main() {
    std::ios::sync_with_stdio(false);

    while (std::cin.peek() != EOF) {
        Multipole g = Multipole::read_plantri_disk_triangulation(std::cin);
        g.write_numeric(std::cout);
    }
}
