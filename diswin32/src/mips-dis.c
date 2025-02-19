/* Print mips instructions for GDB, the GNU debugger, or for objdump.
   Copyright 1989, 1991, 1992 Free Software Foundation, Inc.
   Contributed by Nobuyuki Hikichi(hikichi@sra.co.jp).

This file is part of GDB.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include <string.h>

#include "ansidecl.h"
/*#include "sysdep.h"*/

#include "dis-asm.h"

#include "mips.h"

/* Mips instructions are never longer than this many bytes.  */
#define MAXLEN 4

/* FIXME: This should be shared with gdb somehow.  */

static char *reg_names_1[] = 
{	"zero",	"at",	"v0",	"v1",	"a0",	"a1",	"a2",	"a3", 
	"t0",	"t1",	"t2",	"t3",	"t4",	"t5",	"t6",	"t7", 
	"s0",	"s1",	"s2",	"s3",	"s4",	"s5",	"s6",	"s7", 
	"t8",	"t9",	"k0",	"k1",	"gp",	"sp",	"s8",	"ra", 
	"sr",	"lo",	"hi",	"bad",	"cause","pc",    
	"f0",   "f1",   "f2",   "f3",   "f4",   "f5",   "f6",   "f7", 
	"f8",   "f9",   "f10",  "f11",  "f12",  "f13",  "f14",  "f15", 
	"f16",  "f17",  "f18",  "f19",  "f20",  "f21",  "f22",  "f23",
	"f24",  "f25",  "f26",  "f27",  "f28",  "f29",  "f30",  "f31",
	"fsr",  "fir",  "fp",   "inx",  "rand", "tlblo","ctxt", "tlbhi",
	"epc",  "prid"
};

static char *reg_names_2[] = 
{	"r0",	"r1",	"r2",	"r3",	"r4",	"r5",	"r6",	"r7", 
	"r8",	"r9",	"r10",	"r11",	"r12",	"r13",	"r14",	"r15", 
	"r16",	"r17",	"r18",	"r19",	"r20",	"r21",	"r22",	"r23", 
	"r24",	"r25",	"r26",	"r27",	"r28",	"r29",	"r30",	"r31", 
	"sr",	"lo",	"hi",	"bad",	"cause","pc",    
	"f0",   "f1",   "f2",   "f3",   "f4",   "f5",   "f6",   "f7", 
	"f8",   "f9",   "f10",  "f11",  "f12",  "f13",  "f14",  "f15", 
	"f16",  "f17",  "f18",  "f19",  "f20",  "f21",  "f22",  "f23",
	"f24",  "f25",  "f26",  "f27",  "f28",  "f29",  "f30",  "f31",
	"fsr",  "fir",  "fp",   "inx",  "rand", "tlblo","ctxt", "tlbhi",
	"epc",  "prid"
};

static char **reg_names=reg_names_1;

set_alternate_regname()
{
	reg_names=reg_names_2;
}

#if	0
	#define	REGPRIS	"$%s"
	#define	HEXPRIS	"0x%x"
	#define	HEXPRI4	"%d"
	#define	MHXPRI4	"-%d"
#endif


#define	REGPRIS	"%s"

char	hexpri[16]="-$%lx";

#define	HEXPRIS	(&hexpri[1])
#define	HEXPRI4	(&hexpri[1])
#define	MHXPRI4	(&hexpri[0])

extern	set_ea(char *buf,int addr);






set_alternate_hexa()
{
	strcpy(hexpri,"-0x%lx");
}



//
//	オペランドの解析表示
//
/* subroutine */
static void	print_insn_arg (d, l, pc, info)
     const char *d;
     unsigned int l;
     bfd_vma pc;
     struct disassemble_info *info;
{
  int delta;

  switch (*d)
    {
    case ',':
    case '(':
    case ')':
      (*info->fprintf_func) (info->stream, "%c", *d);
      break;

    case 's':
    case 'b':
    case 'r':
    case 'v':
      (*info->fprintf_func) (info->stream, REGPRIS,
			     reg_names[(l >> OP_SH_RS) & OP_MASK_RS]);
      break;

    case 't':
    case 'w':
      (*info->fprintf_func) (info->stream, REGPRIS,
			     reg_names[(l >> OP_SH_RT) & OP_MASK_RT]);
      break;

    case 'i':
    case 'u':
      (*info->fprintf_func) (info->stream, HEXPRIS,
			(l >> OP_SH_IMMEDIATE) & OP_MASK_IMMEDIATE);
      break;

    case 'j': /* same as i, but sign-extended */
    case 'o':
      delta = (l >> OP_SH_DELTA) & OP_MASK_DELTA;
      if (delta & 0x8000)
	delta |= ~0xffffL;


	if(delta>=0) {
      (*info->fprintf_func) (info->stream, HEXPRI4,
			     delta);
	}else{
      (*info->fprintf_func) (info->stream, MHXPRI4,
			    -delta);
	}
      break;

    case 'h':
      (*info->fprintf_func) (info->stream, HEXPRIS,
			     (unsigned int) ((l >> OP_SH_PREFX)
					     & OP_MASK_PREFX));
      break;

    case 'k':
      (*info->fprintf_func) (info->stream, HEXPRIS,
			     (unsigned int) ((l >> OP_SH_CACHE)
					     & OP_MASK_CACHE));
      break;

    case 'a':
      (*info->print_address_func)
	(((pc & 0xF0000000) | (((l >> OP_SH_TARGET) & OP_MASK_TARGET) << 2)),
	 info);
      break;

    case 'p':
      /* sign extend the displacement */
      delta = (l >> OP_SH_DELTA) & OP_MASK_DELTA;
      if (delta & 0x8000)
	delta |= ~0xffffL;
      (*info->print_address_func)
	((delta << 2) + pc + 4,
	 info);
      break;

    case 'd':
      (*info->fprintf_func) (info->stream, REGPRIS,
			     reg_names[(l >> OP_SH_RD) & OP_MASK_RD]);
      break;

    case 'z':
      (*info->fprintf_func) (info->stream, REGPRIS, reg_names[0]);
      break;

    case '<':
      (*info->fprintf_func) (info->stream, HEXPRIS,
			     (l >> OP_SH_SHAMT) & OP_MASK_SHAMT);
      break;

    case 'c':
      (*info->fprintf_func) (info->stream, HEXPRIS,
			     (l >> OP_SH_CODE) & OP_MASK_CODE);
      break;

    case 'C':
      (*info->fprintf_func) (info->stream, HEXPRIS,
			     (l >> OP_SH_COPZ) & OP_MASK_COPZ);
      break;

    case 'B':
      (*info->fprintf_func) (info->stream, HEXPRIS,
			     (l >> OP_SH_SYSCALL) & OP_MASK_SYSCALL);
      break;

    case 'S':
    case 'V':
      (*info->fprintf_func) (info->stream, "$f%d",
			     (l >> OP_SH_FS) & OP_MASK_FS);
      break;

    case 'T':
    case 'W':
      (*info->fprintf_func) (info->stream, "$f%d",
			     (l >> OP_SH_FT) & OP_MASK_FT);
      break;

    case 'D':
      (*info->fprintf_func) (info->stream, "$f%d",
			     (l >> OP_SH_FD) & OP_MASK_FD);
      break;

    case 'R':
      (*info->fprintf_func) (info->stream, "$f%d",
			     (l >> OP_SH_FR) & OP_MASK_FR);
      break;

    case 'E':
      (*info->fprintf_func) (info->stream, "$%d",
			     (l >> OP_SH_RT) & OP_MASK_RT);
      break;

    case 'G':
      (*info->fprintf_func) (info->stream, "$%d",
			     (l >> OP_SH_RD) & OP_MASK_RD);
      break;

    case 'N':
      (*info->fprintf_func) (info->stream, "%d",
			     (l >> OP_SH_BCC) & OP_MASK_BCC);
      break;

    case 'M':
      (*info->fprintf_func) (info->stream, "%d",
			     (l >> OP_SH_CCC) & OP_MASK_CCC);
      break;

    default:
      (*info->fprintf_func) (info->stream,
			     "# internal error, undefined modifier(%c)", *d);
      break;
    }
}


/* Print the mips instruction at address MEMADDR in debugged memory,
   on using INFO.  Returns length of the instruction, in bytes, which is
   always 4.  BIGENDIAN must be 1 if this is big-endian code, 0 if
   this is little-endian code.  */


static	int	dslot=0;
static	int	dline=0;
static	int	prevword=0;
static	int	currword=0;
static	int		currcode;
static	int		srcreg;
static	int		dstreg;
static	int		setreg;

int crlf_for_mips(void)
{
	return (dline!=0);
}

//
//	ニーモニックが jal で始まっているかどうか調べる.
//
int is_jal(const char *s)
{
	return ( (s[0]=='j') && (s[1]=='a') && (s[2]=='l') );
}

//
//	lui命令の直後の ロード命令にコメントを振る処理.
//
set_remark(const struct mips_opcode *op)
{
	short	offset;
	int    eaddr;
	char    ea[80];

	srcreg = (currword >>21L) & 31;
	dstreg = (currword >>16L) & 31;
	setreg = (prevword >>16L) & 31;
	currcode=(currword >>24L) & 0xfc;

	if( ( prevword & 0xffe00000L )==0x3c000000 ) {		// luiならば、
		if(setreg == srcreg) {	//直前にセットしたレジスタを参照している、
			switch(currcode) {
			 case 0x34:	// oriならば、
				eaddr = (prevword<<16L) | (currword & 0xffff);set_ea(ea,eaddr);
				pr_rem("; %s = %s",reg_names[dstreg], ea );
				break;
			 case 0x24:	// addiuならば、
				offset = currword;
				eaddr  = (prevword<<16L) + offset;set_ea(ea,eaddr);
				pr_rem("; %s = %s",reg_names[dstreg], ea );
				break;
			 case 0x8c:	// lw
			 case 0x84:	// lh
			 case 0x94:	// lhu
			 case 0x80:	// lb
			 case 0x90:	// lbu
				offset = currword;
				eaddr  = (prevword<<16L) + offset;set_ea(ea,eaddr);
				pr_rem("; %s =[%s]",reg_names[dstreg], ea );
				break;
			 case 0xac:	// sw
			 case 0xa4:	// sh
			 case 0xa0:	// sb
				offset = currword;
				eaddr  = (prevword<<16L) + offset;set_ea(ea,eaddr);
				pr_rem("; [%s]= %s", ea ,reg_names[dstreg] );
				break;
			 default:
				break;
			}
		}
	}
}
//
//	ニーモニックの表示
//
static int	_print_insn_mips (memaddr, word, info)
     bfd_vma memaddr;
     struct disassemble_info *info;
     unsigned int word;
{
  const struct mips_opcode *op;
  static boolean init = 0;
  static const struct mips_opcode *mips_hash[OP_MASK_OP + 1];

  /* Build a hash table to shorten the search time.  */
  if (! init)
    {
      unsigned int i;

      for (i = 0; i <= OP_MASK_OP; i++)
	{
	  for (op = mips_opcodes; op < &mips_opcodes[NUMOPCODES]; op++)
	    {
	      if (op->pinfo == INSN_MACRO)
		continue;
	      if (i == ((op->match >> OP_SH_OP) & OP_MASK_OP))
		{
		  mips_hash[i] = op;
		  break;
		}
	    }
        }

      init = 1;
    }

  op = mips_hash[(word >> OP_SH_OP) & OP_MASK_OP];
  
  
  if (op != NULL)
    {
      for (; op < &mips_opcodes[NUMOPCODES]; op++)
	{

	  if (op->pinfo != INSN_MACRO && (word & op->mask) == op->match)
	    {
	      register const char *d;
		 	char *dmark;

			currword = word;
			set_remark(op);

			if(dslot) {dmark=" -s- ";}
			else      {dmark="     ";}

			dline = (dslot & INSN_UNCOND_BRANCH_DELAY);			//改行を挿入するかどうか?
			
			{
				dslot = (op->pinfo & (INSN_UNCOND_BRANCH_DELAY|INSN_COND_BRANCH_DELAY) ) ;
				if(dslot && is_jal(op->name)) dslot = INSN_COND_BRANCH_DELAY;
				
				prevword = word;
			}

	      (*info->fprintf_func) (info->stream, "%s", dmark);	//ディレイスロット	-s-
	      
	      (*info->fprintf_func) (info->stream, "%s", op->name);	//ニモニックを出力する.

	      d = op->args;
	      if (d != NULL)
		{
			if( *d != 0 )
			{int i;
				for(i=strlen(op->name);i<8;i++)
		  			(*info->fprintf_func) (info->stream, " ");	//ニモニックの後ろに空白を入れる.
			}

		  for (; *d != '\0'; d++)
		    print_insn_arg (d, word, memaddr, info);
		}

	      return 4;
	    }
	}
    }


  /* Handle undefined instructions.  */
//  (*info->fprintf_func) (info->stream, "dl      $%lx", word);

  return 4;
}

int	print_insn_big_mips (memaddr, info)
     bfd_vma memaddr;
     struct disassemble_info *info;
{
  bfd_byte buffer[4];
  int status = (*info->read_memory_func) (memaddr, buffer, 4, info);
  if (status == 0)
    return _print_insn_mips (memaddr, (unsigned int) bfd_getb32 (buffer), info);
  else
    {
      (*info->memory_error_func) (status, memaddr, info);
      return -1;
    }
}

int	print_insn_little_mips (memaddr, info)
     bfd_vma memaddr;
     struct disassemble_info *info;
{
  bfd_byte buffer[4];
  int status = 	(*info->read_memory_func) (memaddr, buffer, 4, info);
  				(*info->print_memory_func) (memaddr, buffer, 4, info);
  if (status == 0)
    return _print_insn_mips (memaddr, (unsigned int) bfd_getl32 (buffer), info);
  else
    {
      (*info->memory_error_func) (status, memaddr, info);
      return -1;
    }
}

