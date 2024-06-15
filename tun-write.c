#include <unistd.h>
#include <stdint.h>
#include <arpa/inet.h>

const int tun_fd=3, stdin_fd=0;

int read_n_bytes(char *buf, int length) {
    int res;
    char *buf_end = buf + length;
    while (buf < buf_end) {
        res = read(stdin_fd, buf, length);
        if (res <= 0) {
            return 1;  // error
        }
        buf += res;
        length -= res;
    }
    return 0;  // ok
}

int main() {
    int length, res;
    uint16_t coded_length;
    char buf[65536];
    while(1) {
        res = read_n_bytes((char*)&coded_length, 2);
        if (res != 0) {
            return 1;  // end
        }
	    length = (int)ntohs(coded_length);
        res = read_n_bytes(buf, length);
        if (res != 0) {
            return 1;  // end
        }
        write(tun_fd, buf, length);
    }
}
