#include "private.h"
#include <stdio.h>
#include <stdlib.h>

//UI쓰레드가 실행시킬 함수의 인자를 반환하는 함수
UIFuncArg *createUIFuncArg(UI *ui, int uiNum) {
    UIFuncArg *uifuncarg = malloc(sizeof(UIFuncArg));
    uifuncarg->state = ui->state + uiNum;
    uifuncarg->mutex = ui->mutex + uiNum;
    uifuncarg->cond = ui->cond + uiNum;
    uifuncarg->id = uiNum;
    return uifuncarg;
}

//동적인 갯수의 UI쓰레드를 실행시키기 위해 필요한 각종 정보를 담는 UI구조체를 반환하는 함수
UI *createUI(void *(**buffer)(void *), int size) {
    UI *ui = malloc(sizeof(UI));
    ui->size = 0;
    for (int i = 0; i < size; i++) {
        *(ui->buffer + ui->size++) = *(buffer + i);
        *(ui->state + i) = UI_READY;
        *(ui->mutex + i) = PTHREAD_MUTEX_INITIALIZER;
        *(ui->cond + i) = PTHREAD_COND_INITIALIZER;
        *(ui->arg + i) = (void*)createUIFuncArg(ui, i);
    }
    return ui;
}

//UI구조체를 인자로 받아 구조체에 저장된 UI쓰레드의 갯수를 참조하여 그 갯수만큼 UI쓰레드를 전부 실행하는 함수
void startUI(UI *ui) {
    for (int i = 0; i < ui->size; i++) {
        if (*(ui->state + i) == UI_READY) {
            pthread_create(&(*(ui->th + i)), NULL, *(ui->buffer + i), *(ui->arg + i));
            *(ui->state + i) = UI_WORK;
        }
    }
}

//UI 구조체의 쓰레드 버퍼를 참조하여 특정 쓰레드 혹은 모든 쓰레드의 상태를 UI_PAUSE로 변경
//일시정지로 바뀐 상태들의 인덱스와 동일한 쓰레드들은 함수에 내장된 조건문을 통해 pthread_cond_wait을 실행하여 sleep
void pauseUI(UI *ui, int uiNum) {
    if (uiNum < 0) {
        for (int i = 0; i < ui->size; i++) {
            *(ui->state + i) = UI_PAUSE;
        }
    }
    else {
        *(ui->state + uiNum) = UI_PAUSE;
    }
}

//UI구조체의 모든 쓰레드 혹은 특정 인덱스의 쓰레드 상태를 참조하여 해당 쓰레드의 상태를 UI_WORK로 변경
//이후 phread_signal을 통해 해당 쓰레드를 깨움
void resumeUI(UI *ui, int uiNum) {
    if (uiNum < 0) {
        for (int i = 0; i < ui->size; i++) {
            *(ui->state + i) = UI_WORK;
            pthread_cond_signal(ui->cond + i);
        }
    }
    else {
        *(ui->state + uiNum) = UI_WORK;
        pthread_cond_signal(ui->cond + uiNum);
    }
}

//UI구조체의 모든 쓰레드 혹은 특정  인덱스의 쓰레드 상태를 UI_EXIT로 변경
//이후 해당 쓰레드들의 함수에서 조건문을 통해 반복문을 빠져나온 후 인자로 받은 힙 공간에 할당된 데이터들의 주소를 free로 메모리 사용 해제 후 함수가 종료됨에 따라 쓰레드도 종료됨
//exitUI함수에선 pthread_join으로 해당 쓰레드들이 모두 종료됨을 확인 후 함수 실행을 마침
void exitUI(UI *ui, int uiNum) {
    if (uiNum < 0) {
        for (int i = 0; i < ui->size; i++) {
            *(ui->state + i) = UI_EXIT;
            pthread_join(*(ui->th + i), NULL);
        }
    }
    else {
        *(ui->state + uiNum) = UI_EXIT;
        pthread_join(*(ui->th + uiNum), NULL);
    }
}

//메인 쓰레드가 실행시킬 함수의 인자를 반환하는 함수
ProcessFuncArg *createProcessFuncArg(Process *process) {
    ProcessFuncArg *processFuncArg = malloc(sizeof(ProcessFuncArg));
    processFuncArg->ui = process->ui;
    processFuncArg->state = &process->state;
    processFuncArg->mutex = &process->mutex;
    processFuncArg->cond = &process->cond;
    processFuncArg->pid = (void*)process;
    processFuncArg->name = process->name;
    return processFuncArg;
}

//메인 쓰레드를 실행시키기 위해 필요한 각종 정보를 담는 UI구조체를 반환하는 함수
Process *createProcess(char *processName, void *(*func)(void *), void *(**uiFuncBuffer)(void *), int uiBufferSize) {
    Process *process = malloc(sizeof(Process));
    process->name = processName;
    process->pid = (void*)process;
    process->func = func;
    process->state = EMPTY;
    process->mutex = PTHREAD_MUTEX_INITIALIZER;
    process->mutex_task = PTHREAD_MUTEX_INITIALIZER;
    process->cond = PTHREAD_COND_INITIALIZER;
    process->ui = createUI(uiFuncBuffer, uiBufferSize);
    process->arg = (void*)createProcessFuncArg(process);
    return process;
}

//프로세스 구조체의 상태를 문자열로 반환해주는 함수
char *mainStateToString(MainState mainState) {
    if (mainState == EMPTY) {
        return "EMPTY";
    }
    else if (mainState == WORK) {
        return "WORK";
    }
    else if (mainState == PAUSE) {
        return "PAUSE";
    }
    else {
        return "EXIT";
    }
}

//프로세스 구조체의 상태가 EMPTY즉 아직 실행 전이라면 메인 쓰레드를 실행 시킴, 이후 상태를 WORK로 변경
int startProcess(Process *process) {
    if (process->state == EMPTY) {
        printf("[START - SUCCESS]: %s - pid: %x\n", process->name, process->pid);
        process->state = WORK;
        pthread_create(&process->th, NULL, process->func, process->arg);
        startUI(process->ui);
        return 1;
    }
    else {
        printf("[START - FAILD]: aleady running - %s\n", mainStateToString(process->state));
        return 0;
    }
}

//프로세스 구조체의 상태를 PAUSE로 변경
//메인 쓰레드는 함수에 내장된 조건문을 통해 pthread_cond_wait을 실행하여 sleep
int pauseProcess(Process *process) {
    pthread_mutex_lock(&process->mutex_task);
    int returnSuccess;
    if (process->state == WORK) {
        process->state = PAUSE;
        pauseUI(process->ui, -1);
        returnSuccess = 1;
    }
    else {
        returnSuccess = 0;
    }
    pthread_mutex_unlock(&process->mutex_task);
    return returnSuccess;
}

//프로세스 구조체의 상태를 WORK로 변경
//이후 phread_signal을 통해 메인 쓰레드를 깨움
int resumeProcess(Process *process) {
    pthread_mutex_lock(&process->mutex_task);
    int returnSuccess;
    if (process->state == PAUSE) {
        process->state = WORK;
        resumeUI(process->ui, -1);
        pthread_cond_signal(&process->cond);
        returnSuccess = 1;
    }
    else {
        returnSuccess = 0;
    }
    pthread_mutex_unlock(&process->mutex_task);
    return returnSuccess;
}

//프로세스 구조체의 상태를 EXIT로 변경
//메인 쓰레드에서 실행중인 함수에서 조건문을 통해 반복문을 빠져나온 후 인자로 받은 힙 공간에 할당된 데이터들의 주소를 free로 메모리 사용 해제
//그리고 프로세스 구조체에 저장된 UI 구조체의 주소를 free로 메모리 사용 해제
//이후 exitProcess함수에선 pthread_join으로 메인 쓰레드가 종료됨을 확인 후 함수 실행을 마침
int exitProcess(Process *process) {
    pthread_mutex_lock(&process->mutex_task);
    int returnSuccess = 0;
    if (process->state == WORK) {
        exitUI(process->ui, -1);
        free(process->ui);
        process->state = EXIT;
        returnSuccess = 1;
    }
    else if (process->state == PAUSE) {
        resumeUI(process->ui, -1);
        exitUI(process->ui, -1);
        free(process->ui);
        process->state = EXIT;
        returnSuccess = 1;
    }
    pthread_join(process->th, NULL);
    pthread_mutex_unlock(&process->mutex_task);
    free(process);
    return returnSuccess;
}