/********************************************************/
/*		逆アセンブル表示								*/
/********************************************************/

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "sh7000.h"
#include "symbol.h"



extern	char	*opstring[];
extern	char	 opdelay[];
extern	char	*operands[];
extern	int      ra,rn,rm,rx,opc;
extern	long     disp,imm;
extern	int      cflag;
extern	int      iflag;
extern	int      rflag;

static	char	disbuf[128];
static	char	rembuf[128];
static	char    oprbuf[128];

static	char	opsrc[SYMLEN];
static	char	opdst[SYMLEN];

static	char    jmpdst[16][SYMLEN];
static	char    *dp;
static	char	*op_name;

static	int		opword;
static	int		opcode;
static	int		optype;
extern	Ulong	gbr;


/***  プロトタイプ宣言　***/
void disasm(Ulong start,Ulong length);
int disasm_1L(long ip);
void disout(void);
void disputc(int c);
void disputs(char *s);
void clrjmp(int s,int e);
void clrjmps(void);
char *symbol_check(long val);
long extsign8(int i);
void disoperand(int mnem,long adr);
void remout_imm4(long adr);
void remout_imm40w(long adr);
void remout_imm20w(long adr);
void remout_imm2(long adr);
void put_label(unsigned int c,int tabs);
int  sprint_adrs(char *p,char *q,long rela);
void cutopr(void);
int  regnum(char *s);
void setdst(void);
void remarks(char *buf);

extern	int	 get_mark(long current,long next);
extern	int	 set_mark(long current,long next,int flag);
extern	void dis_putln(char *s);

extern	int	 dis_peekw(long	adrs);
extern	long dis_peekl(long adrs);

/*
 *	逆アセンブル表示
 */
void disasm(Ulong start,Ulong length)
{
	Ulong adrs;
	for(adrs=start;adrs< (start+length) ; adrs+=2) {
		disasm_1L(adrs);		/** 逆アセンブル **/
	}
}

/*
 *
 *	逆アセンブル表示　１行分
 *
 *		開始アドレス＝ ip  (ターゲット側の論理アドレス)
 *		戻り値：   命令サイズ
 *
 */
int	disasm_1L(long ip)
{
	int mne,i,seg;
	int mflg,xflg;
	int crlff,markf;
	long l;
	char *slotmark;

	rembuf[0]=0;

	dp=disbuf;
	{
		put_label(ip,13);			/* ラベルをシンボルで表示： */
		dp=disbuf;
	}

	mne=dis_peekw(ip);
	if( opdelay[opcode]=='s') slotmark="-s-";	/*１つ前のOPCODEだと思う*/
	else	slotmark="   ";
	markf = get_mark(ip,ip);

	switch(opcode) {
		case JMP:
		case RTS:
		case RTE:
			crlff=1;break;
		default:
			crlff=0;break;
	}
	
	if(cflag) {
		if(markf!=DD ) {
			dp += sprintf(dp,"%08lx %04x %s ",ip,mne,slotmark);
		}else{
			dp += sprintf(dp,"%08lx %08lx "  ,ip,dis_peekl(ip));
		}
	}else{
			dp += sprintf(dp,"%08lx %s ",ip,slotmark);
	}
	opcode  = get_opcode(mne);
	optype  = opcode >>8;
	opcode &= 0xff;
	
	switch(markf) {
		case DW:
			opcode = WORD;break;
		case DD:
			opcode = LONG;break;
		case DD2:
			opcode = LONG2;break;
		case NOMARK:
		default:
			break;
	}
	
	op_name = opstring[ opcode ];			/*ニモニック*/
	disputs(op_name);					/*ニモニック*/

	for(i=strlen(op_name);i<8;i++) disputc(' ');

	oprbuf[0]=0;
	switch(markf) {
		case DW:
			sprint_adrs(oprbuf,"",mne & 0xffffL);
			sprintf(rembuf,"(%d)"  ,mne);
			disputs(oprbuf);
			break;
		case DD:
			l=dis_peekl(ip);
			sprint_adrs(oprbuf,"",l);
			sprintf(rembuf,"(%ld)",l);
			disputs(oprbuf);
			break;
		case DD2:
			break;
		case NOMARK:
		default:
			disoperand(mne,ip);				/*オペランド*/
			break;
	}

	cutopr();					/* 解析済オペランドを２つに分割しopsrc,opdstに入れる */
	setdst();					/* レジスタ値の管理 */
	
	remarks(rembuf);
	if(markf!=DD2)				/* long operandの２ワード目は省略する */
		disout();
	if(crlff) {dis_putln("");}	/* RTS RTE JMP の後ろに空行を入れる */
	clrjmps();
	set_mark(ip,ip,NOMARK);
	return 2;	/* opsize(mne);*/
}

/*
 *	逆アセンブラ専用出力ルーチン
 *
 */
void disout(void)
{
	char outbuf[1024];
	*dp=0;dp=disbuf;

	if(rflag==0) rembuf[0]=0;
	if(rembuf[0]) {
		sprintf(outbuf,"%-40s;%s",disbuf,rembuf);
		rembuf[0]=0;
	}else{
		sprintf(outbuf,"%s",disbuf);
	}
	dis_putln(outbuf);
}

void disputc(int c)
{
	*dp++ = c;
}
void disputs(char *s)
{
	while(*s) {
		*dp++ = *s++;
	}
}


/*---------------------------------------------------------------------------*/

void clrjmp(int s,int e)
{
	int i;
	for(i=s;i<e;i++) jmpdst[i][0]=0;
}

void clrjmps(void)
{
	switch(opcode) {
		case JMP:
		case RTS:
		case RTE:

#if	0
		case BRA:
		case BRAF:
		case BSR:
		case BSRF:
		case BF:
		case BFS:
		case BT:
		case BTS:
#endif

		case ILL:
		case WORD:
		case TRAPA:
		case LONG:
			clrjmp(0,16);
			break;
		case JSR:
			clrjmp(0,8);
			break;
		default:
			break;
	}
}

char *symbol_check(long val)
{
	static	SYMBOL s;
	static	char ss[80];
	
	if(find_symbol_by_adrs(&s,val)) {	/* 検索成功 */
		strcpy(ss,s.name);
		return ss;
	}
	return(NULL);
}


/*
 *	オペランドの解釈(命令コード、オペランドのバイト列、カレントＰＣ)
 */

long extsign8(int i)
{
	long l;
	l = i & 0xff;
	if(l>=0x80) l|= -0x80;
	return l;
}

void disoperand(int mnem,long	adr)
{
	char *fmt,*p;
	int  c;
	char *gbrsym;
	int  branch12;
	int  branch8;
	int  rela,l;


	fmt = operands[ optype ];
	p   = oprbuf;
	
	while(	*fmt) {
		if(	*fmt=='$') {
			c=fmt[1];
			switch(c) {
				case 'm':
					p += sprintf(p,"r%d",rm);break;
				case 'n':
					p += sprintf(p,"r%d",rn);break;
				case 'x':
					switch(opcode) {
					case MOVL:p += sprintf(p,"%d",rx*4);break;
					case MOVW:p += sprintf(p,"%d",rx*2);break;
					default:
					case MOVB:p += sprintf(p,"%d",rx*1);break;
					}
					break;
				case 'b':
					branch12  = mnem & 0xfffL;
					if(branch12 & 0x800) branch12 |= ~0xfffL;
					branch12 <<=1;
					branch12 += ( adr + 4 );
					p += sprint_adrs(p,"",branch12);break;
				case 'd':
					branch8  = mnem & 0xffL;
					if(branch8 & 0x80) branch8 |= ~0xffL;
					branch8 <<=1;
					branch8 += ( adr + 4 );
					p += sprint_adrs(p,"",branch8);break;
				case '4':
					rela = mnem & 0xffL; rela *= 4; rela +=(adr & (-4L) )+4;
					if(opcode == MOVA) {
						/* mov address のときはデータは参照しない. */
						p += sprint_adrs(p,"",rela);
					}else{
					  if(iflag) {	/* 4byteの即値として評価する */
						l=dis_peekl(rela);
						p += sprint_adrs(p,"#",l);
						remout_imm40w(l);
					  }else{		/* 即値の入っているアドレスを表示する */
						p += sprint_adrs(p,"",rela);
						remout_imm4(rela);
					  }
					  /* 参照されたアドレスに .longの印を付けておく  */
						set_mark(adr,rela  ,DD);
						set_mark(adr,rela+2,DD2);
					}
					break;
				case '2':
					rela = mnem & 0xffL; rela *= 2; rela +=(adr+4);
				  if(iflag) {
					l=(short)dis_peekw(rela);
					p += sprint_adrs(p,"#",l);
					remout_imm20w(l);
				  }else{
					p += sprint_adrs(p,"",rela);
					remout_imm2(rela);
				  }
					set_mark(adr,rela,DW);
				  	break;
				case '1':	/* 使ってないっぽい */
					rela = mnem & 0xffL; rela *= 1; rela +=(adr+4);
					p += sprintf(p,"%08x",rela);break;
				case 'i':
					p += sprintf(p, "%ld",extsign8(imm));break;
				case 'g':	/* GBR base offset */
				    l=imm;
					switch(opcode) {
					case MOVL:l=imm*4;break;
					case MOVW:l=imm*2;break;
					default:
					case MOVB:l=imm*1;break;
					}
					if((gbr != 0xffffffffL)&&
					   ((gbrsym = symbol_check(gbr+l))!=NULL) 
					  ) {
						p += sprintf(p,"%s",gbrsym);
					}else{
						p += sprintf(p,"%d",l);
					}
					break;
				case '$':
					p += sprintf(p,"0x%04x",opc);break;
				default :
					printf("fmt err $%c\n",c);
			}
			fmt+=2;
		}else{
			*p++ = *fmt++;
		}
	}
	*p=0;
	disputs( oprbuf );
}




void remout_imm4(long adr)
{
	long l;
	l=dis_peekl(adr);
	if(l & 0xff000000L) {
/*		sprintf(rembuf," = $%08lx",l);	*/
		strcpy(rembuf," = ");
		sprint_adrs(rembuf+3,"",l);
	}else{
		sprintf(rembuf," = 0x%08lx (%5ld)",l,l);
	}
}

#define	RANGE10	( 1L << 24 )

void remout_imm40w(long adr)
{
	if( (adr < RANGE10 ) && (adr >= -RANGE10 ) )
	sprintf(rembuf," (%5ld)",adr);
}

void remout_imm20w(long adr)
{
	sprintf(rembuf," (%5d)",(int)adr);
}

void remout_imm2(long adr)
{
	long l;
	l=(short)dis_peekw(adr);
	sprintf(rembuf," = 0x%04lx (%5ld)",l,l);
}

void put_label(unsigned int c,int tabs)
{
	unsigned int	bank,ofs;
	int		f,i;
	SYMBOL 		syment;
	static char	ss[80]="";

//	printf("put_label(%x)\n",c);
	
	if( find_symbol_by_adrs( &syment,c ) ){
//		printf("put_label(%x) HIT\n",c);
		strcpy(ss , syment.name);
		dp=disbuf;
		for(i=0;i<tabs;i++) disputs(" ");
		disputs(ss);
		disputc(':');
		rembuf[0]=0;
		disout();
	}
}

int	sprint_adrs(char *p,char *q,long rela)
{
	int r;
	char *name;
	
	if( (name=symbol_check(rela))==NULL ){
		r = sprintf(p,"%s0x%lx",q,rela);
	}else{
		r = sprintf(p,"%s%s"   ,q,name);
	}
	return r;
}



void cutopr()
{
	char *p;
	opdst[0]=0;
	strcpy(opsrc,oprbuf);
	p=opsrc;
	if((p[0]=='@')&&(p[1]=='(')) {
		while( (*p!=0)&&(*p!=')') ) p++;
		if(*p!=')') return;
		p++;
		if(*p!=',') return;
		*p++=0;strcpy(opdst,p);
		return;
	}
	
	while( (*p!=0)&&(*p!=',') ) p++;
		 if(*p!=',') return;
	*p++=0;strcpy(opdst,p);
}

int	regnum( char *s)
{
	int i;
	if( s[0]!='r' ) return (-1);

	if( s[2]==0) {
		return s[1] - '0';
	}

	if( (s[1]=='1') && (s[3]==0) ) {
		if( (s[2] >='0' ) && (s[2]<='5') ) {
			return s[2] -'0' + 10;
		}
	}
	
	return (-1);
}

void setdst()
{
	int dst;

	dst = regnum(opdst);
#if	0
	{char s[80];
		sprintf(s," dst = %d <%s>",dst,opdst);
		dis_putln(s);
	}
#endif
	if(dst>=0) {
	 switch( opcode ) {
		case MOVL:
			strcpy(jmpdst[dst],opsrc);break;
		default:
			jmpdst[dst][0]=0;
	 }
	}
}


void remarks(char *buf)
{
	char *t,*p;
	int  r;
	if(*buf) return;

	t="";
	switch( opcode ) {
		case BF:
		case BFS:
			t = " if( T==0 ) branch!";break;
		case BT:
		case BTS:
			t = " if( T==1 ) branch!";break;
		case ADDC:
			t = " $t = $t + $s + T ,carry -->T";break;
		case ADDV:
			t = " $t = $t + $s , overflow -->T";break;
		case BRA:
			t = " delayed branch";break;
		case BSR:
			t = " delayed branch subr";break;
		case BRAF:
			t = " pc = pc + $s ; delayed branch";break;
		case BSRF:
			t = " pc = pc + $s ; delayed branch subr";break;
		case CLRMAC:
			t = " 0 --> MACH,MACL";break;
		case CLRT:
			t = " 0 --> T";break;
		case CMPEQ:
			t = " if( $t == $s )T=1";break;
		case CMPGE:
			t = " if( $t >= $s ) T=1";break;
		case CMPGT:	 
			t = " if( $t >  $s ) T=1";break;
		case CMPHI:	 
			t = " if( (unsigned) $t > $s ) T=1";break;
		case CMPHS:	 
			t = " if( (unsigned) $t >=$s ) T=1";break;
		case CMPPL:
			t = " if( $s > 0 ) T=1";break;
		case CMPPZ:
			t = " if( $s >=0 ) T=1";break;
		case CMPSTR:
			t = " if any byte(s) matching then T=1";break;
		case DIV0S:
			t = " M=MSB of $s ,Q=MSB of $t,T=M^Q";break;
		case DIV0U:
			t = " M/Q/T = 0";break;
		case DIV1:
			t = " $t / $s 1step divide";break;
		case DT:
			t = " if( --$s == 0 ) T=1;else T=0";break;
		case DMULUL:
		case DMULSL:
			t = " MACHL = $s * $t";break;
		case EXTSB:
			t = " byte->long (sign extend)";break;
		case EXTSW:
			t = " word->long (sign extend)";break;
		case EXTUB:
			t = " byte->long (zerofill)";break;
		case EXTUW:
			t = " word->long (zerofill)";break;
		case JMP:
		case JSR:
			r = regnum( &opsrc[1] );
			if( r >= 0 ) {
				t = jmpdst[r];
				if(*t=='#') t++;
			}
			break;
		case LDC:
		case LDCL:
		case LDS:
		case LDSL:
			break;
		case MACW:
		case MACL:
			t = " MACHL += $s * $t";break;

		case MOV:
		case MOVB:
		case MOVL:
		case MOVW:
			break;
		case MOVA:
			t = " load effective address";break;
		case MOVT:
			t = " T --> $s (0 or 1)";break;
		case MULSW:
		case MULUW:
		case MULL:
			t = " MACL = $s * $t";break;
		case NEG:
			t = " $t = -$s";break;
		case NEGC:
			t = " $t = -$s - T; borrow->T";break;
		case NOP:
		case NOT:
		case OR:
		case ORB:
			break;
		case ROTCL:
			t = " T <- $s <- T";break;
		case ROTCR:
			t = " T -> $s -> T";break;
		case ROTL:
			t = " T <- $s <- MSB";break;
		case ROTR:
			t = " LSB -> $s -> T";break;
		case RTE:
		case RTS:
			break;
		case SETT:
			t = " T=1";break;
		case SHAL:
			t = " T <- $s <- 0";break;
		case SHAR:
			t = " 0 -> $s -> T";break;
		case SHLL:
			t = " $s <<= 1";break;
		case SHLL2:
			t = " $s <<= 2";break;
		case SHLL8:
			t = " $s <<= 8";break;
		case SHLL16:
			t = " $s <<= 16";break;
		case SHLR:
			t = " $s >>= 1";break;
		case SHLR2:
			t = " $s >>= 2";break;
		case SHLR8:
			t = " $s >>= 8";break;
		case SHLR16:
			t = " $s >>= 16";break;
		case SLEEP:
			t = " good night";break;
		case STC:
		case STCL:
		case STS:
		case STSL:
		case SUB:
			break;
		case SUBC:
			t = " $t = $t - $s - T, T=borrow";break;
		case SUBV:
			t = " underflow -->T";break;
		case SWAPB:
			t = " $t[0:7] = $s[8:15] , $t[8:15] = $s[0:7]";break;
		case SWAPW:
			t = " $t[0:15] = $s[16:31] , $t[16:31] = $s[0:15]";break;
		case TASB:
			t = " if( $s == 0) {T=1;$s|=0x80;}";break;
		case TRAPA:
			break;
		case TST:
			if((opsrc[0]=='r')&&(opdst[1]==opsrc[1])) {
				t = " if( $s == 0 ) T=1";
			}else{
				t = " if( ($t & $s)==0 ) T=1";
			}
			break;
		case TSTB:
			t = " if( ($t & $s)==0 ) T=1";break;
		case XOR:
		case XORB:
			break;
		case XTRCT:
			t = " $t = center of [$s:$t]";break;
	}

/*	strcpy(buf,t);	の代役	*/
	while(*t) {
		if(*t=='$') {
			t++;
			switch(*t) {
				case 't':
					strcpy(buf,opdst);buf+=strlen(buf);break;
				case 's':
					strcpy(buf,opsrc);buf+=strlen(buf);break;
				default :
					*buf++ = *t;break;
			}
			t++;
		}else{
			*buf++ = *t++;
		}
	}
	*buf=0;
}
