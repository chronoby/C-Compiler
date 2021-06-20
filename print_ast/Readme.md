编译时使用的命令

bison -d syntax.y

flex lexical.l

gcc main.c Syntax.tab.c Node.c -lfl -ly -o scanner

在主程序中引用此程序的方法请参考test.c文件

其中，str表示要测试的文件，test为字符串"./scanner "，result为调用程序所用到的命令，通过system(result)这一条命令就可以生成html文件

min.jsecharts.min.js为本地静态js文件
