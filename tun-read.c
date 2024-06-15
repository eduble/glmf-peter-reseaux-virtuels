#include <unistd.h>
#include <stdint.h>
#include <arpa/inet.h>

const int tun_fd=3, stdout_fd=1;

int main() {
    int length;
    uint16_t coded_length;
    char buf[65536];
    while(1) {
        length = read(tun_fd, buf, 65536);
        if (length <= 0) {
            return 1;  // end
        }
        coded_length = htons((uint16_t)length);
        write(stdout_fd, &coded_length, 2);
        write(stdout_fd, buf, length);
    }
}
