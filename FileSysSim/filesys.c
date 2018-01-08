#include "P4.h"

int main() {

   char buff[MAX_NAME_LENGTH];
   char s[2];
   SELECTION sel;

   fileSysInit();

   while(sel != QQ) {

      printf("\nnf - Create File   nd - Create Directory\n");
      printf("df - Delete File   dd - Delete Directory\n");
      printf("gi - Get Info      pm - Print Memory\n");
      printf("cd - Change Dir.   qq - Quit\n");
      printf("Enter Selection: ");

      scanf("%s", &s);

      if(strcmp(s, "nf") == 0)
         sel = NF;
      else if(strcmp(s, "nd") == 0)
         sel = ND;
      else if(strcmp(s, "df") == 0)
         sel = DF;
      else if(strcmp(s, "dd") == 0)
         sel = DD;
      else if(strcmp(s, "gi") == 0)
         sel = GI;
      else if(strcmp(s, "pm") == 0)
         sel = PM;
      else if(strcmp(s, "cd") == 0)
         sel = CD;
      else if(strcmp(s, "qq") == 0)
         sel = QQ;
      else
         sel = XX;

      switch(sel) {
         case NF:
            printf("Enter the file name: ");
            scanf("%s", &buff);
            createFile(buff);
            break;
         case ND:
            printf("Enter the directory name: ");
            scanf("%s", &buff);
            createDir(buff);
            break;
         case DF:
            printf("Delete which file? ");
            scanf("%s", &buff);
            deleteFile(buff);
            break;
         case DD:
            printf("Delete which directory? ");
            scanf("%s", &buff);
            deleteDir(buff);
            break;
         case GI:
            printf("Get which file's info?\n");
            scanf("%s", &buff);
            getInfo(buff);
            break;
         case PM:
            printMem();
            break;
         case CD:
            printf("Change to which directory?\n");
            scanf("%s", &buff);
            changeDir(buff);
            break;
         case QQ:
            printf("QQ\n");
            break;
         case XX:
         default:
            printf("Something went wrong~\n");
      }
   }
}
