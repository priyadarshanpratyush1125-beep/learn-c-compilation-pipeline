c-compilation-pipeline
What is C Compilation Pipeline?

When we write a C program, the .c file cannot be executed directly by the computer.

It passes through multiple stages before becoming an executable program.

The basic workflow is:

.c

 │

 │  Preprocessing

 ▼

.i

 │

 │  Compilation

 ▼

.s

 │

 │  Assembly

 ▼

.o

 │

 │  Linking

 ▼

Executable

For a multi-file C project, each source file goes through the same compilation stages independently and the resulting object files are finally linked together.

This project demonstrates the complete process manually using GCC.

The project will also demonstrate two different ways of linking a library:

Static Linking

     │

     ▼

libcalculator.a

     │

     ▼

calculator_static

and:

Dynamic Linking

     │

     ▼

libcalculator.so

     │

     ▼

calculator_dynamic

The goal is not to prove that one linking method is always better than the other.

The goal is to understand how both work, what they produce, how they differ, and where each is useful.

Why This Project?

Usually, when we compile a C program, we simply write:

gcc main.c -o main

and get an executable.

This hides several important steps.

For example:

main.c

   │

   └── gcc

         │

         ▼

      executable

We don't directly see:

preprocessing
generated C source
assembly code
object files
symbols
relocation information
libraries
static linking
dynamic linking
shared-library dependencies
ELF executable structure

This project makes those stages visible.

Instead of treating GCC as a single command, we manually perform each stage and inspect the generated files.

Project Goal

The project has two main goals.

1. Understand the C compilation pipeline
.c → .i → .s → .o
2. Understand linking
.o → static library → executable

and:

.o → shared library → executable

Finally, we compare the generated programs using Linux tools such as:

size

ls

file

nm

readelf

ldd

objdump
Calculator Project

To demonstrate the compilation pipeline, we use a small calculator application.

The calculator supports:

Addition

Subtraction

Multiplication

Division

The calculator itself is intentionally simple.

The main purpose of the project is not calculator development.

The calculator is used as a practical example for studying the compilation and linking process.

Project Structure

The project structure is:

.

├── build
│   ├── assembly
│   │   ├── add.s
│   │   ├── div.s
│   │   ├── main.s
│   │   ├── mul.s
│   │   └── sub.s
│   ├── dynamic
│   ├── objects
│   │   ├── add.o
│   │   ├── div.o
│   │   ├── main.o
│   │   ├── mul.o
│   │   └── sub.o
│   ├── preprocess
│   │   ├── add.i
│   │   ├── div.i
│   │   ├── main.i
│   │   ├── mul.i
│   │   └── sub.i
│   └── static
├── c
│   ├── inc
│   │   ├── add.h
│   │   ├── div.h
│   │   ├── mul.h
│   │   └── sub.h
│   └── src
│       ├── add.c
│       ├── div.c
│       ├── mul.c
│       └── sub.c
├── lib
│   ├── libcalculator.a
│   └── libcalculator.so
├── main.c
└── README.md

The source code is separated from generated files.

The inc directory contains function declarations.

The src directory contains function definitions.

main.c uses those functions.

The build directory will contain files generated during the compilation experiments.

The lib directory contains the static and shared calculator libraries.

How the Calculator is Organized
main.c

main.c is the entry point of the application.

It uses the functions provided by the calculator modules.

#include <stdio.h>

#include "add.h"

#include "sub.h"

#include "mul.h"

#include "div.h"

int main()

{

    int a = 20;

    int b = 5;

    printf("Addition       : %d\n", add(a, b));

    printf("Subtraction    : %d\n", sub(a, b));

    printf("Multiplication : %d\n", mul(a, b));

    printf("Division       : %d\n", divide(a, b));

    return 0;

}
add.c
#include "add.h"

int add(int a, int b)

{

    return a + b;

}
add.h
#ifndef ADD_H

#define ADD_H

int add(int a, int b);

#endif
sub.c
#include "sub.h"

int sub(int a, int b)

{

    return a - b;

}
sub.h
#ifndef SUB_H

#define SUB_H

int sub(int a, int b);

#endif
mul.c
#include "mul.h"

int mul(int a, int b)

{

    return a * b;

}
mul.h
#ifndef MUL_H

#define MUL_H

int mul(int a, int b);

#endif
div.c
#include "div.h"

int divide(int a, int b)

{

    if (b == 0)

        return 0;

    return a / b;

}
div.h
#ifndef DIV_H

#define DIV_H

int divide(int a, int b);

#endif
Normal Compilation

Before investigating the individual stages, we can first compile the complete program normally.

gcc main.c c/src/add.c c/src/sub.c c/src/mul.c c/src/div.c -Ic/inc -o calculator

Run it:

./calculator
Output
Addition       : 25

Subtraction    : 15

Multiplication : 100

Division       : 4

Normally, GCC hides the individual stages from us.

In this project, we will perform those stages manually.

C Compilation Pipeline

The complete pipeline is:

                 C SOURCE FILES

main.c
add.c
sub.c
mul.c
div.c

   │
   │
   │ gcc -E
   ▼

 PREPROCESSED FILES

main.i
add.i
sub.i
mul.i
div.i

   │
   │
   │ gcc -S
   ▼

 ASSEMBLY FILES

main.s
add.s
sub.s
mul.s
div.s

   │
   │
   │ gcc -c
   ▼

 OBJECT FILES

main.o
add.o
sub.o
mul.o
div.o

After this point, the object files can be linked.

Stage 1 — Preprocessing

The first stage is preprocessing.

We use:

gcc -E main.c -Ic/inc -o build/preprocess/main.i

Similarly:

gcc -E c/src/add.c -Ic/inc -o build/preprocess/add.i

gcc -E c/src/sub.c -Ic/inc -o build/preprocess/sub.i

gcc -E c/src/mul.c -Ic/inc -o build/preprocess/mul.i

gcc -E c/src/div.c -Ic/inc -o build/preprocess/div.i

Now:

.c

↓

gcc -E

↓

.i

For example:

main.c

   │

   ▼

main.i
What happens?

The preprocessor handles directives such as:

#include

#define

#ifdef

#ifndef

#if

#endif

For example:

#include "add.h"

causes the contents of the header to become part of the preprocessed source.

We can inspect the result:

less build/preprocess/main.i

The .i file is still C source-like text.

It has not yet become assembly.

Learning

At this stage we learn:

What preprocessing means
How #include works
How header files participate in compilation
How macros are expanded
Why header files affect the compilation process
What a .i file contains

The important idea is:

.c

↓

Preprocessor

↓

.i
Stage 2 — Compilation

Now we convert the preprocessed C code into assembly language.

For example:

gcc -S build/preprocess/main.i -o build/assembly/main.s

And:

gcc -S build/preprocess/add.i -o build/assembly/add.s

gcc -S build/preprocess/sub.i -o build/assembly/sub.s

gcc -S build/preprocess/mul.i -o build/assembly/mul.s

gcc -S build/preprocess/div.i -o build/assembly/div.s

The result is:

.i

↓

gcc -S

↓

.s

For example:

main.i

   │

   ▼

main.s

We can inspect the assembly:

less build/assembly/add.s

Instead of C:

return a + b;

we will see architecture-specific assembly instructions.

Learning

At this stage we learn:

How C code becomes assembly
What assembly language looks like
How functions appear in assembly
How variables and operations are represented
Why assembly is architecture-dependent

The important idea is:

C source

↓

Compiler

↓

Assembly
Stage 3 — Assembly

The assembly code is converted into an object file.

We use:

gcc -c build/assembly/main.s -o build/objects/main.o

Similarly:

gcc -c build/assembly/add.s -o build/objects/add.o

gcc -c build/assembly/sub.s -o build/objects/sub.o

gcc -c build/assembly/mul.s -o build/objects/mul.o

gcc -c build/assembly/div.s -o build/objects/div.o

Now:

.s

↓

gcc -c

↓

.o

The project now contains:

build/objects/

├── main.o
├── add.o
├── sub.o
├── mul.o
└── div.o

These are object files.

Inspecting Object Files

We can investigate an object file using:

file build/objects/main.o

Example:

main.o: ELF 64-bit LSB relocatable, x86-64, ...

The important word here is:

relocatable

An object file is not yet the final executable.

Inspect ELF Header
readelf -h build/objects/main.o

This allows us to inspect information such as:

ELF class
architecture
object-file type
entry information
section information
Inspect Sections
readelf -S build/objects/main.o

We can see sections such as:

.text

.data

.bss

.rodata

.symtab

.strtab

.rela.text

The exact sections depend on the compiler and source code.

Inspect Symbols
nm build/objects/main.o

We can see symbols defined or referenced by the object file.

For example, main.o can contain references to:

add

sub

mul

divide

These functions are implemented in different object files.

This leads us to the next stage.
