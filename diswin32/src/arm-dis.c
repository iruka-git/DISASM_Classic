/* Instruction printing code for the ARM
   Copyright (C) 1994, 95, 96, 97, 1998 Free Software Foundation, Inc. 
   Contributed by Richard Earnshaw (rwe@pegasus.esprit.ec.org)
   Modification by James G. Smith (jsmith@cygnus.co.uk)

This file is part of libopcodes. 

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2 of the License, or (at your option)
any later version. 

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
more details. 

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include  <malloc.h>

#include "ansidecl.h"	// added.

#include "dis-asm.h"
#define DEFINE_TABLE
#include "arm-opc.h"
//#include "coff/internal.h"
//#include "internal.h"
//#include "libcoff.h"
//#include "opintl.h"

static	int		crlf_flg;
static	int		is_thumb=0;

static char *arm_conditional[] =
{"eq", "ne", "cs", "cc", "mi", "pl", "vs", "vc",
 "hi", "ls", "ge", "lt", "gt", "le", "", "nv"};

static char *arm_regnames[] =
{"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
 "r8", "r9", "sl", "fp", "ip", "sp", "lr", "pc"};

static char *arm_fp_const[] =
{"0.0", "1.0", "2.0", "3.0", "4.0", "5.0", "0.5", "10.0"};

static char *arm_shift[] = 
{"lsl", "lsr", "asr", "ror"};

static int print_insn_arm PARAMS ((bfd_vma, struct disassemble_info *,
				   int));

static void
arm_decode_shift (given, func, stream)
     int given;
     fprintf_ftype func;
     void *stream;
{
  func (stream, "%s", arm_regnames[given & 0xf]);
  if ((given & 0xff0) != 0)
    {
      if ((given & 0x10) == 0)
	{
	  int amount = (given & 0xf80) >> 7;
	  int shift = (given & 0x60) >> 5;
	  if (amount == 0)
	    {
	      if (shift == 3)
		{
		  func (stream, ", rrx");
		  return;
		}
	      amount = 32;
	    }
	  func (stream, ", %s #%d", arm_shift[shift], amount);
	}
      else
	func (stream, ", %s %s", arm_shift[(given & 0x60) >> 5],
	      arm_regnames[(given & 0xf00) >> 8]);
    }
}

/* Print one instruction from PC on INFO->STREAM.
   Return the size of the instruction (always 4 on ARM). */

static int
print_insn_arm (pc, info, given)
     bfd_vma         pc;
     struct disassemble_info *info;
     int given;
{
  struct arm_opcode *insn;
  void *stream = info->stream;
  fprintf_ftype func = info->fprintf_func;

  for (insn = arm_opcodes; insn->assembler; insn++)
    {
      if ((given & insn->mask) == insn->value)
	{
	  char *c;

#if	1	// crlf_flg
	  if((insn->value==0x08100000) && (given & (1 << 15)) ) crlf_flg=1;	// ldmdb {pcを含む}
	  if((insn->value==0x012FFF10) ) crlf_flg=1;						// bx
#endif

	  for (c = insn->assembler; *c; c++)
	    {
	      if (*c == '%')
		{
		  switch (*++c)
		    {
		    case '%':
		      func (stream, "%%");
		      break;

		    case 'a':
		      if (((given & 0x000f0000) == 0x000f0000)
			  && ((given & 0x02000000) == 0))
			{
			  int offset = given & 0xfff;
			  if ((given & 0x00800000) == 0)
			    offset = -offset;
			  (*info->print_address_func)
			    (offset + pc + 8, info);
			}
		      else
			{
			  func (stream, "[%s", 
				arm_regnames[(given >> 16) & 0xf]);
			  if ((given & 0x01000000) != 0)
			    {
			      if ((given & 0x02000000) == 0)
				{
				  int offset = given & 0xfff;
				  if (offset)
				    func (stream, ", %s#%d",
					  (((given & 0x00800000) == 0)
					   ? "-" : ""), offset);
				}
			      else
				{
				  func (stream, ", %s",
					(((given & 0x00800000) == 0)
					 ? "-" : ""));
				  arm_decode_shift (given, func, stream);
				}

			      func (stream, "]%s", 
				    ((given & 0x00200000) != 0) ? "!" : "");
			    }
			  else
			    {
			      if ((given & 0x02000000) == 0)
				{
				  int offset = given & 0xfff;
				  if (offset)
				    func (stream, "], %s#%d",
					  (((given & 0x00800000) == 0)
					   ? "-" : ""), offset);
				  else 
				    func (stream, "]");
				}
			      else
				{
				  func (stream, "], %s",
					(((given & 0x00800000) == 0) 
					 ? "-" : ""));
				  arm_decode_shift (given, func, stream);
				}
			    }
			}
		      break;

		    case 's':
                      if ((given & 0x004f0000) == 0x004f0000)
			{
                          /* PC relative with immediate offset */
			  int offset = ((given & 0xf00) >> 4) | (given & 0xf);
			  if ((given & 0x00800000) == 0)
			    offset = -offset;
			  (*info->print_address_func)
			    (offset + pc + 8, info);
			}
		      else
			{
			  func (stream, "[%s", 
				arm_regnames[(given >> 16) & 0xf]);
			  if ((given & 0x01000000) != 0)
			    {
                              /* pre-indexed */
			      if ((given & 0x00400000) == 0x00400000)
				{
                                  /* immediate */
                                  int offset = ((given & 0xf00) >> 4) | (given & 0xf);
				  if (offset)
				    func (stream, ", %s#%d",
					  (((given & 0x00800000) == 0)
					   ? "-" : ""), offset);
				}
			      else
				{
                                  /* register */
				  func (stream, ", %s%s",
					(((given & 0x00800000) == 0)
					 ? "-" : ""),
                                        arm_regnames[given & 0xf]);
				}

			      func (stream, "]%s", 
				    ((given & 0x00200000) != 0) ? "!" : "");
			    }
			  else
			    {
                              /* post-indexed */
			      if ((given & 0x00400000) == 0x00400000)
				{
                                  /* immediate */
                                  int offset = ((given & 0xf00) >> 4) | (given & 0xf);
				  if (offset)
				    func (stream, "], %s#%d",
					  (((given & 0x00800000) == 0)
					   ? "-" : ""), offset);
				  else 
				    func (stream, "]");
				}
			      else
				{
                                  /* register */
				  func (stream, "], %s%s",
					(((given & 0x00800000) == 0)
					 ? "-" : ""),
                                        arm_regnames[given & 0xf]);
				}
			    }
			}
		      break;
			  
		    case 'b':
		      (*info->print_address_func)
			(BDISP (given) * 4 + pc + 8, info);
		      break;

		    case 'c':
		      func (stream, "%s",
			    arm_conditional [(given >> 28) & 0xf]);
		      break;

		    case 'm':
		      {
			int started = 0;
			int reg;

			func (stream, "{");
			for (reg = 0; reg < 16; reg++)
			  if ((given & (1 << reg)) != 0)
			    {
			      if (started)
				func (stream, ", ");
			      started = 1;
			      func (stream, "%s", arm_regnames[reg]);
			    }
			func (stream, "}");
		      }
		      break;

		    case 'o':
		      if ((given & 0x02000000) != 0)
			{
			  int rotate = (given & 0xf00) >> 7;
			  int immed = (given & 0xff);
			  func (stream, "#%d",
				((immed << (32 - rotate))
				 | (immed >> rotate)) & 0xffffffff);
			}
		      else
			arm_decode_shift (given, func, stream);
		      break;

		    case 'p':
		      if ((given & 0x0000f000) == 0x0000f000)
			func (stream, "p");
		      break;

		    case 't':
		      if ((given & 0x01200000) == 0x00200000)
			func (stream, "t");
		      break;

		    case 'h':
		      if ((given & 0x00000020) == 0x00000020)
			func (stream, "h");
                      else
                        func (stream, "b");
		      break;

		    case 'A':
		      func (stream, "[%s", arm_regnames [(given >> 16) & 0xf]);
		      if ((given & 0x01000000) != 0)
			{
			  int offset = given & 0xff;
			  if (offset)
			    func (stream, ", %s#%d]%s",
				  ((given & 0x00800000) == 0 ? "-" : ""),
				  offset * 4,
				  ((given & 0x00200000) != 0 ? "!" : ""));
			  else
			    func (stream, "]");
			}
		      else
			{
			  int offset = given & 0xff;
			  if (offset)
			    func (stream, "], %s#%d",
				  ((given & 0x00800000) == 0 ? "-" : ""),
				  offset * 4);
			  else
			    func (stream, "]");
			}
		      break;

		    case 'C':
		      switch (given & 0x00090000)
			{
			default:
			  func (stream, "_???");
			  break;
			case 0x90000:
			  func (stream, "_all");
			  break;
			case 0x10000:
			  func (stream, "_ctl");
			  break;
			case 0x80000:
			  func (stream, "_flg");
			  break;
			}
		      break;

		    case 'F':
		      switch (given & 0x00408000)
			{
			case 0:
			  func (stream, "4");
			  break;
			case 0x8000:
			  func (stream, "1");
			  break;
			case 0x00400000:
			  func (stream, "2");
			  break;
			default:
			  func (stream, "3");
			}
		      break;
			
		    case 'P':
		      switch (given & 0x00080080)
			{
			case 0:
			  func (stream, "s");
			  break;
			case 0x80:
			  func (stream, "d");
			  break;
			case 0x00080000:
			  func (stream, "e");
			  break;
			default:
			  func (stream, _("<illegal precision>"));
			  break;
			}
		      break;
		    case 'Q':
		      switch (given & 0x00408000)
			{
			case 0:
			  func (stream, "s");
			  break;
			case 0x8000:
			  func (stream, "d");
			  break;
			case 0x00400000:
			  func (stream, "e");
			  break;
			default:
			  func (stream, "p");
			  break;
			}
		      break;
		    case 'R':
		      switch (given & 0x60)
			{
			case 0:
			  break;
			case 0x20:
			  func (stream, "p");
			  break;
			case 0x40:
			  func (stream, "m");
			  break;
			default:
			  func (stream, "z");
			  break;
			}
		      break;

		    case '0': case '1': case '2': case '3': case '4': 
		    case '5': case '6': case '7': case '8': case '9':
		      {
			int bitstart = *c++ - '0';
			int bitend = 0;
			while (*c >= '0' && *c <= '9')
			  bitstart = (bitstart * 10) + *c++ - '0';

			switch (*c)
			  {
			  case '-':
			    c++;
			    while (*c >= '0' && *c <= '9')
			      bitend = (bitend * 10) + *c++ - '0';
			    if (!bitend)
			      abort ();
			    switch (*c)
			      {
			      case 'r':
				{
				  int reg;
				  reg = given >> bitstart;
				  reg &= (2 << (bitend - bitstart)) - 1;
				  func (stream, "%s", arm_regnames[reg]);
				}
				break;
			      case 'd':
				{
				  int reg;
				  reg = given >> bitstart;
				  reg &= (2 << (bitend - bitstart)) - 1;
				  func (stream, "%d", reg);
				}
				break;
			      case 'x':
				{
				  int reg;
				  reg = given >> bitstart;
				  reg &= (2 << (bitend - bitstart)) - 1;
				  func (stream, "0x%08x", reg);
				}
				break;
			      case 'f':
				{
				  int reg;
				  reg = given >> bitstart;
				  reg &= (2 << (bitend - bitstart)) - 1;
				  if (reg > 7)
				    func (stream, "#%s",
					  arm_fp_const[reg & 7]);
				  else
				    func (stream, "f%d", reg);
				}
				break;
			      default:
				abort ();
			      }
			    break;
			  case '`':
			    c++;
			    if ((given & (1 << bitstart)) == 0)
			      func (stream, "%c", *c);
			    break;
			  case '\'':
			    c++;
			    if ((given & (1 << bitstart)) != 0)
			      func (stream, "%c", *c);
			    break;
			  case '?':
			    ++c;
			    if ((given & (1 << bitstart)) != 0)
			      func (stream, "%c", *c++);
			    else
			      func (stream, "%c", *++c);
			    break;
			  default:
			    abort ();
			  }
			break;

		      default:
			abort ();
		      }
		    }
		}
	      else
		func (stream, "%c", *c);
	    }
	  return 4;
	}
    }
  abort ();
}

/* Print one instruction from PC on INFO->STREAM.
   Return the size of the instruction. */

static int
print_insn_thumb (pc, info, given)
     bfd_vma         pc;
     struct disassemble_info *info;
     int given;
{
  struct thumb_opcode *insn;
  void *stream = info->stream;
  fprintf_ftype func = info->fprintf_func;

  for (insn = thumb_opcodes; insn->assembler; insn++)
    {
      if ((given & insn->mask) == insn->value)
        {
          char *c = insn->assembler;

#if	1	// crlf_flg
		  if((insn->value==0xBC00) && (given & (1 << 8)) ) crlf_flg=1;	// pop {pcを含む}
		  if((insn->value==0x4700)) crlf_flg=1;							// bx
#endif

          /* Special processing for Thumb 2 instruction BL sequence: */
          if (!*c) /* check for empty (not NULL) assembler string */
            {
	      info->bytes_per_chunk = 4;
	      info->bytes_per_line  = 4;
	      
          //  func (stream, "%04x\tbl\t", given & 0xffff);
              func (stream, "bl\t");
              (*info->print_address_func)
                (BDISP23 (given) * 2 + pc + 4, info);
              return 4;
            }
          else
            {
	      info->bytes_per_chunk = 2;
	      info->bytes_per_line  = 4;
	  	      
              given &= 0xffff;
              //func (stream, "%04x\t", given);
              for (; *c; c++)
                {
                  if (*c == '%')
                    {
                      int domaskpc = 0;
                      int domasklr = 0;
                      switch (*++c)
                        {
                        case '%':
                          func (stream, "%%");
                          break;

                        case 'S':
                          {
                            int reg;
                            reg = (given >> 3) & 0x7;
                            if (given & (1 << 6))
                              reg += 8;
                            func (stream, "%s", arm_regnames[reg]);
                          }
                          break;

                        case 'D':
                          {
                            int reg;
                            reg = given & 0x7;
                            if (given & (1 << 7))
                             reg += 8;
                            func (stream, "%s", arm_regnames[reg]);
                          }
                          break;

                        case 'T':
                          func (stream, "%s",
                                arm_conditional [(given >> 8) & 0xf]);
                          break;

                        case 'N':
                          if (given & (1 << 8))
                            domasklr = 1;
                          /* fall through */
                        case 'O':
                          if (*c == 'O' && (given & (1 << 8)))
                            domaskpc = 1;
                          /* fall through */
                        case 'M':
                          {
                            int started = 0;
                            int reg;
                            func (stream, "{");
                            /* It would be nice if we could spot
                               ranges, and generate the rS-rE format: */
                            for (reg = 0; (reg < 8); reg++)
                              if ((given & (1 << reg)) != 0)
                                {
                                  if (started)
                                    func (stream, ", ");
                                  started = 1;
                                  func (stream, "%s", arm_regnames[reg]);
                                }

                            if (domasklr)
                              {
                                if (started)
                                  func (stream, ", ");
                                started = 1;
                                func (stream, "lr");
                              }

                            if (domaskpc)
                              {
                                if (started)
                                  func (stream, ", ");
                                func (stream, "pc");
                              }

                            func (stream, "}");
                          }
                          break;


                        case '0': case '1': case '2': case '3': case '4': 
                        case '5': case '6': case '7': case '8': case '9':
                          {
                            int bitstart = *c++ - '0';
                            int bitend = 0;
                            while (*c >= '0' && *c <= '9')
                              bitstart = (bitstart * 10) + *c++ - '0';

                            switch (*c)
                              {
                              case '-':
                                {
                                  int reg;
                                  c++;
                                  while (*c >= '0' && *c <= '9')
                                    bitend = (bitend * 10) + *c++ - '0';
                                  if (!bitend)
                                    abort ();
                                  reg = given >> bitstart;
                                  reg &= (2 << (bitend - bitstart)) - 1;
                                  switch (*c)
                                    {
                                    case 'r':
                                      func (stream, "%s", arm_regnames[reg]);
                                      break;

                                    case 'd':
                                      func (stream, "%d", reg);
                                      break;

                                    case 'H':
                                      func (stream, "%d", reg << 1);
                                      break;

                                    case 'W':
                                      func (stream, "%d", reg << 2);
                                      break;

                                    case 'a':
				      /* PC-relative address -- the bottom two
					 bits of the address are dropped before
					 the calculation.  */
                                      info->print_address_func
					(((pc + 4) & ~3) + (reg << 2), info);
                                      break;

                                    case 'x':
                                      func (stream, "0x%04x", reg);
                                      break;

                                    case 'I':
                                      reg = ((reg ^ (1 << bitend)) - (1 << bitend));
                                      func (stream, "%d", reg);
                                      break;

                                    case 'B':
                                      reg = ((reg ^ (1 << bitend)) - (1 << bitend));
                                      (*info->print_address_func)
                                        (reg * 2 + pc + 4, info);
                                      break;

                                    default:
                                      abort();
                                    }
                                }
                                break;

                              case '\'':
                                c++;
                                if ((given & (1 << bitstart)) != 0)
                                  func (stream, "%c", *c);
                                break;

                              case '?':
                                ++c;
                                if ((given & (1 << bitstart)) != 0)
                                  func (stream, "%c", *c++);
                                else
                                  func (stream, "%c", *++c);
                                break;

                              default:
                                 abort();
                              }
                          }
                          break;

                        default:
                          abort ();
                        }
                    }
                  else
                    func (stream, "%c", *c);
                }
             }
          return 2;
       }
    }

  /* no match */
  abort ();
}

/* NOTE: There are no checks in these routines that the relevant number of data bytes exist */

int
print_insn_big_arm (pc, info)
     bfd_vma pc;
     struct disassemble_info *info;
{
  unsigned char      b[4];
  int               given;
  int                status;
//  coff_symbol_type   *cs;
//  int                is_thumb;
  
    is_thumb = false;
	crlf_flg=0;

#if	0
  if (info->symbols != NULL)
    {
    if (bfd_asymbol_flavour (*info->symbols) == bfd_target_coff_flavour)
     {
       cs = coffsymbol (*info->symbols);
       is_thumb = (cs->native->u.syment.n_sclass == C_THUMBEXT
                   || cs->native->u.syment.n_sclass == C_THUMBSTAT
                   || cs->native->u.syment.n_sclass == C_THUMBLABEL
                   || cs->native->u.syment.n_sclass == C_THUMBEXTFUNC
                   || cs->native->u.syment.n_sclass == C_THUMBSTATFUNC);
  
     }
   }
#endif

  info->bytes_per_chunk = 4;
  info->display_endian = BFD_ENDIAN_BIG;

  /* Always fetch word aligned values.  */
  
  status = (*info->read_memory_func) (pc & ~ 0x3, (bfd_byte *) &b[0], 4, info);
  if (status != 0)
    {
      (*info->memory_error_func) (status, pc, info);
      return -1;
    }

  if (is_thumb)
    {
      if (pc & 0x2)
	{
	  given = (b[2] << 8) | b[3];

	  status = info->read_memory_func ((pc + 4) & ~ 0x3, (bfd_byte *) b, 4, info);
	  if (status != 0)
	    {
	      info->memory_error_func (status, pc + 4, info);
	      return -1;
	    }
	  
	  given |= (b[0] << 24) | (b[1] << 16);
	}
      else
	{
	  given = (b[0] << 8) | b[1] | (b[2] << 24) | (b[3] << 16);
	}
    }
  else
    {
      given = (b[0] << 24) | (b[1] << 16) | (b[2] << 8) | (b[3]);
    }

  if (is_thumb)
    {
      status = print_insn_thumb (pc, info, given);
    }
  else
    {
      status = print_insn_arm (pc, info, given);
    }

  return status;
}

int
print_insn_little_arm (pc, info)
     bfd_vma pc;
     struct disassemble_info * info;
{
  unsigned char      b[4];
  int               given;
  int                status;
//  coff_symbol_type   *cs;
//  int                is_thumb;

	crlf_flg=0;
//  is_thumb = false;
#if	0
  if (info->symbols != NULL)
    {
    if (bfd_asymbol_flavour (*info->symbols) == bfd_target_coff_flavour)
     {
       cs = coffsymbol (*info->symbols);
       is_thumb = (cs->native->u.syment.n_sclass == C_THUMBEXT
                   || cs->native->u.syment.n_sclass == C_THUMBSTAT
                   || cs->native->u.syment.n_sclass == C_THUMBLABEL
                   || cs->native->u.syment.n_sclass == C_THUMBEXTFUNC
                   || cs->native->u.syment.n_sclass == C_THUMBSTATFUNC);
  
     }
   }
#endif

  info->bytes_per_chunk = 4;
  info->display_endian = BFD_ENDIAN_LITTLE;

  status = (*info->read_memory_func) (pc, (bfd_byte *) &b[0], 4, info);

      	   (*info->print_memory_func)(pc, (bfd_byte *) &b[0], 4, info);	//アドレスと、メモリー内容をプリント.
  		   (*info->fprintf_func) (info->stream,"\t");					//タブストップ１個プリント.

  if (status != 0 && is_thumb)
    {
      info->bytes_per_chunk = 2;

      status = info->read_memory_func (pc, (bfd_byte *) b, 2, info);
      b[3] = b[2] = 0;
    }
  if (status != 0)
    {
      (*info->memory_error_func) (status, pc, info);
      return -1;
    }

  given = (b[0]) | (b[1] << 8) | (b[2] << 16) | (b[3] << 24);

  if (is_thumb)
    {
      status = print_insn_thumb (pc, info, given);
    }
  else
    {
      status = print_insn_arm (pc, info, given);
    }

  return status;
}

int crlf_for_arm(void)
{
	return crlf_flg;
}
