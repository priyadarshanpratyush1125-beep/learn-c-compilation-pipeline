# c-compilation-pipeline

## What is C Compilation Pipeline?

When we write a C program, the `.c` file cannot be executed directly by the computer.

It passes through multiple stages before becoming an executable program.

The basic workflow is:

```text
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
```

For a multi-file C project, each source file goes through the same compilation stages independently and the resulting object files are finally linked together.

This project demonstrates the complete process manually using GCC.

The project will also demonstrate two different ways of linking a library:

```text
Static Linking
     │
     ▼
libcalculator.a
     │
     ▼
calculator_static
```

and:

```text
Dynamic Linking
     │
     ▼
libcalculator.so
     │
     ▼
calculator_dynamic
```

The goal is not to prove that one linking method is always better than the other.

The goal is to understand **how both work, what they produce, how they differ, and where each is useful.**

---

# Why This Project?

Usually, when we compile a C program, we simply write:

```bash
gcc main.c -o main
```

and get an executable.

This hides several important steps.

For example:

```text
main.c
   │
   └── gcc
         │
         ▼
      executable
```

We don't directly see:

* preprocessing
* generated C source
* assembly code
* object files
* symbols
* relocation information
* libraries
* static linking
* dynamic linking
* shared-library dependencies
* ELF executable structure

This project makes those stages visible.

Instead of treating GCC as a single command, we manually perform each stage and inspect the generated files.

---

# Project Goal

The project has two main goals.

### 1. Understand the C compilation pipeline

```text
.c → .i → .s → .o
```

### 2. Understand linking

```text
.o → static library → executable
```

and:

```text
.o → shared library → executable
```

Finally, we compare the generated programs using Linux tools such as:

```text
size
ls
file
nm
readelf
ldd
objdump
```

---

# Calculator Project

To demonstrate the compilation pipeline, we use a small calculator application.

The calculator supports:

```text
Addition
Subtraction
Multiplication
Division
```

The calculator itself is intentionally simple.

The main purpose of the project is not calculator development.

The calculator is used as a practical example for studying the compilation and linking process.

---

# Project Structure

```text
.
├── include
│   ├── add.h
│   ├── sub.h
│   ├── mul.h
│   └── div.h
│
├── src
│   ├── add.c
│   ├── sub.c
│   ├── mul.c
│   └── div.c
│
├── main.c
│
├── build
│   ├── preprocess
│   ├── assembly
│   ├── objects
│   ├── static
│   └── dynamic
│
└── lib
    ├── libcalculator.a
    └── libcalculator.so
```

The source code is separated from generated files.

The `include` directory contains function declarations.

The `src` directory contains function definitions.

`main.c` uses those functions.

The `build` directory will contain files generated during the compilation experiments.

The `lib` directory contains the static and shared calculator libraries.

---

# How the Calculator is Organized

## main.c

`main.c` is the entry point of the application.

It uses the functions provided by the calculator modules.

```c
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
```

---

## add.c

```c
#include "add.h"

int add(int a, int b)
{
    return a + b;
}
```

## add.h

```c
#ifndef ADD_H
#define ADD_H

int add(int a, int b);

#endif
```

---

## sub.c

```c
#include "sub.h"

int sub(int a, int b)
{
    return a - b;
}
```

## sub.h

```c
#ifndef SUB_H
#define SUB_H

int sub(int a, int b);

#endif
```

---

## mul.c

```c
#include "mul.h"

int mul(int a, int b)
{
    return a * b;
}
```

## mul.h

```c
#ifndef MUL_H
#define MUL_H

int mul(int a, int b);

#endif
```

---

## div.c

```c
#include "div.h"

int divide(int a, int b)
{
    if (b == 0)
        return 0;

    return a / b;
}
```

## div.h

```c
#ifndef DIV_H
#define DIV_H

int divide(int a, int b);

#endif
```

---

# Normal Compilation

Before investigating the individual stages, we can first compile the complete program normally.

```bash
gcc main.c src/add.c src/sub.c src/mul.c src/div.c -Iinclude -o calculator
```

Run it:

```bash
./calculator
```

## Output

```text
Addition       : 25
Subtraction    : 15
Multiplication : 100
Division       : 4
```

Normally, GCC hides the individual stages from us.

In this project, we will perform those stages manually.

---

# C Compilation Pipeline

The complete pipeline is:

```text
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
```

After this point, the object files can be linked.

---

# Stage 1 — Preprocessing

The first stage is preprocessing.

We use:

```bash
gcc -E main.c -Iinclude -o build/preprocess/main.i
```

Similarly:

```bash
gcc -E src/add.c -Iinclude -o build/preprocess/add.i
gcc -E src/sub.c -Iinclude -o build/preprocess/sub.i
gcc -E src/mul.c -Iinclude -o build/preprocess/mul.i
gcc -E src/div.c -Iinclude -o build/preprocess/div.i
```

Now:

```text
.c
 ↓
gcc -E
 ↓
.i
```

For example:

```text
main.c
   │
   ▼
main.i
```

## What happens?

The preprocessor handles directives such as:

```c
#include
#define
#ifdef
#ifndef
#if
#endif
```

For example:

```c
#include "add.h"
```

causes the contents of the header to become part of the preprocessed source.

We can inspect the result:

```bash
less build/preprocess/main.i
```

The `.i` file is still C source-like text.

It has not yet become assembly.

---

# Learning

At this stage we learn:

* What preprocessing means
* How `#include` works
* How header files participate in compilation
* How macros are expanded
* Why header files affect the compilation process
* What a `.i` file contains

The important idea is:

```text
.c
 ↓
Preprocessor
 ↓
.i
```

---

# Stage 2 — Compilation

Now we convert the preprocessed C code into assembly language.

For example:

```bash
gcc -S build/preprocess/main.i -o build/assembly/main.s
```

And:

```bash
gcc -S build/preprocess/add.i -o build/assembly/add.s
gcc -S build/preprocess/sub.i -o build/assembly/sub.s
gcc -S build/preprocess/mul.i -o build/assembly/mul.s
gcc -S build/preprocess/div.i -o build/assembly/div.s
```

The result is:

```text
.i
 ↓
gcc -S
 ↓
.s
```

For example:

```text
main.i
   │
   ▼
main.s
```

We can inspect the assembly:

```bash
less build/assembly/add.s
```

Instead of C:

```c
return a + b;
```

we will see architecture-specific assembly instructions.

---

# Learning

At this stage we learn:

* How C code becomes assembly
* What assembly language looks like
* How functions appear in assembly
* How variables and operations are represented
* Why assembly is architecture-dependent

The important idea is:

```text
C source
   ↓
Compiler
   ↓
Assembly
```

---

# Stage 3 — Assembly

The assembly code is converted into an object file.

We use:

```bash
gcc -c build/assembly/main.s -o build/objects/main.o
```

Similarly:

```bash
gcc -c build/assembly/add.s -o build/objects/add.o
gcc -c build/assembly/sub.s -o build/objects/sub.o
gcc -c build/assembly/mul.s -o build/objects/mul.o
gcc -c build/assembly/div.s -o build/objects/div.o
```

Now:

```text
.s
 ↓
gcc -c
 ↓
.o
```

The project now contains:

```text
build/objects/
├── main.o
├── add.o
├── sub.o
├── mul.o
└── div.o
```

These are object files.

---

# Inspecting Object Files

We can investigate an object file using:

```bash
file build/objects/main.o
```

Example:

```text
main.o: ELF 64-bit LSB relocatable, x86-64, ...
```

The important word here is:

```text
relocatable
```

An object file is not yet the final executable.

---

## Inspect ELF Header

```bash
readelf -h build/objects/main.o
```

This allows us to inspect information such as:

* ELF class
* architecture
* object-file type
* entry information
* section information

---

## Inspect Sections

```bash
readelf -S build/objects/main.o
```

We can see sections such as:

```text
.text
.data
.bss
.rodata
.symtab
.strtab
.rela.text
```

The exact sections depend on the compiler and source code.

---

## Inspect Symbols

```bash
nm build/objects/main.o
```

We can see symbols defined or referenced by the object file.

For example, `main.o` can contain references to:

```text
add
sub
mul
divide
```

These functions are implemented in different object files.

This leads us to the next stage.

---

# Stage 4 — Linking

At this point we have:

```text
main.o
add.o
sub.o
mul.o
div.o
```

But these object files are separate.

For example:

```text
main.o
   │
   ├── references add()
   ├── references sub()
   ├── references mul()
   └── references divide()
```

while:

```text
add.o      → defines add()
sub.o      → defines sub()
mul.o      → defines mul()
div.o      → defines divide()
```

The linker resolves these references and combines the required object code into an executable.

---

# Normal Object-File Linking

Before creating libraries, we can directly link the object files:

```bash
gcc build/objects/main.o \
    build/objects/add.o \
    build/objects/sub.o \
    build/objects/mul.o \
    build/objects/div.o \
    -o build/calculator
```

Now:

```text
.o files
   │
   │ linker
   ▼
executable
```

Run:

```bash
./build/calculator
```

Output:

```text
Addition       : 25
Subtraction    : 15
Multiplication : 100
Division       : 4
```

---

# Static Linking

Now we introduce a static library.

A static library normally has the extension:

```text
.a
```

We create:

```text
libcalculator.a
```

from:

```text
add.o
sub.o
mul.o
div.o
```

Use:

```bash
ar rcs lib/libcalculator.a \
    build/objects/add.o \
    build/objects/sub.o \
    build/objects/mul.o \
    build/objects/div.o
```

Now:

```text
add.o
sub.o
mul.o
div.o
    │
    │ ar
    ▼
libcalculator.a
```

---

# Static Linking the Application

Now we link `main.o` with the static library:

```bash
gcc build/objects/main.o \
    -Llib \
    -lcalculator \
    -o build/static/calculator_static
```

The linker finds:

```text
lib/libcalculator.a
```

and uses the required library code while producing the executable.

The result is:

```text
main.o
    │
    │
    ▼
libcalculator.a
    │
    │ static linking
    ▼
calculator_static
```

Run:

```bash
./build/static/calculator_static
```

Output:

```text
Addition       : 25
Subtraction    : 15
Multiplication : 100
Division       : 4
```

---

# Inspecting the Static Executable

Check its size:

```bash
size build/static/calculator_static
```

Check its type:

```bash
file build/static/calculator_static
```

Inspect symbols:

```bash
nm build/static/calculator_static
```

We can also inspect the ELF sections:

```bash
readelf -S build/static/calculator_static
```

---

# What Does Static Linking Mean?

With a static library, the linker obtains the required code from the `.a` archive and incorporates it into the executable.

Conceptually:

```text
libcalculator.a
       │
       │ linker
       ▼
calculator_static

calculator_static
┌──────────────────────┐
│ main                  │
│ add                   │
│ sub                   │
│ mul                   │
│ divide                │
└──────────────────────┘
```

The executable therefore contains the calculator library code it needs.

---

# Dynamic Linking

Now we create a shared library.

A shared library normally has the extension:

```text
.so
```

Before creating it, the library source should be compiled as position-independent code.

For example:

```bash
gcc -fPIC -c src/add.c -Iinclude -o build/objects/add_pic.o
gcc -fPIC -c src/sub.c -Iinclude -o build/objects/sub_pic.o
gcc -fPIC -c src/mul.c -Iinclude -o build/objects/mul_pic.o
gcc -fPIC -c src/div.c -Iinclude -o build/objects/div_pic.o
```

Now create the shared library:

```bash
gcc -shared \
    build/objects/add_pic.o \
    build/objects/sub_pic.o \
    build/objects/mul_pic.o \
    build/objects/div_pic.o \
    -o lib/libcalculator.so
```

Now:

```text
add_pic.o
sub_pic.o
mul_pic.o
div_pic.o
       │
       │ gcc -shared
       ▼
libcalculator.so
```

---

# Dynamically Linked Executable

Now link `main.o` against the shared library:

```bash
gcc build/objects/main.o \
    -Llib \
    -lcalculator \
    -Wl,-rpath,'$ORIGIN/../../lib' \
    -o build/dynamic/calculator_dynamic
```

The result is:

```text
main.o
   │
   │
   ▼
libcalculator.so
   │
   │ dynamic linking
   ▼
calculator_dynamic
```

The runtime executable knows that it depends on the shared library.

---

# Checking Dynamic Dependencies

Use:

```bash
ldd build/dynamic/calculator_dynamic
```

You should see an entry for:

```text
libcalculator.so
```

along with the normal system libraries required by the executable.

We can also inspect the ELF dynamic section:

```bash
readelf -d build/dynamic/calculator_dynamic
```

Look for:

```text
NEEDED
```

This indicates shared-library dependencies recorded in the executable.

---

# What Does Dynamic Linking Mean?

With dynamic linking, the shared library remains a separate file.

Conceptually:

```text
calculator_dynamic
┌──────────────────────┐
│ main                  │
│ references calculator │
└──────────┬───────────┘
           │
           │ runtime dependency
           ▼
┌──────────────────────┐
│ libcalculator.so     │
│ add                  │
│ sub                  │
│ mul                  │
│ divide               │
└──────────────────────┘
```

The executable does not contain the entire calculator library in the same way as the static-linked version.

The dynamic loader is involved in loading required shared libraries.

---

# Static vs Dynamic Linking

Now we have two executables:

```text
build/static/calculator_static
build/dynamic/calculator_dynamic
```

and two libraries:

```text
lib/libcalculator.a
lib/libcalculator.so
```

We can compare them.

---

# Comparison 1 — File Size

Use:

```bash
ls -lh \
    build/static/calculator_static \
    build/dynamic/calculator_dynamic \
    lib/libcalculator.a \
    lib/libcalculator.so
```

We can also use:

```bash
size build/static/calculator_static
size build/dynamic/calculator_dynamic
```

The exact numbers depend on:

* compiler version
* architecture
* optimization level
* source code
* libraries
* linker options

Therefore, we should measure the files rather than assuming exact sizes.

---

# Comparison 2 — Dependencies

Static version:

```bash
ldd build/static/calculator_static
```

Dynamic version:

```bash
ldd build/dynamic/calculator_dynamic
```

The dynamic version should show:

```text
libcalculator.so
```

as a runtime dependency.

This demonstrates an important difference:

```text
Static:
Executable ──> contains linked calculator code

Dynamic:
Executable ──> depends on libcalculator.so
```

---

# Comparison 3 — ELF Information

Inspect both executables:

```bash
readelf -h build/static/calculator_static
readelf -h build/dynamic/calculator_dynamic
```

Inspect sections:

```bash
readelf -S build/static/calculator_static
readelf -S build/dynamic/calculator_dynamic
```

Inspect the dynamic section:

```bash
readelf -d build/static/calculator_static
readelf -d build/dynamic/calculator_dynamic
```

The dynamic executable will contain dynamic-linking information and shared-library dependency entries.

---

# Comparison 4 — Symbols

Use:

```bash
nm build/static/calculator_static
```

and:

```bash
nm build/dynamic/calculator_dynamic
```

This allows us to investigate how symbols appear in the two executables.

For more detailed symbol information:

```bash
readelf -s build/static/calculator_static
```

```bash
readelf -s build/dynamic/calculator_dynamic
```

---

# Static Linking Advantages

Static linking has several advantages.

### Self-contained deployment

The application can contain the required library code directly.

```text
application
     │
     └── required library code
```

This can make deployment simpler because a separate copy of that particular library is not required at runtime.

### Predictability

The application is linked against a specific version of the library at build time.

This can be useful when reproducibility and controlled environments are important.

### Fewer runtime library concerns

There is less dependence on the runtime presence of the particular application library.

---

# Static Linking Disadvantages

### Larger executable

Library code linked into the executable can increase its size.

### Library updates

If the library implementation needs to change, applications generally need to be rebuilt and relinked to incorporate the new static library version.

### Duplication

If many applications statically link the same library, each executable can contain its own copy of that library code.

For example:

```text
program A ──> calculator code
program B ──> calculator code
program C ──> calculator code
```

This can increase storage usage.

---

# Dynamic Linking Advantages

### Smaller application-specific executable

The shared library remains separate from the executable.

```text
program
   │
   └──> shared library
```

### Library sharing

Multiple processes can use the same shared-library file, and the operating system can share suitable read-only code pages in memory.

### Independent library updates

A compatible shared library can often be updated without rebuilding every application that uses it.

### Useful for large systems

Shared libraries are common when many applications use the same functionality.

---

# Dynamic Linking Disadvantages

### Runtime dependency

The required shared library must be available and compatible at runtime.

```text
program
   │
   └──> libcalculator.so
```

If the library cannot be found, the program may fail to start.

### Version and ABI compatibility

Updating a shared library can cause problems if the new version is not binary-compatible with applications expecting the old interface.

### More runtime complexity

The dynamic loader has to locate and load required shared libraries and perform the required runtime linking work.

---

# Is Dynamic Linking Better?

Not always.

It is better to think of the two approaches as solving different problems.

```text
                 Linking
                    │
          ┌─────────┴─────────┐
          │                   │
       Static              Dynamic
          │                   │
          ▼                   ▼
 Self-contained          Shared library
 executable              dependency
```

Dynamic linking can be advantageous when:

* many applications share the same library
* reducing application-specific executable size matters
* libraries need to be updated independently
* shared memory usage is beneficial

Static linking can be advantageous when:

* self-contained deployment is important
* a controlled library version is required
* runtime library availability is a concern
* predictable deployment is more important than sharing

Therefore:

```text
Dynamic linking ≠ always better
Static linking  ≠ always better
```

The appropriate choice depends on the application and deployment requirements.

---

# Important Clarification About "Static Executable"

In this project, when we say:

```text
calculator_static
```

we mean that our **calculator library is statically linked** into the application.

That does not necessarily mean that the entire executable is completely statically linked.

For example, the executable may still dynamically depend on the system C library.

We can investigate this using:

```bash
ldd build/static/calculator_static
```

A completely statically linked executable is a separate experiment and can be produced with options such as:

```bash
gcc -static ...
```

when the required static system libraries are available.

A fully static executable can be considerably larger, so it should not be confused with simply statically linking our own `libcalculator.a`.

---

# Complete Workflow

The entire project can now be represented as:

```text
                    SOURCE CODE

       ┌────────┬────────┬────────┬────────┬────────┐
       │        │        │        │        │        │
    main.c    add.c    sub.c    mul.c    div.c
       │        │        │        │        │
       └────────┴────────┴────────┴────────┘
                         │
                         │ gcc -E
                         ▼
                    PREPROCESSING

       main.i  add.i  sub.i  mul.i  div.i
                         │
                         │ gcc -S
                         ▼
                     ASSEMBLY

       main.s  add.s  sub.s  mul.s  div.s
                         │
                         │ gcc -c
                         ▼
                    OBJECT FILES

       main.o  add.o  sub.o  mul.o  div.o
                         │
             ┌───────────┴────────────┐
             │                        │
             │                        │
             ▼                        ▼
       STATIC LIBRARY           SHARED LIBRARY
             │                        │
             ▼                        ▼
     libcalculator.a          libcalculator.so
             │                        │
             │                        │
             ▼                        ▼
      calculator_static       calculator_dynamic
             │                        │
             └──────────┬─────────────┘
                        │
                        ▼
                   COMPARISON
                        │
          ┌─────────────┼─────────────┐
          │             │             │
         size          ldd         readelf
          │             │             │
          └─────────────┼─────────────┘
                        │
                        ▼
                FINAL OBSERVATIONS
```

---

# Commands Learned

## Preprocessing

```bash
gcc -E source.c -Iinclude -o source.i
```

## Compilation to Assembly

```bash
gcc -S source.i -o source.s
```

## Assembly to Object

```bash
gcc -c source.s -o source.o
```

## Static Library

```bash
ar rcs libcalculator.a *.o
```

## Position Independent Code

```bash
gcc -fPIC -c source.c -o source.o
```

## Shared Library

```bash
gcc -shared *.o -o libcalculator.so
```

## Linking

```bash
gcc main.o -Llib -lcalculator -o calculator
```

## ELF Inspection

```bash
file program
readelf -h program
readelf -S program
readelf -s program
readelf -d program
```

## Symbol Inspection

```bash
nm program
```

## Dependency Inspection

```bash
ldd program
```

## Size Inspection

```bash
size program
ls -lh program
```

---

# What I Learned

After completing this project, I should be able to explain:

### Compilation

```text
What is preprocessing?
What is compilation?
What is assembly?
What is an object file?
```

### GCC

```text
What does gcc -E do?
What does gcc -S do?
What does gcc -c do?
```

### Object Files

```text
What is an ELF object file?
What are ELF sections?
What are symbols?
What is relocation?
```

### Linking

```text
What does the linker do?
Why can't main.o execute by itself?
How are symbols resolved?
```

### Static Linking

```text
What is a .a file?
How is libcalculator.a created?
How is it linked into an executable?
```

### Dynamic Linking

```text
What is a .so file?
Why is -fPIC used?
What does a shared library provide?
What does ldd show?
What does the ELF NEEDED entry mean?
```

### Comparison

```text
Why can static executables be larger?
Why can dynamic linking reduce duplication?
Why does dynamic linking introduce runtime dependencies?
Why can static linking be useful for controlled deployment?
```

---

# Final Learning

The most important thing learned from this project is that:

```text
gcc program.c -o program
```

is not one mysterious operation.

It represents a sequence of stages:

```text
Preprocessing
     ↓
Compilation
     ↓
Assembly
     ↓
Linking
```

For a multi-file program:

```text
multiple .c files
       ↓
multiple .o files
       ↓
library / linking
       ↓
ELF executable
```

And linking can be performed using different strategies:

```text
Static Linking
      ↓
library code incorporated during linking

Dynamic Linking
      ↓
shared library remains a runtime dependency
```

Understanding these stages makes it easier to understand GCC, ELF binaries, libraries, linkers, loaders, and Linux systems programming.

---

# Future Experiments

After completing the basic project, the following experiments can be added:

```text
1. Compare -O0 and -O2
2. Compare executable sizes
3. Inspect .text, .data and .bss
4. Investigate symbols using nm
5. Investigate relocations using readelf
6. Inspect assembly generated with different optimization levels
7. Add more functions to the library
8. Compare static and shared libraries after increasing library size
9. Investigate what happens when libcalculator.so is removed
10. Investigate library search paths
11. Experiment with RPATH and LD_LIBRARY_PATH
12. Investigate a completely statically linked executable
```

The project can therefore start with a simple calculator and gradually become an experiment in **GCC, ELF, static linking, dynamic linking, and Linux binary analysis**.
