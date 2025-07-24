#ifndef PRIVATE_H
#define PRIVATE_H

#include <pthread.h>

#define MAX_UI_BUFFER 100

typedef void *(*ThFunc)(void*);

//UI쓰레드의 상태
typedef enum {
    UI_READY, UI_WORK, UI_PAUSE, UI_EXIT
} UIState;

//UI쓰레드들을 관리하기 위해 필요한 데이터들을 저장하는 구조체
typedef struct {
    void *(*buffer[MAX_UI_BUFFER])(void *);
    void *arg[MAX_UI_BUFFER];
    UIState state[MAX_UI_BUFFER];
    pthread_t th[MAX_UI_BUFFER]; //UI를 멀티쓰레드로 실행하기 위함
    pthread_mutex_t mutex[MAX_UI_BUFFER]; //processPause를 하기 위함
    pthread_cond_t cond[MAX_UI_BUFFER]; //processPause를 하기 위함
    int size;
} UI;

//UI 쓰레드들이 돌릴 함수에 인자로 넣을 데이터들을 담는 구조체
typedef struct {
    UIState *state;
    pthread_mutex_t *mutex;
    pthread_cond_t *cond;
    int id;
} UIFuncArg;

//메인 쓰레드의 상태, UIState와 상태들이 같은데 분할한 이유는 추후 UI와 메인 쓰레드에 상태들이 다르게 추가될 수 있음을 고려하여 확장성을 이유로 분할
typedef enum {
    EMPTY, WORK, PAUSE, EXIT
} MainState;

//UI쓰레드들의 정보를 담은 UI구조체와 메인 쓰레드를 관리하기 위해 필요한 데이터들을 저장하는 구조체
//추후 여러 프로세스들을 실행 시키고 관리를 하게 되었을때 이름 혹은 프로세스의 주소를 이용하여 관리할 수 있도록 확장성을 이유로 pid와 name을 저장
typedef struct Process {
    UI *ui;
    char *name;
    void *pid; //프로세스 식별을 위한 해당 프로세스 구조체의 주소
    void *(*func)(void*);
    void *arg;
    MainState state;
    pthread_t th;
    pthread_mutex_t mutex;
    pthread_mutex_t mutex_task;
    pthread_cond_t cond;
} Process;

//메인 쓰레드가 돌릴 함수에 인자로 넣을 데이터들을 저장하는 구조체
typedef struct {
    UI *ui;
    MainState* state;
    pthread_mutex_t* mutex;
    pthread_cond_t* cond;
    void *pid;
    char *name;
    int workCount;
} ProcessFuncArg;

Process *createProcess(char *processName, void *(*func)(void *), void *(**uiFuncBuffer)(void *), int uiBufferSize);
char *mainStateToString(MainState mainState);

#endif