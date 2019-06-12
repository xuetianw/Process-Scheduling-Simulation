#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Process.h"


int main(int argc, char* argv[]) {
    Semaphore semaphores[5];
    for (int i = 0; i < 5; i++) {
        semaphores[i].id = i;
        semaphores[i].init = 0;
        semaphores[i].waitlist = ListCreate();
        semaphores[i].value = 0;
    }


    /*creating priority lists*/
    LIST* hp_list = ListCreate();
    LIST* np_list = ListCreate();
    LIST* lp_list = ListCreate();

    LIST* sendBlocked = ListCreate();
    LIST* receiveBlocked = ListCreate();

    int terminate = 0;
    char input;
    while (!terminate) {
        Process* running = ListCurr(hp_list);
        if (running == NULL) {
            running = ListCurr(np_list);
            if (running == NULL) {
                running = ListCurr(lp_list);
            }
        }

        if (running != NULL) {
            printf("running process number: %d\n", running->pid);
            running->state = 2;
        }

        scanf("%c", &input);
        if (input == 'c') {
            int priority;
            printf("priority: ");
            scanf("%d", &priority);
            Process* p = malloc(sizeof(Process));
            if (priority == 0 || priority == 1 || priority == 2) {
                p->pid = ++last_pid;
                p->priority = priority;
                p->state = 0;
                p->messages = ListCreate();
            }
            if (priority == 0) {
                create(hp_list, p);
            } else if (priority == 1) {
                create(np_list, p);
            } else if (priority == 2) {
                create(lp_list, p);
            } else {
                printf("wrong number for priority\n");
                continue;
            }
            printf("created a new process %d\n", p->pid);
        } else if (input == 'f') {
            if (running == NULL) {
                printf("there is no process running fork failed\n");
                continue;
            }

            if (running->priority == 0) {
                fork_process(hp_list, running);
            } else if (running->priority == 1) {
                fork_process(np_list, running);
            } else if (running->priority == 2) {
                fork_process(lp_list, running);
            }
        } else if (input == 'k') {
            int deleting_pid;
            printf("please enter pid that you want to delete: ");
            scanf("%d", &deleting_pid);

            if (ListCurr(hp_list) == NULL && ListCurr(np_list) == NULL && ListCurr(lp_list) == NULL) {
                printf("Killed failed\n");
            }
            // High
            Process* curr = ListCurr(hp_list);
            if (curr != NULL) {
                // if the process that user wants to kill is the one running
                if (curr->pid == deleting_pid) {
                    exit_process(hp_list);
                    printf("Killed succeeded\n");
                    continue;
                } else if (kill_process(hp_list, deleting_pid, curr)) {
                    continue;
                }
            }

            curr = ListCurr(np_list);
            if (curr != NULL) {
                if (curr->pid == deleting_pid) {
                    exit_process(np_list);
                    printf("Killed succeeded\n");
                    continue;
                } else if (kill_process(np_list, deleting_pid, curr)) {
                    continue;
                }
            }

            curr = ListCurr(lp_list);
            if (curr != NULL) {
                if (curr->pid == deleting_pid) {
                    exit_process(lp_list);
                    printf("Killed succeeded\n");
                    continue;
                }else if (kill_process(lp_list, deleting_pid, curr)) {
                    continue;
                }
            }
            printf("Killed failed\n");

        } else if (input == 'e') {
            if (running == NULL) {
                printf("nothing is running at the moment\n");
            } else {
                if (running->priority == 0) {
                    exit_process(hp_list);
                } else if (running->priority == 1) {
                    exit_process(np_list);
                } else {
                    exit_process(lp_list);
                }
            }
        } else if (input == 'q') {
            if (running == NULL) {
                printf("nothing is running at the moment\n");
            } else {
                running->state = 0;
                if (running->priority == 0) {
                    quantum(hp_list);
                } else if (running->priority == 1) {
                    quantum(np_list);
                } else {
                    if (ListNext(lp_list) == NULL) {
                        ListFirst(lp_list);
                    }
                }
            }
        } else if (input == 's') {
            if (running == NULL) {
                printf("nothing is running at the moment\n");
                continue;
            }

            int send_to_pid;
            printf("type target pid: ");
            scanf("%d", &send_to_pid);

            printf("type message you want to send: ");
            char message[40];
            scanf("%s", message);
            printf("your message%s\n", message);
            Process* send_to_process = malloc(sizeof(Process));
            send_to_process->pid = send_to_pid;
            Process* receiver_process;

            int found = 0; //finding revceiver from the 3 lists

            // return the current pointer int a temp lovation curr so that
            // we could find the original current pointer location

            // we are searching the process sent to though 3 Lists from HP to LP
            Process* curr = ListCurr(hp_list);
            if (curr != NULL) {
                ListFirst(hp_list);
                receiver_process = ListSearch(hp_list, comparator, send_to_process);
                if (receiver_process != NULL) {
                    found = 1;
                }
                ListFirst(hp_list);
                ListSearch(hp_list, comparator, curr);// and move current point back to the original place
            }

            // if process sent to does not exist in the HP List, then we go to the np_list
            if (found == 0) {
                curr = ListCurr(np_list);
                if (curr != NULL) {
                    ListFirst(np_list);
                    receiver_process = ListSearch(np_list, comparator, send_to_process);
                    if (receiver_process != NULL) {
                        found = 1;
                    }
                    ListFirst(np_list);
                    ListSearch(np_list, comparator, curr);
                }
            }

            // if the process sent to does not exist in the np_list, then we go to the lp_list
            if (found == 0) {
                curr = ListCurr(lp_list);
                if (curr != NULL) {
                    ListFirst(lp_list);
                    receiver_process = ListSearch(lp_list, comparator, send_to_process);
                    if (receiver_process != NULL) {
                        found = 1;
                    }
                    ListFirst(lp_list);
                    ListSearch(lp_list, comparator, curr);
                }
            }

            // if the process sent to is in none of the list, we try finding it in the sendBlock List
            if (found == 0) {
                curr = ListCurr(sendBlocked);
                if (curr != NULL) {
                    ListFirst(sendBlocked);
                    receiver_process = ListSearch(sendBlocked, comparator, send_to_process);
                    if (receiver_process != NULL) {
                        found = 2;
                    }
                    ListFirst(sendBlocked);
                    ListSearch(sendBlocked, comparator, curr);
                }
            }

            if (found == 0) {
                curr = ListCurr(receiveBlocked);
                if (curr != NULL) {
                    ListFirst(receiveBlocked);
                    receiver_process = ListSearch(receiveBlocked, comparator, send_to_process);
                    if (receiver_process != NULL) {
                        found = 3;
                    }
                    ListFirst(receiveBlocked);
                    ListSearch(receiveBlocked, comparator, curr);
                }
            }

            if (found == 0) {
                printf("pid does not exist in the 3 Priority lists\n");
            } else { // found the revceiver
                Message* msg = malloc(sizeof(Message));
                msg->sender = (Process*) running;
                strcpy(msg->message, message);
                // if receiver_process is blocked and is on the blocked list, we put it in the ready
                if (found == 3) {
                    // change the state of receiver_process to ready
                    receiver_process->state = 0; // Ready
                    ListFirst(receiveBlocked);
                    if (ListSearch(receiveBlocked, comparator, receiver_process) != NULL) {
                        ListRemove(receiveBlocked); // remove receiver_process from receiveBlocked list
                        // put the msg in the receiver_process'messages list
                        ListAdd(receiver_process->messages, msg);
                        if (receiver_process->priority == 0) {
                            create(hp_list, receiver_process); //put it at the back of the queue
                        } else if (receiver_process->priority == 1) {
                            create(np_list, receiver_process); //put it at the back of the queue
                        } else {
                            create(lp_list, receiver_process); //put it at the back of the queue
                        }
                    }
                }

                running->state = 3; // Block the sender
                ListAdd(sendBlocked, running);
                //remove it running process
                if (running->priority == 0) {
                    exit_process(hp_list);
                } else if (running->priority == 1) {
                    exit_process(np_list);
                } else {
                    exit_process(lp_list);
                }
            }
        } else if (input == 'r') {
            if (running == NULL) {
                printf("nothing is running at the moment\n");
                continue;
            }
            Message* msg;
            if ((msg = ListRemove(running->messages)) != NULL) {
                printf("Process %d: %s\n", msg->sender->pid, msg->message);
            } else {
                running->state = 3; // No message to receive, block the receiver
                ListAdd(receiveBlocked, running);
                if (running->priority == 0) {
                    exit_process(hp_list);
                } else if (running->priority == 1) {
                    exit_process(np_list);
                } else {
                    exit_process(lp_list);
                }
            }
        } else if (input == 'Y') {
            if (running == NULL) {
                printf("nothing is running at the moment\n");
                continue;
            }

            int temp_pid;
            printf("please enter sender_process pid: ");
            scanf("%d", &temp_pid);
            printf("message: ");
            char message[40];
            scanf("%s", message);
            Process* temp_process = malloc(sizeof(Process));
            temp_process->pid = temp_pid;
            Process* sender_process;

            ListFirst(sendBlocked);
            sender_process = ListSearch(sendBlocked, comparator, temp_process);
            if (sender_process == NULL) {
                printf("sender_process process does not exist\n");
            } else {
                // TODO: send the reply message to sender_process, and sender_process should print this message at its turn
                // Message *msg = malloc(sizeof(Message));
                // msg.sender_process = running;
                // strcpy(message, msg.message, 40);

                sender_process->state = 0; // Unblock the sender_process
                if (ListSearch(sendBlocked, comparator, sender_process) != NULL) {
                    ListRemove(sendBlocked); // remove receiver from receiveBlocked list
                    if (sender_process->priority == 0) {
                        create(hp_list, sender_process);
                    } else if (sender_process->priority == 1) {
                        create(np_list, sender_process);
                    } else {
                        create(lp_list, sender_process);
                    }
                }

                Message* msg = malloc(sizeof(Message));
                msg->sender = running;
                strcpy(msg->message, message);
                ListAdd(sender_process->messages, msg);
            }
        } else if (input == 'n') {
            int sem_id;
            printf("please enter semphore ID that you want to initialize: ");
            scanf("%d", &sem_id);
            if (semaphores[sem_id].init == 1) {
                printf("semaphore #%d has been initialized before!\n", sem_id);
                continue;
            }

            int sem_value;
            printf("please semphore value: ");
            scanf("%d", &sem_value);
            if (sem_value < 0) {
                printf("wrong semaphore value\n");
                continue;
            }

            semaphores[sem_id].value = sem_value;
            printf("semaphore #%d has been initialized !\n", sem_id);
        } else if (input == 'p') {
            int semphoreid;
            printf("semphore ID: ");
            scanf("%d", &semphoreid);
            if (semaphores[semphoreid].value < 0) {
                ListAdd(semaphores[semphoreid].waitlist, running);
                running->state = 2;
                if (running->priority == 0) {
                    exit_process(hp_list);
                } else if (running->priority == 1) {
                    exit_process(np_list);
                } else {
                    exit_process(lp_list);
                }
            } else {
                semaphores[semphoreid].value--;
            }
        } else if (input == 'v') {
            int sem_id;
            printf("semphore ID: ");
            scanf("%d", &sem_id);
            if (ListCount(semaphores[sem_id].waitlist) > 0) {
                Process* blockedP = ListRemove(semaphores[sem_id].waitlist);
                if (blockedP->priority == 0) {
                    ListInsert(hp_list, blockedP);
                    if (ListNext(hp_list) == NULL) {
                        ListPrev(hp_list);
                    }
                } else if (blockedP->priority == 1) {
                    ListInsert(np_list, blockedP);
                    if (ListNext(np_list) == NULL) {
                        ListPrev(np_list);
                    }

                } else {
                    ListInsert(lp_list, blockedP);
                    if (ListNext(lp_list) == NULL) {
                        ListPrev(lp_list);
                    }
                }
            } else {
                semaphores[sem_id].value++;
            }
        } else if (input == 'i') {
            printf("complete information about the current running process pid: %d priority: %d state: %d \n",
                   ((Process*) ListCurr(hp_list))->pid, ((Process*) ListCurr(hp_list))->priority,
                   ((Process*) ListCurr(hp_list))->state);
        } else if (input == 't') {
            Process* temp = ListCurr(hp_list);
            while (ListCurr(hp_list) != NULL) {
                printf("pid: %d priority: %d state: %d \n",
                       ((Process*) ListCurr(hp_list))->pid, ((Process*) ListCurr(hp_list))->priority,
                       ((Process*) ListCurr(hp_list))->state);
                ListNext(hp_list);
            }
            ListFirst(hp_list);
            while (ListCurr(hp_list) != temp) {
                printf("pid: %d priority: %d state: %d \n",
                       ((Process*) ListCurr(hp_list))->pid, ((Process*) ListCurr(hp_list))->priority,
                       ((Process*) ListCurr(hp_list))->state);
                ListNext(hp_list);
            }

            temp = ListCurr(np_list);
            while (ListCurr(np_list) != NULL) {
                printf("pid: %d priority: %d state: %d \n",
                       ((Process*) ListCurr(np_list))->pid, ((Process*) ListCurr(np_list))->priority,
                       ((Process*) ListCurr(np_list))->state);
                ListNext(np_list);
            }
            ListFirst(np_list);
            while (ListCurr(np_list) != temp) {
                printf("pid: %d priority: %d state: %d \n",
                       ((Process*) ListCurr(np_list))->pid, ((Process*) ListCurr(np_list))->priority,
                       ((Process*) ListCurr(np_list))->state);
                ListNext(np_list);
            }

            temp = ListCurr(lp_list);
            while (ListCurr(lp_list) != NULL) {
                printf("pid: %d priority: %d state: %d \n",
                       ((Process*) ListCurr(lp_list))->pid, ((Process*) ListCurr(lp_list))->priority,
                       ((Process*) ListCurr(lp_list))->state);
                ListNext(lp_list);
            }
            ListFirst(lp_list);
            while (ListCurr(lp_list) != temp) {
                printf("pid: %d priority: %d state: %d \n",
                       ((Process*) ListCurr(lp_list))->pid, ((Process*) ListCurr(lp_list))->priority,
                       ((Process*) ListCurr(lp_list))->state);
                ListNext(lp_list);
            }
        }

    }

    return 0;
}
