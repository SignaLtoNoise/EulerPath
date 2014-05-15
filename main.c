//
//  main.c
//  Problem2: Euler Path
//
//  Created by Michael Wilcox on 4/14/13.
//  Copyright (c) 2013 Michael Wilcox. All rights reserved.
//

#include <sys/time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

typedef struct  {
    int start;
    int end;
} edge;

typedef struct {
    edge *collection;
    int count;  //how many edges have been used in path
    int total;  //how many edges exist on node
} nodeEdge;

struct path{
    edge current;
    struct path *next;
};
typedef struct path path;

// prototype
int timeval_subtract(struct timeval *result, struct timeval *t2, struct timeval *t1);
void timeval_print(struct timeval *tv);


int main(int argc, const char * argv[])
{
    // get size
    int nodeNum;
    printf("Enter number of nodes: ");
    scanf("%d", &nodeNum);

    // begin timestamp
    struct timeval timeBegin, timeEnd, timeDiff;
    gettimeofday(&timeBegin, NULL);
    timeval_print(&timeBegin);
    
    // create graph
    bool **adjacency;
    adjacency = malloc(nodeNum*sizeof(bool*));
    for(int i=0; i<nodeNum;i++)
        adjacency[i] = (bool*)malloc(nodeNum*sizeof(bool));
    
    srand(time(NULL));      // for random number
    for(int i=0; i<nodeNum; i++)
        for(int j=0; j<nodeNum; j++) {
            if (i != j) {
                adjacency[i][j] = adjacency [j][i] = true;          //1=true, 0=false since
            }                                                        //edge length irrelevant
            else
                adjacency[i][j] = false;                            //no edge to itself
        }

    // Create nodeConnection structs
    nodeEdge *nodeConnections = malloc(nodeNum*sizeof(nodeEdge));
    for (int i=0; i<nodeNum; i++) {
        nodeConnections[i].count = 0;
        nodeConnections[i].total = 0;
    }
    
    // Assume that Euler Path exists
    // If a node does not have even edges, then no path, break
    
    int nodeEdgeCount;
    int graphEdgeCount = 0;
    int eulerTrigger = 0;
    for (int i=0; i<nodeNum; i++) {
        nodeEdgeCount = 0;
        for (int j=0; j<nodeNum; j++)
            if (adjacency[i][j]) {
                nodeEdgeCount++;
                graphEdgeCount++;
                nodeConnections[i].total++;
            }
        if (nodeEdgeCount % 2 != 0 || nodeEdgeCount == 0) {     //every node isn't connected, or an odd edge count
            // no euler path, break
            printf("Node %d has %d edges, no possible euler path\n", i, nodeEdgeCount);
            eulerTrigger++;
            
            // end timestamp
            gettimeofday(&timeEnd, NULL);
            timeval_print(&timeEnd);
            
            // print difference
            timeval_subtract(&timeDiff, &timeEnd, &timeBegin);
            printf("%ld.%06d\n", timeDiff.tv_sec, timeDiff.tv_usec);

            return -1;
        }
    }
    printf("Total edges is %d\n", graphEdgeCount);

    if (eulerTrigger == 0) {
        printf("Euler Path detected\n");
        
        // fill nodeConnections with edges
        for (int i=0; i<nodeNum; i++) {
            nodeConnections[i].collection = malloc(nodeConnections[i].total*sizeof(edge));
        }
        for (int i=0; i<nodeNum; i++) {
            int edgeTrack = 0;
            for (int j=0; j<nodeNum; j++) {
                if (adjacency[i][j]) {
                    nodeConnections[i].collection[edgeTrack].start = i;
                    nodeConnections[i].collection[edgeTrack].end = j;
                    edgeTrack++;
                }
            }
        }
        
        int endNode,startNode;
        
        //arbitrarily begin Euler path at node 0
        path *root;
        root = malloc(sizeof(path));
        root->current.start = 0;
        for (int i=0; i<nodeConnections[0].total; i++) {
            if (nodeConnections[0].collection[i].end >= 0) {
                root->current.end = nodeConnections[0].collection[i].end;
                nodeConnections[0].count++;
                startNode = root->current.start;
                endNode = root->current.end;
                nodeConnections[0].collection[i].end = -1;
                for (int j=0; j<nodeConnections[endNode].total; j++) {
                    if (nodeConnections[endNode].collection[j].end == startNode) {
                        nodeConnections[endNode].collection[j].end = -1;
                        nodeConnections[endNode].count++;
                        break;
                    }
                }
                break;
            }
        }
         
        int edgeTrack = 1;
        
        //continue path until return to node 0
        //if path is used, set .end to -1
        path *conductor = root;
        while (conductor->current.end != 0) {
            for (int i=0; i<nodeConnections[conductor->current.end].total; i++) {
                if (nodeConnections[conductor->current.end].collection[i].end >= 0) {
                    path *temp = malloc(sizeof(path));
                    temp->current.start = conductor->current.end;
                    temp->current.end = nodeConnections[conductor->current.end].collection[i].end;
                    startNode=temp->current.start;
                    endNode=temp->current.end;
                    nodeConnections[startNode].count++;
                    nodeConnections[startNode].collection[i].end = -1;
                    for (int j=0; j<nodeConnections[endNode].total; j++) {
                        if (nodeConnections[endNode].collection[j].end == startNode) {
                            nodeConnections[endNode].collection[j].end = -1;
                            nodeConnections[endNode].count++;
                            break;
                        }
                    }
                    
                    conductor->next = temp;
                    conductor = temp;
                    
                    edgeTrack++;
                    break;
                }
            }
        }
        path *end = malloc(sizeof(path));
        end->current.start = 0;
        end->current.end = -1;
        conductor->next = end;
        
        while (edgeTrack < graphEdgeCount/2) {
            for (int newStart=0; newStart<nodeNum; newStart++) {
                if (nodeConnections[newStart].count != nodeConnections[newStart].total) {
                    conductor = root;

                    while (conductor->current.end != newStart) {
                        conductor = conductor->next;
                    }
                
                    //store continued path in temp variable
                    path *tempHolder = malloc(sizeof(path));
                    tempHolder = conductor->next;
                    
                    path *newRoot;
                    newRoot = malloc(sizeof(path));
                    newRoot->current.start = conductor->current.end;
                    for (int i=0; i<nodeConnections[conductor->current.end].total; i++) {
                        if (nodeConnections[conductor->current.end].collection[i].end >= 0) {
                            newRoot->current.end = nodeConnections[conductor->current.end].collection[i].end;
                            nodeConnections[conductor->current.end].count++;
                            nodeConnections[conductor->current.end].collection[i].end = -1;
                            for (int j=0; j<nodeConnections[newRoot->current.end].total; j++) {
                                if (nodeConnections[newRoot->current.end].collection[j].end == conductor->current.end) {
                                    nodeConnections[newRoot->current.end].collection[j].end = -1;
                                    nodeConnections[newRoot->current.end].count++;
                                    break;
                                }
                            }
                            break;
                        }
                    }
                    edgeTrack++;
                    
                    //continue path until return to node 0
                    //if path is used, set .end to -1
                    conductor->next = newRoot;
                    conductor = conductor->next;

                    while (conductor->current.end != newStart) {
                        for (int i=0; i<nodeConnections[conductor->current.end].total; i++) {
                            if (nodeConnections[conductor->current.end].collection[i].end >= 0) {
                                path *temp = malloc(sizeof(path));
                                temp->current.start = conductor->current.end;
                                temp->current.end = nodeConnections[conductor->current.end].collection[i].end;
                                conductor->next = temp;
                                conductor = conductor->next;
                                nodeConnections[conductor->current.start].count++;
                                nodeConnections[conductor->current.start].collection[i].end = -1;
                                for (int j=0; j<nodeConnections[conductor->current.end].total; j++) {
                                    if (nodeConnections[conductor->current.end].collection[j].end == conductor->current.start) {
                                        nodeConnections[conductor->current.end].collection[j].end = -1;
                                        nodeConnections[conductor->current.end].count++;
                                        break;
                                    }
                                }

                                edgeTrack++;
                                break;
                            }
                        }
                    }
                    conductor->next = tempHolder;
                    break;
                }
            }
        }
        
        //print path
        conductor = root;
            
        while (conductor) {
            printf("%d -> %d\n", conductor->current.start, conductor->current.end);
            conductor = conductor->next;
        }
    }
    
    // deallocate
    for (int i = 0; i < nodeNum; i++)
        free(adjacency[i]);
    free(adjacency);
    for (int i=0; i<nodeNum; i++) {
        free(nodeConnections[i].collection);
    }
    free(nodeConnections);

    
    // end timestamp
    gettimeofday(&timeEnd, NULL);
    timeval_print(&timeEnd);
    
    // print difference
    timeval_subtract(&timeDiff, &timeEnd, &timeBegin);
    printf("%ld.%06d\n", timeDiff.tv_sec, timeDiff.tv_usec);
    


    
    return 0;

}

// Return 1 if the difference is negative, otherwise 0.
int timeval_subtract(struct timeval *result, struct timeval *t2, struct timeval *t1)
{
    long int diff = (t2->tv_usec + 1000000 * t2->tv_sec) - (t1->tv_usec + 1000000 * t1->tv_sec);
    result->tv_sec = diff / 1000000;
    result->tv_usec = diff % 1000000;
    
    return (diff<0);
}

void timeval_print(struct timeval *tv)
{
    char buffer[30];
    time_t curtime;
    
    printf("%ld.%06d", tv->tv_sec, tv->tv_usec);
    curtime = tv->tv_sec;
    strftime(buffer, 30, "%m-%d-%Y  %T", localtime(&curtime));
    printf(" = %s.%06d\n", buffer, tv->tv_usec);
}
