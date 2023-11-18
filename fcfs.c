#include <stdio.h>
#include <stdlib.h>
#define SORT_BY_ARRIVAL 0
#define SORT_BY_PID 1
#define SORT_BY_BURST 2
#define SORT_BY_START 3

typedef struct
{
    int iPID;
    int iArrival, iBurst;
    int iStart, iFinish, iWaiting, iResponse, iTaT;
} PCB;

void inputProcess(int n, PCB P[])
{
    for (int i = 0; i < n; i++)
    {
        printf("Input P%d:", i);
        printf("\n\tInput arrival time: ");
        scanf("%d", &P[i].iArrival);
        printf("\tInput burst time: ");
        scanf("%d", &P[i].iBurst);
        P[i].iPID = i;
    }
}

void printProcess(int n, PCB P[])
{
    for (int i = 0; i < n; i++)
    {
        printf("P%d[arrival = %d, burst = %d]\n", P[i].iPID, P[i].iArrival, P[i].iBurst);
    }
}

void printStats(int n, PCB P[])
{
    for (int i = 0; i < n; i++)
    {
        printf("P%d[arrival = %d, burst = %d, start=%d, finish=%d, wait=%d, response=%d, turnaround=%d]\n", P[i].iPID, P[i].iArrival, P[i].iBurst, P[i].iStart, P[i].iFinish, P[i].iWaiting, P[i].iResponse, P[i].iTaT);
    }
}

void exportGanttChart(int n, PCB P[]) {}
void pushProcess(int *n, PCB P[], PCB Q)
{
    P[(*n)++] = Q;
}

void removeProcess(int *n, int index, PCB P[])
{
    if (*n == 1)
    {
        P[0].iArrival = 0;
        P[0].iBurst = 0;
        P[0].iFinish = 0;
        P[0].iPID = 0;
        P[0].iResponse = 0;
        P[0].iStart = 0;
        P[0].iTaT = 0;
        P[0].iWaiting = 0;
    }
    else
    {
        for (int i = index; i < (*n - 1); i++)
        {
            P[i] = P[i + 1];
            P[i + 1].iArrival = 0;
            P[i + 1].iBurst = 0;
            P[i + 1].iFinish = 0;
            P[i + 1].iPID = 0;
            P[i + 1].iResponse = 0;
            P[i + 1].iStart = 0;
            P[i + 1].iTaT = 0;
            P[i + 1].iWaiting = 0;
        }
    }

    *n = (*n) - 1;
    // printf("after operation: %d", *n);
}

int swapProcess(PCB *P, PCB *Q)
{
    PCB tmp = *P;
    *P = *Q;
    *Q = tmp;
}
int partition(PCB arr[], int low, int high, int iCriteria)
{
    PCB pivot = arr[high];
    int i = (low - 1);

    switch (iCriteria)
    {
    case 0:
        for (int j = low; j <= high; j++)
        {
            if (arr[j].iArrival < pivot.iArrival)
            {
                i++;
                swapProcess(arr + i, arr + j);
            }
        }
        break;
    case 1:
        for (int j = low; j <= high; j++)
        {
            if (arr[j].iPID < pivot.iPID)
            {
                i++;
                swapProcess(arr + i, arr + j);
            }
        }
        break;
    case 2:
        for (int j = low; j <= high; j++)
        {
            if (arr[j].iBurst < pivot.iBurst)
            {
                i++;
                swapProcess(arr + i, arr + j);
            }
        }
        break;
    case 3:
        for (int j = low; j <= high; j++)
        {
            if (arr[j].iStart < pivot.iStart)
            {
                i++;
                swapProcess(arr + i, arr + j);
            }
        }
        break;
    default:
        printf("Error: Unknown quicksort criteria %d", iCriteria);
        exit(1);
        break;
    }

    swapProcess(arr + i + 1, arr + high);
    return i + 1;
}
void quickSort(PCB arr[], int low, int high, int iCriteria)
{
    if (low >= high)
        return;

    int pi = partition(arr, low, high, iCriteria);
    quickSort(arr, low, pi - 1, iCriteria);
    quickSort(arr, pi + 1, high, iCriteria);
}
void calculateAWT(int n, PCB P[])
{
    int totalWaitingTime = 0;
    for (int i = 0; i < n; i++)
    {
        totalWaitingTime += P[i].iWaiting;
    }

    float avgWaitingTime = (float)totalWaitingTime / n;
    printf("Average Waiting Time: %.2fs\n", avgWaitingTime);
}

void calculateATaT(int n, PCB P[])
{
    int totalTurnaroundTime = 0;
    
    for (int i = 0; i < n; i++)
    {
        totalTurnaroundTime += P[i].iTaT;
    }

    float avgTurnaroundTime = (float)totalTurnaroundTime / n;
    printf("Average Turnaround Time: %.2fs\n", avgTurnaroundTime);
}

int main()
{
    PCB Input[10];
    PCB ReadyQueue[10];
    PCB TerminatedArray[10];
    int iNumberOfProcess;
    printf("Please input number of Process: ");
    scanf("%d", &iNumberOfProcess);
    int iRemain = iNumberOfProcess, iReady = 0, iTerminated = 0;
    inputProcess(iNumberOfProcess, Input);
    quickSort(Input, 0, iNumberOfProcess - 1, SORT_BY_ARRIVAL);
    pushProcess(&iReady, ReadyQueue, Input[0]);
    removeProcess(&iRemain, 0, Input);
    ReadyQueue[0].iStart = ReadyQueue[0].iArrival;
    ReadyQueue[0].iFinish = ReadyQueue[0].iStart + ReadyQueue[0].iBurst;
    ReadyQueue[0].iResponse = ReadyQueue[0].iStart - ReadyQueue[0].iArrival;
    ReadyQueue[0].iWaiting = ReadyQueue[0].iResponse;
    ReadyQueue[0].iTaT = ReadyQueue[0].iFinish - ReadyQueue[0].iArrival;

    printf("\nReady Queue: ");
    printProcess(1, ReadyQueue);

    while (iTerminated < iNumberOfProcess)
    {
        while (iRemain > 0)
        {
            // Bug: This line will cause infinite while - loop if Input[0] <= ReadyQueue[0].iFinish
            if (Input[0].iArrival <= ReadyQueue[0].iFinish)
            {
                pushProcess(&iReady, ReadyQueue, Input[0]);
                removeProcess(&iRemain, 0, Input);
                continue;
            }
            else
                break;
        }
        // printf("Exited while!\n");
        // printf("iReady: %d", iReady);
        if (iReady > 0)
        {
            pushProcess(&iTerminated, TerminatedArray, ReadyQueue[0]);
            removeProcess(&iReady, 0, ReadyQueue);

            // Bug fix:
            if (iReady == 0)
            {
                // If ReadyQueue is empty, we move process from Input to ReadyQueue, regardless of the condition described above.
                if (iRemain > 0)
                {
                    pushProcess(&iReady, ReadyQueue, Input[0]);
                    removeProcess(&iRemain, 0, Input);
                }
                // If there aren't any remaining process in Input, we break out of while - loop
                else
                {
                    break;
                }
            }

            ReadyQueue[0].iStart =
                (TerminatedArray[iTerminated - 1].iFinish) < (ReadyQueue[0].iArrival)
                    ? ReadyQueue[0].iArrival
                    : TerminatedArray[iTerminated - 1].iFinish;
            ReadyQueue[0].iFinish = ReadyQueue[0].iStart + ReadyQueue[0].iBurst;
            ReadyQueue[0].iResponse = ReadyQueue[0].iStart - ReadyQueue[0].iArrival;
            ReadyQueue[0].iWaiting = ReadyQueue[0].iResponse;
            ReadyQueue[0].iTaT = ReadyQueue[0].iFinish - ReadyQueue[0].iArrival;
        }
    }
    
    printf("\n===== FCFS Scheduling =====\n");
    exportGanttChart(iTerminated, TerminatedArray);
    quickSort(TerminatedArray, 0, iTerminated - 1, SORT_BY_PID);
    printf("Analysis:\n");
    printStats(iTerminated, TerminatedArray);

    calculateAWT(iTerminated, TerminatedArray);
    calculateATaT(iTerminated, TerminatedArray);

    return 0;
}
