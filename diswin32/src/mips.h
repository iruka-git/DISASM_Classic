/* mips.h.  Mips opcode list for GDB, the GNU debugger.
   Copyright 1993, 1995 Free Software Foundation, Inc.
   Contributed by Ralph Campbell and OSF
   Commented and modified by Ian Lance Taylor, Cygnus Support

This file is part of GDB, GAS, and the GNU binutils.

GDB, GAS, and the GNU binutils are free software; you can redistribute
them and/or modify them under the terms of the GNU General Public
License as published by the Free Software Foundation; either version
1, or (at your option) any later version.

GDB, GAS, and the GNU binutils are distributed in the hope that they
will be useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this file; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

/* These are bit masks and shift counts to use to access the various
   fields of an instruction.  To retrieve the X field of an
   instruction, use the expression
        (i >> OP_SH_X) & OP_MASK_X
   To set the same field (to j), use
        i = (i &~ (OP_MASK_X << OP_SH_X)) | (j << OP_SH_X)

   Make sure you use fields that are appropriate for the instruction,
   of course.  

   The 'i' format uses OP, RS, RT and IMMEDIATE.  

   The 'j' format uses OP and TARGET.

   The 'r' format uses OP, RS, RT, RD, SHAMT and FUNCT.

   The 'b' format uses OP, RS, RT and DELTA.

   The floating point 'i' format uses OP, RS, RT and IMMEDIATE.

   The floating point 'r' format uses OP, FMT, FT, FS, FD and FUNCT.

   A breakpoint instruction uses OP, CODE and SPEC (10 bits of the
   breakpoint instruction are not defined; Kane says the breakpoint
   code field in BREAK is 20 bits; yet MIPS assemblers and debuggers
   only use ten bits).

   The syscall instruction uses SYSCALL.

   The general coprocessor instructions use COPZ.  */

#define OP_MASK_OP              0x3fL
#define OP_SH_OP                26L
#define OP_MASK_RS              0x1fL
#define OP_SH_RS                21L
#define OP_MASK_FR              0x1fL
#define OP_SH_FR                21L
#define OP_MASK_FMT             0x1fL
#define OP_SH_FMT               21L
#define OP_MASK_BCC             0x7L
#define OP_SH_BCC               18L
#define OP_MASK_CODE            0x3ffL
#define OP_SH_CODE              16L
#define OP_MASK_RT              0x1fL
#define OP_SH_RT                16L
#define OP_MASK_FT              0x1fL
#define OP_SH_FT                16L
#define OP_MASK_CACHE           0x1fL
#define OP_SH_CACHE             16L
#define OP_MASK_RD              0x1fL
#define OP_SH_RD                11L
#define OP_MASK_FS              0x1fL
#define OP_SH_FS                11L
#define OP_MASK_PREFX           0x1fL
#define OP_SH_PREFX             11L
#define OP_MASK_CCC             0x7L
#define OP_SH_CCC               8L
#define OP_MASK_SYSCALL         0xfffffL
#define OP_SH_SYSCALL           6L
#define OP_MASK_SHAMT           0x1fL
#define OP_SH_SHAMT             6L
#define OP_MASK_FD              0x1fL
#define OP_SH_FD                6L
#define OP_MASK_TARGET          0x3ffffffL
#define OP_SH_TARGET            0L
#define OP_MASK_COPZ            0x1ffffffL
#define OP_SH_COPZ              0L
#define OP_MASK_IMMEDIATE       0xffffL
#define OP_SH_IMMEDIATE         0L
#define OP_MASK_DELTA           0xffffL
#define OP_SH_DELTA             0L
#define OP_MASK_FUNCT           0x3fL
#define OP_SH_FUNCT             0L
#define OP_MASK_SPEC            0x3fL
#define OP_SH_SPEC              0L

/* This structure holds information for a particular instruction.  */

struct mips_opcode
{
  /* The name of the instruction.  */
  const char *name;
  /* A string describing the arguments for this instruction.  */
  const char *args;
  /* The basic opcode for the instruction.  When assembling, this
     opcode is modified by the arguments to produce the actual opcode
     that is used.  If pinfo is INSN_MACRO, then this is instead the
     ISA level of the macro (0 or 1 is always supported, 2 is ISA 2,
     etc.).  */
  unsigned int match;
  /* If pinfo is not INSN_MACRO, then this is a bit mask for the
     relevant portions of the opcode when disassembling.  If the
     actual opcode anded with the match field equals the opcode field,
     then we have found the correct instruction.  If pinfo is
     INSN_MACRO, then this field is the macro identifier.  */
  unsigned int mask;
  /* For a macro, this is INSN_MACRO.  Otherwise, it is a collection
     of bits describing the instruction, notably any relevant hazard
     information.  */
  unsigned int pinfo;
};

/* These are the characters which may appears in the args field of an
   instruction.  They appear in the order in which the fields appear
   when the instruction is used.  Commas and parentheses in the args
   string are ignored when assembling, and written into the output
   when disassembling.

   Each of these characters corresponds to a mask field defined above.

   "<" 5 bit shift amount (OP_*_SHAMT)
   ">" shift amount between 32 and 63, stored after subtracting 32 (OP_*_SHAMT)
   "a" 26 bit target address (OP_*_TARGET)
   "b" 5 bit base register (OP_*_RS)
   "c" 10 bit breakpoint code (OP_*_CODE)
   "d" 5 bit destination register specifier (OP_*_RD)
   "h" 5 bit prefx hint (OP_*_PREFX)
   "i" 16 bit unsigned immediate (OP_*_IMMEDIATE)
   "j" 16 bit signed immediate (OP_*_DELTA)
   "k" 5 bit cache opcode in target register position (OP_*_CACHE)
   "o" 16 bit signed offset (OP_*_DELTA)
   "p" 16 bit PC relative branch target address (OP_*_DELTA)
   "r" 5 bit same register used as both source and target (OP_*_RS)
   "s" 5 bit source register specifier (OP_*_RS)
   "t" 5 bit target register (OP_*_RT)
   "u" 16 bit upper 16 bits of address (OP_*_IMMEDIATE)
   "v" 5 bit same register used as both source and destination (OP_*_RS)
   "w" 5 bit same register used as both target and destination (OP_*_RT)
   "C" 25 bit coprocessor function code (OP_*_COPZ)
   "B" 20 bit syscall function code (OP_*_SYSCALL)
   "x" accept and ignore register name
   "z" must be zero register

   Floating point instructions:
   "D" 5 bit destination register (OP_*_FD)
   "M" 3 bit compare condition code (OP_*_CCC) (only used for mips4 and up)
   "N" 3 bit branch condition code (OP_*_BCC) (only used for mips4 and up)
   "S" 5 bit fs source 1 register (OP_*_FS)
   "T" 5 bit ft source 2 register (OP_*_FT)
   "R" 5 bit fr source 3 register (OP_*_FR)
   "V" 5 bit same register used as floating source and destination (OP_*_FS)
   "W" 5 bit same register used as floating target and destination (OP_*_FT)

   Coprocessor instructions:
   "E" 5 bit target register (OP_*_RT)
   "G" 5 bit destination register (OP_*_RD)

   Macro instructions:
   "A" General 32 bit expression
   "I" 32 bit immediate
   "F" 64 bit floating point constant in .rdata
   "L" 64 bit floating point constant in .lit8
   "f" 32 bit floating point constant
   "l" 32 bit floating point constant in .lit4
*/

/* These are the bits which may be set in the pinfo field of an
   instructions, if it is not equal to INSN_MACRO.  */

/* Modifies the general purpose register in OP_*_RD.  */
#define INSN_WRITE_GPR_D            0x00000001L
/* Modifies the general purpose register in OP_*_RT.  */
#define INSN_WRITE_GPR_T            0x00000002L
/* Modifies general purpose register 31.  */
#define INSN_WRITE_GPR_31           0x00000004L
/* Modifies the floating point register in OP_*_FD.  */
#define INSN_WRITE_FPR_D            0x00000008L
/* Modifies the floating point register in OP_*_FS.  */
#define INSN_WRITE_FPR_S            0x00000010L
/* Modifies the floating point register in OP_*_FT.  */
#define INSN_WRITE_FPR_T            0x00000020L
/* Reads the general purpose register in OP_*_RS.  */
#define INSN_READ_GPR_S             0x00000040L
/* Reads the general purpose register in OP_*_RT.  */
#define INSN_READ_GPR_T             0x00000080L
/* Reads the floating point register in OP_*_FS.  */
#define INSN_READ_FPR_S             0x00000100L
/* Reads the floating point register in OP_*_FT.  */
#define INSN_READ_FPR_T             0x00000200L
/* Reads the floating point register in OP_*_FR.  */
#define INSN_READ_FPR_R             0x00000400L
/* Modifies coprocessor condition code.  */
#define INSN_WRITE_COND_CODE        0x00000800L
/* Reads coprocessor condition code.  */
#define INSN_READ_COND_CODE         0x00001000L
/* TLB operation.  */
#define INSN_TLB                    0x00002000L
/* Reads coprocessor register other than floating point register.  */
#define INSN_COP                    0x00004000L
/* Instruction loads value from memory, requiring delay.  */
#define INSN_LOAD_MEMORY_DELAY      0x00008000L
/* Instruction loads value from coprocessor, requiring delay.  */
#define INSN_LOAD_COPROC_DELAY      0x00010000L
/* Instruction has unconditional branch delay slot.  */
#define INSN_UNCOND_BRANCH_DELAY    0x00020000L
/* Instruction has conditional branch delay slot.  */
#define INSN_COND_BRANCH_DELAY      0x00040000L
/* Conditional branch likely: if branch not taken, insn nullified.  */
#define INSN_COND_BRANCH_LIKELY     0x00080000L
/* Moves to coprocessor register, requiring delay.  */
#define INSN_COPROC_MOVE_DELAY      0x00100000L
/* Loads coprocessor register from memory, requiring delay.  */
#define INSN_COPROC_MEMORY_DELAY    0x00200000L
/* Reads the HI register.  */
#define INSN_READ_HI                0x00400000L
/* Reads the LO register.  */
#define INSN_READ_LO                0x00800000L
/* Modifies the HI register.  */
#define INSN_WRITE_HI               0x01000000L
/* Modifies the LO register.  */
#define INSN_WRITE_LO               0x02000000L
/* Takes a trap (easier to keep out of delay slot).  */
#define INSN_TRAP                   0x04000000L
/* Instruction stores value into memory.  */
#define INSN_STORE_MEMORY           0x08000000L
/* MIPS ISA field--CPU level at which insn is supported.  */
#define INSN_ISA                    0x70000000L
/* MIPS ISA 2 instruction (R6000 or R4000).  */
#define INSN_ISA2                   0x10000000L
/* MIPS ISA 3 instruction (R4000).  */
#define INSN_ISA3                   0x20000000L
/* MIPS R4650 instruction.  */
#define INSN_4650                   0x30000000L
/* MIPS ISA 4 instruction (R8000).  */
#define INSN_ISA4                   0x40000000L
/* LSI R4010 instruction.  */
#define INSN_4010                   0x50000000L
/* NEC VR4100 instruction. */
#define INSN_4100                   0x60000000L

/* Instruction is actually a macro.  It should be ignored by the
   disassembler, and requires special treatment by the assembler.  */
#define INSN_MACRO                  0xffffffff

/* This is a list of macro expanded instructions.
 *
 * _I appended means immediate
 * _A appended means address
 * _AB appended means address with base register
 * _D appended means 64 bit floating point constant
 * _S appended means 32 bit floating point constant
 */
enum {
    M_ABS,
    M_ADD_I,
    M_ADDU_I,
    M_AND_I,
    M_BEQ_I,
    M_BEQL_I,
    M_BGE,
    M_BGEL,
    M_BGE_I,
    M_BGEL_I,
    M_BGEU,
    M_BGEUL,
    M_BGEU_I,
    M_BGEUL_I,
    M_BGT,
    M_BGTL,
    M_BGT_I,
    M_BGTL_I,
    M_BGTU,
    M_BGTUL,
    M_BGTU_I,
    M_BGTUL_I,
    M_BLE,
    M_BLEL,
    M_BLE_I,
    M_BLEL_I,
    M_BLEU,
    M_BLEUL,
    M_BLEU_I,
    M_BLEUL_I,
    M_BLT,
    M_BLTL,
    M_BLT_I,
    M_BLTL_I,
    M_BLTU,
    M_BLTUL,
    M_BLTU_I,
    M_BLTUL_I,
    M_BNE_I,
    M_BNEL_I,
    M_DABS,
    M_DADD_I,
    M_DADDU_I,
    M_DDIV_3,
    M_DDIV_3I,
    M_DDIVU_3,
    M_DDIVU_3I,
    M_DIV_3,
    M_DIV_3I,
    M_DIVU_3,
    M_DIVU_3I,
    M_DLA_AB,
    M_DLI,
    M_DMUL,
    M_DMUL_I, 
    M_DMULO,
    M_DMULO_I, 
    M_DMULOU,
    M_DMULOU_I, 
    M_DREM_3,
    M_DREM_3I,
    M_DREMU_3,
    M_DREMU_3I,
    M_DSUB_I,
    M_DSUBU_I,
    M_J_A,
    M_JAL_1,
    M_JAL_2,
    M_JAL_A,
    M_L_DOB,
    M_L_DAB,
    M_LA_AB,
    M_LB_A,
    M_LB_AB,
    M_LBU_A,
    M_LBU_AB,
    M_LD_A,
    M_LD_OB,
    M_LD_AB,
    M_LDC1_AB,
    M_LDC2_AB,
    M_LDC3_AB,
    M_LDL_AB,
    M_LDR_AB,
    M_LH_A,
    M_LH_AB,
    M_LHU_A,
    M_LHU_AB,
    M_LI,
    M_LI_D,
    M_LI_DD,
    M_LI_S,
    M_LI_SS,
    M_LL_AB,
    M_LLD_AB,
    M_LS_A,
    M_LW_A,
    M_LW_AB,
    M_LWC0_A,
    M_LWC0_AB,
    M_LWC1_A,
    M_LWC1_AB,
    M_LWC2_A,
    M_LWC2_AB,
    M_LWC3_A,
    M_LWC3_AB,
    M_LWL_A,
    M_LWL_AB,
    M_LWR_A,
    M_LWR_AB,
    M_LWU_AB,
    M_MUL,
    M_MUL_I, 
    M_MULO,
    M_MULO_I, 
    M_MULOU,
    M_MULOU_I, 
    M_NOR_I,
    M_OR_I,
    M_REM_3,
    M_REM_3I,
    M_REMU_3,
    M_REMU_3I,
    M_ROL,
    M_ROL_I,
    M_ROR,
    M_ROR_I,
    M_S_DA,
    M_S_DOB,
    M_S_DAB,
    M_S_S,
    M_SC_AB,
    M_SCD_AB,
    M_SD_A,
    M_SD_OB,
    M_SD_AB,
    M_SDC1_AB,
    M_SDC2_AB,
    M_SDC3_AB,
    M_SDL_AB,
    M_SDR_AB,
    M_SEQ,
    M_SEQ_I,
    M_SGE,
    M_SGE_I,
    M_SGEU,
    M_SGEU_I,
    M_SGT,
    M_SGT_I,
    M_SGTU,
    M_SGTU_I,
    M_SLE,
    M_SLE_I,
    M_SLEU,
    M_SLEU_I,
    M_SLT_I,
    M_SLTU_I,
    M_SNE,
    M_SNE_I,
    M_SB_A,
    M_SB_AB,
    M_SH_A,
    M_SH_AB,
    M_SW_A,
    M_SW_AB,
    M_SWC0_A,
    M_SWC0_AB,
    M_SWC1_A,
    M_SWC1_AB,
    M_SWC2_A,
    M_SWC2_AB,
    M_SWC3_A,
    M_SWC3_AB,
    M_SWL_A,
    M_SWL_AB,
    M_SWR_A,
    M_SWR_AB,
    M_SUB_I,
    M_SUBU_I,
    M_TEQ_I,
    M_TGE_I,
    M_TGEU_I,
    M_TLT_I,
    M_TLTU_I,
    M_TNE_I,
    M_TRUNCWD,
    M_TRUNCWS,
    M_ULD,
    M_ULD_A,
    M_ULH,
    M_ULH_A,
    M_ULHU,
    M_ULHU_A,
    M_ULW,
    M_ULW_A,
    M_USH,
    M_USH_A,
    M_USW,
    M_USW_A,
    M_USD,
    M_USD_A,
    M_XOR_I
};

/* The order of overloaded instructions matters.  Label arguments and
   register arguments look the same. Instructions that can have either
   for arguments must apear in the correct order in this table for the
   assembler to pick the right one. In other words, entries with
   immediate operands must apear after the same instruction with
   registers.

   Many instructions are short hand for other instructions (i.e., The
   jal <register> instruction is short for jalr <register>).  */

extern const struct mips_opcode mips_opcodes[];
extern const int bfd_mips_num_opcodes;
#define NUMOPCODES bfd_mips_num_opcodes
