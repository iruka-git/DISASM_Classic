/**********************************************************************
 *	R3000 DISASM
 **********************************************************************
 */		char USAGE[]=

"*  R3000 DISASM Ver0.2\n"
"   使い方：\n"
"C:> DISR -Option FILENAME.BIN\n"
"   オプション\n"
"         -s<start address>  先頭番地を１６進数で指定.\n"
"         -m                 ビッグエンディアンコードを解析する.\n"
"         -r                 レジスタ名称をr0〜r31に変える.\n"
"         -h                 １６進数の表示を $ でなく 0x にする.\n"



;

#include "std.h"
#include <sys/stat.h>
#include <stdarg.h>
#include "ansidecl.h"
#include "dis-asm.h"

#include "risc.h"
#include "symbol.h"
#include "loadfile.h"

long	lpeek(long adr);
int	    print_insn_little_mips (unsigned long memaddr,struct disassemble_info *info);
void init_symbol(void);
void set_alternate_regname(void);
void set_alternate_hexa(void);
int is_crlf(void);
long disasm_mips(char *buf,long start,long size,FILE *ofp);

struct 	disassemble_info info;
long	filesize;
struct  stat statbuf;
long	startaddr=0;
long	endaddr=0;
int		bigmips=0;

#define MAX_BUF_SIZ 1280

extern char	 lbuf[MAX_BUF_SIZ];
extern char	 remarkbuf[MAX_BUF_SIZ];
extern char	*lbufp;

void usage(void)
{
	printf("%s",USAGE);
	exit(1);
}

#if 0
SYMBOL  lsymbol;
char    label[800];


void pr_rem(cchar *format, ...)
{
	va_list arg;
	va_start(arg,format);
	
	vsnprintf(remarkbuf,MAX_BUF_SIZ,format,arg);
	va_end( arg );
}

static	int prfun(FILE *stream,cchar *format, ...)
{
	if(lbufp==NULL) return 0;

	va_list arg;
	va_start(arg,format);
	
	vsnprintf(lbufp,255,format,arg);
	va_end( arg );

	lbufp+=strlen(lbufp);
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

static	int memfun(long adr,long *buf)
{
	long data;

	if( (adr>=startaddr) && (adr<endaddr) ) {
		Read(&data,4);

		*buf = data;
		sprintf(lbufp,"%08lX %08lX ",adr,chkbig(data));lbufp+=strlen(lbufp);
		return 0;
	}else{
		return 1;
	}
}

static	void	errfun()
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
		sprintf(lbufp," ;%s\n",label);lbufp+=strlen(lbufp);
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
		sprintf(buf,"0x%08x",addr);
	}
}


static	void	prafun(long addr)
{
	sprintf(lbufp,"$%08x",addr);lbufp+=strlen(lbufp);
	pr_addr(addr);
}


static	void dislmips_init(void)
{
	info.stream				= NULL;
	info.fprintf_func       = prfun;
	info.read_memory_func   = memfun;
	info.memory_error_func  = errfun;
	info.print_address_func = prafun;
}

void dislmips(long memaddr)
{
	lbufp=lbuf;
	remarkbuf[0]=0;

	if(find_symbol_by_adrs(&lsymbol,memaddr)) {
		char *name=lsymbol.name;
		char *p = label;
		while(*name) *p++ = *name++;
					 *p++ = 0;
		printf("                  %s:\n",label);
	}

	if(bigmips) {
		print_insn_big_mips (memaddr,&info);
	}else{
		print_insn_little_mips (memaddr,&info);
	}
	if(remarkbuf[0]) {
		printf("%-48s %s\n",lbuf,remarkbuf);
	}else{
		printf("%s\n",lbuf);
	}
	if(is_crlf())
		printf("\n");
}

bfd_vma		bfd_getl32(const unsigned char *buffer)
{
	unsigned long *l=(unsigned long *) buffer;
	return *l;
}

bfd_vma		bfd_getb32(const unsigned char *buffer)
{
	return (((unsigned long )buffer[0]<<24L) |
			((unsigned long )buffer[1]<<16L) |
			((unsigned long )buffer[2]<< 8L) |
			((unsigned long )buffer[3]     ) )
	;
}
#endif



/*
 *	一行入力ユーティリティ
 */
int getln(char *buf,FILE *fp)
{
	int c;
	int l;
	l=0;
	while(1) {
		c=getc(fp);
		if(c==EOF)  return(EOF);/* EOF */
		if(c==0x0d) continue;
		if(c==0x0a) {
			*buf = 0;	/* EOL */
			return(0);	/* OK  */
		}
		*buf++ = c;l++;
		if(l>=255) {
			*buf = 0;
			return(1);	/* Too long line */
		}
	}
}
/*
 *	拡張子付加ユーティリティ
 *
 *	addext(name,"EXE") のようにして使う
 *	もし拡張子が付いていたらリネームになる
 */
void addext(char *name,char *ext)
{
	char *p,*q;
	p=name;q=NULL;
	while( *p ) {
		if ( *p == '.' ) q=p;
		p++;
	}
	if(q==NULL) q=p;
	/*if( (p-q) >= 4) return;  なんかbugっている*/
	strcpy(q,".");
	strcpy(q+1,ext);
}

/**********************************************************************
 *	シンボルファイルがあればそれを読み込む
 **********************************************************************
 */
void read_symbol(char *name)
{
	char mapname[800]="";
	char buf[2560]="";
	char sym[2560]="";
	char dum[2560]="";
	SYMBOL symbol;
	long adr=0;

	strcpy(mapname,name);
	addext(mapname,"map");

//	printf(">%s<\n",mapname);
	
	
	ifp=fopen(mapname,"rb");
	if(ifp!=NULL) {
		while(getln(buf,ifp)!=EOF) {
			if(sscanf(buf,"%lx %s %s",&adr,sym,dum)==2) {
				symbol.adrs = adr;
				symbol.name = sym;
				reg_symbol(&symbol);
//				printf("#%s\n%lx <%s>\n",buf,adr,sym);
			}
		}
		fclose(ifp);
	}
}


int main(int argc,char **argv)
{
	long off;
	long adr;
	
	Getopt(argc,argv);
	if(argc<2) usage();

	if(IsOpt('m')) bigmips=1;
	if(IsOpt('r')) set_alternate_regname();
	if(IsOpt('h')) set_alternate_hexa();
	if(IsOpt('s')) sscanf(Opt('s'),"%lx",&startaddr);
	adr = startaddr;
	dislmips_init();

	init_symbol();
	read_symbol(argv[1]);

	char *buf = loadfile(argv[1]);
	filesize  = loadsize();
	endaddr   = startaddr + filesize;

	disasm_mips(buf,adr,filesize,stdout);

//	for(off=0;off<filesize;off+=4) {
//		dislmips(adr);adr+=4;
//	}
	
}

