#include <unistd.h>
class FileRAII {
private:
    int fd;
public:
    explicit FileRAII(int _fd = -1) : fd(_fd) {}
    void change(int temp) {
        fd = temp;
    }
    ~FileRAII() { if(fd != -1) close(fd); }
    int get() { return fd; }
};