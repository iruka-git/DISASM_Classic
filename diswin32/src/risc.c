/**********************************************************************
 *	R3000 DISASM
 **********************************************************************
 */
#include <sys/stat.h>
#include "ansidecl.h"
#include "dis-asm.h"

long	lpeek(long adr);
//int	print_insn_little_mips (long memaddr,struct disassemble_info *info);

static struct 	disassemble_info info;
static long	filesize;
static struct  stat statbuf;
static long	startaddr=0;
static long	endaddr=0;
static char *bufferp;
static int		bigmips=0;

static char	 lbuf[128];
static char	 remarkbuf[128];
static char	*lbufp;


typedef struct SYMBOL {
	long	  adrs;		/* 論理アドレス  */
	char     *name;		/* シンボル名	 */
} SYMBOL;

static SYMBOL  lsymbol;
static char    label[80];

pr_rem(form,val)
	char *form;
	char *val;
{
	vsprintf(remarkbuf,form,(void *)&val);
}

static	prfun(stream,form,val)
	FILE *stream;
	char *form;
	char *val;
{
	vsprintf(lbufp,form,(void *)&val);lbufp+=strlen(lbufp);
}

find_symbol_by_adrs(SYMBOL *symbol,long address)
{
	char *s = (char *)search_hash2(address,0);
	if(s!=NULL) {
		symbol->name = s;
		return 1;
	}
	return 0;
}

static	unsigned long	chkbig(unsigned long data)
{
	if(bigmips) {
		return	(((data <<24L) & 0xff000000L) |
				 ((data << 8L) & 0x00ff0000L) |
				 ((data >> 8L) & 0x0000ff00L) |
				 ((data >>24L) & 0x000000ffL) );
	}
	return data;
}

static	int memfun(unsigned long adr,unsigned char *buf,int size,struct disassemble_info *info)
{
	unsigned long data;
	unsigned char *p;
	int i;

	if( (adr>=startaddr) && (adr<endaddr) ) {
		p = &bufferp[adr - startaddr];
		data = 0;
		for(i=0;i<size;i++) {
			buf[i]=p[i];
		}
		return 0;
	}else{
		return 1;
	}
}

static	int prmfun(unsigned long adr,unsigned char *buf,int size,struct disassemble_info *info)
{
	unsigned long data;
	int i,n;

	data = 0;n=0;
	for(i=0;i<size;i++) {
		data |= (buf[i]<<n);n+=8;
	}
	if(size==4) {
		sprintf(lbufp,"%08lx %08lx ",adr,chkbig(data));lbufp+=strlen(lbufp);
	}else{
		sprintf(lbufp,"%08lx %04lx ",adr,chkbig(data));lbufp+=strlen(lbufp);
	}
	return 0;
}

static	void	errfun(int status, bfd_vma memaddr, struct disassemble_info *info)
{
	//printf("errfun()\n");
}


static	int	pr_addr(long addr)
{
	if(find_symbol_by_adrs(&lsymbol,addr)) {
		char *name=lsymbol.name;
		char *p = label;
		while(*name) *p++ = *name++;
					 *p++ = 0;
//		sprintf(lbufp," ;%s\n",label);lbufp+=strlen(lbufp);
		sprintf(lbufp," ;%s",label);lbufp+=strlen(lbufp);
	}
}

void set_ea(char *buf,long addr)
{
	if(find_symbol_by_adrs(&lsymbol,addr)) {
		char *name=lsymbol.name;
		char *p = label;
		while(*name) *p++ = *name++;
					 *p++ = 0;
		sprintf(buf,"%s",label);
	}else{
		sprintf(buf,"0x%08lx",addr);
	}
}


static	void	prafun(unsigned long addr,struct disassemble_info *info)
{
	sprintf(lbufp,"$%08lx",addr);lbufp+=strlen(lbufp);
	pr_addr(addr);
}


dislmips_init()
{
	info.stream				= NULL;
	info.fprintf_func       = (fprintf_ftype) prfun;
	info.read_memory_func   = memfun;
	info.print_memory_func  = prmfun;
	info.memory_error_func  = errfun;
	info.print_address_func = prafun;
}


static void print_labels(FILE *ofp,long memaddr)
{
	lbufp=lbuf;
	remarkbuf[0]=0;
	if(find_symbol_by_adrs(&lsymbol,memaddr)) {
		char *name=lsymbol.name;
		char *p = label;
		while(*name) *p++ = *name++;
					 *p++ = 0;
		fprintf(ofp,"                  %s:\n",label);
	}
}

static void	print_comments(FILE *ofp)
{
	if(remarkbuf[0]) {
		fprintf(ofp,"%-48s %s\n",lbuf,remarkbuf);
	}else{
		fprintf(ofp,"%s\n",lbuf);
	}
}
//-------------------------------------------------------------
//	MIPS用.
//-------------------------------------------------------------
static void disl_mips(FILE *ofp,long memaddr)
{
	print_labels(ofp,memaddr);
	print_insn_little_mips (memaddr,&info);
	print_comments(ofp);
	if(crlf_for_mips()) fprintf(ofp,"\n");
}

long disasm_mips(char *buf,long start,long size,FILE *ofp)
{
	long off;
	long adr;
	
	startaddr = start;
	endaddr   = start + size;
	bufferp   = buf;
	
	for(off=0;off<size;off+=4,start+=4) {
		disl_mips(ofp,start);
	}
	return size;
}

//-------------------------------------------------------------
//	SH用.
//-------------------------------------------------------------
#define LITTLE_BIT 2

static void disl_sh(FILE *ofp,long memaddr)
{
	print_labels(ofp,memaddr);
    info.flags = LITTLE_BIT;
  	print_insn_shx (memaddr, &info);
	print_comments(ofp);
	if(crlf_for_sh()) fprintf(ofp,"\n");
}

long disasm_sh(char *buf,long start,long size,FILE *ofp)
{
	long off;
	long adr;
	
	startaddr = start;
	endaddr   = start + size;
	bufferp   = buf;
	
	for(off=0;off<size;off+=2,start+=2) {
		disl_sh(ofp,start);
	}
	return size;
}

//-------------------------------------------------------------
//	ARM用.
//-------------------------------------------------------------
static void disl_arm(FILE *ofp,long memaddr)
{
	print_labels(ofp,memaddr);
    info.flags = 0;		//  info.flags = LITTLE_BIT;
	print_insn_little_arm (memaddr, &info);
	print_comments(ofp);
	if(crlf_for_arm()) fprintf(ofp,"\n");
}

long disasm_arm(char *buf,long start,long size,FILE *ofp)
{
	long off;
	long adr;
	
	startaddr = start;
	endaddr   = start + size;
	bufferp   = buf;
	
	for(off=0;off<size;off+=4,start+=4) {
		disl_arm(ofp,start);
	}
	return size;
}

bfd_vma		bfd_getl32(const unsigned char *buffer)
{
	return (((unsigned long )buffer[3]<<24L) |
			((unsigned long )buffer[2]<<16L) |
			((unsigned long )buffer[1]<< 8L) |
			((unsigned long )buffer[0]     ) )
	;
}

bfd_vma		bfd_getb32(const unsigned char *buffer)
{
	return (((unsigned long )buffer[0]<<24L) |
			((unsigned long )buffer[1]<<16L) |
			((unsigned long )buffer[2]<< 8L) |
			((unsigned long )buffer[3]     ) )
	;
}

bfd_vma		bfd_getl16(const unsigned char *buffer)
{
	return (((unsigned long )buffer[1]<< 8L) |
			((unsigned long )buffer[0]     ) )
	;
}

bfd_vma		bfd_getb16(const unsigned char *buffer)
{
	return (((unsigned long )buffer[0]<< 8L) |
			((unsigned long )buffer[1]     ) )
	;
}

