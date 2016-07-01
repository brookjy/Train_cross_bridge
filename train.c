/*
 * train.c
 */
 
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "train.h"
 
/* A global to assign IDs to our trains */ 
int idNumber = 0;

/* If this value is set to 1, trains lengths
 * etc will be generated randomly.
 * 
 * If it is set to 0, the lengths etc will be
 * input from a file.
 */
int doRandom = 0;

/* The file to input train data from */
FILE *inputFile;

char *files;

/* You can assume that no more than 80 characters
 * will be on any line in the input file
 */
#define MAXLINE		80

void	initTrain ( char *filename )
{
	doRandom = 0;
	FILE * fd;
	
	/* If no filename is specified, generate randomly */
	if ( !filename )
	{
		doRandom = 1;
		srandom(getpid());
	}
	else
	{
		files = filename;
		fd= fopen(filename,"r");
		if (fd == NULL){
			printf ("File input not implemented.\n");
		}
		
	}
}
 
/*
 * Allocate a new train structure with a new trainId, trainIds are
 * assigned consecutively, starting at 0
 *
 * Either randomly create the train structures or read them from a file
 *
 * This function malloc's space for the TrainInfo structure.  
 * The caller is responsible for freeing it.
 */
TrainInfo *createTrain ( void )
{
	TrainInfo *info = (TrainInfo *)malloc(sizeof(TrainInfo));

	/* I'm assigning the random values here in case
	 * there is a problem with the input file.  Then
	 * at least we know all the fields are initialized.
	 */	 
	
	info->trainId = idNumber;
	info->arrival = 0;
	info->direction = (random() % 2 + 1);
	info->length = (random() % MAX_LENGTH) + MIN_LENGTH;

	if (!doRandom)
	{
		/* Your code here to read a line of input
		 * from the input file 
		 */
		char line[MAXLINE];
		char *read;
	    //size_t len = 0;
	    //ssize_t read;
	    //char *read;
	    int loop;
	    inputFile= fopen(files, "r");
	    if (inputFile == NULL){
	    	printf("Failed to open file\n");
	        exit(EXIT_FAILURE);
	    }
	    loop = idNumber;
	    while (loop >=0){
	    	fgets(line, MAXLINE, inputFile);
	    	loop--;
	    }
	    
	    if (line[0] == 'W'){
	    	info->direction = DIRECTION_WEST;
	    }
	    else if (line[0] == 'E'){
	    	info->direction = DIRECTION_EAST;
	    }else{
	    	info->direction = DIRECTION_NONE;
	    }
	    read = line;
	    read++;

		info->trainId = idNumber;
		info->arrival = 0;
		info->length = atoi(read);

	    fclose(inputFile);
	    //free(line);
	    


		idNumber++;
	}
	return info;

}
