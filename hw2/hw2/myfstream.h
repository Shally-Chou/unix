#ifndef MYFSTREAM_H
#define MYFSTREAM_H

#include <string>

using namespace std;

class myfstream {
public:
    myfstream();
    ~myfstream();

    bool Open(const char* filename);
    void Close();
    int Read(char* buffer, int size);
    int Write(const char* buffer, int size);

    myfstream& operator>>(string& s);
    myfstream& operator<<(const string& s);

    operator bool() const;

private:
    int fd; // UNIX 檔案描述符 (file descriptor)，這是一個整數
    char read_buffer[1024];
    int buffer_pos;
    int buffer_end;

    bool eof_flag;
};

#endif // MYFSTREAM_H
