#include <stdio.h>
#include <ctype.h>

int main(int argc, char *argv[])
{
    if (argc != 2){
        printf("please input your username\n");
    }
    else{
        int i;
        for(i=0;argv[i]!='\0';i++)
           // printf("%s\n",argv[i]);
          //  printf("%s\n",isalpha(argv[i]));
            if(isalpha(argv[i]!=0)){
               printf("aaaaa");
              }
                
 //   printf("%d\n", argc);
 //   printf("%s\n", argv[1]);
    }
    
    return(0);
}
