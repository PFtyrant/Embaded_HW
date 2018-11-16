#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sem.h>

#define SEM_MODE 0666 /*rw(owner)-rw(group)-rw(other) permission*/

int main (int argc, char ** argv) {
    int s;
    long int key;
    int val;

    if ( argc != 3) {
        fprintf(stderr,
            "%s: specify a key (long) and initial value (int)\n",
            argv[0]);

        exit(1);
    }

    /* get values from command line */
    if (sscanf(argv[1], "%ld", &key) != 1) {
        /* convert arg to long integer */
        fprintf(stderr, "%s: argument #1 must be an long integer/n",argv[0]);
        exit(1);
    }
    if(sscanf(argv[2], "%d", &val)!=1) {
        fprintf(stderr,"%s: argument #2 must be an integer\n",argv[0]);
        exit(1);
    }

    s = semget(
            key,
            1,
            IPC_CREAT | IPC_EXCL | SEM_MODE
    );

    if ( s < 0 ) {
        fprintf(stderr,
            "%s: creation of semaphore %ld failed: %s\n", argv[0], 
            key, strerror(errno));
        exit(1);
    }
    printf("Semaphore %ld created\n", key);

    if ( semctl(s,0,SETVAL, val) < 0) {
        fprintf(stderr, "%s: Unable to initialize semaphore: %s\n",
            argv[0], strerror(errno));
            exit(0);
    }

    printf("Semaphore %ld has been initialized to %d\n",key, val);
    return 0;
}