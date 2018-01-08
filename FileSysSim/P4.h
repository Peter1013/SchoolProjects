#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define BLOCK_SIZE 256
#define MAX_NAME_LENGTH 32
#define DATA_SIZE 254
#define INDEX_SIZE 127
#define NUM_OF_BLOCKS 16
#define GLOBAL_TABLE_FILE_SIZE 997 // must be prime
#define MAX_OPEN_FILES_PER_PROCESS 16

typedef char data_t;
typedef unsigned short index_t;

typedef enum {
   NF,
   ND,
   DF,
   DD,
   GI,
   PM,
   CD,
   QQ,
   XX
} SELECTION;

typedef enum {
   EMPTY,
   DIR,
   FIL,
   INDEX,
   DATA
} NODE_TYPE;

typedef struct fs_node {
   char name[MAX_NAME_LENGTH];
   time_t creat_t; // creation time
   time_t access_t; // last access
   time_t mod_t; // last modification
   mode_t access; // access rights for the file
   unsigned short owner; // owner ID
   unsigned short size; // number of data blocks
   index_t block_ref; // reference to the data or index block
} FS_NODE;              // unsigned short

typedef struct node {
   NODE_TYPE type;
   union
   {
      FS_NODE fd;
      data_t data[DATA_SIZE];
      index_t index[INDEX_SIZE];
   } content;
} NODE;
// elements of the hash table
typedef struct open_file_global_type {
   unsigned short fd; // file ref
   unsigned short data; // index ref (depending on size)
   mode_t access; // access rights
   unsigned short size; //file size
   unsigned short reference_count; // ??
   struct open_file_global_type *next;
} OPEN_FILE_GLOBAL_TYPE;

typedef struct open_file_local_type { // local list of open files
   mode_t access_rights; // accessr rights
   unsigned short global_ref;
} OPEN_FILE_LOCAL_TYPE;

static NODE *memory;
static char *bitVector;
static int curDir;
static time_t init_time;

void fileSysInit();
//allocate 2^16 blocks and super block
void createFile(char *);
//creates file node and places in next available slot
void createDir(char *);
//creates dir node and places in next available slot
int deleteFile(char *);
//deletes indicated file
int deleteDir(char *);
//deletes indicated directory
int getInfo(char *);
//prints file/directory Info
index_t nextEmpty();
// returns index of next empty block in memory, and sets bitvector bit to full
void printMem();
// prints non-empty memory blocks w/some Info
int changeDir();
