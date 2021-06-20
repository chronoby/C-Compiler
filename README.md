# C-Compiler

## Build

```
make
```

## Run
```
./compiler <FILE> [options]
```
运行 `./compiler --help` 或 `./compiler -h` 获得提示信息

## File Structure

* src：编译器源代码文件
* test：测试使用的源文件
  * test/ast：测试源文件的AST可视化结果
  * test/result：测试代码的运行结果
* print_ast：代码可视化部分，请阅读该目录下的README了解如何使用
* build：包含编译好的Compiler文件，请在Linux环境下运行
* report.pdf：实验报告
