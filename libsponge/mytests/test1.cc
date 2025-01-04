#include "../byte_stream.hh"

int main() {
    auto bs = ByteStream(5);
    bs.write("cat");
    bs.pop_output(3); 
}