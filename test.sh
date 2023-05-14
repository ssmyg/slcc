#!/bin/bash
assert() {
  expected="$1"
  input="$2"

  ./slcc "$input" > tmp.s
  #cat tmp.s
  cc -o tmp tmp.s -Wa,--noexecstack
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
assert 0 "0;"
assert 42 "42;"
assert 21 "5+20-4;"
assert 21 " 5  + 20 -  4  ;        "
assert 4 "4 * ( 5 - 2 ) / 3;"
assert 47 "5 + 6 * 7;"
assert 15 "5 * (9 - 6);"
assert 4 "(3 + 5) / 2;"
assert 10 "-10 + 20;"
assert 40 "+10 + 20 - ( -10 );"

# 比較演算子
assert 1 "5 == 5;"
assert 0 "5 == 6;"

assert 0 "5 != 5;"
assert 1 "5 != 6;"

assert 0 "5 < 4;"
assert 0 "5 < 5;"
assert 1 "5 < 6;"

assert 0 "5 <= 4;"
assert 1 "5 <= 5;"
assert 1 "5 <= 6;"

assert 1 "5 > 4;"
assert 0 "5 > 5;"
assert 0 "5 > 6;"

assert 1 "5 >= 4;"
assert 1 "5 >= 5;"
assert 0 "5 >= 6;"

assert 1 "-1 * 2 + 1 < -2 + 4;"
assert 0 "+1 * 2 + 1 < -2 + 4;"

# 複数行
assert 42 "32; 42;"

# 変数
assert 6 "a = 1 + 2; a + 3;"
assert 23 "a = 10; b = a + 2; z = a + b; z + 1;"
assert 4 "a = b = 2; a + b;"
assert 10 "aaa = 5; aba = 2; xyz = aaa * aba / 2 + 5; xyz;"
assert 15 "aa = 10; aa = aa + 5; bb = 30; aa;"
assert 12 "a1 = 10; Ab2 = 5; ret_a = a1 / Ab2 * 6;"

# return文
assert 6 "return_= 1 + 2; return_+ 3;"
assert 6 "return1 = 1 + 2; return1 + 3;"
assert 6 "return 2 * 3; return 10;"
assert 6 "a = 5; return a + 1;"

# if文
assert 6 "if(1==1) return 6; return 0;"
assert 6 "if(0==1) return 0; return 6;"
assert 20 "a = 10; if(a == 10) a = 20; return a;"
assert 10 "a = 10; if(a != 10) a = 20; return a;"

assert 30 "a = 10; if(a != 10) a = 20; if(a == 10)  a = 30 ;return a;"
assert 30 "a = 10; if(a != 10) a = 20; else a = 30 ;return a;"

# while
assert 10 "a = 0; while(a < 10) a = a + 1; return a;"

# for
assert 10 "a = 0; for(i = 0; i < 10; i = i + 1) a = a + 1; return a;"
assert 0 "a = 0; for(;;) return a;"
assert 0 "for(i = 0;;) return i;"
assert 0 "i = 0; for(;i == 0;) return i;"
assert 0 "i = 0; for(;;i = i + 1) return i;"

# block
assert 10 "{1 + 1; 2 - 3; return 2 * 5; return 0;}"
assert 10 "a = 0; for(;;){ if( a == 10 ) return a; a = a + 1; }"

echo OK

