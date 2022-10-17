/**
 * 
 * Name: Richelin Metellus
 * OS Assignment 1
 * This Parent calculated the offset and the Child read the file and process it
 */
#include <stdio.h>
#include <stdlib.h>
#include<time.h>
#include<errno.h>
#include<unistd.h>
#include<string.h>

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char*argv[]) {

    /* check for number of argument*/
    if (argc < 4) {
        fprintf(stderr, "usage %s file_name_to_read.txt no_of_lines number_of process\n", argv[0]);
        exit(0);
    }
    char FILENAME[strlen(argv[1])];
    strcpy(FILENAME, argv[1]);
    int LINE_CNT = atoi(argv[2]);      // the number of int in the file
    printf("File: %s, no of int: %d ", FILENAME, LINE_CNT);
  
    FILE *file_ptr;
    int *data = malloc(sizeof (int)* LINE_CNT); 
    int* i = malloc(sizeof (int)); // index for different array
    struct timespec start, end; 
    int no_of_proc = atoi(argv[3]); 

    int* sub_array_size = malloc(sizeof (int)); // integer pointer that will act as the max number of times each process may read from its file.

    // various files descriptors used to send the offsets to the children and receive the children's calculated values.

    int fd1_OUT[2];
    int fd1_IN[2];
    int fd2_OUT[2];
    int fd2_IN[2];
    int fd3_OUT[2];
    int fd3_IN[2];
    int fd4_OUT[2];
    int fd4_IN[2];
    
    unsigned long int *child1offset = malloc(sizeof (unsigned long int));
    unsigned long int *child2offset = malloc(sizeof (unsigned long int));
    unsigned long int *child3offset = malloc(sizeof (unsigned long int));
    unsigned long int *child4offset = malloc(sizeof (unsigned long int));

    //decided to create multi pipe for easier management
    pipe(fd1_OUT);
    pipe(fd1_IN);
    pipe(fd2_OUT);
    pipe(fd2_IN);
    pipe(fd3_OUT);
    pipe(fd3_IN);
    pipe(fd4_OUT);
    pipe(fd4_IN);



   
    // long intergers pointer that will store the offset value of each of the process.
    unsigned long int* processValue1 = malloc(sizeof (unsigned long int));
    unsigned long int* processValue2 = malloc(sizeof (unsigned long int));
    unsigned long int* processValue3 = malloc(sizeof (unsigned long int));
    unsigned long int* processValue4 = malloc(sizeof (unsigned long int));

    //integers used to store the values from the test vile in each child process.
    int temp1 = 0;
    int temp2 = 0;
    int temp3 = 0;
    int temp4 = 0;

    // long integer pointer that will store the final calculated result

    unsigned long int* sum = malloc(sizeof (unsigned long int));

    // open the file specified above
    file_ptr = fopen(FILENAME, "r");

    if (file_ptr == NULL) {
        error("Unable to read the file. Exiting system now");
    }


    printf("\n Starting the main\n");
    /**
     *  R. this read from a file pointed by the file pointer file_ptr
     * @param file_ptr : the file pointer
     * @param "%d": the format the date in the file is in
     * @param &data[*i] : the memory address where we want to save that value read.
     * @return 
     */
    while (fscanf(file_ptr, "%d", &data[*i]) == 1)
        *i = *i + 1;

    printf("\nThe array has been filled");
    printf("\nThe size of the file :%ld bytes \n", ftell(file_ptr));
     
    int sec, nsec;
    printf("Clock Start time now");
    clock_gettime(CLOCK_REALTIME, &start);
    *sub_array_size = *i / no_of_proc; // now the sub_array_size value is  used for child process reading. Whatever i was

    // write to all the pipe depending on the entered user value
    printf("no of process Entered: %d", no_of_proc);
    if (no_of_proc == 1) {
        *child1offset = 0;                              // 1st child start at 0 byte
        printf("\nchild 1 offset byte: %ld\n", *child1offset);
        printf("Parent writing the offset to the pipe 1");
        write(fd1_OUT[1], child1offset, sizeof (child1offset));

    } 
    else if (no_of_proc == 2) {
        *child1offset = 0;
        printf("\nChild 1 offset byte: %ld\n", *child1offset);

        *child2offset = (sizeof (int) * (LINE_CNT / 2));
        printf("\nChild 2 offset byte: %ld\n", *child2offset);

        // The pipes are then written to with the offset values

        write(fd1_OUT[1], child1offset, sizeof (child1offset));
        write(fd2_OUT[1], child2offset, sizeof (child2offset));
    }
    else if (no_of_proc == 4) {
        *child1offset = 0;
        printf("\n child 1 offset byte: %ld\n", *child1offset);
        // offset value should be 1/4 of the bytes of the total bytes size of the file.
        *child2offset = (sizeof (int) *(LINE_CNT / 4));
        printf("child 2 offset byte: %ld\n", *child2offset);

        //The third offset value need to be 2/4 OR 2 * ofset2
        *child3offset = (*child2offset * 2);
        printf("child 3 offset byte: %ld\n", *child3offset);

        *child4offset = (*child2offset * 3);
        printf("process 4 offset byte: %ld\n", *child4offset);

        //the pipes are then written to with the offset values
        write(fd1_OUT[1], child1offset, sizeof (child1offset));
        write(fd2_OUT[1], child2offset, sizeof (child2offset));
        write(fd3_OUT[1], child3offset, sizeof (child3offset));
        write(fd4_OUT[1], child4offset, sizeof (child4offset));

    }
    fseek(file_ptr, 0, SEEK_SET); // set the file pointer back to the startning
    pid_t pids2[no_of_proc + 1];
    pid_t wpid;

    //set i back to 0
    *i = 0;
    printf("\nEntering Child Process Block\n");
    if (no_of_proc == 1) {
        //Child process block only. Parent don't enter
        if (pids2[0] = fork() == 0) {
            // read the offsetcalc1 value from the pipe read end.
            read(fd1_OUT[0], child1offset, sizeof (child1offset));
            //the file pointer will move to the begining of the file. R. child1offset is 0
            fseek(file_ptr, *child1offset, SEEK_SET);
            //free the memory. c -type garbage collection.
            free(child1offset);

            //if unable to open the file
            if (file_ptr == NULL) {
                error("File Empty or non-exiting. Exiting system now");
            }
            while (fscanf(file_ptr, "%d", &temp1) == 1) {
                if (*i >= *sub_array_size) {
                    // i is no longer need free
                    free(i);
                    free(sub_array_size);
                    break;
                }
                *processValue1 += temp1;
                //increment i each loop
                *i = *i + 1;
            }
            write(fd1_IN[1], processValue1, sizeof (processValue1));
            printf("Child 1 sent value: %ld\n,", processValue1);
            free(processValue1);
            printf("Child 1 exited\nChild 1 process Finish\n\n");
            exit(0);

        }
    }
    if (no_of_proc == 2) {
        //child process block only. Parent don't enter
        if (pids2[0] = fork() == 0) {
            FILE *file_ptr1 = fopen(FILENAME, "r");
            read(fd1_OUT[0], child1offset, sizeof (child1offset));

            //the file pointer will move to the startning of the file.
            fseek(file_ptr1, 0, SEEK_SET); // <---
            //offsetCalck1 value is no longer needed so free the memory
            free(child1offset);

            //if the file is empty
            if (file_ptr1 == NULL) {
                error("File Empty or non-exiting. Exiting system now");
            }
            while (fscanf(file_ptr1, "%d", &temp1) == 1) {
                if (*i >= *sub_array_size) {
                    // i is no longer need free
                    free(i);
                    free(sub_array_size);
                    break;
                }
                *processValue1 += temp1;
                //increment i each loop
                *i = *i + 1;
            }
            write(fd1_IN[1], processValue1, sizeof (processValue1));
            printf("Child 1 sent value: %ld\n,", processValue1);
            free(processValue1);
            printf("Child 1 exited\nChild 1 process Finish\n\n");
            fclose(file_ptr1);
            exit(0);

        }
        if (pids2[1] = fork() == 0) {
            FILE *file_ptr2;
            file_ptr2 = fopen(FILENAME, "r");
            read(fd2_OUT[0], child2offset, sizeof (child2offset));
            fseek(file_ptr2, *child2offset, SEEK_SET);
            free(child2offset);
            if (file_ptr2 == NULL) {
                error("File Empty or non-exiting. Exiting system now");
            }
            while (fscanf(file_ptr2, "%d", &temp2) == 1) {
                if (*i >= *sub_array_size) {
                    free(i);
                    free(sub_array_size);
                    break;
                }
                *processValue2 += temp2; // <---R to improve on
                //increment i each loop
                *i = *i + 1;
            }
            write(fd2_IN[1], processValue2, sizeof (processValue2));
            printf("Child 2 sent value: %ld\n,", processValue2);
            free(processValue2);
            printf("Child 2 exited\nChild 2 process Finish\n\n");
            fclose(file_ptr2);
            exit(0);

        }
    }
    if (no_of_proc == 4) {
        
        if (pids2[0] = fork() == 0) {
            FILE *file_ptr1;
            file_ptr1 = fopen(FILENAME, "r"); // take mem loc
            read(fd1_OUT[0], child1offset, sizeof (child1offset));
            fseek(file_ptr1, 0, SEEK_SET);
            free(child1offset);
            if (file_ptr1 == NULL) {
                error("File Empty or non-exiting. Exiting system now");
            }
            while (fscanf(file_ptr1, "%d", &temp1) == 1) {
                if (*i >= *sub_array_size) {
                    // i is no longer need free
                    free(i);
                    free(sub_array_size);
                    break;
                }
                *processValue1 += temp1;
                //increment i each loop
                *i = *i + 1;
            }
            write(fd1_IN[1], processValue1, sizeof (processValue1));
            printf("Child 1 sent value: %ld\n,", processValue1);
            free(processValue1);
            printf("Child 1 exited\nChild 1 process Finish\n\n");
            fclose(file_ptr1);
            exit(0);

        }
        if (pids2[1] = fork() == 0) {
            FILE *file_ptr2;
            file_ptr2 = fopen(FILENAME, "r");
            read(fd2_OUT[0], child2offset, sizeof (child2offset));
            fseek(file_ptr2, *child2offset, SEEK_SET);
            free(child2offset);
            if (file_ptr2 == NULL) {
                error("File Empty or non-exiting. Exiting system now");
            }
            while (fscanf(file_ptr2, "%d", &temp2) == 1) {
                if (*i >= *sub_array_size) {
                    free(i);
                    free(sub_array_size);
                    break;
                }
                *processValue2 += temp2;
                *i = *i + 1;
            }
            write(fd2_IN[1], processValue2, sizeof (processValue2));
            printf("Child 2 sent value: %ld\n,", processValue2);
            free(processValue2);
            printf("Child 2 exited\nChild 2 process Finish\n\n");
            fclose(file_ptr2);
            exit(0);

        }
        if (pids2[2] = fork() == 0) {
            FILE *file_ptr3;
            file_ptr3 = fopen(FILENAME, "r");
            read(fd3_OUT[0], child3offset, sizeof (child3offset));
            // fseek will make sure the file indicator is at the right byte
            fseek(file_ptr3, *child3offset, SEEK_SET);
            free(child3offset);
            if (file_ptr3 == NULL) {
                error("File Empty or non-exiting. Exiting system now");
            }
            while (fscanf(file_ptr3, "%d", &temp3) == 1) {
                if (*i >= *sub_array_size) {
                    free(i);
                    free(sub_array_size);
                    break;
                }
                *processValue3 += temp3;
                *i = *i + 1;
            }
            write(fd3_IN[1], processValue3, sizeof (processValue3));
            printf("Child 3 sent value: %ld\n,", processValue3);
            free(processValue3);
            printf("Child 3 exited\nChild 3 process Finish\n\n");
            fclose(file_ptr3);
            exit(0);

        }
        if (pids2[3] = fork() == 0) {
            FILE *file_ptr4;
            file_ptr4 = fopen(FILENAME, "r");
            read(fd4_OUT[0], child4offset, sizeof (child4offset));
            fseek(file_ptr4, *child4offset, SEEK_SET);
            free(child4offset);
            if (file_ptr4 == NULL) {
                error("File Empty or non-exiting. Exiting system now");
            }
            while (fscanf(file_ptr4, "%d", &temp4) == 1) {
                if (*i >= *sub_array_size) {
                    // i is no longer need free
                    free(i);
                    free(sub_array_size);
                    break;
                }
                *processValue4 += temp4;
                //increment i each loop
                *i = *i + 1;
            }
            // write the final process value to a  pipe that will send the value back to the parent
            write(fd4_IN[1], processValue4, sizeof (processValue4));
            printf("Child 4 sent value: %ld\n,", processValue4);
            free(processValue4);
            printf("Child 4 exited\nChild 3 process Finish\n\n");
            fclose(file_ptr4);
            exit(0);

        }
    }


    // -------------------------------Parents now read values and output final sum -------------------------------------
    if (no_of_proc == 1) {
        //the parent reads the value from the pipe
        read(fd1_IN[0], processValue1, sizeof (processValue1));
        *sum = *processValue1;
    }
    else if (no_of_proc == 2) {
        read(fd1_IN[0], processValue1, sizeof (processValue1));
        read(fd2_IN[0], processValue2, sizeof (processValue2));
        *sum = *processValue1 + *processValue2;
    } 
    else if (no_of_proc == 4) {
        read(fd1_IN[0], processValue1, sizeof (processValue1));
        read(fd2_IN[0], processValue2, sizeof (processValue2));
        read(fd3_IN[0], processValue3, sizeof (processValue3));
        read(fd4_IN[0], processValue4, sizeof (processValue4));
        *sum = *processValue1 + *processValue2 + *processValue3 + *processValue4;
    }


    free(processValue1);
    free(processValue2);
    free(processValue3);
    free(processValue4);

    printf("\nSummation of final Value: %ld\n\n", *sum);
    free(sum);
    
    clock_gettime(CLOCK_REALTIME, &end);
//    sec = end.tv_sec - start.tv_nsec;
//    nsec = end.tv_nsec - start.tv_nsec;
//    if ( nsec < 0){
//        nsec = nsec + 1000000000;
//    }
//    printf("This is the elapsed time: %10d.%09d\n\n", sec, nsec);
    if (end.tv_nsec < start.tv_nsec) {
        end.tv_nsec += 1000000000;
        end.tv_sec--;
    }
    clock_gettime(CLOCK_REALTIME, &end);
    printf("This is the elapsed time: %ld.%09ld\n\n", (end.tv_sec - start.tv_sec), (end.tv_nsec - start.tv_nsec));
    
    
    return (EXIT_SUCCESS);
}

