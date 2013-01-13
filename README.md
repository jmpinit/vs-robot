vs-robot
========

6.270 robot entry for 2013 competition at MIT

Dependencies
============

* AVR-GCC

Compiling & Installing
======================

1) Clone this repo.
2) Clone the JoyOS repo from [here](https://github.com/sixtwoseventy/joyos).
3) Navigate into the root of the JoyOS directory structure (.../joyos/).
4) Paste the contents of this repo there. The JoyOS source will be changed as required.
5) Run "make" in the root of the JoyOS directory structure. Hopefully compilation completes with no errors.
6) Run "make program" with the HappyBoard connected to flash everything into it.

Setting up for development
==========================

1) Clone this repo.
2) Clone the JoyOS repo from [here](https://github.com/sixtwoseventy/joyos).
3) Move the Makefile from the root of this repo to the root of JoyOS.
4) Delete the joyos/user/robot directory and replace with a symbolic link to the vs-robot/robot directory.
