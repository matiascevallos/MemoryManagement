To run Part 1 in a Linux Enironment, follow the following steps:
1. Download and save the part1.c file and input1.txt file in a directory that you want to compile it on.
2. Once you have moved both of the files in your preferred directory, open up the terminal.
3. Run the following command:gcc part1.c -o part1
4. This will create an executable c program called part1.
5. Then, run this code on the terminal:.\part1
6. This will lead to a printout on your terminal, meaning that the program has completed.

If needed, the input1.txt file can be changed, but because of the way the code is, be sure to leave an empty line after the last virtual adress e.g.
...
v2
v3
v4

An extra line is added above^.

The design choices implemented for the C file make it easier for the user to understand the code as you progress through it, moreoever, variable names are relavent so that the user is aware of what each variable does and can follow along. Also, a method was used to actually evaluate the offset and the page number using bitwise operations in C, which would would make the code more readable and more structured. 