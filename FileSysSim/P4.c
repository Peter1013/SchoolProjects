#include "P4.h"

void fileSysInit() {
   init_time = time(NULL);
   //allocate 2^16 blocks
   memory = calloc(NUM_OF_BLOCKS, sizeof(NODE));
   //allocate bit vector
   bitVector = malloc(sizeof(char) * NUM_OF_BLOCKS/8);

   char clear = 0xFF;
   for(int i=0; i<(NUM_OF_BLOCKS/8); i++) {
      bitVector[i] = clear;
   }
   //instantiate super block & its index
   FS_NODE sb;
   strcpy(sb.name, "/");
   sb.creat_t = 0;
   sb.access = 0777; //change, maybe too much permission
   sb.owner = 1;
   sb.size = 0; // number of files in directory
   sb.block_ref = 1; // index of root dir's index block

   NODE sblock;
   sblock.type = DIR;
   sblock.content.fd = sb;

   memory[0] = sblock;

   NODE sindex;
   sindex.type = INDEX;

   memory[1] = sindex;


   // set bitvector for newly created node;
   bitVector[0] &= ~(0xC0);
   // set working directory to super block
   curDir = 0;
}

void createFile(char *fileName) {
   //creates file node and places in next available slot
   FS_NODE f;
   strcpy(f.name, fileName);
   f.creat_t = time(NULL);
   f.access_t = time(NULL);
   f.mod_t = time(NULL);
   f.access = 0777; //        temp, change later
   f.owner = 0;
   f.size = 0;
   f.block_ref = NULL;

   NODE n;
   n.type = FIL;
   n.content.fd = f;

   index_t ind = nextEmpty();
   memory[ind] = n;

   // create index in working directories index block and increment dir size
   memory[memory[curDir].content.fd.block_ref].content.index[memory[curDir].content.fd.size] = ind;
   memory[curDir].content.fd.size++;

}

void createDir(char *dirName) {
//creates dir node and places in next available slot

   FS_NODE f;
   strcpy(f.name, dirName);
   f.creat_t = time(NULL);
   f.access_t = time(NULL);
   f.mod_t = time(NULL);
   f.access = 0777; //        temp, change later
   f.owner = curDir;
   f.block_ref = nextEmpty();
   f.size = 0;

   NODE n;
   n.type = DIR;
   n.content.fd = f;

   index_t dirIndx = nextEmpty();

   NODE i;
   i.type = INDEX;


   memory[dirIndx] = n;
   memory[f.block_ref] = i;

   memory[memory[curDir].content.fd.block_ref].content.index[memory[curDir].content.fd.size] = dirIndx;
   memory[curDir].content.fd.size++;
}

int deleteFile(char *delFile) { // return 0 if success
//deletes indicated file
   for(int i=0; i<memory[curDir].content.fd.size; i++) {
      if(strcmp(delFile, memory[memory[memory[curDir].content.fd.block_ref].content.index[i]].content.fd.name) == 0) {
         int indx = memory[memory[curDir].content.fd.block_ref].content.index[i];
         printf("Deleting: %s", memory[indx].content.fd.name);
         memory[indx].type = EMPTY;
         char mask = 0x0001;
         mask <<= (7 - (indx % 8));
         bitVector[i] |= mask;

         for(int j=indx; j<=memory[curDir].content.fd.size; j++) {
            memory[memory[curDir].content.fd.block_ref].content.index[j] = memory[memory[curDir].content.fd.block_ref].content.index[j+1];
            memory[curDir].content.fd.size--;
         }
         return 0;
      }
   }
   return -1;
}
int deleteDir(char *delDir) {
   for(int i=0; i<memory[curDir].content.fd.size; i++) {
      if (strcmp(delDir, memory[memory[memory[curDir].content.fd.block_ref].content.index[i]].content.fd.name) == 0) {
         int indx = memory[memory[curDir].content.fd.block_ref].content.index[i];
         for(int j=0; j<memory[indx].content.fd.size; j++) {
            if ((memory[memory[memory[indx].content.fd.block_ref].content.index[j]].type) == FIL) {
               changeDir(delDir);
               deleteFile(memory[memory[memory[indx].content.fd.block_ref].content.index[j]].content.fd.name);
               changeDir("..");
            }
            else if  ((memory[memory[memory[indx].content.fd.block_ref].content.index[j]].type) == DIR) {
               changeDir(delDir);
               deleteDir(memory[memory[memory[indx].content.fd.block_ref].content.index[j]].content.fd.name);
               changeDir("..");
            }
         }
         printf("Deleting: %s\n", memory[indx].content.fd.name);
         //first index block
         memory[memory[indx].content.fd.block_ref].type = EMPTY;
         char mask = 0x0001;
         mask <<= (7 -(memory[indx].content.fd.block_ref % 8));
         bitVector[i] |= mask;
         //then dir block
         memory[indx].type = EMPTY;
         mask = 0x0001;
         mask <<= (7 - (indx % 8));
         bitVector[i] |= mask;

         for(int j=indx; j<=memory[curDir].content.fd.size; j++) {
            memory[memory[curDir].content.fd.block_ref].content.index[j] = memory[memory[curDir].content.fd.block_ref].content.index[j+1];
            memory[curDir].content.fd.size--;
         }
         return 0;
      }
   }
   return -1;
}

int getInfo(char * file) {
   for(int i=0; i< memory[curDir].content.fd.size; i++) {
      if(strcmp(file, (memory[memory[memory[curDir].content.fd.block_ref].content.index[i]].content.fd.name)) == 0) {
         printf("Created: %s", asctime(gmtime(&memory[memory[memory[curDir].content.fd.block_ref].content.index[i]].content.fd.creat_t)));
         printf("Accessed: %s", asctime(gmtime(&memory[memory[memory[curDir].content.fd.block_ref].content.index[i]].content.fd.access_t)));
         printf("Modified: %s", asctime(gmtime(&memory[memory[memory[curDir].content.fd.block_ref].content.index[i]].content.fd.mod_t)));
         printf("Size: %d\n", memory[memory[memory[curDir].content.fd.block_ref].content.index[i]].content.fd.size);
         return 0;
      }
   }
   return -1;
}

index_t nextEmpty() {
   int i, x = 7;
   for(i=0; i<NUM_OF_BLOCKS/8; i++) {
      if(bitVector[i] != 0) {
         int mask = 0x80;
         while(1) {
            if ((mask & bitVector[i]) != 0) {
               bitVector[i] &= ~(mask);
               return (index_t) (i * 8) + (7 - x);
            }
            else {
               mask >>= 1;
               x--;
            }
         }
      }
   }
}
void printMem() { // for debug
   printf("Current Directory: %s\n", memory[curDir].content.fd.name);
   int flag = 0;
   for(int j=0; j<NUM_OF_BLOCKS/8; j++) {
      if(bitVector[j] != 0xFF) {
         printf("%x ", bitVector[j]);
         flag ++;
         if(flag > 8) {
            printf("\n");
            flag = 0;
         }
      }
   }
   printf("\n");

   for(int i=0; i<NUM_OF_BLOCKS; i++) {
      if(memory[i].type != EMPTY) {
         printf("Type: %d ", memory[i].type);
         if(memory[i].type == FIL || memory[i].type == DIR)
            printf("Name: %s\n", memory[i].content.fd.name);
         else
            printf("\n");
      }
   }
}
int changeDir(char *dir) {
   for(int i=0; i<memory[curDir].content.fd.size; i++) {
      if(strcmp(dir, "..") == 0) {
         curDir = memory[curDir].content.fd.owner;
         printf("Changing directory to: /%s\n", memory[curDir].content.fd.name);
         return 0;
      }
      else if (strcmp(dir,memory[memory[memory[curDir].content.fd.block_ref].content.index[i]].content.fd.name) == 0) {
         int indx = memory[memory[curDir].content.fd.block_ref].content.index[i];
         curDir = indx;
         printf("Changing directory to: /%s\ndf", memory[indx].content.fd.name);
         return 0;
      }
   }
   return -1;
}
