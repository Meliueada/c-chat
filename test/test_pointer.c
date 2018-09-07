#include<stdio.h>
#include<stdlib.h>
#include<string.h>


char *join3(char *, char*);

int main(int argc, char *argv[]){
    char *result;
    //char *head = "register";
    result = join3("register",argv[1]);
    printf("%s", result);
    return(0);
}

char* join3(char *s1, char *s2)
{
    char *result = malloc(strlen(s1)+strlen(s2)+1);//+1 for the zero-terminator
    //in real code you would check for errors in malloc here
    if (result == NULL) exit (1);
 
    strcpy(result, s1);
    strcat(result, s2);
 
    return result;

}

