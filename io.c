#include <stdio.h>

int main()
{
     /* Pointer to the file */
     FILE *fp1;
     /* Character variable to read the content of file */
     char c;
     while(1){
     /* Opening a file in r mode*/
     fp1= fopen ("a.txt", "r");

     fclose(fp1);
    }
     return 0;
}