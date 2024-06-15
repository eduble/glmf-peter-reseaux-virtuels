#include <unistd.h>
#include <stdint.h>
#include <arpa/inet.h>

/* Ce programme lit un flux de paquets sur son entrée standard, chaque
 * paquet étant précédé de deux octets indiquant sa longueur, et écrit
 * ces paquets un par un sur tun_fd=3 (le descripteur de fichier
 * correspondant à une interface TUN). */

/* Il faut gérer les possibles lectures partielles (lecture partielle
 * d'un paquet, ou, si on n'a vraiment pas de chance, lecture d'un seul
 * octet de longueur au lieu de deux), le flux en entrée étant soumis
 * aux aléas du buffering réseau et à des contraintes de taille maximale
 * du payload SSH. */

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
        /* lecture de 2 octets de longueur */
        res = read_n_bytes((char*)&coded_length, 2);
        if (res != 0) {
            return 1;  // end
        }
	    length = (int)ntohs(coded_length);
        /* lecture du paquet */
        res = read_n_bytes(buf, length);
        if (res != 0) {
            return 1;  // end
        }
        /* écriture du paquet sur le TUN */
        write(tun_fd, buf, length);
    }
}
