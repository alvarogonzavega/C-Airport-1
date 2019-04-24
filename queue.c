#include <stdio.h>
#include <stdlib.h>
#include "queue.h"
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "plane.h"


struct Node {
    struct plane *planeInfo;
    struct Node *next;
};

extern char **global_argv;
extern int global_argc;
struct Node *head;// global variable - pointer to head node.
struct Node *tail;
struct Node *deleteTemp;
struct Node *temp;
int queueSize, argv5;
pthread_mutex_t mut_id1 = PTHREAD_MUTEX_INITIALIZER;

/*Creates a new Node and returns pointer to it.*/
struct Node *GetNewNode(struct plane *newPlane) {
    struct Node *newNode = (struct Node *) malloc(sizeof(struct Node));
    newNode->planeInfo = newPlane;
    newNode->next = NULL;
    return newNode;
}


struct Node deleteNode() {
    deleteTemp = head;
    if (head == NULL);
    else {
        if (head == tail) {
            head = tail = NULL;
        } else {
            head = head->next;
            tail->next = head;
        }
        deleteTemp->next = NULL;
        queueSize--;
    }
}

/*To create a queue*/
int queue_init(int size) {
    printf("[QUEUE] Buffer initialized.\n");
    argv5 = size;
    return 0;
}

/* To Enqueue an element*/
int queue_put(struct plane *x) {
    pthread_mutex_lock(&mut_id1);
    if (queueSize == argv5) {
        write(1, "Maximum Queue Size Reached.\n", 28);
    } else {
        if (global_argc == 1) {
            if (x->id_number > 6) return -1;
            if (x->id_number == 6) {
                x->last_flight = 1;
            } else { x->last_flight = 0; }
            if (x->action == 0) x->time_action = 2;
            if (x->action == 1) x->time_action = 3;
        } else {
            int argv1, argv2, argv3, argv4, sum;
            argv1 = atoi(global_argv[1]);
            argv2 = atoi(global_argv[2]);
            argv3 = atoi(global_argv[3]);
            argv4 = atoi(global_argv[4]);
            sum = argv1 + argv3;
            if (x->id_number > sum) return -1;
            if (x->id_number == sum) {
                x->last_flight = 1;
            } else { x->last_flight = 0; }
            if (x->action == 0) x->time_action = argv2;
            if (x->action == 1) x->time_action = argv4;
        }
        struct Node *newNode = GetNewNode(x);
        switch (queueSize) {
            case 0: {
                head = tail = newNode;
                head->next = tail;
                tail->next = head;
                break;
            }
            case 1: {
                tail = newNode;
                head->next = tail;
                tail->next = head;
                break;
            }
            default: {
                temp = head;
                for (int i = 0; i < queueSize - 1; i++) {
                    temp = temp->next;
                }
                tail = newNode;
                temp->next = tail;
                tail->next = head;
                break;
            }
        }
        printf("[QUEUE] Storing plane with id <%d>\n", x->id_number);
        queueSize++;
        pthread_mutex_unlock(&mut_id1);
        return 0;
    }
}


/* To Dequeue an element.*/
struct plane *queue_get(void) {
    if (queueSize >= 0) {
        switch (queueSize) {
            case 0: {
                write(1, "Cannot empty queue when it's already empty.\n", 44);
                break;
            }
            case 1: {
                temp = head;
                //queue_destroy();
                printf("[QUEUE] Getting plane with id <%d>\n", temp->planeInfo->id_number);
                return temp->planeInfo;
            }
            default: {
                temp = head;
                tail->next = temp->next;
                head = tail->next;
                queueSize--;
                printf("[QUEUE] Getting plane with id <%d>\n", temp->planeInfo->id_number);
                return temp->planeInfo;
            }
        }
    }
    return NULL;
}


/*To check queue state*/
int queue_empty(void) {
    int isEmpty = 0;
    if (queueSize == 0) {
        isEmpty = 1;
    }
    return isEmpty;

}

/*To check queue state*/
int queue_full(void) {
    if (global_argc == 1) {
        if (queueSize == 6)return 1;
    } else {
        if (atoi(global_argv[5]) == queueSize) {
            return 1;
        }
    }
    return 0;
}

/*To destroy the queue and free the resources*/
int queue_destroy(void) {
    while (queue_empty() == 0) deleteNode();
    return 0;
}

/*int main(int argc, char **global_argv) {
    int j = queue_empty();
    struct plane plane1;
    plane1.action = 1;
    struct plane plane2;
    plane2.action = 0;
    struct plane plane3;
    plane3.action = 1;
    struct plane plane4;
    plane4.action = 1;
    struct plane plane5;
    plane5.action = 1;
    struct plane plane6;
    plane6.action = 0;
    struct plane plane11;
    plane6.action = 0;
    queue_put(&plane1);
    queue_put(&plane2);
    queue_put(&plane3);
    queue_put(&plane4);
    queue_put(&plane5);
    queue_put(&plane6);
    struct plane *plane7 = queue_get();
    struct plane *plane8 = queue_get();
    int i = queue_full();
    int r = queue_empty();
    queue_destroy();
    int a = 0;
}*/
