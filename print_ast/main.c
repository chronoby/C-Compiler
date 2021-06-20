#include<stdio.h>
#include<stdlib.h>
#include"syntax.tab.h"
#include"Node.h"
int mistake=0;
extern Node* ROOT;
extern void yyrestart  (FILE * input_file );
extern int yyparse (void);
int main(int argc,char** argv)
{
    if(argc<=1)
        return 1;
    FILE* f=fopen(argv[1],"r");
    if(!f)
    {
        perror(argv[1]);
        return 1;
    }
    yyrestart(f);
    //yydebug=1;
    FILE* fp;
    fp = freopen("draw.py", "w", stdout);
    yyparse();
    if(mistake==0){
    printf("import json\n");
    printf("import os\n");
    printf("from pyecharts import options as opts\n");
    printf("from pyecharts.charts import Page, Tree\n");
    printf("class TreeBase(Tree):\n");
    printf("\tdef __init__(self, **kwargs):\n");
    printf("\t\tsuper(TreeBase, self).__init__(**kwargs)\n");
    printf("\t\tself.js_host = 'min.js'\n");
    printf("\n");
    printf("\n");

    printf("data=[\n");
    Treeprint(ROOT,0);
    printf("]\n");
    printf("tree=(\n");
    printf("TreeBase(init_opts=opts.InitOpts(width=\"1400px\", height=\"1000px\"))\n");
    printf(".add(\"\", data,orient=\"TB\")\n");
    printf(".set_global_opts(title_opts=opts.TitleOpts(title=\"AST\")))\n");
    printf("tree.render()\n");
}
   fclose(fp);
   system("python3 draw.py");
    
    return 0;
}
