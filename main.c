#include "process/processList/testProcess.h"
#include <stdio.h>

int main() {
    Process *testProcess = createTestProcess();
    startProcess(testProcess); //testProcess 실행
    printf("start\n");
    getchar();
    pauseProcess(testProcess); //testProcess 일시정지
    printf("pause\n");
    getchar();
    resumeProcess(testProcess); //testProcess 일시정지 해제
    printf("resume\n");
    getchar();
    exitProcess(testProcess); //testProcess 종료
    printf("exit\n");
    getchar();
    printf("END\n"); //testProcess 종료 확인
}