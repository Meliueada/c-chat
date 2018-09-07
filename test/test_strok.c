#include <stdio.h>
#include <string.h>

main(){
    char s[] = "name:bdu";
    char username;
    char *delim = ":";
    char *p;
    char *a;
    char name[10][10]={0};
    printf("%s\n",strtok(s, delim));
    while(p=strtok(NULL,delim)){
//          printf("delim:%s\n",&delim);
//          printf("delim:%s\n",delim);
            printf("%s\n",p);
            sprintf(name[0],"%s",p);
//          printf("%d\n",*p);
//          printf("%d\n",&p);
//         // name[0] = p;
//          name[0]= p;
         printf("%s\n",name[0]);
    }
      int i;
      int j;
    for(i=0;i<10;i++){
      printf("before:%s\n",name[i]);
        for (j=0;j<10;j++){
       if(name[i][j]){
        printf("%d\n",i);
        printf("%d\n",j);
        printf("name:%s\n",name[i]);
        name[i][j]=0;
       }
     } 
      printf("delete:%s\n",name[i]);
  }
//}
    

   // printf("%d",name[0]);
  
    


}



//main(){
//    char s[] = "ab-cd : ef;gh :i-jkl;mnop;qrs-tu: vwx-y;z";
//    char *delim = "-: ";
//    char *p;
//    printf("%s ", strtok(s, delim));
//    while((p = strtok(NULL, delim)))
//        printf("%s ", p);
//        printf("\n");
//}
