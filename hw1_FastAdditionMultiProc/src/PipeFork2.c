#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include<errno.h>
#include<unistd.h>
#define MAXVALUES 1000000

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char** argv) {
    struct timespec begin, end;
    FILE *fp;
    static const char FILENAME[] = "large.txt";
    
    int * data = malloc(sizeof(int)*MAXVALUES); // int ptr tht will act as an array to hold every number in the text file
    
    int *i = malloc(sizeof(int));
    int status = 0;
    int userInput;
    int *arraySplit = malloc(sizeof(int)); // int pointer that will act as the max number of times each process may read from its file
    
    //various file descriptor used to send the offsets to the children and received the children calculated value
    int fd1_SEND[2];
    int fd1_RECEIVE[2];
    int fd2_SEND[2];
    int fd2_RECEIVE[2];
    int fd3_SEND[2];
    int fd3_RECEIVE[2];
    int fd4_SEND[2];
    int fd4_RECEIVE[2];
    
    pipe(fd1_SEND);
    pipe(fd1_RECEIVE);
    pipe(fd2_SEND);
    pipe(fd2_RECEIVE);
    pipe(fd3_SEND);
    pipe(fd3_RECEIVE);
    pipe(fd4_SEND);
    pipe(fd4_RECEIVE);
    
    
    int arrayHold  = 0;
    //these varible will store the final calculated values of each of the process
    unsigned long int* processValue1 = malloc(sizeof(unsigned long int));
    unsigned long int* processValue2= malloc(sizeof(unsigned long int));
    unsigned long int* processValue3 = malloc(sizeof(unsigned long int));
    unsigned long int* processValue4 = malloc(sizeof(unsigned long int));
    
    unsigned long int* combinedFinalValue = malloc(sizeof(unsigned long int));
    
    fp = fopen(FILENAME,"r");
    if(fp == NULL)
    {
        error("Unable to read the file. Exiting system now");
    }
    printf("Program is working still");
    while(fscanf(fp, "%d", &data[*i]==1))
        *i = *i + 1;
    printf("\nArray has been filled!\n");
    printf("This is the size of the file: %ld",ftell(fp));
     do{
        printf("how many process would you like to create? (1, 2, or 4)\n\n ");
        scanf("%d", &userInput);
    }while(!((userInput ==1)||(userInput ==2)||(userInput ==4)));
    clock_gettime(CLOCK_REALTIME, &begin);
    *arraySplit = *i/userInput; // the arraySplit is finally calculated and will be used later for child process file reading
   
    // initialize all all array to hold the necessary number, some may not be used
    int *splitArray1   = malloc(sizeof(int) * *arraySplit);
    int *splitArray2 = malloc(sizeof(int) * *arraySplit);
    int *splitArray3 = malloc(sizeof(int)* *arraySplit);
    int *splitArray4   = malloc(sizeof(int) * *arraySplit);
    int *splitArray5 = malloc(sizeof(int) * *arraySplit);
    int *splitArray6 = malloc(sizeof(int)* *arraySplit);
    
    printf("\nProgram is working ...\n");
    
   if(userInput == 1)
       //send the entire data array to child 1 to process
       write(fd1_SEND[1], &data, sizeof(data));
    
   else if(userInput==2)
   {
       //allocate a local variable to be used in a loop
       int *z = malloc(sizeof(int));
       // fills each smaller array with the correct values to be sent to the 2 children process
       for(*z= 0; *z< *arraySplit;(*z =*z +1))
       {
           splitArray1[*z] = data[*z];
           splitArray2[*z] = data[*z + *arraySplit];
       }
       
       //the pipe is then written to  with the smaller arrays
       write(fd1_SEND[1], &splitArray1, sizeof(splitArray1));
       write(fd2_SEND[1], &splitArray2, sizeof(splitArray2));
   }
    
   else if(userInput == 4)
   {
       int *y = malloc(sizeof(int));
       for(*y = 0; *y<*arraySplit; (*y = *y +1))
       {
           splitArray3[*y] = data[*y];
           splitArray4[*y] = data[*y + *arraySplit];
           splitArray5[*y] = data[*y +(*arraySplit * 2)];
           splitArray6[*y] = data[*y + (*arraySplit * 3)];
           
       }
       
       write(fd1_SEND[1], &splitArray3, sizeof(splitArray3));
       write(fd2_SEND[1], &splitArray4, sizeof(splitArray4));
       write(fd3_SEND[1], &splitArray5, sizeof(splitArray5));
       write(fd4_SEND[1], &splitArray6, sizeof(splitArray6));
   }
    
    pid_t pids[userInput+1];
    pid_t wpid;
    
    if(userInput == 1)
    {
        // child process block; parent never enter this
        if(pids[0] = fork() == 0)
        {
            //the array that was written to the pipe is read by the child
            read(fd1_SEND[0], &data, sizeof(data));
            
            int *a = malloc(sizeof(int));
            
            *processValue1 = 0;
            
            for(*a = 0; *a<*arraySplit; (*a = *a +1))
            {
                *processValue1 += data[*a];
            }
            write(fd1_RECEIVE[1], processValue1, sizeof(processValue1));
            printf("Child 1 sent value: %ld\n", *processValue1);
            free(processValue1);
            printf("Child 1 exited\n");
            printf("Child Process 1 finished\n\n");
            exit(0);
        }
    }
    
    if(userInput ==2)
    {
        if(pids[0] = fork() == 0)
        {
            //the array that was written to the pipe is read by the child
            read(fd1_SEND[0], &splitArray1, sizeof(splitArray1));
            
            int *a = malloc(sizeof(int));
            
            *processValue1 = 0;
            
            for(*a = 0; *a<*arraySplit; (*a = *a +1))
            {
                *processValue1 += splitArray1[*a];
            }
            // write the process value to the new pipe that will send the value back to the parent
            write(fd1_RECEIVE[1], processValue1, sizeof(processValue1));
            printf("Child 1 sent value: %ld\n", *processValue1);
            free(processValue1);
            printf("Child 1 exited\n");
            printf("Child Process 1 finished\n\n");
            exit(0);
        }
        
        if(pids[1] == fork()== 0)
        {
            read(fd2_SEND[0], &splitArray2, sizeof(splitArray2));
            
            int *a = malloc(sizeof(int));
            
            *processValue2 = 0;
            
            for(*a = 0; *a<*arraySplit; (*a = *a +1))
            {
                *processValue2 += splitArray2[*a];
            }
            // write the process value to the new pipe that will send the value back to the parent
            write(fd2_RECEIVE[1], processValue2, sizeof(processValue2));
            printf("Child 2 sent value: %ld\n", *processValue1);
            free(processValue2);
            printf("Child 2 exited\n");
            printf("Child Process 2 finished\n\n");
            exit(0);
            
        }
    }
    
    if(userInput == 4)
    {
        if(pids[0] = fork() == 0)
        {
            //the array that was written to the pipe is read by the child
            read(fd1_SEND[0], &splitArray3, sizeof(splitArray3));
            
            int *a = malloc(sizeof(int));
            
            *processValue1 = 0;
            
            for(*a = 0; *a<*arraySplit; (*a = *a +1))
            {
                *processValue1 += splitArray3[*a];
            }
            write(fd1_RECEIVE[1], processValue1, sizeof(processValue1));
            printf("Child 1 sent value: %ld\n", *processValue1);
            free(processValue1);
            printf("Child 1 exited\n");
            printf("Child Process 1 finished\n\n");
            exit(0);
        }
        
        if(pids[0] = fork() == 0)
        {
            //the array that was written to the pipe is read by the child
            read(fd2_SEND[0], &splitArray4, sizeof(splitArray4));
            
            int *a = malloc(sizeof(int));
            
            *processValue2 = 0;
            
            for(*a = 0; *a<*arraySplit; (*a = *a +1))
            {
                *processValue2 += splitArray4[*a];
            }
            write(fd2_RECEIVE[1], processValue2, sizeof(processValue2));
            printf("Child 2 sent value: %ld\n", *processValue2);
            free(processValue2);
            printf("Child 2 exited\n");
            printf("Child Process 2 finished\n\n");
            exit(0);
        }
        
        if(pids[0] = fork() == 0)
        {
            //the array that was written to the pipe is read by the child
            read(fd3_SEND[0], &splitArray5, sizeof(splitArray5));
            
            int *a = malloc(sizeof(int));
            
            *processValue3 = 0;
            
            for(*a = 0; *a<*arraySplit; (*a = *a +1))
            {
                *processValue3 += splitArray5[*a];
            }
            write(fd3_RECEIVE[1], processValue3, sizeof(processValue3));
            printf("Child 3 sent value: %ld\n", *processValue3);
            free(processValue3);
            printf("Child 3 exited\n");
            printf("Child Process 3 finished\n\n");
            exit(0);
        }
        
        //process 4
        if(pids[0] = fork() == 0)
        {
            //the array that was written to the pipe is read by the child
            read(fd4_SEND[0], &splitArray6, sizeof(splitArray6));
            
            int *a = malloc(sizeof(int));
            
            *processValue4 = 0;
            
            for(*a = 0; *a<*arraySplit; (*a = *a +1))
            {
                *processValue4 += splitArray6[*a];
            }
            write(fd2_RECEIVE[1], processValue4, sizeof(processValue4));
            printf("Child 4 sent value: %ld\n", *processValue4);
            free(processValue2);
            printf("Child 4 exited\n");
            printf("Child Process 4 finished\n\n");
            exit(0);
        }
    }
    
    if(userInput ==1)
    {
        read(fd1_RECEIVE[0], processValue1, sizeof(processValue1));
    }
    else if(userInput ==2)
    {
        read(fd1_RECEIVE[0], processValue1, sizeof(processValue1));
        read(fd2_RECEIVE[0], processValue2, sizeof(processValue2));
    }
    else if(userInput ==4)
    {
        read(fd1_RECEIVE[0], processValue1, sizeof(processValue1));
        read(fd2_RECEIVE[0], processValue2, sizeof(processValue2));
        read(fd3_RECEIVE[0], processValue3, sizeof(processValue3));
        read(fd4_RECEIVE[0], processValue4, sizeof(processValue4));
    }
    
    if (userInput == 1)
        *combinedFinalValue = *processValue1;
    else if(userInput == 2)
    {
        *combinedFinalValue = *processValue1 + *processValue2;
    }
    else if(userInput == 4)
    {
        *combinedFinalValue = *processValue1 + *processValue2 + *processValue3 + *processValue4;
    }
    
    printf("Program Final value: %ld", *combinedFinalValue);
    
    free(combinedFinalValue);
    free(data);
    clock_gettime(CLOCK_REALTIME, &end);
    printf("\n Total Elapses in sec: %ld" ,(long)(end.tv_sec - begin.tv_sec));
    return (EXIT_SUCCESS);
}

