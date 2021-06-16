; ModuleID = 'main'
source_filename = "main"

@arr1 = global [10 x i32] zeroinitializer
@0 = private unnamed_addr constant [5 x i8] c"%lf\0A\00", align 1
@1 = private unnamed_addr constant [13 x i8] c"%d %d %d %d\0A\00", align 1

define i32 @main() {
entry:
  %arr2 = alloca [10 x i32], align 4
  %a = alloca i32, align 4
  store i32 10, i32* %a, align 4
  %b = alloca i32, align 4
  store i32 12, i32* %b, align 4
  %x = alloca double, align 8
  store double 5.000000e+00, double* %x, align 8
  %y = alloca double, align 8
  store double 3.000000e+00, double* %y, align 8
  %0 = load i32, i32* %b, align 4
  %1 = load double, double* %x, align 8
  %2 = sitofp i32 %0 to double
  %fdiv = fdiv double %2, %1
  %printf_call = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @0, i32 0, i32 0), double %fdiv)
  %3 = load [10 x i32], [10 x i32]* @arr1, align 4
  %4 = load i32, i32* %a, align 4
  %5 = load i32, i32* %b, align 4
  %sub = sub i32 %5, %4
  %sub1 = sub i32 %sub, 1
  %6 = getelementptr [10 x i32], [10 x i32]* @arr1, i32 0, i32 %sub1
  %7 = load i32, i32* %6, align 4
  store i32 1, i32* %6, align 4
  %8 = load [10 x i32], [10 x i32]* %arr2, align 4
  %9 = getelementptr [10 x i32], [10 x i32]* %arr2, i32 0, i32 2
  %10 = load i32, i32* %9, align 4
  %11 = load [10 x i32], [10 x i32]* @arr1, align 4
  %12 = load i32, i32* getelementptr inbounds ([10 x i32], [10 x i32]* @arr1, i32 0, i32 1), align 4
  %add = add i32 %12, 4
  store i32 %add, i32* %9, align 4
  %13 = load i32, i32* %a, align 4
  %14 = load [10 x i32], [10 x i32]* @arr1, align 4
  %15 = load i32, i32* getelementptr inbounds ([10 x i32], [10 x i32]* @arr1, i32 0, i32 1), align 4
  store i32 %15, i32* %a, align 4
  %16 = load i32, i32* %b, align 4
  %17 = load [10 x i32], [10 x i32]* %arr2, align 4
  %18 = getelementptr [10 x i32], [10 x i32]* %arr2, i32 0, i32 2
  %19 = load i32, i32* %18, align 4
  store i32 %19, i32* %b, align 4
  %20 = load [10 x i32], [10 x i32]* @arr1, align 4
  %21 = load i32, i32* getelementptr inbounds ([10 x i32], [10 x i32]* @arr1, i32 0, i32 1), align 4
  %22 = load [10 x i32], [10 x i32]* %arr2, align 4
  %23 = getelementptr [10 x i32], [10 x i32]* %arr2, i32 0, i32 2
  %24 = load i32, i32* %23, align 4
  %25 = load i32, i32* %a, align 4
  %26 = load i32, i32* %b, align 4
  %printf_call2 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([13 x i8], [13 x i8]* @1, i32 0, i32 0), i32 %21, i32 %24, i32 %25, i32 %26)
  ret i32 0
}

declare i32 @printf(i8*, ...)
