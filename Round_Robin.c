#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>


#define SORT_BY_ARRIVAL 0
#define SORT_BY_PID     1
#define SORT_BY_BURST   2
#define SORT_BY_START   3
#define HAVE_NOT_RAN    -1

typedef struct {
    int iPID;
    int iArrival, iBurst, BurstTime;
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
        P[i].BurstTime = P[i].iBurst;
        P[i].iStart = HAVE_NOT_RAN;
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
            return 0;
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
int CheckShortestJob(int n, PCB P[]){
    int shortestIndex = 0;
    for (int i = 1; i < n; i++){
        if (P[i].iBurst < P[shortestIndex].iBurst)
            shortestIndex = i;
    }
    return shortestIndex;
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
    printf("\nAverage turn around time = %.1f\n\n", (float) sum / n);
}

void exportGanttChart(int n, PCB P[]){ 
    int sizeB = 0;
    PCB *b = (PCB*) malloc(n * sizeof(PCB));
    for (int i = 0; i < n; i++){
        b[sizeB++].iPID = P[i].iPID; 
        while(i + 1 < n && P[i].iPID == P[i + 1].iPID){
            i++;
        }
    }

    printf("|");
    for (int i = 0; i < sizeB; i++){
        printf(" p%d |", b[i].iPID + 1);
    }
    printf("\n");

    free(b);
}

int Max(int a, int b){
    return (a > b) ? a : b;
}

int main(){
    srand(time(NULL));
    PCB Input[10];
    PCB ReadyQueue[10];
    PCB TerminatedArray[10];
    PCB GanntChart[25];

    int iNumberOfProcess, iRobin;
    printf("Please input Round Robin: ");
    scanf("%d", &iRobin);
    printf("Please input number of Process: ");
    scanf("%d", &iNumberOfProcess);

    int iRemain = iNumberOfProcess, iReady = 0, iTerminated = 0, iGantt = 0;

    //In và sắp xếp các tiến trình
    inputProcess(iNumberOfProcess, Input);
    printf("\n===== Bang tom tat cac tien trinh =====\n");
    printProcess(iNumberOfProcess, Input);
    quickSort(Input, 0, iNumberOfProcess - 1, SORT_BY_ARRIVAL);
    printf("\n===== Bang tom tat cac tien trinh sau khi sort theo Arrival Time =====\n");
    printProcess(iNumberOfProcess, Input);

    //Khai báo biến currentTime
    int currentTime = 0;
    if (iRemain > 0 && Input[0].iArrival > currentTime){
        currentTime = Input[0].iArrival;
    }


    while (iTerminated < iNumberOfProcess){

        //Đẩy các tiến trình vào ReadyQueue
        while (iRemain > 0 && Input[0].iArrival <= currentTime) {
            pushProcess(&iReady, ReadyQueue, Input[0]);
            removeProcess(&iRemain, 0, Input);
        }

        //Kiểm tra và đẩy các tiến trình vào readyqueue khi thời gian tới sau finish time
        if (iReady == 0 && iRemain > 0) {
            if (currentTime < Input[0].iArrival) {
                currentTime = Input[0].iArrival;
            }
            pushProcess(&iReady, ReadyQueue, Input[0]);
            removeProcess(&iRemain, 0, Input);
        }

        //Chọn ra tiến trình đầu tiên để chạy theo round robin
        PCB* Running = &ReadyQueue[0];
        pushProcess(&iGantt, GanntChart, ReadyQueue[0]);

        if(Running->iStart == HAVE_NOT_RAN){
            Running->iStart = currentTime;
        }

        int runTime = (Running->iBurst < iRobin) ? Running->iBurst : iRobin;
        Running->iBurst -= runTime;
        currentTime += runTime;

        //Khi tiến trình chạy xong hết thì đẩy vào TerminatedArray
        if (Running->iBurst == 0){
            Running->iFinish   = currentTime;
            Running->iTaT      = Running->iFinish - Running->iArrival;
            Running->iWaiting  = Running->iTaT - Running->BurstTime;
            Running->iResponse = Running->iStart - Running->iArrival;
            pushProcess(&iTerminated, TerminatedArray, *Running);
            removeProcess(&iReady, 0, ReadyQueue);
        } else { //Nếu tiến trình chưa chạy xong thì quay ngược lại ReadyQueue
            pushProcess(&iReady, ReadyQueue, *Running);
            removeProcess(&iReady, 0, ReadyQueue);
        }


    }


    printf("\n===== Round Robin Scheduling =====\n");
    exportGanttChart(iGantt, GanntChart); 
    
    // quickSort(TerminatedArray, 0, iTerminated - 1, SORT_BY_PID); 
    
    calculateAWT(iTerminated, TerminatedArray); 
    calculateATaT(iTerminated, TerminatedArray);

    return 0;
}