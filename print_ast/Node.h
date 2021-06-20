#ifndef _NODE_H_
#define _NODE_H_

typedef struct NODE
{
    char name[1000];
    char text[1000];
    int childnum;
    struct NODE *child[100];
    int line;
    int flag;
}Node;

Node* create(char *name,char *s,int flag);

void add(int chilenum,Node* parent,...);

char *strrpc(char *str,char *oldstr,char *newstr);

void Treeprint(Node* root,int count);

Node * ROOT;

#endif
