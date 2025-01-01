#include "byte_stream.hh"

#include <cstddef>

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

ByteStream::ByteStream(const size_t capacity) : _capacity(capacity), _buf(capacity, '\n') {}

size_t ByteStream::write(const string &data) {
    if (!_writable) {
        set_error();
        return 0;
    }
    const size_t remainRooms = getRemainRoom();
    const size_t thisWrite = std::min(remainRooms, data.size());
    // increase the total written bytes
    _nwrite += thisWrite;
    // move the write pointer
    _writePtr = (_writePtr + thisWrite) / _capacity;
    return thisWrite;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    string tmp;
    // without changing the read pointer
    size_t i = 0;
    while (i < len) {
        tmp += _buf[(_readPtr + i) % _capacity];
        i++;
    }
    return tmp;
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
    // Move the read pointer
    size_t used = _capacity - getRemainRoom();
    if (len > used) {
        set_error();
        return;
    }
    _readPtr = (_readPtr + len) % _capacity;
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    if (!_readable) {
        set_error();
        return "";
    }
    const string res = peek_output(len);
    pop_output(len);
    return res;
}

void ByteStream::end_input() { _writable = false; }

bool ByteStream::input_ended() const { return _writable; }

size_t ByteStream::buffer_size() const { return _capacity - getRemainRoom(); }

bool ByteStream::buffer_empty() const { return getRemainRoom() == _capacity; }

bool ByteStream::eof() const { return buffer_empty() && input_ended(); }

size_t ByteStream::bytes_written() const { return _nwrite; }

size_t ByteStream::bytes_read() const { return _nread; }

size_t ByteStream::remaining_capacity() const { return getRemainRoom(); }
