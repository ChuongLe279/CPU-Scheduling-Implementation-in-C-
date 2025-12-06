#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SORT_BY_ARRIVAL 0
#define SORT_BY_PID     1
#define SORT_BY_BURST   2
#define SORT_BY_START   3

typedef struct {
    int iPID;
    int iArrival, iBurst;
    int iStart, iFinish, iWaiting, iResponse, iTaT;
} PCB;

int generatingRandom(int min, int max){
    return (rand() % (max - min + 1)) + min;
}

void inputProcess(int n, PCB P[]){ //an array of PCB
    for (int i = 0; i < n; i++){
        P[i].iArrival = generatingRandom(0, 20);
        P[i].iBurst = generatingRandom(2, 12);
        P[i].iPID = i;
    }
}

void printProcess(int n, PCB P[]){
    printf("Process       |Arrival time       |Burst time\n");
    for (int i = 0; i < n; i++){
        printf("Process %d     |%-14d     |%-8d\n", P[i].iPID + 1, P[i].iArrival, P[i].iBurst);
    }
}

void swap(PCB *a, PCB *b){
    PCB temp;
    temp = *a;
    *a = *b;
    *b = temp;
}

int sortCriteria(PCB p, int iCriteria){
 switch (iCriteria){
        case SORT_BY_ARRIVAL:
            return p.iArrival;
            break;
        case SORT_BY_PID:
            return p.iPID;
            break;
        case SORT_BY_BURST:
            return p.iBurst;
            break;
        case SORT_BY_START:
            return p.iStart;
            break;
        default:
            break;
    }
}

int partition(PCB P[], int low, int high, int iCriteria){
    int i = low;
    int j = high;
    int pivot = sortCriteria(P[low], iCriteria);;
    while (i < j) {
        while (sortCriteria(P[i], iCriteria) <= pivot && i <= high - 1) {
            i++;
        }
        while (sortCriteria(P[j], iCriteria) > pivot && j >= low + 1) {
            j--;
        }
        if (i < j) {
            swap(&P[i], &P[j]);
        }
    }
    swap(&P[low], &P[j]);
    return j;
}

void quickSort(PCB P[], int low, int high, int iCriteria){
    if (low < high) {
        int pi = partition(P, low, high, iCriteria);
        quickSort(P, low, pi - 1, iCriteria);
        quickSort(P, pi + 1, high, iCriteria);
    }
}

void pushProcess(int *n, PCB P[], PCB Q){
    P[*n] = Q;
    (*n)++;
}
void removeProcess(int *n, int index, PCB P[]){
    for (int i = index; i < *n - 1; i++){
        P[i] = P[i + 1];
    }
    (*n)--; 
}

void calculateAWT(int n, PCB P[]){ 
    int sum = 0;
    for (int i = 0; i < n; i++){
        sum += P[i].iWaiting;
    }
    printf("\nAverage waiting time = %.1f\n", (float) sum / n);
}
void calculateATaT(int n, PCB P[]){ 
    int sum = 0;
    for (int i = 0; i < n; i++){
        sum += (P[i].iFinish - P[i].iArrival);
    }
    printf("\nAverage turn around time = %.1f\n", (float) sum / n);
}
void exportGanttChart(int n, PCB P[]){ 
    printf("|");
    for (int i = 0; i < n; i++){
        printf(" p%d |", P[i].iPID + 1);
    }
    printf("\n");
}
int Max(int a, int b){
    return (a > b) ? a : b;
}
int main(){
    srand(time(NULL));
    PCB Input[10];
    PCB ReadyQueue[10];
    PCB TerminatedArray[10];

    int iNumberOfProcess;
    printf("Please input number of Process: ");
    scanf("%d", &iNumberOfProcess);

    int iRemain = iNumberOfProcess, iReady = 0, iTerminated = 0;

    inputProcess(iNumberOfProcess, Input);
    printf("\n===== Bang tom tat cac tien trinh =====\n");
    printProcess(iNumberOfProcess, Input);
    quickSort(Input, 0, iNumberOfProcess - 1, SORT_BY_ARRIVAL);
    printf("\n===== Bang tom tat cac tien trinh sau khi sort =====\n");
    printProcess(iNumberOfProcess, Input);

    //Đẩy vào hàng đợi ReadyQueue
    pushProcess(&iReady, ReadyQueue, Input[0]);
    removeProcess(&iRemain, 0, Input);
    printf("\n===== Ready Queue ===== \n");
    printProcess(1, ReadyQueue);

    //Đẩy process vào Terminated Array
    pushProcess(&iTerminated, TerminatedArray, ReadyQueue[0]);
    removeProcess(&iReady, 0, ReadyQueue);
    TerminatedArray[0].iStart    = TerminatedArray[0].iArrival;
    TerminatedArray[0].iFinish   = TerminatedArray[0].iArrival + TerminatedArray[0].iBurst;
    TerminatedArray[0].iWaiting  = TerminatedArray[0].iStart - TerminatedArray[0].iArrival; 
    TerminatedArray[0].iResponse = TerminatedArray[0].iWaiting;
    TerminatedArray[0].iTaT      = TerminatedArray[0].iFinish - TerminatedArray[0].iArrival;


    while (iTerminated < iNumberOfProcess) {
        int currentTime = TerminatedArray[iTerminated - 1].iFinish;
        while (iRemain > 0) {
            if (Input[0].iArrival <= currentTime) {
                pushProcess(&iReady, ReadyQueue, Input[0]);
                removeProcess(&iRemain, 0, Input);
                continue;
            } else {
                break;
            }
        }

        if (iReady == 0 && iRemain > 0) {
            if (currentTime < Input[0].iArrival) {
                currentTime = Input[0].iArrival;
            }
            pushProcess(&iReady, ReadyQueue, Input[0]);
            removeProcess(&iRemain, 0, Input);
        }

        if (iReady > 0) {
            ReadyQueue[0].iStart    = Max(ReadyQueue[0].iArrival , currentTime);            
            ReadyQueue[0].iFinish   = ReadyQueue[0].iStart + ReadyQueue[0].iBurst;
            ReadyQueue[0].iResponse = ReadyQueue[0].iStart - ReadyQueue[0].iArrival;
            ReadyQueue[0].iWaiting  = ReadyQueue[0].iResponse;
            ReadyQueue[0].iTaT      = ReadyQueue[0].iFinish - ReadyQueue[0].iArrival;
            currentTime = ReadyQueue[0].iFinish;

            pushProcess(&iTerminated, TerminatedArray, ReadyQueue[0]);
            removeProcess(&iReady, 0, ReadyQueue);
        }
        
    }


    printf("\n===== FCFS Scheduling =====\n");
    exportGanttChart(iTerminated, TerminatedArray); 
    
    // quickSort(TerminatedArray, 0, iTerminated - 1, SORT_BY_PID); 
    
    calculateAWT(iTerminated, TerminatedArray); 
    calculateATaT(iTerminated, TerminatedArray);

    return 0;
}