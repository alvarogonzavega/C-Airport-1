#include <stdio.h>
#include <stdlib.h>
#include "queue.h"
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include "plane.h"

#define NUM_TRACKS 1
pthread_mutex_t mut_id = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mut_id2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full, empty;
int numOfPlanesTakenOff = 0;
int numOfPlanesLanded = 0;
int totalPlanesProcessed = 0;
int planeIDCounter = 0;
char **global_argv;
int global_argc;


void trackboss() {
    while ((global_argc == 1 && numOfPlanesTakenOff < 4) ||
           (global_argc != 1 && (numOfPlanesTakenOff < atoi(global_argv[1])))) {
        pthread_mutex_lock(&mut_id);
        while (queue_full()) { pthread_cond_wait(&full, &mut_id); }
        struct plane *plane1 = (struct plane *) malloc(sizeof(struct plane));
        plane1->action = 0;
        plane1->id_number = planeIDCounter;
        printf("[TRACKBOSS] Plane with id %d checked\n", planeIDCounter);
        queue_put(plane1);
        printf("[TRACKBOSS] Plane with id %d ready to take off\n", planeIDCounter);
        numOfPlanesTakenOff++;
        planeIDCounter++;
        pthread_mutex_unlock(&mut_id);
        pthread_cond_signal(&empty);
    }
    pthread_exit(0);
}

void radar() {
    while ((global_argc == 1 && numOfPlanesLanded < 3) ||
           (global_argc != 1 && (numOfPlanesLanded < atoi(global_argv[3])))) {
        pthread_mutex_lock(&mut_id);
        while (queue_full()) { pthread_cond_wait(&full, &mut_id); }
        struct plane *plane1 = (struct plane *) malloc(sizeof(struct plane));
        plane1->action = 1;
        plane1->id_number = planeIDCounter;
        printf("[RADAR] Plane with id %d detected!\n", planeIDCounter);
        queue_put(plane1);
        printf("[RADAR] Plane with id %d ready to land\n", planeIDCounter);
        numOfPlanesLanded++;
        planeIDCounter++;
        pthread_mutex_unlock(&mut_id);
        pthread_cond_signal(&empty);
    }
    pthread_exit(0);
}


void control() {
    int c = 7;
    if (global_argc != 1) {
        int a, b;
        a = atoi(global_argv[1]);
        b = atoi(global_argv[3]);
        c = a + b;
    }
    while (1) {
        pthread_cond_wait(&empty, &mut_id);
        //pthread_mutex_lock(&mut_id2);
        struct plane *activePlane = queue_get();
        if (activePlane == NULL)break;
        // pthread_mutex_unlock(&mut_id2);
        if (activePlane->id_number < c) {
            if (activePlane->last_flight == 1) {
                printf("[CONTROL] After plane with id %d the airport will be closed\n", activePlane->id_number);
            }
            if (activePlane->action == 0) {
                /*Takeoff*/
                printf("[CONTROL] Putting plane with id %d in track\n", activePlane->id_number);
                if (global_argc == 1) {
                    sleep(2);
                    printf("[CONTROL] Plane %d took off after %d seconds\n", activePlane->id_number, 2);
                    totalPlanesProcessed++;
                    pthread_cond_signal(&full);

                }
                if (global_argc != 1) {
                    sleep(atoi(global_argv[2]));
                    printf("[CONTROL] Plane %d took off after %d seconds\n", activePlane->id_number,
                           atoi(global_argv[2]));
                    totalPlanesProcessed++;
                    pthread_cond_signal(&full);

                }
            } else {
                /*Landing*/
                printf("[CONTROL] Track is free for plane with id %d\n", activePlane->id_number);
                if (global_argc == 1) {
                    sleep(3);
                    printf("[CONTROL] Plane %d landed in %d seconds\n", activePlane->id_number, 3);
                    totalPlanesProcessed++;
                    pthread_cond_signal(&full);

                } else if (global_argc != 1) {
                    sleep(atoi(global_argv[4]));
                    printf("[CONTROL] Plane %d landed in %d seconds\n", activePlane->id_number,
                           atoi(global_argv[4]));
                    totalPlanesProcessed++;
                    pthread_cond_signal(&full);

                }
            }
        }
        if (activePlane->last_flight == 1) {
            printf("Airport Closed!\n");
            pthread_exit(0);
        }
    }
}


void print_banner() {
    printf("\n*****************************************\n");
    printf("Welcome to ARCPORT - The ARCOS AIRPORT.\n");
    printf("*****************************************\n\n");
}

void print_end() {
    printf("\n********************************************\n ---> Thanks for your trust in us <---\n"\
             "********************************************\n");
}

int main(int argc, char **argv) {
    global_argv = argv;
    global_argc = argc;
    print_banner();
    if (queue_empty()) { if (argc == 1) { queue_init(6); } else { queue_init(atoi(argv[5])); }}
    pthread_t th1, th2, th3;
    pthread_create(&th1, NULL, (void *) trackboss, NULL);
    pthread_create(&th2, NULL, (void *) radar, NULL);
    pthread_create(&th3, NULL, (void *) control, NULL);
    (void) pthread_join(th1, NULL);
    (void) pthread_join(th2, NULL);
    (void) pthread_join(th3, NULL);
    print_end();
    FILE *fd = fopen("resume.air", "w+");
    fprintf(fd, "\tTotal number of planes processed: %d\n\tNumber of planes landed: %d\n\tNumber of planes"\
        " taken off: %d", totalPlanesProcessed, numOfPlanesLanded, numOfPlanesTakenOff);
    fclose(fd);
    queue_destroy();
    return 0;
}
