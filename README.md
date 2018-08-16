# no-aslr

This utility allows you to run a program without ASLR (supports Linux and
macOS). It is extremely simple.

## Motivation

Sometimes you just wish to disable ASLR. There can be many reasons to want to
do so. It might be because you want to debug a program, and somehow the
behavior of the program is not consistent across because of memory addresses.
Perhaps your program writes memory addresses to a log and your logs are full
of different memory addresses. Perhaps your program uses a special allocator
that really wants the memory addresses to be the same. Perhaps your program
uses some sort of *dumping mechanism*, like [the Emacs
dumper](https://lwn.net/Articles/707615/), so execution state can be saved
across program runs. There can be many reasons to want to do so.

## How to Use

The program is a single-file executable with no other dependencies other than
libc. Simply compile it. On many systems you can ask `make` to do this
automatically using builtin rules:

    make no-aslr

And then you may execute your program by providing the name and the arguments
to your program to `no-aslr`. For example

    ./no-aslr /bin/ls -l

You can also choose not to provide the full path. This program will use the
suitable functions to search your `PATH` for the executable. So the previous
example can be rewritten as

    ./no-aslr ls -l

## Support

This program supports Linux and macOS.

## License

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License (a copy of which is present in
this repository), or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
