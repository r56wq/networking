#include "../byte_stream.hh"
#include "stream_reassembler.hh"
#include <algorithm>
#include <unordered_set>
#include <iostream>
int main() {
    size_t capacity = 5;
    auto reassembler = StreamReassembler(capacity);
    auto f = [](std::unordered_set<int> range) {
        for (auto i : range) {
            std::cout << i;
            std::cout << std::endl;
        }
    };
    
}