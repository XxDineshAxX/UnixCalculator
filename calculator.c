#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#define MAXLEN 1000

char operators[100];
int fds[100][2];
int fdsc = 0;
int linecount = 0;
int opCnt = 0;
char ch;

int child() {
	
        //rewrite the pipes for fds 0, 1 & 3 before invoking add/subtract/multiply/divide
        //setup FD 0 for reading 1st parameter
        close(0);
        dup(fds[(fdsc)][0]);
	
        //setup FD 3 for reading 2nd parameter
        close(3);
        dup(fds[(fdsc+1)][0]);
	
        //setup FD 1 for outputing the result
        close(1);
        dup(fds[(fdsc+2)][1]);
	
	if (operators[opCnt] == '-'){
        execl("./subtract", "subtract", NULL, NULL);
	
	}
	else if(operators[opCnt] == '+'){
	execl("./add", "add", NULL, NULL);
	
	}
	else if(operators[opCnt] == '/'){
        execl("./divide", "divide", NULL, NULL);
	
        }
	else if(operators[opCnt] == '*'){
        execl("./multiply", "multiply", NULL, NULL);
	
        }
        fputs("I hope you do not see me!", stderr);
        exit(1);
}

int innerChild() {

        //rewrite the pipes for fds 0, 1 & 3 before invoking add/subtract/multiply/divide
        //setup FD 0 for reading 1st parameter
        close(0);
        dup(fds[(fdsc-1)][0]);

        //setup FD 3 for reading 2nd parameter
        close(3);
        dup(fds[(fdsc)][0]);

        //setup FD 1 for outputing the result
        close(1);
        dup(fds[(fdsc+1)][1]);

        if (operators[opCnt] == '-'){
        execl("./subtract", "subtract", NULL, NULL);

        }
        else if(operators[opCnt] == '+'){
        execl("./add", "add", NULL, NULL);

        }
        else if(operators[opCnt] == '/'){
        execl("./divide", "divide", NULL, NULL);

        }
        else if(operators[opCnt] == '*'){
        execl("./multiply", "multiply", NULL, NULL);

        }
        fputs("I hope you do not see me!", stderr);
        exit(1);
}



int main(int argc, char *argv[]) {
	char line[MAXLEN], *temp;

	FILE *dataFile = fopen(argv[1], "r");
	
	while((ch=fgetc(dataFile))!=EOF){

		if (ch=='\n'){
			linecount++;
		}
	

	}
	linecount = linecount-1;
	fclose(dataFile);
	dataFile = fopen(argv[1], "r");
	//read the first line - it contains the configuration
	fgets(line, MAXLEN, dataFile); 

	// sample content for line: a + b - c
	strtok(line, " \n"); //skip the symbol representing variable/parameter
	int count=0;
	while (temp = strtok(NULL, " \n")) {
		operators[count] = temp[0];
		//printf("operator: %c\n", operators[count]);
		count++;
		strtok(NULL, " \n"); //skip the symbol representing variable/parameter
	}


	for(int i=0; i<((2*count)+1); i++){
                pipe(fds[i]);
        }

        for(int j=0; j<count; j++){
		if (j ==0){
                if (fork() == 0){
                        child();
                }
		}
		else {
		if (fork() == 0){
			innerChild();
		}
		}
		if (j == 0){
		fdsc = fdsc+3;
		}
		else {
		fdsc = fdsc+2;
		}
		opCnt++;
        }

	
	//setup the configuration with necessary # of children
	//continue to read the data from the file
	//you can use fscanf() to read the remaining data!
	//Here is some code to get started!
	int x = 0, y = 0, z = 0;
	for(int l=0; l < linecount; l++){
		
		for(int t=0; t<(count); t++){
			if (t == 0){
		
			fscanf(dataFile, "%d", &x);
               		fscanf(dataFile, "%d", &y);
			if (write(fds[0][1], &x, sizeof(int)) == 0)
                        exit(1);
        	        if (write(fds[1][1], &y, sizeof(int)) == 0)
                        exit(2);
	                

			}
			else if (t == (count-1)){
			fscanf(dataFile, "%d", &x);
			if (write(fds[((t+2)+(t-1))][1], &x, sizeof(int)) == 0)
                        exit(2);
                        if (read(fds[((t+2)+t)][0], &z, sizeof(int)) == 0)
                        exit(3);
			printf("%d\n",z);
			}
			else{
			fscanf(dataFile, "%d", &x);
			if (write(fds[((t+2)+(t-1))][1], &x, sizeof(int)) == 0)
                        exit(2);

			}

			
	}
		
	}
	for(int q = 0; q < ((2*count)+1);q++){
		close(fds[q][0]);
		close(fds[q][1]);
	}
 
}

