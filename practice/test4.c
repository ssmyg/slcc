char main[] = "\x48\xc7\xc0\x2a\x00\x00\x00\xc3";
//$ cc -o test4.o -c test4.c
//$ objdump -D -M intel test4.o
// 0000000000000000 <main>:
//   0:   48 c7 c0 2a 00 00 00    mov    rax,0x2a
//   7:   c3                      ret
//$ cc -static -Wl,--omagic -o test4 test4.o

// うまくコンパイルできなかった
// /usr/lib/gcc/x86_64-pc-linux-gnu/12/../../../../x86_64-pc-linux-gnu/bin/ld: warning: test4 has a LOAD segment with RWX permissions
// /usr/lib/gcc/x86_64-pc-linux-gnu/12/../../../../x86_64-pc-linux-gnu/bin/ld: /usr/lib/gcc/x86_64-pc-linux-gnu/12/../../../../lib64/libc.a(libc-start.o): in function `__libc_start_main_impl':
// (.text+0x181a): undefined reference to `__ehdr_start'
// /usr/lib/gcc/x86_64-pc-linux-gnu/12/../../../../x86_64-pc-linux-gnu/bin/ld: (.text+0x1a02): undefined reference to `__ehdr_start'
// /usr/lib/gcc/x86_64-pc-linux-gnu/12/../../../../x86_64-pc-linux-gnu/bin/ld: (.text+0x1a09): undefined reference to `__ehdr_start'
// /usr/lib/gcc/x86_64-pc-linux-gnu/12/../../../../x86_64-pc-linux-gnu/bin/ld: (.text+0x1a17): undefined reference to `__ehdr_start'
// collect2: エラー: ld はステータス 1 で終了しました
