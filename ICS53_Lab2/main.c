
// Sargis Dudaklyan

#include "csapp.h"
#include "vector.h"

//#include "DoubleLinkedList.h"

/* $begin shellmain */

#define MAXARGS   128

/* function prototypes */
void eval(char *cmdline);
int parseline(char *buf, char **argv);
int builtin_command(char **argv);

vector v;

int main()
{
    char cmdline[MAXLINE]; /* Command line */
    
    //Initialize the heap
    mem_init();
    //Initialize vector
    vector_init(&v);

    while (1) {
        /* Read */
        printf("> ");
        Fgets(cmdline, MAXLINE, stdin);
        if (feof(stdin))
            exit(0);
        
        /* Evaluate */
        eval(cmdline);
    } 
}
/* $end shellmain */


void handler2(int sig){
    //Puts th
    pid_t pid;
    //int status;
    
    while ((pid = wait(NULL)) > 0){}
    //it waits until a child (any child) of the process terminates, not all of them.
    
    /*
     The waitpid function is complicated. By default (when options = 0), waitpid suspends execution of the calling process until a child process in its wait set terminates. If a process in the wait set has already terminated at the time of the call, then waitpid returns immediately. In either case, waitpid returns the PID of the terminated child that caused waitpid to return, and the terminated child is removed from the system.
     */
    
    /*If the calling process has no children, then waitpid returns −1 and sets errno to ECHILD */
    if (errno != ECHILD)
        //checks if there are child processes left and gives error if there is
        unix_error("wait error");
    return;
}

void stdOut(char *fileName){
    // Creates a file pointer to write into
    FILE *fp;
    fp = fopen(fileName,"w+");
    dup2(fileno(fp),STDOUT_FILENO);
    fclose(fp);
}

void stdIn(char *fileName){
    // Creates a file pointer to read from
    FILE *fp;
    fp = fopen(fileName, "r");
    dup2(fileno(fp),STDIN_FILENO);
    fclose(fp);
}

/* $begin eval */
/* eval - Evaluate a command line */
void eval(char *cmdline){
    //Input array
    char *argv[MAXARGS]; /* Argument list execve() */
    //Modified input command splitted
    char buf[MAXLINE];   /* Holds modified command line */
    int bg;              /* Should the job run in bg or fg? */
    pid_t pid;           /* Process id */
    
    strcpy(buf, cmdline);
    bg = parseline(buf, argv); // Populates argv with everything in the buff and returns 1 if runs in the background(with &)
    if (argv[0] == NULL)
        return;   /* Ignore empty lines */
    
    if (!builtin_command(argv)) {
        //If not builtin command, not quit or &, create a child
        if ((pid = Fork()) == 0) {   /* Child runs user job */
            //If Fork returns 0, it runs in the background
            int i;
            for (i=0; i<MAXARGS; i++){
                //looks for < > in the argument vector
                if (argv[i]==NULL){
                    break;
                }
                else if (!strcmp(argv[i], ">")){
                    stdOut(argv[i+1]);
                }
                else if (!strcmp(argv[i], "<")){
                    stdIn(argv[i+1]);
                }
            }
            
            if (execv(argv[0], argv) < 0) {
                printf("%s: Command not found.\n", argv[0]);
                exit(0);
            }
        }
        
        /* Parent waits for foreground job to terminate */
        if (!bg) { //if foreground

            //int status;
            wait(NULL);
            
            //it waits until a child (any child) of the process terminates, not all of them.
            // Using wait instead of waitpid
            // waitpid(-1,NULL,0) == wait(NULL)
            // waitpid(pid, *status, options) if pid is neg, entire group. status returns child killed
            //CHECK THIS
        }
        else{ // if in background
            
           // printf("Process executed in background: %d %s", pid, cmdline);
            if (signal(SIGCHLD, handler2) == SIG_ERR) //When a child process terminates or stops, the kernel sends a SIGCHLD signal to the parent.
                //Checks if signal returns an error
                //signal returns a status of a child and the parents reaps it depending on the stauts number
                unix_error("signal error");

        }
    }
    
    return;
}




/* This function allows the user to allocate a block of memory from
 your heap. This function should take one argument, the number of bytes
 which the user wants in the allocated block. This function should call
 mm_malloc. This function should print out a unique block number which is
 associated with the block of memory which has just been allocated. The block
 numbers should increment each time a new block is allocated. So the first
 allocated block should be block number 1, the second is block number 2, etc.
 Notice that only the allocated blocks receive block numbers
*/
void allocateMem(char numBytes){
    
    int numBytesInt = numBytes - '0';
    vector_add(&v, mm_malloc(numBytesInt));
    printf("%i\n",vector_total(&v));
}


/* This function allows the user to free a block of memory. This function
 takes one argument, the block number associated with the previously
 allocated block of memory. This function must call mm_free. When a block is freed its block number is no longer valid. The block number should
 not be reused to number any newly allocated block in the future. */
void freeMem(char blockNum){
    
    mm_free(vector_get(&v,blockNum));
    //vector_delete(&v, blockNum);
    //vector_set(&v, blockNum, NULL);
}


/* This command prints out information about all of the blocks in your
 heap. It takes no arguments. The following information should be printed
 about each block:
 • Size
 • Allocated (yes or no)
 • Start address
 • End address
 Addresses should be printed in hexadecimal. The blocks should be printed in the
 order in which they are found in the heap. */
void blocklist(){
    
    if (vector_total(&v)>0){
        printf("Size Allocated Start          End");
        int i;
        for (i=0; i < v.total; i++){
            //char *isAllocated =(vector_get(&v, i) != NULL) ? "yes" : "no";
            //printf("%i%-5s%-9s%-10s",vector_get);
            //printblock(vector_get(&v, i));
        }
    }

}



/* This function writes characters into a block in the heap. The
 function takes three arguments: the block number to write to, the character to
 write into the block, and the number of copies of the character to write into the
 block. The specified character will be written into n successive locations of
 the block, where n is the third argument. This function should not overwrite
 the header of the block which it is writing to, only the payload. */
void writeheap(char blockNum, char writeChar, char numCopies){
    
    
}


/* This prints out the contents of a portion of the heap. This function
 takes two arguments: the number of the block to be printed, and the number
 of bytes to print after the start of the block. This function should not print the
 header of the chosen block, only the payload. If the number of bytes to print
 is larger than the size of the block, this function should print the bytes anyway,
 even though they might extend into a neighboring block. */
void printheap(char numBlock, char numBytes){
    

}

/* This function tells the allocator to use the bestfit allocation algorithm
 from now on. This function takes no arguments. */
void bestfit(){
    
    
}


/*This function tells the allocator to use the firstfit allocation algorithm
 from now on. This function takes no arguments. */
void firstfit(){
    
    
}



/* If first arg is a builtin command, run it and return true */
int builtin_command(char **argv)
{

    
    
    
    if (!strcmp(argv[0], "&"))    /* Ignore singleton & */
        return 1;
    if (!strcmp(argv[0], "quit")) /* quit command */
        exit(0);
    else if (!strcmp(argv[0], "allocate")){
        allocateMem(*argv[1]);
        return 1;
    }
    else if (!strcmp(argv[0], "free")){
        freeMem(*argv[1]);
        return 1;
    }
    else if (!strcmp(argv[0], "blocklist")){
        blocklist();
        return 1;
    }
    else if (!strcmp(argv[0], "writeheap")){
        writeheap(*argv[1], *argv[2], *argv[3]);
        return 1;
    }
    else if (!strcmp(argv[0], "printheap")){
        printheap(*argv[1], *argv[2]);
        return 1;
    }
    else if (!strcmp(argv[0], "bestfit")){
        bestfit();
        return 1;
    }
    else if (!strcmp(argv[0], "firstfit")){
        firstfit();
        return 1;
    }
    
    return 0;                     /* Not a builtin command */
}
/* $end eval */

/* $begin parseline */
/* parseline - Parse the command line and build the argv array */
int parseline(char *buf, char **argv)
{
    char *delim;         /* Points to first space delimiter */
    int argc;            /* Number of args */
    int bg;              /* Background job? */
    
    buf[strlen(buf)-1] = ' ';  /* Replace trailing '\n' with space */
    while (*buf && (*buf == ' ')) /* Ignore leading spaces */
        buf++;
    
    /* Build the argv list */
    argc = 0;
    while ((delim = strchr(buf, ' '))) {
        argv[argc++] = buf;
        
        *delim = '\0';
        buf = delim + 1;
        while (*buf && (*buf == ' ')) /* Ignore spaces */
            buf++;
    }
    argv[argc] = NULL;
    
    if (argc == 0)  /* Ignore blank line */
        return 1;
    
    /* Should the job run in the background? */
    if ((bg = (*argv[argc-1] == '&')) != 0)
        argv[--argc] = NULL;

    return bg;
}
/* $end parseline */

