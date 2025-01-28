/*
 *		Ｈｉｔａｃｈｉ ＳＨ用 逆アセンブラ
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>

#define	ADDEXT	1
#define	GETLN	1
#include "std.h"
#include "symbol.h"


/***  プロトタイプ宣言　***/
static int	seek(long l);
void   read_symbol(char *name);
short  dis_peekw(long adrs);
long   dis_peekl(long adrs);
void   dis_putln(char *s);
void   usage(void);

extern	void init_symbol(void);
extern	void init_mark(void);
extern	void reg_symbol(SYMBOL *symbol);



Usage(
	"* SH CPU DisAssembler Ver 0.3 *\n"
	"使い方：\n"
	"    dissh  -option  infile.bin outfile.asm\n"
	"オプション：\n"
	"          -s<address>  開始アドレスオフセットの指定\n"
	"          -g<gbr>      デフォルトＧＢＲの値の指定\n"
	"          -c           命令コード１６進表示の抑制\n"
	"          -i           PC相対にある即値表示の抑制\n"
	"          -r           命令の解説表示\n"
)

static	struct stat sbuf;
static	Ulong		start=0;
static	Ulong		filelength=0;
#define	VIBUFSIZ	0x4000
#define	VOBUFSIZ	0x4000
static	char	vibuf[VIBUFSIZ];
static	char	vobuf[VOBUFSIZ];
		Ulong	gbr=0xffffffffL;

int	cflag=1;
int	iflag=1;
int	rflag=0;
disasm(Ulong start,Ulong length);

/**********************************************************************
 *	メインルーチン
 **********************************************************************
 */
int	main(int argc,char **argv)
{
	Getopt(argc,argv);
	if(argc<3) usage();
	if(IsOpt('c')) cflag^=1;
	if(IsOpt('i')) iflag^=1;
	if(IsOpt('r')) rflag^=1;
	if(IsOpt('s')) { sscanf( Opt('s'),"%lx",&start ); }
	if(IsOpt('g')) { sscanf( Opt('g'),"%lx",&gbr   ); }
    if( stat(argv[1],&sbuf) ) {
    	printf("fatal: file not exist\n");
    	exit(1);
    }
    filelength = sbuf.st_size;
	init_symbol();
	init_mark();
	read_symbol(argv[1]);
	Ropen(argv[1]);
	Wopen(argv[2]);
	setvbuf(ifp,vibuf,0,VIBUFSIZ);
	setvbuf(ofp,vobuf,0,VOBUFSIZ);
	disasm(start,filelength);	/*逆アセンブルを実行する*/
	Rclose();
	Wclose();
	return 0;
}

/**********************************************************************
 *	シンボルファイルがあればそれを読み込む
 **********************************************************************
 */
void read_symbol(char *name)
{
	char mapname[80];
	char buf[256];
	char sym[256];
	SYMBOL symbol;
	long adr=0;

	strcpy(mapname,name);
	addext(mapname,"map");
	ifp=fopen(mapname,"rb");if(ifp!=NULL) {
		while(getln(buf,ifp)!=EOF) {
			if(sscanf(buf,"%lx %s",&adr,sym)==2) {
				symbol.adrs = adr;
				symbol.name = sym;
				reg_symbol(&symbol);
			/*	printf("#%s\n%lx <%s>\n",buf,adr,sym);*/
			}
		}
		fclose(ifp);
	}
}

/**********************************************************************
 *	以下はすべてdis.cモジュールからのコールバック関数（ヘルパー）
 **********************************************************************
 */

Ulong	seekptr=0;

static	int	seek(long l)
{
	if( seekptr != l) {
		seekptr  = l;
		return Seek(ifp,l);
	}
	return 0;
}

/**********************************************************************
 *	ＳＨバイナリーコードを実アドレス指定でフェッチする（ｗｏｒｄ）
 **********************************************************************
 */
short dis_peekw(long adrs)
{
	short s;
	adrs -= start;
	if( (Ulong)adrs >=filelength ) return -1;

	seek(adrs);
	s = getc(ifp) << 8  ;
	s|= getc(ifp) & 0xff;
	seekptr+=2;
	return s;
}
/**********************************************************************
 *	ＳＨバイナリーコードを実アドレス指定でフェッチする（ｌｏｎｇ）
 **********************************************************************
 */
long dis_peekl(long adrs)
{
	long l;
	adrs -= start;
	if( (Ulong)adrs >=filelength ) return -1L;

	seek(adrs);
	l = (long)getc(ifp) << 24L ;
	l|= (long)getc(ifp) << 16L ;
	l|= (long)getc(ifp) << 8L  ;
	l|= (long)getc(ifp)  & 0xff;
	seekptr+=4;
	return l;
}


/**********************************************************************
 *	結果のテキスト文字列を行単位で出力する
 **********************************************************************
 */
void dis_putln(char *s)
{
	fprintf(ofp,"%s\r\n",s);
}

