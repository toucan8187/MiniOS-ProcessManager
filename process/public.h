#ifndef PUBLIC_H
#define PUBLIC_H

typedef struct Process Process;

typedef void *(*ThFunc)(void*);

int startProcess(Process *process);
int pauseProcess(Process *process);
int resumeProcess(Process *process);
int exitProcess(Process *process);

#endif