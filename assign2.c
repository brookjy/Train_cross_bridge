/*
 * assign2.c
 *
 * Name: JIN YI
 * Student Number: V00831659
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "train.h"

pthread_mutex_t bridgeLock;
pthread_mutex_t WaitList;
pthread_cond_t CondA;

A_list westQueue;
A_list eastQueue;

/* #define DEBUG	1 */

void ArriveBridge (TrainInfo *train);
void CrossBridge (TrainInfo *train);
void LeaveBridge (TrainInfo *train);


/* Remove a train from the list.  */
void removeList(A_list *direction)
{
    int i;
    
    for(i = 0; i < direction->size - 1; i++)
    {
        direction->head[i] = direction->head[i + 1];
    }
    direction->size = direction->size - 1;
    if(direction == &westQueue)
    {
        westQueue.Crossed = 0;
    }else{
        westQueue.Crossed = westQueue.Crossed + 1;
    }
}

void * Train ( void *arguments )
{
    TrainInfo	*train = (TrainInfo *)arguments;
    
    /* Sleep to simulate different arrival times */
    usleep (train->length*SLEEP_MULTIPLE);
    
    ArriveBridge (train);
    CrossBridge  (train);
    LeaveBridge  (train);
    
    free (train);
    return NULL;
}

void ArriveBridge ( TrainInfo *train )
{
    printf ("Train %2d arrives going %s\n", train->trainId,
            (train->direction == DIRECTION_WEST ? "West" : "East"));
    
    /* Initialize the list. */
    pthread_mutex_lock(&WaitList);
    if(train->direction == DIRECTION_WEST)
    {
        if(westQueue.size == 0)
        {
            westQueue.Crossed = 0;
        }
        westQueue.head[westQueue.size++] = train;
    }else{
        eastQueue.head[eastQueue.size++] = train;
    }
    pthread_mutex_unlock(&WaitList);
    
    /* Check the Bridge, */
    pthread_mutex_lock(&bridgeLock);
    
    /* Heading West*/
    if(train->direction == DIRECTION_WEST)
    {
        pthread_mutex_lock(&WaitList);
        
        /* Waiting for the turn.... */
        while(!((eastQueue.size == 0 || westQueue.Crossed == 2) 
        	&& (train == westQueue.head[0])))
        {
            pthread_mutex_unlock(&WaitList);
            pthread_cond_wait(&CondA, &bridgeLock);
            pthread_mutex_lock(&WaitList);
        }
        
        pthread_mutex_unlock(&WaitList);
        
    }
    else
    {
    	/* Heading East*/
        pthread_mutex_lock(&WaitList);
        
        /* Waiting for the turn... */
        while(!((westQueue.Crossed < 2 || westQueue.size == 0) 
        	&& (train == eastQueue.head[0])))
        {
            pthread_mutex_unlock(&WaitList);
            pthread_cond_wait(&CondA, &bridgeLock);
            pthread_mutex_lock(&WaitList);
        }
        pthread_mutex_unlock(&WaitList);
    }
}

void CrossBridge ( TrainInfo *train )
{
    printf ("Train %2d is ON the bridge (%s)\n", train->trainId,
            (train->direction == DIRECTION_WEST ? "West" : "East"));
    fflush(stdout);
    
    usleep (train->length*SLEEP_MULTIPLE);
    
    printf ("Train %2d is OFF the bridge(%s)\n", train->trainId,
            (train->direction == DIRECTION_WEST ? "West" : "East"));
    fflush(stdout);
}

void LeaveBridge ( TrainInfo *train )
{
    pthread_mutex_lock(&WaitList);
    /* Remove the trains from the list.*/
    if(train->direction == DIRECTION_WEST)
    {
        removeList(&westQueue);
    }else{
        removeList(&eastQueue);
    }
    /* Broadcast for all the trains and release the bridge.*/
    pthread_mutex_unlock(&WaitList);
    pthread_cond_broadcast(&CondA);
    pthread_mutex_unlock(&bridgeLock);
}

int main ( int argc, char *argv[] )
{
    int		trainCount = 0;
    char 		*filename = NULL;
    pthread_t	*tids;
    int		i;
    
    
    /* Parse the arguments */
    if ( argc < 2 )
    {
        printf ("Usage: part1 n {filename}\n\t\tn is number of trains\n");
        printf ("\t\tfilename is input file to use (optional)\n");
        exit(0);
    }
    
    if ( argc >= 2 )
    {
        trainCount = atoi(argv[1]);
        westQueue.head = malloc(sizeof(TrainInfo*)*trainCount);
        eastQueue.head = malloc(sizeof(TrainInfo*)*trainCount);
    }
    if ( argc == 3 )
    {
        filename = argv[2];
    }

    printf("");
    initTrain(filename);
    
    /*
     * Since the number of trains to simulate is specified on the command
     * line, we need to malloc space to store the thread ids of each train
     * thread.
     */
    tids = (pthread_t *) malloc(sizeof(pthread_t)*trainCount);
    
    /*
     * Create all the train threads pass them the information about
     * length and direction as a TrainInfo structure
     */
    
    for (i=0;i<trainCount;i++)
    {
        TrainInfo *info = createTrain();
        
        printf ("Train %2d headed %s length is %d\n", info->trainId,
                (info->direction == DIRECTION_WEST ? "West" : "East"),
                info->length );
        
        if ( pthread_create (&tids[i],0, Train, (void *)info) != 0 )
        {
            printf ("Failed creation of Train.\n");
            exit(0);
        }
    }
    
    /*
     * This code waits for all train threads to terminate
     */
    for (i=0;i<trainCount;i++)
    {
        pthread_join (tids[i], NULL);
    }
    pthread_mutex_destroy(&WaitList);
    pthread_mutex_destroy(&bridgeLock);
    pthread_cond_destroy(&CondA);
    
    free(tids);
    return 0;
}

