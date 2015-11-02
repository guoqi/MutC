//
// Created by guoqi on 10/25/15.
//

#ifndef MUTCC_INSTRUCTION_H
#define MUTCC_INSTRUCTION_H

// 以下为支持的所有指令的宏定义
#define HALT    0x00
#define RET     0x01
#define PUSHSP  0x02
#define POPSP   0x03

#define CALL    0x40
#define JZ      0x41
#define JNZ     0x42
#define JMP     0x43

#define MOV     0x80
// TODO 位运算暂时先不实现，留个坑
#define AND     0x81 // 按位与
#define OR      0x82 // 按位或
#define NOT     0x83 // 按位取反
#define XOR     0x84 // 按位异或
#define ADD     0x85
#define SUB     0x86
#define MUL     0x87
#define DIV     0x88
#define MOD     0x89
#define LAND    0x8A // 逻辑与
#define LOR     0x8B // 逻辑或
#define LNOT    0x8C // 逻辑非

#define CG      0xC0 // 大于比较
#define CL      0xC1 // 小于比较
#define CGE     0xC2 // 大于等于比较
#define CLE     0xC3 // 小于等于比较
#define CE      0xC4 // 等于比较
#define CNE     0xC5 // 不等于比较



// 以下为特殊字段地址定义
#define BP      0xFFFF
#define SP      0xFFFE
#define NIL     0xFFFD // 表示此地址空置


// 以下为二元指令中方式字的定义
#define MM      0x00 // 内存-内存
#define MI      0x01 // 内存-立即数
#define IM      0x02 // 立即数-内存
#define II      0x03 // 立即数-立即数

// 以下为涉及到地址的一元指令中方式字的定义
#define AD      0x00    // 绝对地址
#define RD      0x01   // 相对地址，相对于当前BP寄存器的值的偏移量

#endif //MUTCC_INSTRUCTION_H
