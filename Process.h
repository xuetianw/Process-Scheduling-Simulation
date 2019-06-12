//
// Created by fred on 08/06/19.
//

#ifndef ASSIGNMENT2_PROCESS_H
#define ASSIGNMENT2_PROCESS_H

#include "list.h"

typedef struct {
    int pid;
    int priority;
    int state; // ready: 0, running: 2, blocked: 3
    LIST* messages;
} Process;

typedef struct {
    Process* sender;
    char message[40];
} Message;


typedef struct {
    int id;
    int init;
    LIST* waitlist;
    int value;
} Semaphore;

static int last_pid = 0;

int comparator(const void* i1, const void* i2);

void create(LIST* list, Process* p);

int kill_process(LIST* list, int deleting_pid, const Process* curr);

void exit_process(LIST* list);

void fork_process(LIST* list, const Process* running);


void quantum(LIST* list);

#endif //ASSIGNMENT2_PROCESS_H

