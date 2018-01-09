#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

struct Process;

typedef struct Message {
	struct Process* sender;
	char message[40];
} Message;

typedef struct Process {
	int pid;
	int priority;
	int state; // ready: 0, running: 1, blocked: 2
	LIST* messages;
} Process;


typedef struct Semaphores{
	int id;
	int init;
	LIST* waitlist;
	int value;
} Semaphore;

int comparator(const void* i1, const void* i2) {
	Process *p1 = (Process *)i1;
	Process *p2 = (Process *)i2;
	if (p1->pid == p2->pid) {
		return 1;
	}
	else {
		return 0;
	}
}

int main(int argc, char *argv[])
{
	Semaphore semaphores[5];
	for (int i = 0; i < 5; i++) {
		semaphores[i].id = i;
		semaphores[i].init = 0;
		semaphores[i].waitlist = ListCreate();
		semaphores[i].value = 0;
	}

	int last_pid = 0;
	LIST *HPlist = ListCreate();
	LIST *NPlist = ListCreate();
	LIST *LPlist = ListCreate();
	LIST *sendBlocked = ListCreate();
	LIST *receiveBlocked = ListCreate();
	int terminate = 0;
	char input;
	while (!terminate){
		Process *running = ListCurr(HPlist);
		if (running == NULL) {
			running = ListCurr(NPlist);
			if (running == NULL) {
				running = ListCurr(LPlist);
			}
		}


		if (running != NULL) {
			printf("running process number: %d\n", running->pid); 
		}

		scanf("%c", &input);
		if(input == 'c'){
			int priority;
			printf("priority: ");
			scanf("%d", &priority);
			Process *p = malloc(sizeof(Process));
			if(priority == 0 || priority == 1 || priority == 2){
				p->pid = ++last_pid;
				p->priority = priority;
				p->state = 0;
				p->messages = ListCreate();
			}
			if(priority == 0) {
				ListInsert(HPlist, p);
				if (ListNext(HPlist) == NULL) {
					ListPrev(HPlist);
				}
			}
			else if(priority == 1) {
				ListInsert(NPlist, p);
				if (ListNext(NPlist) == NULL) {
					ListPrev(NPlist);
				}

			}
			else if(priority == 2) {
				ListInsert(LPlist, p);
				if (ListNext(LPlist) == NULL) {
					ListPrev(LPlist);
				}
			}
			else {
				printf("wrong number for priority\n");
				continue;
			}
			printf("created a new process %d\n", p->pid);
		}
		else if (input == 'f') {
			if (running == NULL) {
				printf("fork failed\n");
				continue;
			}
			Process *p = malloc(sizeof(Process));
			p->pid = ++last_pid;
			p->priority = running->priority;
			p->state = 0;
			p->messages = ListCreate();
			if(p->priority == 0) {
				ListInsert(HPlist, p);
				ListNext(HPlist);
			}
			else if(p->priority == 1) {
				ListInsert(NPlist, p);
				ListNext(NPlist);

			}
			else if(p->priority == 2) {
				ListInsert(LPlist, p);
				ListNext(LPlist);
			}
			printf("forked process %d to %d\n", running->pid, p->pid);
		}
		else if (input == 'k') {
			int deleting_pid;
			printf("pid: ");
			scanf("%d", &deleting_pid);
			Process *p = malloc(sizeof(Process));
			p->pid = deleting_pid;
			
			if ((ListCurr(HPlist)) == NULL){
				if ((ListCurr(NPlist)) == NULL){
					if ((ListCurr(LPlist)) == NULL){
						printf("Killed failed\n");
					}
				}
			}
			// High
			Process *curr = ListCurr(HPlist);
			if (curr != NULL) {
				// if the process that user wants to kill is the one running
				if (curr->pid == deleting_pid) {
					if (ListNext(HPlist) == NULL) {
						ListPrev(HPlist);
						ListRemove(HPlist);
						ListFirst(HPlist);
					}
					else {
						ListPrev(HPlist);
						ListRemove(HPlist);

					}
					printf("Killed succeeded\n");
				}
				// if the process that user wants to kill is not the one running
				else {
					// current pointer goes to the first process
					// and ListSearch takes the current pointer to the found item
					ListFirst(HPlist);
					if (ListSearch(HPlist, comparator, p) != NULL) {
						ListRemove(HPlist);
						printf("Killed succeeded\n");

					}
					ListFirst(HPlist);
				}
				//if we cannot find the process, kill fails
			}

			// Norm
			curr = ListCurr(NPlist);
			if (curr != NULL) {
				if (curr->pid == deleting_pid) {
					if (ListNext(NPlist) == NULL) {
						ListPrev(NPlist);
						ListRemove(NPlist);
						ListFirst(NPlist);
					}
					else {
						ListPrev(NPlist);
						ListRemove(NPlist);
					}
					printf("Killed succeeded\n");
				}
				else {
					ListFirst(NPlist);
					if (ListSearch(NPlist, comparator, p) != NULL) {
						ListRemove(NPlist);
					}
					ListFirst(NPlist);
				}
			}

			// Low
			curr = ListCurr(LPlist);
			if (curr != NULL) {
				if (curr->pid == deleting_pid) {
					if (ListNext(LPlist) == NULL) {
						ListPrev(LPlist);
						ListRemove(LPlist);
						ListFirst(LPlist);
					}
					else {
						ListPrev(LPlist);
						ListRemove(LPlist);
					}
					printf("Killed succeeded\n");
				}
				else {
					ListFirst(LPlist);
					if (ListSearch(LPlist, comparator, p) != NULL) {
						ListRemove(LPlist);
						printf("Killed succeeded\n");
					}
					ListFirst(LPlist);
				}
			}
		}

		else if (input == 'e'){
			if(running == NULL){
				printf("nothing is running at the moment\n");
			}
			else{
				if(running->priority == 0){
					if (ListNext(HPlist) == NULL) {
						ListPrev(HPlist);
						ListRemove(HPlist);
						ListFirst(HPlist);
					}
					else {
						ListPrev(HPlist);
						ListRemove(HPlist);
					}
				}
				else if(running->priority == 1){					
					if (ListNext(NPlist) == NULL) {
						ListPrev(NPlist);
						ListRemove(NPlist);
						ListFirst(NPlist);
					}
					else {
						ListPrev(NPlist);
						ListRemove(NPlist);
					}
				}

				else{
					if (ListNext(LPlist) == NULL) {
						ListPrev(LPlist);
						ListRemove(LPlist);
						ListFirst(LPlist);
					}
					else {
						ListPrev(LPlist);
						ListRemove(LPlist);
					}
				}
			}
		}

		else if (input == 'q'){
			if(running == NULL){
				printf("nothing is running at the moment\n");
			}
			else{
				if(ListCurr(HPlist) != NULL){
					if (ListNext(HPlist) == NULL) {
						ListFirst(HPlist);
					}
				}
				else if(ListCurr(NPlist) != NULL){					
					if (ListNext(NPlist) == NULL) {
						ListFirst(NPlist);
					}
				}
				else{
					if (ListNext(LPlist) == NULL) {
						ListFirst(LPlist);
					}
				}
			}
		}

		else if (input == 's'){
			if (running == NULL) {
				printf("nothing is running at the moment\n");
				continue;
			}

			int send_to_pid;
			printf("target pid: ");
			scanf("%d", &send_to_pid);

			printf("message: ");
			char message[40];
			scanf("%s", message);
			printf("%s\n",message);
			Process *send_to_process = malloc(sizeof(Process));
			send_to_process->pid = send_to_pid;
			Process *receiver;

			int found = 0; //finding revceiver from the 3 lists
			// return the current pointer to curr so that
			// we could find the original current pointer location
			// and move it back using ListSearch

			// we are searching the process sent to though 3 Lists from HP to LP
			Process *curr = ListCurr(HPlist);
			if (curr != NULL) {
				ListFirst(HPlist);
				if ((receiver = ListSearch(HPlist, comparator, send_to_process)) != NULL) {
					found = 1;
				}
				ListFirst(HPlist);
				ListSearch(HPlist, comparator, curr);
			}

			// if process sent to does not exist in the HP List, then we go to the NPlist
			if (found == 0) {
				curr = ListCurr(NPlist);
				if (curr != NULL) {
					ListFirst(NPlist);
					if ((receiver = ListSearch(NPlist, comparator, send_to_process)) != NULL) {
						found = 1;
					}
					ListFirst(NPlist);
					ListSearch(NPlist, comparator, curr);
				}
			}

			// if the process sent to does not exist in the NPlist, then we go to the LPlist 
			if (found == 0) {
				curr = ListCurr(LPlist);
				if (curr != NULL) {
					ListFirst(LPlist);
					if ((receiver = ListSearch(LPlist, comparator, send_to_process)) != NULL) {
						found = 1;
					}
					ListFirst(LPlist);
					ListSearch(LPlist, comparator, curr);
				}
			}

			// if the process sent to is in none of the list, we try finding it in the sendBlock List
			if (found == 0) {
				curr = ListCurr(sendBlocked);
				if (curr != NULL) {
					ListFirst(sendBlocked);
					if ((receiver = ListSearch(sendBlocked, comparator, send_to_process)) != NULL) {
						found = 2; // if receiver is blocked due to the send command, do not unblock it.
					}
					ListFirst(sendBlocked);
					ListSearch(sendBlocked, comparator, curr);
				}
			}

			if (found == 0) {
				curr = ListCurr(receiveBlocked);
				if (curr != NULL) {
					ListFirst(receiveBlocked);
					if ((receiver = ListSearch(receiveBlocked, comparator, send_to_process)) != NULL) {
						found = 3; // if receiver is already blocked due to the receiver command, unblock it while sending your message.
					}
					ListFirst(receiveBlocked);
					ListSearch(receiveBlocked, comparator, curr);
				}
			}

			if (found == 0) {
					printf("pid does not exist in the 3 Prioritylists\n");
			}
			// found the revceiver
			else {
				Message *msg = malloc(sizeof(Message));
				msg->sender = running;
				strcpy(msg->message, message);
				// if receiver is in the blocked list, we put it in the ready
				if (receiver->state == 2 && found == 3) {
				// change the state of receiver to ready 
					receiver->state = 0; // Ready
					if (ListSearch(receiveBlocked, comparator, receiver) != NULL) {
						ListRemove(receiveBlocked); // remove receiver from receiveBlocked list
						if(receiver->priority == 0) {
							ListInsert(HPlist, receiver);
							if (ListNext(HPlist) == NULL) {
								ListPrev(HPlist);
							}
						}
						else if(receiver->priority == 1) {
							ListInsert(NPlist, receiver);
							if (ListNext(NPlist) == NULL) {
								ListPrev(NPlist);
							}
						}
						else {
							ListInsert(LPlist, receiver);
							if (ListNext(LPlist) == NULL) {
								ListPrev(LPlist);
							}
						}
					}
				}

				ListAdd(receiver->messages, msg);
				running->state = 2; // Block the sender of the message
				ListAdd(sendBlocked, running);
				if(running->priority == 0){
					if (ListNext(HPlist) == NULL) {
						ListPrev(HPlist);
						ListRemove(HPlist);
						ListFirst(HPlist);
					}
					else {
						ListPrev(HPlist);
						ListRemove(HPlist);
					}
				}
				else if(running->priority == 1){					
					if (ListNext(NPlist) == NULL) {
						ListPrev(NPlist);
						ListRemove(NPlist);
						ListFirst(NPlist);
					}
					else {
						ListPrev(NPlist);
						ListRemove(NPlist);
					}
				}
				else{
					if (ListNext(LPlist) == NULL) {
						ListPrev(LPlist);
						ListRemove(LPlist);
						ListFirst(LPlist);
					}
					else {
						ListPrev(LPlist);
						ListRemove(LPlist);
					}
				}
			}

		}
		else if (input == 'r'){
			if (running == NULL) {
				printf("nothing is running at the moment\n");
				continue;
			}

			Message* msg;
			if ((msg = ListRemove(running->messages)) != NULL) {
				printf("Process %d: %s\n", msg->sender->pid, msg->message);
			}
			else {
				running->state = 2; // No message to receive, block the receiver
				ListAdd(receiveBlocked, running);
				if(running->priority == 0){
					if (ListNext(HPlist) == NULL) {
						ListPrev(HPlist);
						ListRemove(HPlist);
						ListFirst(HPlist);
					}
					else {
						ListPrev(HPlist);
						ListRemove(HPlist);
					}
				}
				else if(running->priority == 1){					
					if (ListNext(NPlist) == NULL) {
						ListPrev(NPlist);
						ListRemove(NPlist);
						ListFirst(NPlist);
					}
					else {
						ListPrev(NPlist);
						ListRemove(NPlist);
					}
				}
				else{
					if (ListNext(LPlist) == NULL) {
						ListPrev(LPlist);
						ListRemove(LPlist);
						ListFirst(LPlist);
					}
					else {
						ListPrev(LPlist);
						ListRemove(LPlist);
					}
				}				
			}
		}

		else if (input == 'Y'){
			if (running == NULL) {
				printf("nothing is running at the moment\n");
				continue;
			}

			int temp_pid;
			printf("sender pid: ");
			scanf("%d", &temp_pid);
			printf("message: ");
			char message[40];
			scanf("%s", message);
			Process *temp = malloc(sizeof(Process));
			temp->pid = temp_pid;
			Process *sender;

			ListFirst(sendBlocked);
			if ((sender = ListSearch(sendBlocked, comparator, temp)) == NULL) {
				printf("sender process does not exist\n");
			}
			else {
				// TODO: send the reply message to sender, and sender should print this message at its turn
				// Message *msg = malloc(sizeof(Message));
				// msg.sender = running;
				// strcpy(message, msg.message, 40);

				sender->state = 0; // Unblock the sender
				if (ListSearch(sendBlocked, comparator, sender) != NULL) {
					ListRemove(sendBlocked); // remove receiver from receiveBlocked list
					if(sender->priority == 0) {
						ListInsert(HPlist, sender);
						if (ListNext(HPlist) == NULL) {
							ListPrev(HPlist);
						}
					}
					else if(sender->priority == 1) {
						ListInsert(NPlist, sender);
						if (ListNext(NPlist) == NULL) {
							ListPrev(NPlist);
						}

					}
					else {
						ListInsert(LPlist, sender);
						if (ListNext(LPlist) == NULL) {
							ListPrev(LPlist);
						}
					}
				}

				Message *msg = malloc(sizeof(Message));
				msg->sender = running;
				strcpy(msg->message, message);
				ListAdd(sender->messages, msg);
			}
		}

		else if (input == 'n'){
			int sem_id;
			printf("semphore ID: ");
			scanf("%d", &sem_id);
			if (semaphores[sem_id].init == 1) {
				printf("semaphore #%d has been initialized before!\n", sem_id);
				continue;
			}

			int sem_value;
			printf("semphore value: ");
			scanf("%d", &sem_value);
			if (sem_value < 0) {
				printf("wrong semaphore value\n");
				continue;
			}

			semaphores[sem_id].value = sem_value;
			printf("semaphore #%d has been initialized !\n", sem_id);
		}
		else if (input == 'p'){
			int semphoreid;
			printf("semphore ID: ");
			scanf("%d", &temp_pid);
			if(semaphores[semphoreid].value < 0){
				ListAdd(semaphores[semphoreid].waitlist, running);
				running.state = 2;
				if(running->priority == 0){
					if (ListNext(HPlist) == NULL) {
						ListPrev(HPlist);
						ListRemove(HPlist);
						ListFirst(HPlist);
					}
					else {
						ListPrev(HPlist);
						ListRemove(HPlist);
					}
				}
				else if(running->priority == 1){					
					if (ListNext(NPlist) == NULL) {
						ListPrev(NPlist);
						ListRemove(NPlist);
						ListFirst(NPlist);
					}
					else {
						ListPrev(NPlist);
						ListRemove(NPlist);
					}
				}

				else{
					if (ListNext(LPlist) == NULL) {
						ListPrev(LPlist);
						ListRemove(LPlist);
						ListFirst(LPlist);
					}
					else {
						ListPrev(LPlist);
						ListRemove(LPlist);
					}
				}
			} else {
				semaphores[semphoreid].value --;
			}
		}

		else if (input == 'v'){
			int temp_pid;
			printf("semphore ID: ");
			scanf("%d", &temp_pid);
			if(ListCount(semaphores[semphoreid].waitlist) > 0) {
				Process* blockedP = ListRemove(semaphores[semphoreid].waitlist);
				if(blockedP->priority == 0) {
					ListInsert(HPlist, blockedP);
					if (ListNext(HPlist) == NULL) {
						ListPrev(HPlist);
					}
				}
				else if(blockedP->priority == 1) {
					ListInsert(NPlist, blockedP);
					if (ListNext(NPlist) == NULL) {
						ListPrev(NPlist);
					}

				}
				else {
					ListInsert(LPlist, blockedP);
					if (ListNext(LPlist) == NULL) {
						ListPrev(LPlist);
					}
				}
			}
			else {
				semaphores[semphoreid].value ++;
			}
		}

		else if (input == 'i'){
			printf("complete information about the current running process pid: %d priority: %d state: %d \n", 
					((Process *)ListCurr(HPlist))->pid, ((Process *)ListCurr(HPlist))-> priority, ((Process *)ListCurr(HPlist))->state );
		}
		else if (input == 't'){
			Process * temp = ListCurr(HPlist);
			while(ListCurr(HPlist) != NULL){
				printf("pid: %d priority: %d state: %d \n", 
					((Process *)ListCurr(HPlist))->pid, ((Process *)ListCurr(HPlist))-> priority, ((Process *)ListCurr(HPlist))->state );
				ListNext(HPlist);
			}
			ListFirst(HPlist);
			while(ListCurr(HPlist) != temp){
				printf("pid: %d priority: %d state: %d \n", 
					((Process *)ListCurr(HPlist))->pid, ((Process *)ListCurr(HPlist))-> priority, ((Process *)ListCurr(HPlist))->state );
				ListNext(HPlist);
			}

			temp = ListCurr(NPlist);
			while(ListCurr(NPlist) != NULL){
				printf("pid: %d priority: %d state: %d \n", 
					((Process *)ListCurr(NPlist))->pid, ((Process *)ListCurr(NPlist))-> priority, ((Process *)ListCurr(NPlist))->state );
				ListNext(NPlist);
			}
			ListFirst(NPlist);
			while(ListCurr(NPlist) != temp){
				printf("pid: %d priority: %d state: %d \n", 
					((Process *)ListCurr(NPlist))->pid, ((Process *)ListCurr(NPlist))-> priority, ((Process *)ListCurr(NPlist))->state );
				ListNext(NPlist);
			}

			temp = ListCurr(LPlist);
			while(ListCurr(LPlist) != NULL){
				printf("pid: %d priority: %d state: %d \n", 
					((Process *)ListCurr(LPlist))->pid, ((Process *)ListCurr(LPlist))-> priority, ((Process *)ListCurr(LPlist))->state );
				ListNext(LPlist);
			}
			ListFirst(LPlist);
			while(ListCurr(LPlist) != temp){
				printf("pid: %d priority: %d state: %d \n", 
					((Process *)ListCurr(LPlist))->pid, ((Process *)ListCurr(LPlist))-> priority, ((Process *)ListCurr(LPlist))->state );
				ListNext(LPlist);
			}
		}



	}

    return 0;
}
