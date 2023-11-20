#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
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

void printDetails(int n, PCB P[]){
    printf("\nProcess Information:\n");
    printf("--------------------------------------------------------------------------------------------------\n");
    printf("| PID | Arrival Time | Burst Time | Start Time | Finish Time | Waiting Time | Response Time | Turnaround Time |\n");
    printf("--------------------------------------------------------------------------------------------------\n");
    
    for (int i = 0; i < n; ++i) {
        printf("| %3d | %12d | %10d | %10d | %11d | %12d | %13d | %15d |\n",
            P[i].iPID, P[i].iArrival, P[i].iBurst, P[i].iStart,  P[i].iFinish, P[i].iWaiting, P[i].iResponse, P[i].iTaT);
    }
    printf("--------------------------------------------------------------------------------------------------\n");
};


int hideStringIfTooLong(char *str, int maxLen)
{
    if (strlen(str) > maxLen)
    {
        strcpy(str, "!");
        return 1;
    }
    return 0;
}

void exportGanttChart(int n, PCB P[])
{
    if (n == 0)
    {
        return;
    }

    // Create a 6 x (n * cellWidth) matrix of chars that represents our Gantt chart
    char *grid[6];
    const int cellWidth = 9;
    const int col = (cellWidth * n) - (n - 1);
    int maxStringWidth = floor(cellWidth / 2);
    int currCell = 1;
    int isHidden = 0;
    char textToDisplay[20];

    for (int i = 0; i < 6; i++)
    {
        grid[i] = malloc(col);
    }

    // Insert starting/finishing time points of each processes into the Gantt chart
    memset(grid[0], ' ', col);
    sprintf(textToDisplay, "%d", P[0].iStart);
    isHidden = hideStringIfTooLong(textToDisplay, maxStringWidth);
    strncpy(grid[0], textToDisplay, strlen(textToDisplay));

    // endOfCell: the index of the terminating '|' character of each cell in the table
    for (int endOfCell = cellWidth - 1; currCell - 1 < n; currCell++, endOfCell += (cellWidth - 1))
    {
        sprintf(textToDisplay, "%d", P[currCell - 1].iFinish);
        isHidden = hideStringIfTooLong(textToDisplay, maxStringWidth);

        if (currCell < n)
        {
            if (P[currCell - 1].iFinish != P[currCell].iStart)
            {
                strncpy(grid[0] + (endOfCell - strlen(textToDisplay)), textToDisplay, strlen(textToDisplay));
                sprintf(textToDisplay, "%d", P[currCell].iStart);
                isHidden = hideStringIfTooLong(textToDisplay, maxStringWidth - 1);
                strncpy(grid[0] + endOfCell + 1, textToDisplay, strlen(textToDisplay));
                continue;
            }
        }
        strncpy(grid[0] + (endOfCell - strlen(textToDisplay) + 1), textToDisplay, strlen(textToDisplay));
    }

    // Draw borders for the chart
    currCell = 1;
    for (int i = 0; i < col; i++)
    {
        if (i == 0)
        {
            grid[1][i] = ' ';
            grid[2][i] = '|';
            grid[3][i] = '|';
            grid[4][i] = '|';
        }
        else if ((i + currCell) % cellWidth == 0)
        {
            grid[1][i] = ' ';
            grid[2][i] = '|';
            grid[3][i] = '|';
            grid[4][i] = '|';
            currCell++;
        }
        else
        {
            grid[1][i] = '_';
            grid[2][i] = ' ';
            grid[3][i] = '_';
            grid[4][i] = ' ';
        }
    }

    // Insert PID('P0, P1, ...') texts into each cell
    maxStringWidth = floor((cellWidth - 2) / 2);
    currCell = 1;
    for (int endOfCell = (cellWidth - 1); currCell - 1 < n; currCell++, endOfCell += (cellWidth - 1))
    {
        int currPid = P[currCell - 1].iPID;
        char tmp[10];
        strcpy(textToDisplay, (currPid > 0 && currPid < 10) ? "P0" : "P");
        sprintf(tmp, "%d", P[currCell - 1].iPID);
        strcat(textToDisplay, tmp);
        isHidden = hideStringIfTooLong(textToDisplay, maxStringWidth);

        int whitespace = floor((cellWidth - 2 - maxStringWidth) / 2);
        int j = 0;
        while (j < whitespace)
        {
            grid[2][endOfCell - 1 - j] = ' ';
            grid[2][endOfCell - (cellWidth - 2) + j] = ' ';
            j++;
        }

        int k = j;
        while (j - k < strlen(textToDisplay))
        {
            grid[2][endOfCell - (cellWidth - 2) + j] = textToDisplay[j - k];
            j++;
        }
    }

    // Print result and free memory
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < col; j++)
        {
            printf("%c", grid[i][j]);
        }
        printf("\n");
        free(grid[i]);
    }

    // Optional
    if (isHidden != 0)
    {
        printf("Some stats were hidden from the chart, analysis:\n");
        printDetails(n, P);
    }
}

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

    (*n)--;
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

    printf("Original: \n");
    printDetails(iRemain, Input);

    quickSort(Input, 0, iNumberOfProcess - 1, SORT_BY_ARRIVAL);
    printf("After sort by arrival: \n");
    printDetails(iRemain, Input);

    for (int start = 0, end; start < iNumberOfProcess - 1; start = end + 1) {
        end = start;
        while (Input[start].iArrival == Input[end + 1].iArrival) {
            end++;
        }
        quickSort(Input, start, end, SORT_BY_BURST);
    }

    printf("After sort by burst: \n");
    printDetails(iRemain, Input);

    pushProcess(&iReady, ReadyQueue, Input[0]);
    removeProcess(&iRemain, 0, Input);
    ReadyQueue[0].iStart = ReadyQueue[0].iArrival;
    ReadyQueue[0].iFinish = ReadyQueue[0].iStart + ReadyQueue[0].iBurst;
    ReadyQueue[0].iResponse = ReadyQueue[0].iStart - ReadyQueue[0].iArrival;
    ReadyQueue[0].iWaiting = ReadyQueue[0].iResponse;
    ReadyQueue[0].iTaT = ReadyQueue[0].iFinish - ReadyQueue[0].iArrival;

    printf("\nReady Queue: ");
    printDetails(1, ReadyQueue);
    
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

        if (iReady > 0)
        {
            pushProcess(&iTerminated, TerminatedArray, ReadyQueue[0]);
            removeProcess(&iReady, 0, ReadyQueue);

            // Bug fix:
            if (iReady == 0)
            {
                // If ReadyQueue is empty and there are still some processes left in Input, we move process from Input to ReadyQueue, regardlessly.
                if (iRemain > 0)
                {
                    pushProcess(&iReady, ReadyQueue, Input[0]);
                    removeProcess(&iRemain, 0, Input);
                }
                // If there aren't any remaining process in Input, we break out of while - loop (done)
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

    printf("\n===== SJF Scheduling =====\n");
    exportGanttChart(iTerminated, TerminatedArray);
    printf("\n");
    calculateAWT(iTerminated, TerminatedArray);
    calculateATaT(iTerminated, TerminatedArray);
    printDetails(iTerminated, TerminatedArray);

    return 0;
}
