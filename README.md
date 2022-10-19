# Cake is Make with makefiles written in C++ (closed-source binary demo)

Cake itself contains only dependency graph scanner + parallel executor. Everything else can be written in C/C++ and linked into `cakefile`.

I just found [cake-build/cake](https://github.com/cake-build/cake) project on github (C#). Hope they won't sue me for name "cake": after all, if anyone has the right --- it's Valve. :)

## What it looks like

See [cakefile.cpp](cakefile.cpp) and console log [screenshot.png](screenshot.png).

## How to run

Built for x86-64 with gcc 12.2.0, glibc 2.36. Depends on libxxhash.so.0.

Since I don't provide headers/libs for you to recompile `./cakefile.cpp` in this demo, you have to keep `target/cakefile` -- and **with mtime greater than** `./cakefile.cpp` so `cake` does not try to recompile it.

You can only play with `./src` tree, try commands shown on [screenshot](screenshot.png), and view `cake -vv` log to see how it works.
