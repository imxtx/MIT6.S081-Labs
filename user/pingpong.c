#include "kernel/types.h"
#include "user/user.h"

#define R 0
#define W 1
#define EXIT_SUCCESS 0

int main(int argc, char const *argv[])
{
    int p2c[2], c2p[2];
    pipe(p2c);
    pipe(c2p);
    char buff = 'x';

    if (fork() == 0) { // child
        close(p2c[W]);
        close(c2p[R]);
        read(p2c[R], &buff, sizeof(char)); // read a byte from parent
        printf("%d: received ping\n", getpid());
        write(c2p[W], &buff, sizeof(char)); // write a byte to parent
        close(p2c[R]);
        close(c2p[W]);
        exit(EXIT_SUCCESS);
    } else { // parent
        close(p2c[R]);
        close(c2p[W]);
        write(p2c[W], &buff, sizeof(char)); // write a byte to child
        read(c2p[R], &buff, sizeof(char)); // read a byte from child
        printf("%d: received pong\n", getpid());
        close(p2c[W]);
        close(c2p[R]);
        exit(EXIT_SUCCESS);
    }
}
