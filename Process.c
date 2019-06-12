//
// Created by fred on 08/06/19.
//

#include "Process.h"
#include <stdio.h>
#include <stdlib.h>


int comparator(const void* i1, const void* i2) {
    Process* p1 = (Process*) i1;
    Process* p2 = (Process*) i2;
    if (p1->pid == p2->pid) {
        return 1;
    } else {
        return 0;
    }
}


void exit_process(LIST* list) {
    if (ListNext(list) == NULL) {
        ListLast(list);
        ListRemove(list);
        ListFirst(list);
    } else {
        ListPrev(list);
        ListRemove(list);
    }
}


int kill_process(LIST* list, int deleting_pid, const Process* curr) {
    Process* p = malloc(sizeof(Process));
    p->pid = deleting_pid;

    // current pointer goes to the first process
    // and ListSearch takes the current pointer to the found item
    ListFirst(list);
    if (ListSearch(list, comparator, p) != NULL) {
        ListRemove(list);
        printf("Killed succeeded\n");
        return 1;
    } else {
        ListFirst(list);
        ListSearch(list, comparator, curr);
//        printf("Killed failed\n");
        return 0;
    }
}

void create(LIST* list, Process* p) {
    ListInsert(list, p);
    if (ListNext(list) == NULL) {
        ListLast(list);
    }
}

void fork_process(LIST* list, const Process* running) {
    Process* process = malloc(sizeof(Process));
    process->pid = ++last_pid;
    process->priority = running->priority;
    process->state = 0;
    process->messages = ListCreate();
    ListInsert(list, process);
    ListNext(list);
    printf("forked process %d from %d\n", process->pid, running->pid);
}

void quantum( LIST* list) {
    if (ListCurr(list) != NULL) {
        if (ListNext(list) == NULL) {
            ListFirst(list);
        }
    }
}

