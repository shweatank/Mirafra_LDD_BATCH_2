#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
//Alphabets
#define A 0x1E
#define B 0x30
#define C 0x2E
#define D 0x20
#define E 0x12
#define F 0x21
#define G 0x22
#define H 0x23
#define I 0x17
#define J 0x24
#define K 0x25
#define L 0x26
#define M 0x32
#define N 0x31
#define O 0x18
#define P 0x19
#define Q 0x10
#define R 0x13
#define S 0x1F
#define T 0x14
#define U 0x16
#define V 0x2F
#define W 0x11
#define X 0x2D
#define Y 0x15
#define Z 0x2C

//Numbers 
#define one_1 0x35
#define two_2 0x1F
#define three_3 0x1A
#define four_4 0x06
#define five_5 0x07
#define six_6 0x08
#define seven_7 0x09
#define eight_8 0x0A
#define nine_9 0x0B
#define zero_0 0x0C

//
#define ENTER 0x0E
#define ESCAPE 0x0F
#define BACKSPACE 0x1C
#define SHIFT_LEFT 0x2A
#define SHIFT_RIGHT 0x36
#define CTRL_LEFT 0x1D
#define ALT_LEFT 0x38
#define CAPS_LOCK 0x3A

//function keys 
#define F1 0x3B
#define F2 0x3C
#define F3 0x3D
#define F4 0x3E
#define F5 0x3F
#define F6 0x40
#define F7 0x41
#define F8 0x42
#define F9 0x43
#define F10 0x44
#define F11 0x57
#define F12 0x58

#define SPACEBAR 0x2B
#define DELETE 0x4F
#define END 0x50
#define PAGE_DOWN 0x51
#define PAGE_UP 0x52
#define HOME 0x53
#define ARROW_LEFT 0x4B
#define ARROW_RIGHT 0x4D
#define ARROW_UP 0x48
#define ARROW_DOWN 0x50
#define NUM_LOCK 0x77



#endif 