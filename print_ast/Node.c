#include<stdio.h>
#include"Node.h"
#include<stdarg.h>
#include<stdlib.h>
#include<string.h>
extern int yylineno;
Node* create(char *name,char *s,int flag)
{
    Node* newnode=(Node*)malloc(sizeof(struct NODE));
    for(int i=0;i<20;i++)
        newnode->child[i]=NULL;
    newnode->line=yylineno;
    newnode->childnum=0;
    strcpy(newnode->text,s);
    strcpy(newnode->name,name);
    newnode->flag=flag;
    return newnode;
}
void add(int childnum,Node* parent,...)
{
    va_list ptr;
    
    va_start(ptr,parent);
    
    for(int i=0;i<childnum;i++)
    {
        Node *cnode; 
        cnode=va_arg(ptr,Node*);
        parent->child[i]=cnode;
    }
    parent->line=parent->child[0]->line;
    parent->childnum=childnum;
    va_end(ptr);
} 


char *strrpc(char *str,char *oldstr,char *newstr){
    char bstr[strlen(str)];
    memset(bstr,0,sizeof(bstr));
 
    for(int i = 0;i < strlen(str);i++){
        if(!strncmp(str+i,oldstr,strlen(oldstr))){
            strcat(bstr,newstr);
            i += strlen(oldstr) - 1;
        }else{
        	strncat(bstr,str + i,1);
	    }
    }
 
    strcpy(str,bstr);
    return str;
} 

void Treeprint(Node* root,int count)
{


    if(root!=NULL){
        for(int i=0;i<count;i++)
            printf("  ");

        if(strcmp(root->name,"SEMI")==0||strcmp(root->name,"RF")==0||strcmp(root->name,"LF")==0||strcmp(root->name,"LB")==0||strcmp(root->name,"RB")==0||strcmp(root->name,"LP")==0||strcmp(root->name,"RP")==0||strcmp(root->name,"ASSIGN")==0||strcmp(root->name,"COMMA")==0||strcmp(root->name,"AND")==0||strcmp(root->name,"MULTI")==0||strcmp(root->name,"PLUS")==0||strcmp(root->name,"MINUS")==0||strcmp(root->name,"WAVE")==0||strcmp(root->name,"QUES")==0||strcmp(root->name,"NOT")==0||strcmp(root->name,"DIV")==0||strcmp(root->name,"MOD")==0||strcmp(root->name,"LESS")==0||strcmp(root->name,"GREATER")==0||strcmp(root->name,"DOT")==0||strcmp(root->name,"COLON")==0||strcmp(root->name,"ELLIPSIS")==0||strcmp(root->name,"PTR_OP")==0||strcmp(root->name,"INC_OP")==0||strcmp(root->name,"DEC_OP")==0||strcmp(root->name,"AND_OP")==0||strcmp(root->name,"OR_OP")==0||strcmp(root->name,"EQ_OP")==0||strcmp(root->name,"NE_OP")==0||strcmp(root->name,"LE_OP")==0||strcmp(root->name,"GE_OP")==0||strcmp(root->name,"VOID")==0||strcmp(root->name,"TYPE_INT")==0||strcmp(root->name,"TYPE_LONG")==0||strcmp(root->name,"TYPE_SHORT")==0||strcmp(root->name,"TYPE_DOUBLE")==0||strcmp(root->name,"TYPE_FLOAT")==0||strcmp(root->name,"TYPE_SIGNED")==0||strcmp(root->name,"TYPE_UNSIGNED")==0||strcmp(root->name,"TYPE_CHAR")==0||strcmp(root->name,"IF")==0||strcmp(root->name,"ELSE")==0||strcmp(root->name,"WHILE")==0||strcmp(root->name,"CONTINUE")==0||strcmp(root->name,"BREAK")==0||strcmp(root->name,"SIZEOF")==0||strcmp(root->name,"RETURN")==0)
		printf("{\"name\":\"%s\"",root->name);
	else	
		printf("{\"name\":\"%s",root->name);
        if(root->flag==0){
            if(strcmp(root->name,"IDENTIFIER")==0)
                printf(": %s\",",root->text);
            else if(strcmp(root->name,"INTEGER")==0)
                printf(": %s\",",root->text);
	    else if(strcmp(root->name,"FLOAT")==0)
                printf(": %s\",",root->text);
	    else if(strcmp(root->name,"CHAR")==0)
                printf(": %s\",",root->text);
	    else if(strcmp(root->name,"STRING")==0)
                printf(": %s\",",strrpc(root->text,"\"","|"));
	    else if(strcmp(root->name,"HEXI")==0)
                printf(": %s\",",root->text);
	    else if(strcmp(root->name,"OCTAL")==0)
                printf(": %s\",",root->text);            
        }
        else
            printf(" (%d)\",",root->line);
        printf("\n");
	if(root->childnum!=0){
	    printf("\"children\":[");
            for(int i=0;i<root->childnum;i++)
            	Treeprint(root->child[i],count+1);
	    printf("],\n");
	}
	printf("},\n");
    }

}
