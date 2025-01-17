#include "stream_reassembler.hh"
#include <cstddef>
#include <ios>
#include <string>
#include <stdexcept>
// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity) : used(), _output(capacity), _capacity(capacity),
first_unacceptable(capacity), _buf(capacity, '\n'){}

// should be used after sending the string from buf to byteStream
void StreamReassembler::update(const size_t bytes_written) {
    first_unread = _output.bytes_read();
    first_unassembled = _output.bytes_written();
    first_unacceptable = first_unread + _capacity;
    _buf_size -= bytes_written;
    
    // Erase some items in used and buf
    for (size_t i = 0; i < bytes_written; i++) {
        used.erase(i);
    }
    _buf.erase(_buf.begin(), _buf.begin() + bytes_written);
    
}


// buffer size is updated here
// The function itself does not call update
void StreamReassembler::send_to_buf(const std::string &data, const size_t index) {
    // Where to start insert and end insert in the buffer, include the start_Looking itself
    // But not include end_Looking
    size_t start_Looking = std::max(static_cast<size_t>(0), index - first_unread);
    size_t end_Looking = std::min(_capacity, start_Looking + data.size());
    size_t buf_idx = start_Looking;
    size_t data_idx = 0;

    for(; buf_idx < end_Looking && data_idx < data.size(); buf_idx++, data_idx++) {
        // If this position is occupied, then we skip it
        if (used.find(buf_idx) != used.end()) continue;

        // Otherwise, add buf_idx to the set and put it to the buf
        _buf.replace(buf_idx, 1, 1, data.at(data_idx));
        used.insert(buf_idx);
        _buf_size++;
    }

}

// Note that this function only tries to send to 
// ByteStream once
size_t StreamReassembler::send_to_bs() {
   // Find the first discontiguous index in the buffer
   // Using brute search, although this can be optimized by
   // binary search
   size_t discontiguous = 1;
   for (; discontiguous < _capacity; discontiguous++) {
    if (used.find(discontiguous) != used.end()) break;
   } 
   auto sent_strings = _buf.substr(0, discontiguous);
   auto bytes_sent = _output.write(sent_strings);
   _buf_size -= bytes_sent;
   if (static_cast<int>(_buf_size) < 0) throw std::runtime_error("_buf_size cannot be less"
   "than 0");
   return bytes_sent; 
}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    //First check if the index is valid
    if (index >= first_unacceptable || index + data.size() <= first_unread) return;
    if (eof) {eof_flag = true;}
    //Send the data to buf
    send_to_buf(data, index);

    // push the string to the bytestream just once
    auto bytes_written = send_to_bs();

    // update relevant memebers
    update(bytes_written);

    if (eof_flag && empty()) _output.end_input(); 
}

size_t StreamReassembler::unassembled_bytes() const {
    return first_unacceptable - first_unread;
}

bool StreamReassembler::empty() const {
    return _buf_size == 0;
}
