#include "myfstream.h"
#include <fcntl.h>   // 為了 open() 的旗標
#include <unistd.h>  // 為了 read(), write(), close()
#include <string.h>  // 為了 strlen()
#include <iostream>

myfstream::myfstream() : fd(-1), buffer_pos(0), buffer_end(0), eof_flag(false) {}

myfstream::~myfstream() {
    Close();
}

bool myfstream::Open(const char* filename) {
    fd = open(filename, O_RDWR | O_CREAT, 0644);
    buffer_pos = buffer_end = 0;
    if (fd == -1){
	    eof_flag = true;
    }
    return (fd != -1);
}

void myfstream::Close() {
    if (fd!= -1) {
        close(fd);
        fd = -1; // 設為 -1 表示已關閉
    }
}

int myfstream::Read(char* buffer, int size) {
    if (fd == -1) return -1;
    return read(fd, buffer, size);
}

int myfstream::Write(const char* buffer, int size) {
    if (fd == -1) return -1;
    return write(fd, buffer, size);
}

myfstream& myfstream::operator<<(const string& s) {
    if (fd!= -1) {
        write(fd, s.c_str(), s.length());
    }
    return *this;
}

myfstream& myfstream::operator>>(string& s) {
    s.clear();
    if (eof_flag) return *this;

    char c;
    bool in_word = false;

    while (true) {
        if (buffer_pos >= buffer_end) {
            buffer_end = read(fd, read_buffer, sizeof(read_buffer));
            buffer_pos = 0;
            if (buffer_end <= 0) {
                eof_flag = true;
		break;
            }
        }

        c = read_buffer[buffer_pos++];
        if (c == ' ' || c == '\n' || c == '\t' || c == '\r') {
            if (in_word) {
                break;
            }
        } else {
            in_word = true;
            s += c;
        }
    }
    return *this;
}

myfstream::operator bool() const{
	return!eof_flag;
}
