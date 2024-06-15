#include <unistd.h>
#include <stdint.h>
#include <arpa/inet.h>

/* Ce programme lit les paquets successifs sur tun_fd=3 (le descripteur
 * de fichier correspondant à une interface TUN), et copie ces paquets
 * sur stdout mais en les préfixant par 2 octets précisant leur longueur. */

/* Il faut gérer les possibles écritures partielles sur le flux de sortie
 * (écriture partielle d'un paquet, ou, si on n'a vraiment pas de chance,
 * écriture d'un seul octet de longueur au lieu de deux), qui peuvent
 * survenir quand le buffer de sortie est plein (congestion sur le tunnel
 * SSH). */

const int tun_fd=3, stdout_fd=1;

int write_n_bytes(char *buf, int length) {
    int res;
    char *buf_end = buf + length;
    while (buf < buf_end) {
        res = write(stdout_fd, buf, length);
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
        /* lecture d'un paquet venant du TUN */
        length = read(tun_fd, buf, 65536);
        if (length <= 0) {
            return 1;  // end
        }
        /* ecriture de 2 octets de longueur sur stdout */
        coded_length = htons((uint16_t)length);
        res = write_n_bytes((char*)&coded_length, 2);
        if (res != 0) {
            return 1;  // end
        }
        /* ecriture du paquet sur stdout */
        res = write_n_bytes(buf, length);
        if (res != 0) {
            return 1;  // end
        }
    }
}
