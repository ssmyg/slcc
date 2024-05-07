#!/bin/bash

cat <<EOF | gcc -xc -c -o tmp2.o -
int ret3() { return 3; }
int ret_5() { return 5; }
int ret_arg1(int a) { return a; }
int ret_arg2(int a, int b) { return a + b; }
int ret_sub(int a, int b) { return a - b; }
int ret_arg5(int a, int b, int c, int d, int e) { return a + b + c + d + e; }
int ret_arg6(int a, int b, int c, int d, int e, int f) { return a + b + c + d + e + f; }
EOF

assert() {
  expected="$1"
  input="$2"

  ./slcc "$input" > tmp.s
  #cat tmp.s
  cc -o tmp tmp.s tmp2.o -Wa,--noexecstack
  ./tmp
  actual="$?"

  if [ "$actual" == "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

# 四則演算
assert 0 "main () {return 0;}"
assert 42 "main () {return 42;}"
assert 21 "main () {return 5+20-4;}"
assert 21 "main () {return  5  + 20 -  4  ;        }"
assert 4 "main () {return 4 * ( 5 - 2 ) / 3;}"
assert 47 "main () {return 5 + 6 * 7;}"
assert 15 "main () {return 5 * (9 - 6);}"
assert 4 "main () {return (3 + 5) / 2;}"
assert 10 "main () {return -10 + 20;}"
assert 40 "main () {return +10 + 20 - ( -10 );}"

# 比較演算子
assert 1 "main () {return 5 == 5;}"
assert 0 "main () {return 5 == 6;}"

assert 0 "main () {return 5 != 5;}"
assert 1 "main () {return 5 != 6;}"

assert 0 "main () {return 5 < 4;}"
assert 0 "main () {return 5 < 5;}"
assert 1 "main () {return 5 < 6;}"

assert 0 "main () {return 5 <= 4;}"
assert 1 "main () {return 5 <= 5;}"
assert 1 "main () {return 5 <= 6;}"

assert 1 "main () {return 5 > 4;}"
assert 0 "main () {return 5 > 5;}"
assert 0 "main () {return 5 > 6;}"

assert 1 "main () {return 5 >= 4;}"
assert 1 "main () {return 5 >= 5;}"
assert 0 "main () {return 5 >= 6;}"

assert 1 "main () {return -1 * 2 + 1 < -2 + 4;}"
assert 0 "main () {return +1 * 2 + 1 < -2 + 4;}"

# 複数行
assert 42 "main () {32; return 42;}"

# 変数
assert 6 "main () {a = 1 + 2; return a + 3;}"
assert 23 "main () {a = 10; b = a + 2; z = a + b; return z + 1;}"
assert 4 "main () {a = b = 2; a + b;}"
assert 10 "main () {aaa = 5; aba = 2; xyz = aaa * aba / 2 + 5; return xyz;}"
assert 15 "main () {aa = 10; aa = aa + 5; bb = 30; return aa;}"
assert 12 "main () {a1 = 10; Ab2 = 5; ret_a = a1 / Ab2 * 6; return ret_a;}"

# return文
assert 6 "main () {return_= 1 + 2; return return_+ 3;}"
assert 6 "main () {return1 = 1 + 2; return return1 + 3;}"
assert 6 "main () {return 2 * 3; return 10;}"
assert 6 "main () {a = 5; return a + 1;}"

# if文
assert 6 "main () {if(1==1) return 6; return 0;}"
assert 6 "main () {if(0==1) return 0; return 6;}"
assert 20 "main () {a = 10; if(a == 10) a = 20; return a;}"
assert 10 "main () {a = 10; if(a != 10) a = 20; return a;}"

assert 30 "main () {a = 10; if(a != 10) a = 20; if(a == 10)  a = 30 ;return a;}"
assert 30 "main () {a = 10; if(a != 10) a = 20; else a = 30 ;return a;}"

# while
assert 10 "main () {a = 0; while(a < 10) a = a + 1; return a;}"

# for
assert 10 "main () {a = 0; for(i = 0; i < 10; i = i + 1) a = a + 1; return a;}"
assert 0 "main () {a = 0; for(;;) return a;}"
assert 0 "main () {for(i = 0;;) return i;}"
assert 0 "main () {i = 0; for(;i == 0;) return i;}"
assert 0 "main () {i = 0; for(;;i = i + 1) return i;}"

# block
assert 10 "main () {{1 + 1; 2 - 3; return 2 * 5; return 0;}}"
assert 10 "main () {a = 0; for(;;){ if( a == 10 ) return a; a = a + 1; }}"

# function
assert 3 "main () {return ret3();}"
assert 5 "main () {return ret_5();}"
assert 6 "main () {return ret_arg1(6);}"
assert 6 "main () {return ret_arg1((6 - 4) * 3);}"
assert 11 "main () {return ret_arg2(5, 6);}"
assert 4 "main () {return ret_sub(10, 6);}"
assert 11 "main () {return ret_arg2(2+3, 2*3);}"
assert 15 "main () {return ret_arg5(1,2,3,4,5);}"
assert 21 "main () {return ret_arg6(1==1,2,3,4,2+3,2*3);}"

# function definition
assert 9 "myfn1 () {return 9;} main () {return myfn1();}"
assert 10 "myfn2 () {a = 1; b = 2; return a + b;} main () {a = 3; b = 4; return myfn2() + a + b;}"
assert 10 "myfn2 () {a = 2; a = 1; b = 2; return a + b;} main () {a = 3; b = 4; return myfn2() + a + b;}"

# argment
assert 42 "myfn3 (a) {return a;} main () {return myfn3(42);}"
assert 8 "myfn3 (a, b) {return a + 2 * b;} main () {return myfn3(2, 3);}"

assert 11 "myfn3 (a, b) {c = 4; return a + b * c;} main () {a = 3; b = 4; return myfn3(a, 2);}"
assert 8 "myfn3 (a, b) {a = 1; b = 2; c = 4; return a * b * c;} main () {a = 3; b = 4; return myfn3(a, 2);}"
assert 24 "myfn3 (a, b) {b = 2; c = 4; return a * b * c;} main () {a = 3; b = 4; return myfn3(a, 10);}"
assert 29 "myfn3 (a, b, c, d, e, f) {return a + 2*b + 3*c + 5*d + 7*e + 11*f;} main () {return myfn3(1,1,1,1,1,1);}"


echo OK
