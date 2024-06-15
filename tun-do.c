#include <arpa/inet.h>
#include <assert.h>
#include <fcntl.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>

int main(int argc, char **argv) {
    struct ifreq ifr;
    struct sockaddr_in *sai;
    if (argc < 3) {
        printf("Usage: %s <ip> <cmd...>\n", argv[0]);
        return -1;
    }
    // open /dev/net/tun and get fd of the new interface
    int tun_fd = open("/dev/net/tun", O_RDWR);
    assert(tun_fd == 3);
    // tell it we want a TUN device and no packet headers
    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = IFF_TUN | IFF_NO_PI;
    strncpy(ifr.ifr_name, "mytun", IFNAMSIZ);
    ioctl(tun_fd, TUNSETIFF, &ifr);
    // set a local IP on it
    int conf_fd = socket(AF_INET, SOCK_DGRAM, 0);
    sai = (struct sockaddr_in *)&ifr.ifr_addr;
    sai->sin_family = AF_INET;
    sai->sin_addr.s_addr = inet_addr(argv[1]);
    ioctl(conf_fd, SIOCSIFADDR, &ifr);
    sai = (struct sockaddr_in *)&ifr.ifr_netmask;
    sai->sin_family = AF_INET;
    sai->sin_addr.s_addr = inet_addr("255.255.255.0");
    ioctl(conf_fd, SIOCSIFNETMASK, &ifr);
    // set it up
    ioctl(conf_fd, SIOCGIFADDR, &ifr);      // get
    ifr.ifr_flags |= IFF_UP | IFF_RUNNING;  // add flags
    ioctl(conf_fd, SIOCSIFFLAGS, &ifr);     // set
    close(conf_fd);
    // set env vars and exec specified command
    setenv("TUN_FD", "3", 1);
    setenv("PS1", "tun-shell> ", 1);
    return execvp(argv[2], argv+2);
}
