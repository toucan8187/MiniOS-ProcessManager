#include "../process.h"
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define NAME "testProcess"
#define MAX_UI 5

void *testUIFunc1(void *arg) {
    UIFuncArg *ufa = (UIFuncArg*)arg;
    pthread_mutex_lock(ufa->mutex);
    while (1) {
        while (*ufa->state == UI_PAUSE) {
            pthread_cond_wait(ufa->cond, ufa->mutex);
        }
        if (*ufa->state == UI_EXIT) {
            break;
        }
        //UI내용은 이 아래부터
        printf("[UI]\tWorking1\n\n");
        Sleep(10);
    }
    free(ufa);
    pthread_mutex_unlock(ufa->mutex);
}

void *testUIFunc2(void *arg) {
    UIFuncArg *ufa = (UIFuncArg*)arg;
    pthread_mutex_lock(ufa->mutex);
    while (1) {
        while (*ufa->state == UI_PAUSE) {
            pthread_cond_wait(ufa->cond, ufa->mutex);
        }
        if (*ufa->state == UI_EXIT) {
            break;
        }
        //UI내용은 이 아래부터
        printf("[UI]\tWorking2\n\n");
        Sleep(10);
    }
    free(ufa);
    pthread_mutex_unlock(ufa->mutex);
}

void *testUIFunc3(void *arg) {
    UIFuncArg *ufa = (UIFuncArg*)arg;
    pthread_mutex_lock(ufa->mutex);
    while (1) {
        while (*ufa->state == UI_PAUSE) {
            pthread_cond_wait(ufa->cond, ufa->mutex);
        }
        if (*ufa->state == UI_EXIT) {
            break;
        }
        //UI내용은 이 아래부터
        printf("[UI]\tWorking3\n\n");
        Sleep(10);
    }
    free(ufa);
    pthread_mutex_unlock(ufa->mutex);
}

void *testUIFunc4(void *arg) {
    UIFuncArg *ufa = (UIFuncArg*)arg;
    pthread_mutex_lock(ufa->mutex);
    while (1) {
        while (*ufa->state == UI_PAUSE) {
            pthread_cond_wait(ufa->cond, ufa->mutex);
        }
        if (*ufa->state == UI_EXIT) {
            break;
        }
        //UI내용은 이 아래부터
        printf("[UI]\tWorking4\n\n");
        Sleep(10);
    }
    free(ufa);
    pthread_mutex_unlock(ufa->mutex);
}

void *testUIFunc5(void *arg) {
    UIFuncArg *ufa = (UIFuncArg*)arg;
    pthread_mutex_lock(ufa->mutex);
    while (1) {
        while (*ufa->state == UI_PAUSE) {
            pthread_cond_wait(ufa->cond, ufa->mutex);
        }
        if (*ufa->state == UI_EXIT) {
            break;
        }
        //UI내용은 이 아래부터
        printf("[UI]\tWorking5\n\n");
        Sleep(10);
    }
    free(ufa);
    pthread_mutex_unlock(ufa->mutex);
}

void *process(void *arg) {
    ProcessFuncArg *pfa = (ProcessFuncArg*)arg;
    pfa->workCount = 0;
    pthread_mutex_lock(pfa->mutex);
    while (1) {
        while (*pfa->state == PAUSE) {
            pthread_cond_wait(pfa->cond, pfa->mutex);
        }
        if (*pfa->state == EXIT) {
            break;
        }
        //메인 쓰레드에서 실행할 내용은 아래부터 입력
        printf("[MAIN]state: %s\tpid: %x\tworkCount: %d\n", mainStateToString(*pfa->state), pfa->pid, ++pfa->workCount);
    }
    free(pfa);
    pthread_mutex_unlock(pfa->mutex);
}

//이 프로세스의 기본 정보들을 바탕으로 Process *를 만들어서 반환함
Process *createTestProcess() {
    ThFunc thFunc[MAX_UI] = { &testUIFunc1,
        &testUIFunc2,
        &testUIFunc3,
        &testUIFunc4,
        &testUIFunc5
    };
    return createProcess(NAME, &process, thFunc, MAX_UI);
}