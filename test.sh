./compiler
llc out.ll
gcc out.s
./a.out
echo $?
