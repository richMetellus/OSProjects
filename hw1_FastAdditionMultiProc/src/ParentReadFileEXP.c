#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include<errno.h>
#include<unistd.h>
#include <string.h>
#include <sys/wait.h>
#include<sys/mman.h>


static int *shWait1;
static int *shWait2;
static int *shWait3;
static int *shWait4;

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char* argv[]) 

   {/* check for number of argument*/
    if (argc < 4) {
        fprintf(stderr, "usage %s file_name_to_read.txt no_of_lines number_of process\n", argv[0]);
        exit(0);
    }
    char FILENAME[strlen(argv[1])];
    strcpy(FILENAME, argv[1]);
    int MAX = atoi(argv[2]);      // the number of int in the file
    int no_of_proc = atoi(argv[3]);
    printf("File: %s, no of int: %d ", FILENAME, MAX);
    
   
        shWait1 = mmap(NULL, sizeof *shWait1, PROT_READ | PROT_WRITE,
                MAP_SHARED | MAP_ANONYMOUS, -1, 0);
        shWait2 = mmap(NULL, sizeof *shWait2, PROT_READ | PROT_WRITE,
                MAP_SHARED | MAP_ANONYMOUS, -1, 0);
        shWait3 = mmap(NULL, sizeof *shWait3, PROT_READ | PROT_WRITE,
                MAP_SHARED | MAP_ANONYMOUS, -1, 0);
        shWait4 = mmap(NULL, sizeof *shWait4, PROT_READ | PROT_WRITE,
                MAP_SHARED | MAP_ANONYMOUS, -1, 0);
        
    *shWait1 = 1; // 1 meaning parent should wait
    *shWait2 = 1;
    *shWait3 = 1;
    *shWait4 = 1;
    
    
    
    struct timespec start, end;
    FILE *file_ptr;
    int *data = malloc(sizeof(int)* MAX); //  ptr to hold the number in the file.
    int *i = malloc(sizeof(int));
    int *sub_array_size = malloc(sizeof(int)); // boundary for where the child should stop reading
    
    //various file descriptor used to send data array to children
    int fd1_OUT[2];
    int fd1_IN[2];
    int fd2_OUT[2];
    int fd2_IN[2];
    int fd3_OUT[2];
    int fd3_IN[2];
    int fd4_OUT[2];
    int fd4_IN[2];
    
    pipe(fd1_OUT);
    pipe(fd1_IN);
    pipe(fd2_OUT);
    pipe(fd2_IN);
    pipe(fd3_OUT);
    pipe(fd3_IN);
    pipe(fd4_OUT);
    pipe(fd4_IN);
 
    //these varible will store the final calculated values of each of the process
    unsigned long int* proc1calculation = malloc(sizeof(unsigned long int));
    unsigned long int* proc2calculation= malloc(sizeof(unsigned long int));
    unsigned long int* proc3calculation = malloc(sizeof(unsigned long int));
    unsigned long int* proc4calculation = malloc(sizeof(unsigned long int));
    
    

   
    unsigned long int* sum = malloc(sizeof(unsigned long int));
    
    file_ptr = fopen(FILENAME,"r");
    if(file_ptr == NULL)
    {
        error("Unable to read the file. Exiting system now");
    }
    printf("Entering Main Process\n");
    while(fscanf(file_ptr, "%d", &data[*i])==1)
        *i = *i + 1;
    
    
    *sub_array_size = *i/no_of_proc; // 
   
    // initialize all all array to hold the necessary number, some may not be used
    int *sub_array1   = malloc(sizeof(int) * *sub_array_size);
    int *sub_array2 = malloc(sizeof(int) * *sub_array_size);
    int *sub_array3 = malloc(sizeof(int)* *sub_array_size);
    int *sub_array4  = malloc(sizeof(int) * *sub_array_size);
     
    
    printf("Clokck Start\n");
    clock_gettime(CLOCK_REALTIME, &start);
   if(no_of_proc == 1){
       //send the entire data array that parent read to child 1 to process
       write(fd1_OUT[1], &data, sizeof(data));
       
   }
   else if(no_of_proc==2)
   {
       //data element pointer
       int *j = malloc(sizeof(int));
       
       for(*j= 0; *j < *sub_array_size;(*j =*j +1))
       {
           sub_array1[*j] = data[*j];
           sub_array2[*j] = data[*j + *sub_array_size];// the uppermost elements of data is save here
       }
       
       //the pipe is then written to  with the smaller arrays
       write(fd1_OUT[1], &sub_array1, sizeof(sub_array1));
       write(fd2_OUT[1], &sub_array2, sizeof(sub_array2));
   }
    
   else if(no_of_proc == 4)
   {
       int *k = malloc(sizeof(int));
       for(*k = 0; *k<*sub_array_size; (*k = *k +1))
       {
           sub_array1[*k] = data[*k];
           sub_array2[*k] = data[*k + *sub_array_size];
           sub_array3[*k] = data[*k +(*sub_array_size * 2)];
           sub_array4[*k] = data[*k + (*sub_array_size * 3)];
           
       }
       
       write(fd1_OUT[1], &sub_array1, sizeof(sub_array1));
       write(fd2_OUT[1], &sub_array2, sizeof(sub_array2));
       write(fd3_OUT[1], &sub_array3, sizeof(sub_array3));
       write(fd4_OUT[1], &sub_array4, sizeof(sub_array4));
   }
    
    pid_t pids[no_of_proc+1]; // array to hold process id
       
    if(no_of_proc == 1)
    {
        // child process block; parent never enter this
        if(pids[0] = fork() == 0)
        {
            //the array of data that was written to the pipe is read by the child to process
            read(fd1_OUT[0], &data, sizeof(data));
            
            int *a = malloc(sizeof(int));
            
            *proc1calculation = 0;
            
            for(*a = 0; *a<*sub_array_size; (*a = *a +1))
            {
                *proc1calculation += data[*a];
            }
            write(fd1_IN[1], proc1calculation, sizeof(proc1calculation));
            printf("Child 1 sent value: %ld\n", *proc1calculation);
            free(proc1calculation);
            printf("Child 1 exited\n");
            printf("Child Process 1 finished\n\n");
            exit(0);
        }
        
        
    }
    
    else if(no_of_proc ==2)
    {
        if(pids[0] = fork() == 0)
        {
            //the array that was written to the pipe is read by the child
            read(fd1_OUT[0], &sub_array1, sizeof(sub_array1));
            
            int *a = malloc(sizeof(int));
            
            *proc1calculation = 0;
            
            for(*a = 0; *a<*sub_array_size; (*a = *a +1))
            {
                *proc1calculation += sub_array1[*a];
            }
            // write the process value to the new pipe that will send the value back to the parent
            write(fd1_IN[1], proc1calculation, sizeof(proc1calculation));
            printf("Child 1 sent value: %ld\n", *proc1calculation);
            free(proc1calculation);
            printf("Child 1 exited\n");
            printf("Child Process 1 finished\n\n");
            exit(0);
        }
        
        if(pids[1] == fork()== 0)
        {
            read(fd2_OUT[0], &sub_array2, sizeof(sub_array2));
            
            int *a = malloc(sizeof(int));
            
            *proc2calculation = 0;
            
            for(*a = 0; *a<*sub_array_size; (*a = *a +1))
            {
                *proc2calculation += sub_array2[*a];
            }
            // write the process value to the new pipe that will send the value back to the parent
            write(fd2_IN[1], proc2calculation, sizeof(proc2calculation));
            printf("Child 2 sent value: %ld\n", *proc1calculation);
            free(proc2calculation);
            printf("Child 2 exited\n");
            printf("Child Process 2 finished\n\n");
            exit(0);
            
        }
       
    }
    
    else if (no_of_proc == 4)
    {
        if(pids[0] = fork() == 0) //pids[0] can either be the chidPID that was return to parent or 0
        {
            //the array that was written to the pipe is read by the child
            read(fd1_OUT[0], &sub_array1, sizeof(sub_array1));
            
            int *a = malloc(sizeof(int));
            
            *proc1calculation = 0;
            
            for(*a = 0; *a<*sub_array_size; (*a = *a +1))
            {
                *proc1calculation += sub_array1[*a];
            }
            write(fd1_IN[1], proc1calculation, sizeof(proc1calculation));
            printf("Child 1 sent value: %ld\n", *proc1calculation);
            free(proc1calculation);
            printf("Child 1 exited\n");
            printf("Child Process 1 finished\n\n");
            *shWait1 = 0;
            printf("shWait1: %d\n", *shWait1);
            exit(0);
        }
         
        
        if(pids[1] = fork() == 0)
        {
            //the array that was written to the pipe is read by the child
            read(fd2_OUT[0], &sub_array2, sizeof(sub_array2));
            
            int *a = malloc(sizeof(int));
            
            *proc2calculation = 0;
            
            for(*a = 0; *a<*sub_array_size; (*a = *a +1))
            {
                *proc2calculation += sub_array2[*a];
            }
            write(fd2_IN[1], proc2calculation, sizeof(proc2calculation));
            printf("Child 2 sent value: %ld\n", *proc2calculation);
            free(proc2calculation);
            printf("Child 2 exited\n");
            printf("Child Process 2 finished\n\n");
            *shWait2 = 0;
            printf("child 2 change shWait2 %d\n", *shWait2);
            exit(0);
        }
        
        
        if(pids[2] = fork() == 0)
        {
            //the array that was written to the pipe is read by the child
            read(fd3_OUT[0], &sub_array3, sizeof(sub_array3));
            
            int *a = malloc(sizeof(int));
            
            *proc3calculation = 0;
            
            for(*a = 0; *a<*sub_array_size; (*a = *a +1))
            {
                *proc3calculation += sub_array3[*a];
            }
            write(fd3_IN[1], proc3calculation, sizeof(proc3calculation));
            printf("Child 3 sent value: %ld\n", *proc3calculation);
            free(proc3calculation);
            printf("Child 3 exited\n");
            printf("Child Process 3 finished\n");
            *shWait3 = 0;
            printf(" child 3 change shWait3: %d\n\n", *shWait3);
            exit(0);
        }
        
        
        //process 4
        if(pids[3] = fork() == 0)
        {
            //the array that was written to the pipe is read by the child
            read(fd4_OUT[0], &sub_array4, sizeof(sub_array4));
            
            int *a = malloc(sizeof(int));
            
            *proc4calculation = 0;
            
            for(*a = 0; *a<*sub_array_size; (*a = *a +1))
            {
                *proc4calculation += sub_array4[*a];
            }
            write(fd4_IN[1], proc4calculation, sizeof(proc4calculation));
            printf("Child 4 sent value: %ld\n", *proc4calculation);
            free(proc2calculation);
            printf("Child 4 exited\n");
            printf("Child Process 4 finished\n");
            *shWait4 = 0;
            printf("child 4 changed shWait4: %d\n\n", *shWait4);
            exit(0);
        }
        
       // printf("Parent should wait..\n Wait1: %d wait2:%d wait3:%d wait4:%d",*shWait1, *shWait2,*shWait3,*shWait4);
     
        
    }
    
    printf("\nback to Parent\n");
    // now Parenst read the value from pipe
    if(no_of_proc ==1)
    {
        printf("Reading for process 1\n");
        read(fd1_IN[0], proc1calculation, sizeof(proc1calculation));
        *sum = *proc1calculation;
    }
    else if(no_of_proc ==2)
    {
        printf("Reading for 2 processes \n");
        read(fd1_IN[0], proc1calculation, sizeof(proc1calculation));
        read(fd2_IN[0], proc2calculation, sizeof(proc2calculation));
        *sum = *proc1calculation + *proc2calculation;
    }
    else if(no_of_proc ==4)
    {
   
        printf("Reading for 4 processes \n");
        printf("Parent  waiting\n Wait1: %d wait2:%d wait3:%d wait4:%d",*shWait1, *shWait2,*shWait3,*shWait4);
         do // to deal with synchronization issues; waste some clock cycle
        {
            //printf("Parent waiting..\n Wait1: %d wait2:%d wait3:%d wait4:%d",*shWait1, *shWait2,*shWait3,*shWait4);
        }while((*shWait1 == 1) || (*shWait2 ==1)||  (*shWait4 ==1) || (*shWait3 ==1 ));
        printf("\nParent done waiting..\n Wait1: %d wait2:%d wait3:%d wait4:%d\n",*shWait1, *shWait2,*shWait3,*shWait4);
        
        
        printf("parent reading pipe\n");
        read(fd1_IN[0], proc1calculation, sizeof(proc1calculation));
        read(fd2_IN[0], proc2calculation, sizeof(proc2calculation));
        read(fd3_IN[0], proc3calculation, sizeof(proc3calculation));
        read(fd4_IN[0], proc4calculation, sizeof(proc4calculation));
        *sum = *proc1calculation + *proc2calculation + *proc3calculation + *proc4calculation;
        printf("Program Final value: %ld\n", *sum);
        
        munmap(shWait1, sizeof *shWait1);
        munmap(shWait2, sizeof *shWait2);
        munmap(shWait3, sizeof *shWait3);
        munmap(shWait4, sizeof *shWait4);
    }
    
    
    printf("Program Final value: %ld\n", *sum);
    
    free(sum);
    free(data);
    clock_gettime(CLOCK_REALTIME, &end);
     if (end.tv_nsec < start.tv_nsec) {
        end.tv_nsec += 1000000000;
        end.tv_sec--;
    }
    clock_gettime(CLOCK_REALTIME, &end);
    printf("This is the elapsed time(my way): %ld.%09ld\n", (end.tv_sec - start.tv_sec), (end.tv_nsec - start.tv_nsec));
    free(sub_array3);
    free(sub_array4);
    int sec, nsec;
    sec = end.tv_sec - start.tv_nsec;
    nsec = end.tv_nsec - start.tv_nsec;
    if ( nsec < 0){
        nsec = nsec + 1000000000;
    }
    printf("This is the elapsed time(teacher's way): %10d.%09d\n\n", sec, nsec);
    return (EXIT_SUCCESS);
}



