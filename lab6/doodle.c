#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sem.h>
#include <unistd.h>

//#define DOODLE_SEM_KEY 1122334455

/* P () -returns 0 if  OK; -1 if there was a problem*/
int P(int s) {
    struct sembuf sop;
    sop.sem_num = 0;
    sop.sem_op = -1;
    sop.sem_flg = 0;

    if(semop (s, &sop,1) < 0) {
        fprintf(stderr,"P(): semop failed %s\n",strerror(errno));
        return -1;
    } else {
        return 0;
    }
}

int V(int s) {
    struct sembuf sop;
    sop.sem_num = 0;
    sop.sem_op = 1;
    sop.sem_flg = 0;

    if (semop(s, &sop, 1) < 0 ) {
        fprintf(stderr,"V(): semopfailed %s\n",strerror(errno));
        return -1;
    }   else {
        return 0;
    }
}

int main(int argc, char const *argv[])
{
    int s, secs;
    long int key;
    if (argc != 2) {
        fprintf(stderr, "%s: specify a key (long)\n",argv[0]);
        exit(1);
    }
    
    if(sscanf(argv[1],"%ld", &key) !=1 ) {
        fprintf(stderr, "%s: argument #1 must be an long integer\n", argv[0]);
        exit(1);
    }

    s = semget(key, 1, 0);

    if ( s<0 ) {
        fprintf(stderr, "%s: cannot find semaphore %ld: %s\n",argv[0], key, strerror(errno));
        exit(1);
    }

    while (1) {
        printf("#secs to doodle in the critical section? (0 to exit):");
        scanf("%d",&secs);

        if ( secs == 0) {
            break;
        }

        printf("Preparting to enter the cirtical section..\n");
        P(s);
        printf("Now in the critical section! Sleep %d secs..\n",secs);

        while (secs) {
            printf("%d...doodle...\n",secs--);
            sleep(1);
        }

        printf("Leaving the critical seciton..\n");
        V(s);
    }
    return 0;
}
