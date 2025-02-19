/**********************************************************************
 *  ８０８６逆アセンブラ
 **********************************************************************
 */
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "diswin.h"

void	touppers(char *s);
char 	*search_hash2(int val,int flg);


/***  プロトタイプ宣言　***/
void	set_cpumode(int op,int ad);
void	set_exemode(int mode);
int 	dbr(void);
int 	db(void);
int 	dw(void);
int	dd(void);
void	mov(void);
int	disasm_i386(char *buf,int start,int size,FILE *ofp);
int 	retchk(char *s);
int 	chkprefix(void);
char	*sputs(char *p,char *s);
void	mne(char *s);
void	op1(char *s);
void	op2(char *s);
//void	mnes(int n,char *s,...);
void	disline(void);
void	hexprints(char *op,int n);
void	disline1(void);
void	relb(void);
void	rel_ll(void);
void	rel_dw(void);
void	immb(void);
void	immr(void);
void	farptr(void);
void	ind(void);
void	ind2(void);
void	farind(void);
void	immw(void);
void	immd(void);
void	immbw(int w);
void	immdw(void);
void	push(char *s);
void	pop(char *s);
void	undef(void);
void	undis(void);
void	modrmov(char *s1,char *s2,int opc);
void	put_regname(char *s,int reg,int w);
void	put_axreg(char *s);
void	modrmi(char *s1,int opc);
void	modrmseg(char *s1,char *s2);
void	modrm(char *s,int opc,int mod_rm,int ptrtype);
void	modrm32(char *s,int opc,int mod_rm,int ptrtype);
void	modrm32_sib(char *s,int mod_rm);
void	set_eop(void);
void	cutspc(char *s);
char	*put_segprefix(char *s);
void	ovride(int opc,char *s);
void	inst_0x0f(void);
void	inst_mmx(int opc);
void	inst_mmx_shift(int opc);
void	inst_mmx_mov(int opc);
void	inst_FPU(int opc);
void inst_kni(int opc,int rep);
void	AMD3dnow(void);


char    hexbuf[1128];    char *hexp;     /* ヘキサダンプ */
char    mnebuf[132];     char *mnep;     /* ニーモニック */
char    op1buf[1256];    char *op1p;     /* オペランド１ */
char    op2buf[1256];    char *op2p;     /* オペランド２ */
char	rembuf[1256];	 char *remp;		/* remark       */
char    disbuf[1256];    char *disp;     /* １行分のbuff */
char	symbol[1256];

char	*instbuf;

char    *regname[] ={"ax","cx","dx","bx","sp","bp","si","di"};
char    *hregname[]={"al","cl","dl","bl","ah","ch","dh","bh"};
char    *segname[] ={"es","cs","ss","ds","fs","gs","hs","is"};
char    *addop[]   ={"add","or","adc","sbb","and","sub","xor","cmp"};
char    *jcmd[]    ={"jo","jno","jb","jnb","jz","jnz","jna","ja",
                     "js","jns","jp","jnp","jl","jnl","jng","jg"};
char    *eregname[]={"eax","ecx","edx","ebx","esp","ebp","esi","edi"};
char    *mregname[]={"mm0","mm1","mm2","mm3","mm4","mm5","mm6","mm7"};
char    *rm00[]=
  {"[bx+si]","[bx+di]","[bp+si]","[bp+di]","[si]","[di]","[bp]","[bx]"};

char    *rm32_00[]=
  {"[eax]","[ecx]","[edx]","[ebx]","[esp]","[ebp]","[esi]","[edi]"};
char    *ptyp[]=
{"byte ptr ","word ptr ","dword ptr ","qword ptr ","tbyte ptr ","fword ptr ","",""};


char    *op;    /* 現在の解析位置を指しているポインタ */
char    *op0;   /* 命令の先頭番地を指しているポインタ */
char    *opp;   /* prefixも含めた命令の先頭番地を指しているポインタ */
int     seg_prefix=0;	/*セグメントオーバーライド・プレフィックスが付いていればそのコード*/
int     opsize32def=0;	/*デフォルト：１なら、オペランドサイズ  が３２ビット*/
int		exemode=0;		/* LE=1 */
int     adsize32def=0;	/*デフォルト：１なら、アドレス指定サイズが３２ビット*/
int     opsize32  =0;	/*現在の状態：１なら、オペランドサイズ  が３２ビット*/
int     adsize32  =0;	/*現在の状態：１なら、アドレス指定サイズが３２ビット*/
int     cnt_prefix=0;	/*プレフィックスの個数をカウント*/
int	startadrs=0;	/*ターゲット上での開始アドレス*/
int     mmx=0;          /*MMX レジスタを使用する*/
extern  int     filtermode;
extern  int     commentmode;
int	win_printlabel(int off,FILE *fp);
int	win_printrel( int off,char *buf);

/* オペランドがポインタである場合、そのサイズを示すヒント */
#define NONTYPE   0		/* byte ptr 	*/
#define PTRTYPE   1		/* word ptr 	*/
#define DWORDTYPE 2		/* dword ptr	*/
#define QWORDTYPE 3		/* qword ptr	*/
#define TBYTETYPE 4		/* tbyte ptr	*/
#define FWORDTYPE 5		/* fword ptr	*/

/* 命令バッファを指すポインタからＩＰを得る. */
#define	IP(ptr)  (startadrs+((ptr)-instbuf)) /* 80x86 Instruction Pointer (offset part)*/
#define UNDEF   "?undef?"


/**********************************************************************
 *  解析前に、セグメントがuse16なのかuse32なのかを指定する
 **********************************************************************
 */
void	set_cpumode(int op,int ad)
{
	opsize32def=op;
	adsize32def=ad;
}
void	set_exemode(int mode)
{
	exemode=mode;
}
/**********************************************************************
 *  解析データを取り出す関数
 **********************************************************************
 */
int     dbr()
{
        return (*op++);         /* 符号付き */
}

int     db()
{
        return (*op++) & 0xff;  /* 符号なし */
}

int     dw()                    /* ワード */
{
        int h,l;
        l = (*op++) & 0xff;
        h = (*op++) & 0xff;
        return l|(h<<8);
}

int    dd()                    /* ロングワード */
{
        int *dword;

        dword=(int *)op;       /* ビッグエンディアンの機械では要注意！！！*/
        op+=4;
        return (*dword);
}

void	mov()
{
        mne("mov");
}






/**********************************************************************
 *  指定されたバイト数の逆アセンブルを行ない、ＦＩＬＥに出力する.
 **********************************************************************
 *  アセンブルしたバイト数をint で返す.
 */
int disasm_i386(char *buf,int start,int size,FILE *ofp)
{
	unsigned short asmsize;
	char *endadrs;
	
	startadrs=start;
        op=instbuf=buf;
        endadrs   =buf+(unsigned)size;
        while(op < endadrs) {

                win_printlabel( IP(op) ,ofp);
        /* １命令分の解析 */
                opp=op;         /* prefixを含めた先頭のポインタ */
                seg_prefix=0;
                opsize32  =opsize32def;
                adsize32  =adsize32def;
                cnt_prefix=0;
                while( chkprefix() ) {  /*prefix命令を読む      */
                        if( ++cnt_prefix>=4) break;
                }
                op0=op;         /* predixを含まない先頭ポインタ */
                disline();      /* prefixを含まない命令解析     */
                cutspc(disbuf); /* 行末の余分なスペースを取り去る*/
/***            win_printrel(IP(op0) + 1 ,disbuf+27);***/

        /* １行分の表示 */
                touppers(disbuf);
                fprintf(ofp,"%s" CRLF,disbuf);
                
                if(filtermode) {
                        mnebuf[3]=0;	/* retf jmpf の最後の1字を削る.(汚い処理) */
                        if(retchk(mnebuf)) {	/* ret,jmp かどうかを調べる */
                                fprintf(ofp,CRLF);/* RET の後ろに空行を付加 */
                        }
                }
        }
        asmsize = op - buf;
        return asmsize;		/* 逆アセンブルした総バイト数を返却する(続きの命令位置確定の為)*/
}

int retchk(char *s)
{
        if(strcmp(s,"ret")==0) return 1;
        if(strcmp(s,"jmp")==0) return 1;
        return 0;
}


int chkprefix(void)
{
        switch(*op) {
         case 0x26:
         case 0x2e:
         case 0x36:
         case 0x3e:
         case 0x64:
         case 0x65:
                seg_prefix=*op++;
                return 1;
         case 0x66:
                opsize32  ^=1;
                op++;
                return 1;
         case 0x67:
                adsize32  ^=1;
                op++;
                return 1;
         default: break;
        }
        return 0;
}

#define sputc(c,p) *(p)++ = c;

char   *sputs(char *p,char *s)
{
	while(*s) {
		*p++ = *s++;
	}
	*p = 0;
	return p;
}

#define PR1    op1p += sprintf
#define PR2    op2p += sprintf

void	mne(char *s)
{
//	printf(" >>> sputs(%lx,%lx)\n",(size_t) mnep,(size_t)s);
      mnep=sputs(mnep,s);
}
void	op1(char *s)
{
        op1p=sputs(op1p,s);
}
void	op2(char *s)
{
        op2p=sputs(op2p,s);
}
void	rem(char *s)
{
	if(commentmode) {
        remp=sputs(remp,s);
	}
}
void	mnes(int n,...)
{
	int i;
	char *t;
	va_list args;
	
	va_start(args,n);

	for(i=0;i<n;i++) {
		t = va_arg(args,char*);
	}
	
	mne(va_arg(args,char*) );

	va_end(args);
}


#if 0
void	mnes(int n,char *s,...)
	char **p;
        p = &s;
        mne(p[n]);
}
#endif


/**********************************************************************
 *  １行分の逆アセンブル
 **********************************************************************
 */
void	disline()
{
        static char *fmt[]={"%-19s %-6s %s"		/*1 operand 16bit seg*/
                           ,"%-19s %-6s %s,%s"	/*2 operand 16bit seg*/
                           ,"%-27s %-7s %s"		/*1 operand 32bit seg*/
                           ,"%-27s %-7s %s,%s"	/*2 operand 32bit seg*/
        };
	int f;

        hexp=hexbuf;*hexp=0;
        mnep=mnebuf;*mnep=0;
        op1p=op1buf;*op1p=0;
        op2p=op2buf;*op2p=0;
        remp=rembuf;*remp=0;
                disline1();
                hexprints(opp,op-opp);
                set_eop();
                *hexp=0;
                *mnep=0;
	f=0;
        if(op2buf[0])  f|=1;
        if(adsize32def)f|=2;
        
        sprintf(disbuf,fmt[f],hexbuf,mnebuf,op1buf,op2buf);
        if(rembuf[0]!=0) {
        	strcpy(hexbuf,disbuf);
 	       sprintf(disbuf,"%-50s ;%s",hexbuf,rembuf);
        }
}

/**********************************************************************
 *  １６進数で命令を表示する
 **********************************************************************
 */
void	hexprints(char *op,int n)
{

//		if(op< (char *)0x00010000){ hexp+=sprintf(hexp,"#ERR %08x",op);return;}
//		if(op>=(char *)0x02000000){ hexp+=sprintf(hexp,"#ERR %08x",op);return;}
//	上記:実はop1buf[128]がオーバフローして死んでいた...
//

        while(n) {
                hexp += sprintf(hexp,"%02x",*op & 0xff);
                op++;
                n--;
        }
}

/**********************************************************************
 *  １行逆アセンブラ（命令解析の本体）
 **********************************************************************
 */
void	disline1(void)
{
        int opc;
        int mod_rm;
        int mod;
        int reg;
        int rm;
        int f;
        
        if(adsize32def) {
            sprintf(hexp,"%08x ",(int)IP(opp) );hexp+=9;
        }else{
            sprintf(hexp,"%04x ",(int)IP(opp) );hexp+=5;
        }
        opc = (*op++) & 0xff;
        
        switch(opc) {
/*****************************************************************/
 case 0x00: case 0x01: case 0x02: case 0x03: case 0x04: case 0x05:
 case 0x08: case 0x09: case 0x0a: case 0x0b: case 0x0c: case 0x0d:
 case 0x10: case 0x11: case 0x12: case 0x13: case 0x14: case 0x15:
 case 0x18: case 0x19: case 0x1a: case 0x1b: case 0x1c: case 0x1d:
 case 0x20: case 0x21: case 0x22: case 0x23: case 0x24: case 0x25:
 case 0x28: case 0x29: case 0x2a: case 0x2b: case 0x2c: case 0x2d:
 case 0x30: case 0x31: case 0x32: case 0x33: case 0x34: case 0x35:
 case 0x38: case 0x39: case 0x3a: case 0x3b: case 0x3c: case 0x3d:
/*****************************************************************/
        f    =opc & 7    ;
        mne( addop[ opc >> 3 ] );
        switch(f) {
         case 0:
         case 1:
                modrmov(op2buf,op1buf,opc);
                break;
         case 2:
         case 3:
                modrmov(op1buf,op2buf,opc);
                break;
         case 4:op1("al")       ;immb();break;
         case 5:put_axreg(op1p);immdw();break;
        }
        break;
/*****************************************************************/
 case 0x06:push("es");break;
 case 0x0e:push("cs");break;
 case 0x16:push("ss");break;
 case 0x1e:push("ds");break;

 case 0x0f:     inst_0x0f();break;
 case 0x07: pop("es");break;
 case 0x17: pop("ss");break;
 case 0x1f: pop("ds");break;

 case 0x26:mne("es:");break;
 case 0x2e:mne("cs:");break;
 case 0x36:mne("ss:");break;
 case 0x3e:mne("ds:");break;

 case 0x27:mne("daa");break;
 case 0x2f:mne("das");break;
 case 0x37:mne("aaa");break;
 case 0x3f:mne("aas");break;

/*****************************************************************/
 case 0x40: case 0x41: case 0x42: case 0x43:
 case 0x44: case 0x45: case 0x46: case 0x47:
 case 0x48: case 0x49: case 0x4a: case 0x4b:
 case 0x4c: case 0x4d: case 0x4e: case 0x4f:
 case 0x50: case 0x51: case 0x52: case 0x53:
 case 0x54: case 0x55: case 0x56: case 0x57:
 case 0x58: case 0x59: case 0x5a: case 0x5b:
 case 0x5c: case 0x5d: case 0x5e: case 0x5f:
/*****************************************************************/
        mnes( (opc>>3)&3 ,"inc","dec","push","pop" );
        put_regname(op1p,opc&7,1);
        break;
/*****************************************************************/
 case 0x60:
        if(opsize32) {
        	mne("pushad");
        	rem("push eax,ecx,edx,ebx,esp,ebp,esi,edi");
        }else{
            mne("pusha");
        	rem("push ax,cx,dx,bx,sp,bp,si,di");
        }
        break;
 case 0x61:
        if(opsize32) {
        	mne("popad");
        	rem("pop edi,esi,ebp,---,ebx,edx,ecx,eax");
        }else{
            mne("popa");
        	rem("pop di,si,bp,--,bx,dx,cx,ax");
        }
        break;
/*****************************************************************/
 case 0x62:
                mne("bound") ;
                modrmov(op1buf,op2buf,1);
                break; 
 case 0x63:
                mne("arpl")  ;
                modrmov(op2buf,op1buf,opc);
                break; 
 case 0x64:mne("fs:")   ;break;
 case 0x65:mne("gs:")   ;break;
 case 0x66:mne("opsize32");break;
 case 0x67:mne("adsize32");break;
/*****************************************************************/
 case 0x68:mne("push")  ;immdw();break;
 case 0x69:
        mne("imul");
        modrmov(op1buf,op2buf,opc);
        *op2p++ = ',';
        immw();
        break;
        
 case 0x6a:mne("push")  ;immr();break;
 case 0x6b:
        mne("imul");
        modrmov(op1buf,op2buf,opc);
        *op2p++ = ',';
        immb();
        break;
        
 case 0x6c:mne("insb")  ;break;
 case 0x6d:mne("insw")  ;break;
 case 0x6e:mne("outsb") ;break;
 case 0x6f:mne("outsw") ;break;
/*****************************************************************/
 case 0x70: case 0x71: case 0x72: case 0x73:
 case 0x74: case 0x75: case 0x76: case 0x77:
 case 0x78: case 0x79: case 0x7a: case 0x7b:
 case 0x7c: case 0x7d: case 0x7e: case 0x7f:
/*****************************************************************/
        mne( jcmd[ opc & 0x0f ] );
        relb();
        break;
/*****************************************************************/
 case 0x80:     /* addop memb,immb */
 case 0x81:     /* addop memw,immw */
 case 0x82:     /* undef  <=== Undocumented 8086 */
 case 0x83:     /* addop memw,immb */
        mod_rm=(*op++) & 0xff;
        reg   =(mod_rm>>3)&7;
        mne( addop[reg] );
        modrm(op1buf,opc,mod_rm,PTRTYPE);
        switch(opc) {
                case 0x80:immb() ;break;
                case 0x81:immdw();break;
                case 0x82:immb() ;break;
                case 0x83:immr() ;break;
        }
        break;

/*****************************************************************/
 case 0x84:
 case 0x85:
 case 0x86:
 case 0x87:
        mnes((opc-0x84)/2,"test","xchg");
        modrmov(op1buf,op2buf,opc);
        break;
/*****************************************************************/
 case 0x88:
 case 0x89:
        mov();
        modrmov(op2buf,op1buf,opc);
        break;
 case 0x8a:
 case 0x8b:
        mov();
        modrmov(op1buf,op2buf,opc);
        break;
 case 0x8c:
        mov();
        modrmseg(op2buf,op1buf);
        break;
 case 0x8d:
        mne("lea");
        modrmov(op1buf,op2buf,opc);
        break;
 case 0x8e:
        mov();
        modrmseg(op1buf,op2buf);
        break;
 case 0x8f:
        mne("pop");
        mod_rm=(*op++) & 0xff;
        modrm(op1p,opc,mod_rm,PTRTYPE);
        break;
/*****************************************************************/
 case 0x90:mne("nop");break;

 case 0x91: case 0x92: case 0x93: case 0x94: case 0x95: case 0x96: case 0x97:
        mne( "xchg" );
        put_axreg(op1p);
        put_regname(op2p,opc&7,1);
        break;
 
/*****************************************************************/
 case 0x98:mne("cbw");break;
 case 0x99:
 	//mne("cwd");break;
      if (opsize32)
        mne("cdq");
      else
        mne("cwd");
      break;

 case 0x9a:mne("call");farptr();break;
 case 0x9b:mne("wait");break;
/*****************************************************************/
 case 0x9c: case 0x9d: case 0x9e: case 0x9f:
		if(opsize32) {
	        mnes(opc-0x9c,"pushfd","popfd","sahf","lahf");
	   	}else{
	        mnes(opc-0x9c,"pushf","popf","sahf","lahf");
	   	}
	   	break;
/*****************************************************************/
 case 0xa0:mov();op1("al")      ;ind2();break;
 case 0xa1:mov();put_axreg(op1p);ind2();break;
 case 0xa2:mov();ind();op2("al")       ;break;
 case 0xa3:mov();ind();put_axreg(op2p) ;break;
/*****************************************************************/
 case 0xa4:
 case 0xa5:
 case 0xa6:
 case 0xa7:
    if(opsize32) {
        mnes(opc-0xa4,"movsb","movsd","cmpsb","cmpsd");
    }else{
        mnes(opc-0xa4,"movsb","movsw","cmpsb","cmpsw");
    }
    break;
 case 0xa8:mne("test");op1("al");immb();break;
 case 0xa9:mne("test");put_axreg(op1p);immdw();break;

 case 0xaa:
 case 0xab:
 case 0xac:
 case 0xad:
 case 0xae:
 case 0xaf:
    if(opsize32) {
        mnes(opc-0xaa,"stosb","stosd","lodsb","lodsd","scasb","scasd");
    }else{
        mnes(opc-0xaa,"stosb","stosw","lodsb","lodsw","scasb","scasw");
    }
        switch( seg_prefix ) {
         case 0x26:ovride(opc,"es:[si]");mnebuf[4]=0;break;
         case 0x2e:ovride(opc,"cs:[si]");mnebuf[4]=0;break;
         case 0x36:ovride(opc,"ss:[si]");mnebuf[4]=0;break;
         case 0x3e:ovride(opc,"ds:[si]");mnebuf[4]=0;break;
         case 0x64:ovride(opc,"fs:[si]");mnebuf[4]=0;break;
         case 0x65:ovride(opc,"gs:[si]");mnebuf[4]=0;break;
         default:break;
        }
        break;
/*****************************************************************/
 case 0xb0: case 0xb1: case 0xb2: case 0xb3:
 case 0xb4: case 0xb5: case 0xb6: case 0xb7:
        mov();op1(hregname[opc&7]);immb();break;
/*****************************************************************/
 case 0xb8: case 0xb9: case 0xba: case 0xbb:
 case 0xbc: case 0xbd: case 0xbe: case 0xbf:
        mov();
        put_regname(op1p,opc&7,1);
        immdw();break;
/*****************************************************************/
/*****************************************************************/
 case 0xc2:mne("ret");immw();break;
 case 0xc3:mne("ret");break;
/*****************************************************************/
 case 0xc4:
 case 0xc5:
        mod_rm=(*op++) & 0xff;
        reg   =(mod_rm>>3)&7;
        mnes(opc-0xc4,"les","lds");
                op1(regname[reg]);
                modrm(op2buf,opc,mod_rm,DWORDTYPE);
        break;
/*****************************************************************/
 case 0xc6:
 case 0xc7:mov();
        modrmi(op1buf,opc);
        break;
/*****************************************************************/
 case 0xc8:mne("enter");immw();immb();break;
 case 0xc9:mne("leave");break;
 case 0xca:mne("retf");immw();break;
 case 0xcb:mne("retf");break;
 case 0xcc:mne("int");op1("3");break;
 case 0xcd:
 		mne("int");
		if( (exemode==1) && (*op==0x20)){	// LE: int 0x20
			immb();
			immd();
		}else{
			immb();
		}
	 	break;
 case 0xce:mne("into");break;
 case 0xcf:mne("iret");break;
/*****************************************************************/
 case 0xc0:             /* SHIFT rmb,immb  */
 case 0xc1:             /* SHIFT rmb,immb  */

 case 0xd0:             /* SHIFT rmb,1  */
 case 0xd1:             /* SHIFT rmw,1  */
 case 0xd2:             /* SHIFT rmb,CL */
 case 0xd3:             /* SHIFT rmw,CL */
 {static char *n[]={"rol","ror","rcl","rcr","shl","shr","sal","sar"};
        mod_rm=(*op++) & 0xff;
        reg   =(mod_rm>>3)&7;
        mne(n[reg]);
        modrm(op1buf,opc,mod_rm,PTRTYPE);
        if(opc >= 0xd2) {
                op2("cl");
        }else
        if(opc <  0xd0) {
                immb();
        }else{
                op2("1");
        }
 }
        break;
/*****************************************************************/
 case 0xd4:
        mod_rm=(*op++) & 0xff;
        mne("aam");

        if(mod_rm!=10) {
                PR1(op1p,"%x",mod_rm);  /* AAM AADのオペランドが１０以外！ */
        }
        
        rem("AL=(AH*N)+AL;AH=0");
        break;
 case 0xd5:
        mod_rm=(*op++) & 0xff;
        mne("aad");
        if(mod_rm!=10) {
                PR1(op1p,"%x",mod_rm);  /* AAM AADのオペランドが１０以外！ */
        }
        rem("AH=AL/N;AL=AL%N");
        break;
 case 0xd6:mne("setalc");rem("set AL to carry(cy->AL)");
 		break;
 case 0xd7:mne("xlat");
 		break;
/*****************************************************************/
 case 0xd8: case 0xd9: case 0xda: case 0xdb:
 case 0xdc: case 0xdd: case 0xde: case 0xdf:
        inst_FPU(opc);break;
/*****************************************************************/
 case 0xe0: case 0xe1: case 0xe2: case 0xe3:
        mnes(opc-0xe0,"loopnz","loopz","loop","jcxz");
        relb();
        break;
/*****************************************************************/
 case 0xe4:     mne("in")  ;op1("al");immb();break;
 case 0xe5:     mne("in")  ;op1("ax");immb();break;
 case 0xe6:     mne("out") ;immb();op2("al");break;
 case 0xe7:     mne("out") ;immb();op2("ax");break;
/*****************************************************************/
 case 0xe8:
 case 0xe9:
        mnes(opc-0xe8,"call","jmp") ;
	rel_dw();
        break;
 case 0xea:     mne("jmp") ;farptr();break;
 case 0xeb:     mne("jmp") ;
        PR1(op1p,"short %04x",(int) IP(op0)+2+dbr());
        break;
 case 0xec:     mne("in") ;op1("al");op2("dx");break;
 case 0xed:     mne("in") ;op1("ax");op2("dx");break;
 case 0xee:     mne("out");op1("dx");op2("al");break;
 case 0xef:     mne("out");op1("dx");op2("ax");break;
/*****************************************************************/
 case 0xf0:     mne("lock");break;
 case 0xf1:     mne("icebp");rem("(386+) int 1h  (AMD386SXLV) SMI");break;
/*****************************************************************/
 case 0xf2:
 case 0xf3:
 {
   static char *a4[2][12] = {
     {
        "movsb","movsd","cmpsb","cmpsd",NULL,NULL,
        "stosb","stosd","lodsb","lodsd","scasb","scasd"
     },
     {
        "movsb","movsw","cmpsb","cmpsw",NULL,NULL,
        "stosb","stosw","lodsb","lodsw","scasb","scasw"
     }
   };
        int c,whitch;
        int kniop;

  #if KNI
        if( (opc == 0xf3)&&(*op == 0x0f) ) {    // KNI single scaler
            op++;           // [f3] [0f]
            kniop = *op++;  //           KNI
            inst_kni(kniop & 0xff,0xf3);
            break;
        }
  #endif
        mnes(opc-0xf2,"repne","rep");
        c=(*op) & 0xff;
        whitch = opsize32 ? 0 : 1;

        if (c == 0x66) {
            op++;
            c=(*op) & 0xff;
            whitch = 1; // opsize32 ^= 1; ?? 自信なし...
        }

        if( (c>=0xa4) && (c<0xb0) ) {
                 c-=0xa4;
                 if(a4[whitch][c]!=NULL) {op1(a4[whitch][c]);op++;}
        }
#if	0
   static char *a4[]={  "movsb","movsw","cmpsb","cmpsw",NULL,NULL,
                        "stosb","stosw","lodsb","lodsw","scasb","scasw"};
        int c;
		int kniop;

#if		KNI
		if( (opc == 0xf3)&&(*op == 0x0f) ) {	// KNI single scaler
			op ++;			// [f3] [0f]
			kniop = *op++;	//           KNI
	   	    inst_kni(kniop & 0xff,0xf3);
	   	    break;
		}
#endif

        mnes(opc-0xf2,"repne","rep");
        c=(*op) & 0xff;
        if( (c>=0xa4) && (c<0xb0) ) {
                 c-=0xa4;
                 if(a4[c]!=NULL) {op1(a4[c]);op++;}
        }
#endif
 }
        break;
/*****************************************************************/
 case 0xf4:     mne("hlt");break;
 case 0xf5:     mne("cmc");break;
/*****************************************************************/
 case 0xf6:
 case 0xf7:
  {static char *n[]={"test",UNDEF,"not","neg","mul","imul","div","idiv"};
        mod_rm=(*op++) & 0xff;
        reg   =(mod_rm>>3)&7;
        mne(n[reg]);
        modrm(op1buf,opc,mod_rm,PTRTYPE);
        if(reg == 0) {  /* TEST rm,imm */
                immbw(opc);
        }
 }
        break;
/*****************************************************************/
 case 0xf8:
 case 0xf9:
 case 0xfa:
 case 0xfb:
 case 0xfc:
 case 0xfd:
                mnes(opc-0xf8,"clc","stc","cli","sti","cld","std");
                break;
 case 0xfe:
        mod_rm=(*op++)&0xff;
        reg   =(mod_rm>>3)&7;
        switch(reg) {
         case 0:
         case 1:
                mnes(reg,"inc","dec");modrm(op1p,opc,mod_rm,PTRTYPE);break;
         default:
                undef();
        }
        break;
/*****************************************************************/
 case 0xff:
        mod_rm=(*op++)&0xff;
        reg   =(mod_rm>>3)&7;
        switch(reg) {
         case 0:
         case 1:
        mnes(reg,"inc","dec");modrm(op1p,opc,mod_rm,PTRTYPE);break;
         case 2:mne("call");modrm(op1p,opc,mod_rm,PTRTYPE)  ;break;
         case 3:mne("call");modrm(op1p,opc,mod_rm,DWORDTYPE);break;
         case 4:mne("jmp") ;modrm(op1p,opc,mod_rm,PTRTYPE)  ;break;
         case 5:mne("jmp") ;modrm(op1p,opc,mod_rm,DWORDTYPE);break;
         case 6:mne("push");modrm(op1p,opc,mod_rm,PTRTYPE)  ;break;
         case 7:undef();break;
        }
        break;
/*****************************************************************/
 default:
                undef();
                break;
        }
}

/**********************************************************************
 *  相対バイト（分岐先）を表示
 **********************************************************************
 */
void	relb()
{
        if(op1buf[0]) {
                PR2(op2p,"%04x",(int) IP(op0)+2+dbr());
        }else{
                PR1(op1p,"%04x",(int) IP(op0)+2+dbr());
        }
}

void	rel_ll()
{
        if(op1buf[0]) {
                PR2(op2p,"%06x",(int) (IP(op0)+5+dd()));
        }else{
                PR1(op1p,"%06x",(int) (IP(op0)+5+dd()));
        }
}

void	rel_dw()
{
	int ea;
	if(adsize32) {
	        ea=dd();ea+=IP(op);PR1(op1p,"%08x",ea);
	}else{
	        ea=dw();ea+=IP(op);PR1(op1p,"%04x",ea & 0xffffL);
	}
}

/**********************************************************************
 *  即値バイト（符号なし）を表示
 **********************************************************************
 */
void	immb()
{
        if(op1buf[0]) {
                PR2(op2p,"%02x",db());
        }else{
                PR1(op1p,"%02x",db());
        }
}

/**********************************************************************
 *  即値バイト（符号付き）を表示
 **********************************************************************
 */
void	immr()
{
        int c;
        c=dbr();
        if(op1buf[0]) {
                if(c>=0) {
                        PR2(op2p,"+%02x",c);
                }else{
                        PR2(op2p,"-%02x",-c);
                }
        }else{
                if(c>=0) {
                        PR1(op1p,"+%02x",c);
                }else{
                        PR1(op1p,"-%02x",-c);
                }
        }
}

/**********************************************************************
 *  ＳＥＧ：ＯＦＦ　を表示
 **********************************************************************
 */
void	farptr()
{
        int seg,off;
        
        if(win_printrel(IP(op),symbol)) {
        	op+=4;op1(symbol);return;
        }

        off=dw();
        if(win_printrel(IP(op),symbol)) {
        	PR1(op1p,"%s:%04x",symbol,off);
        	op+=2;
        	return;
        }

        seg=dw();
        PR1(op1p,"%04x:%04x",seg,off);
}

/**********************************************************************
 *  ［ＯＦＦ］　を表示
 **********************************************************************
 */
void	ind()
{
        op1p = put_segprefix(op1p);

        if(win_printrel(IP(op),symbol)) {
        	PR1(op1p,"[%s]",symbol);
        	op+=2;
        	return;
        }

	if(adsize32) {
	        PR1(op1p,"[%08x]",dd());
	}else{
	        PR1(op1p,"[%04x]",dw());
	}
}

void	ind2()
{
        op2p = put_segprefix(op2p);

        if(win_printrel(IP(op),symbol)) {
        	PR2(op2p,"[%s]",symbol);
        	op+=2;
        	return;
        }

	if(adsize32) {
	        PR2(op2p,"[%08x]",dd());
	}else{
	        PR2(op2p,"[%04x]",dw());
	}
}

/**********************************************************************
 *  ［ＳＥＧ：ＯＦＦ］　を表示
 **********************************************************************
 */
void	farind()
{
        int seg,off,off32;

	if(adsize32) {
	        off32=dd();
	        seg=dw();
	        PR1(op1p,"[%04x:%08x]",seg,off32);
        }else{
	        off=dw();
	        seg=dw();
	        PR1(op1p,"[%04x:%04x]",seg,off);
        }
}

/**********************************************************************
 *  即値ワードを表示
 **********************************************************************
 */
void	immw()
{
        if(win_printrel(IP(op),symbol)) {
	        if(op1buf[0]) {
        		PR2(op2p,"%s",symbol);
        		op+=2;
	        	return;
	        }else{
        		PR1(op1p,"%s",symbol);
        		op+=2;
	        	return;
		}
        }

        if(op1buf[0]) {
                PR2(op2p,"%04x",dw());
        }else{
                PR1(op1p,"%04x",dw());
        }
}

void	immd()
{
        if(op1buf[0]) {
                PR2(op2p,"%08x",dd());
        }else{
                PR1(op1p,"%08x",dd());
        }
}

/**********************************************************************
 *  即値バイトあるいはワードを表示
 **********************************************************************
 */
void	immbw(int w)
{
        if(w&1) immdw();
        else    immb();
}

void	immdw()
{
        if(opsize32) immd();
        else         immw();
}

void	push(char *s)
{
        mne("push");op1(s);
}

void	pop(char *s)
{
        mne("pop");op1(s);
}

void	undef()
{
        mne(UNDEF);             /* 未定義命令である */
}

void	undis()
{
        mne("???");             /* このパターンの解析は未作成 */
}

/**********************************************************************
 *  オペランド解析      reg,mem
 **********************************************************************
 */
void	modrmov(char *s1,char *s2,int opc)
{
        int     mod_rm;
        int     reg,w;

        mod_rm=(*op++) & 0xff;
        reg   =( mod_rm >>3) & 7;
        w     =opc & 1;

//		printf("modrmov(mod_rm=%x reg=%x)\n",mod_rm,reg);

        put_regname(s1,reg,w);          /*最初のオペランドはレジスタである */
        modrm(s2,opc,mod_rm,NONTYPE);   /*第２オペランドはr/m */
        set_eop();
}

/**********************************************************************
 *  オペランド解析      MMX,XMM
 **********************************************************************
 */
void	modrmovmmm(char *s1,char *s2,int mmm1,int mmm2)
{
        int     mod_rm;
        int     reg,w;
		int		opc=1;	// ,w

        mod_rm=(*op++) & 0xff;
        reg   =( mod_rm >>3) & 7;
        w     =opc & 1;

		mmx = mmm1;
	        put_regname(s1,reg,w);          /*最初のオペランドはレジスタである */
		mmx = mmm2;
    	    modrm(s2,opc,mod_rm,NONTYPE);   /*第２オペランドはr/m */
    	    set_eop();
		mmx = 0;
}


void	put_regname(char *s,int reg,int w)
{
        if(mmx==1) {
                strcpy(s,mregname[reg]);	// mm0〜7;
        } else if(mmx==2) {
                s[0] = 'x';
                strcpy(s+1,mregname[reg]);	// xmm0〜7;
        } else if(w&1) {
            if(opsize32)
                strcpy(s,eregname[reg]);
            else
                strcpy(s, regname[reg]);
        }
        else    strcpy(s,hregname[reg]);
}

void	put_axreg(char *s)
{
    if(opsize32)
        strcpy(s,"eax");
    else
        strcpy(s,"ax");
}

/**********************************************************************
 *  
 **********************************************************************
 */
void	modrmi(char *s1,int opc)
{
        int     mod_rm;

        mod_rm=(*op++) & 0xff;
        modrm(s1,opc,mod_rm,PTRTYPE);

        if(opc&1) immdw();
        else      immb();
}

/**********************************************************************
 *  
 **********************************************************************
 */
void	modrmseg(char *s1,char *s2)
{
        int     mod_rm;
        int     reg,w;

        mod_rm=(*op++) & 0xff;
        reg   =( mod_rm >>3) & 7;

        strcpy(s1,segname[reg]);
        modrm(s2,1,mod_rm,NONTYPE);
}

/**********************************************************************
 *  命令のポストバイト（MOD REG R/M）を解析して表示する。
 **********************************************************************
 */
void	modrm(char *s,int opc,int mod_rm,int ptrtype)
{
        int     w     ;
        int     mod   ;
        int     reg   ;
        int     rm    ;
        int     ea;
        char    *rmstr;

        if(adsize32) {
        	modrm32(s,opc,mod_rm,ptrtype);
        	return;
       	}

        w     = opc & 1;
        mod   = mod_rm >>6;
        rm    = mod_rm & 7;
        rmstr = rm00[rm];

        if(opsize32) ptrtype=DWORDTYPE;

        if(!mmx)
        if( (ptrtype!=NONTYPE)&&(mod!=3) ){
                if(ptrtype>=DWORDTYPE) {
                        s += sprintf(s,"%s",ptyp[ptrtype]);/* 2:DWORD 3:... */
                }else{
                        s += sprintf(s,"%s",ptyp[w]);      /* 0:BYTE 1:WORD */
                }
        }

        s = put_segprefix(s);

        switch(mod) {
         case 0:        /* [breg+ireg]  */
                if(rm==6) {
                        sprintf(s,"[%04x]",dw());
                }else{
                        strcpy(s,rmstr);
                }
                break;
         case 1:        /* disp8 [ireg] */
                s = sputs(s,rmstr) - 1;
                ea = dbr();
                if(ea>=0) {
                        sprintf(s,"+%02x]",ea); /* plus  */
                }else{
                        sprintf(s,"-%02x]",-ea);/* minus */
                }
                break;
         case 2:        /* disp16[ireg] */
                s = sputs(s,rmstr) - 1;
                sprintf(s,"+%04x]",dw());
                break;
         case 3:        /* reg,reg */
                put_regname(s,rm,w);
                break;
         default:
                break;
        }
}


void	modrm32(char *s,int opc,int mod_rm,int ptrtype)
{
        int     w     ;
        int     mod   ;
        int     reg   ;
        int     rm    ;
        int     ea;
        	int   dword;
        	char *label;
        char    *rmstr;

        w     = opc & 1;
        mod   = mod_rm >>6;
        rm    = mod_rm & 7;
        rmstr = rm32_00[rm];

        if( w && opsize32) ptrtype=DWORDTYPE;

        if(!mmx)
        if( (ptrtype!=NONTYPE)&&(mod!=3) ){
                if(ptrtype==DWORDTYPE) {
                        s += sprintf(s,"%s",ptyp[2]);
                }else{
                        s += sprintf(s,"%s",ptyp[w]);
                }
        }

        s = put_segprefix(s);

        switch(mod) {
         case 0:        /* [breg+ireg]  */
                switch(rm) {
                case 4: modrm32_sib(s,mod_rm)     ;break;
                case 5: 
						dword=dd();
						label=search_hash2(dword,0);
						if(label != NULL) {
	                		sprintf(s,"[%s]",label);
						}else{
	                		sprintf(s,"[%08x]",dword );
	                	}
	                	break;
                default:
                        strcpy(s,rmstr);break;
                }
                break;
         case 1:        /* disp8 [ireg] */
                if(rm==4) modrm32_sib(s,mod_rm);
                else{
                        s = sputs(s,rmstr) -1;
                        ea = dbr();
                        if(ea>=0) {
                                sprintf(s,"+%02x]",ea); /* plus  */
                        }else{
                                sprintf(s,"-%02x]",-ea);/* minus */
                        }
                }
                break;
         case 2:        /* disp32[ireg] */
                if(rm==4) modrm32_sib(s,mod_rm);
                else{
                        s = sputs(s,rmstr) -1;
                        sprintf(s,"+%08x]",dd());
                }
                break;
         case 3:        /* reg,reg */
                put_regname(s,rm,w);
                break;
         default:
                break;
        }
}

static  char scalestr[]="1248";	/* SCALE: x1 x2 x4 x8 */
/**********************************************************************
 *  SIB = scale-index-base      post byte = ss iii bbb
 **********************************************************************
 *  mod = 00   [base + index*scale ]
 *        01   [base + index*scale + offset8 ]
 *        10   [base + index*scale + offset32]
 *  例外:
 *	index = 100 (ESP) のときはindex*scaleなし.
 *  mod = 00かつ、
 *      base  = 101 (EBP) のときはbaseレジスタなしで代わりに32bit 即値BASEに.
 */
void	modrm32_sib(char *s,int mod_rm)
{
        int sib;
        int scale; /* x1 x2 x4 x8 */
        int index; /* index register "EAX,ECX,EDX,EBX,Noindex,EBP,ESI,EDI"*/
        int base;  /* base  register */
        int mod;
        int ea;
        int nobase;
        
        char *basereg;

        sib   = (*op++) & 0xff;
        scale = sib >> 6;
        index =(sib >> 3) & 7;
        base  = sib       & 7;
        mod   = mod_rm>>6;

        basereg = rm32_00[base];
	nobase=0;
	if((mod==0) && (base==5)) nobase=1;

	if(nobase) {
		*s++ = '[';			/* mod==0 "[" */
	}else{
	        s = sputs(s,basereg)-1;         /* "[basereg" */
	}
	
        if(index!=4) {
                if(nobase==0)*s++ = '+';           /* "+" */
                s=sputs(s,eregname[index]);     /* "index_reg" */
                if(scale!=0) {        		/* "*scale" */
                        *s++ = '*';
                        *s++ = scalestr[scale];
                }
	}


        switch(mod) {
         case 0: /* [ no basereg , always add 32bit base address] */
		if(nobase) {
	                sprintf(s,"+%08x]",dd());
	        }else{
                        sprintf(s,"]");
                }
                break;
         case 1: /* disp8[EAX] */
                ea = dbr();
                if(ea>=0) {
                        sprintf(s,"+%02x]",ea); /* plus  */
                }else{
                        sprintf(s,"-%02x]",-ea);/* minus */
                }
                break;
         case 2: /* disp32[EAX] */
                sprintf(s,"+%08x]",dd());
                break;
         case 3: /* これはありえない */
                break;
         default:
                break;
        }

}


void	set_eop()
{
        op1p=op1buf+strlen(op1buf);
        op2p=op2buf+strlen(op2buf);
}

void	cutspc(char *s)
{
        char *p=NULL;
        while(*s) {
                if(*s!=' ') p=s;
                s++;
        }
        if(p!=NULL) p[1]=0;
}

char *put_segprefix(char *s)
{
        switch( seg_prefix ) {
         case 0x26:s=sputs(s,"es:");break;
         case 0x2e:s=sputs(s,"cs:");break;
         case 0x36:s=sputs(s,"ss:");break;
         case 0x3e:s=sputs(s,"ds:");break;
         case 0x64:s=sputs(s,"fs:");break;
         case 0x65:s=sputs(s,"gs:");break;
         default:break;
        }
        return s;
}
void	ovride(int opc,char *s)
{
        if(opc & 1) {
            if(opsize32)
                PR1(op1p,"dword ptr %s",s);
            else
                PR1(op1p,"word ptr %s",s);
        }else{
                PR1(op1p,"byte ptr %s",s);
        }
}

void	inst_0x0f()
{
        int     opc;
        int mod_rm;
        int mod;
        int reg;
        int rm;

        opc=(*op++) & 0xff;
        mod_rm=(*op)& 0xff;
        reg   =(mod_rm>>3)&7;
        rm    =(mod_rm   )&7;

        switch(opc) {
/*****************************************************************/
 case 0x00: 
  {static char *n[]={"sldt","str","lldt","ltr","verr","verw",UNDEF,UNDEF};
        op++;
        mne(n[reg]);
        modrm(op1buf,1,mod_rm,PTRTYPE);
  }
        break;
 case 0x01:
  {static char *n[]={"sgdt","sidt","lgdt","lidt","smsw",UNDEF,"lmsw","invlpg"};
        op++;
        mne(n[reg]);
        switch(reg) {
        case 0x00:	/* "sgdt" */
        case 0x01:	/* "lgdt" */
                modrm(op1buf,opc,mod_rm,FWORDTYPE);break;
        case 0x07:	/* "invlpg" */
                PR1(op1p,"%02x",rm);break;
        default:
                modrm(op1buf,opc,mod_rm,PTRTYPE);break;
        }
  }
        break;

/*****************************************************************/
 case 0x02: /* LAR     rw,rmw    */
        mne("lar");
        modrmov(op1buf,op2buf,1);
        break;
 case 0x03: /* LSL     rw,rmw    */
        mne("lsl");
        modrmov(op1buf,op2buf,1);
        break;
/*****************************************************************/
 case 0x05: /* LOADALL           */
		mne("loadall");rem("load registers from 0:800 (286 only)");
		break;
 case 0x06: /* CLTS              */
		mne("clts");
		break;
 case 0x07: /* LOADALL           */
		mne("loadall");rem("load registers from es:edi (386/486)");
		break;
 case 0x08: /* INVD              */
		mne("invd");
		break;
 case 0x09: /* WBINVD            */
		mne("wbinvd");
		break;
/*****************************************************************/
#if	!KNI
 case 0x10:
 case 0x11:
 case 0x12:
 case 0x13:
 		op++;
		mne("usermov");rem("hidden memory");
		break;
#endif
/*****************************************************************/
 case 0x20: /* MOV     rd,cr     */
 case 0x21: /* MOV     rd,dr     */
 case 0x24: /* MOV     rd,tr     */
        op++;
        mne("mov");
        op1(eregname[reg]);
		switch(opc) {
	 	 case 0x20:	PR2(op2p,"cr%d",rm);break;
	 	 case 0x21:	PR2(op2p,"dr%d",rm);break;
	 	 case 0x24:	PR2(op2p,"tr%d",rm);break;
		}
        break;
 case 0x22: /* MOV     cr,rd     */
 case 0x23: /* MOV     dr,rd     */
 case 0x26: /* MOV     tr,rd     */
        op++;
        mne("mov");
		switch(opc) {
	 	 case 0x22:	PR1(op1p,"cr%d",rm);break;
	 	 case 0x23:	PR1(op1p,"dr%d",rm);break;
	 	 case 0x26:	PR1(op1p,"tr%d",rm);break;
		}
        op2(eregname[reg]);
        break;
/*****************************************************************/
 case 0x30: /* WRMSR             */
        mne("wrmsr");
        break;
 case 0x31: /* RDTSC             */
        mne("rdtsc");rem("read time stamp counter into edx:eax");
        break;
 case 0x32: /* RDMSR             */
        mne("rdmsr");rem("read model specific register");
        break;
/*****************************************************************/
 case 0x80: /* JO      ll        */
 case 0x81: /* JNO     ll        */
 case 0x82: /* JB      ll        */
 case 0x83: /* JNB     ll        */
 case 0x84: /* JZ      ll        */
 case 0x85: /* JNZ     ll        */
 case 0x86: /* JNA     ll        */
 case 0x87: /* JA      ll        */
 case 0x88: /* JS      ll        */
 case 0x89: /* JNS     ll        */
 case 0x8A: /* JP      ll        */
 case 0x8B: /* JNP     ll        */
 case 0x8C: /* JL      ll        */
 case 0x8D: /* JNL     ll        */
 case 0x8E: /* JNG     ll        */
 case 0x8F: /* JG      ll        */
        mne( jcmd[ opc & 0x0f ] );
        rel_dw();
        break;
/*****************************************************************/
 case 0x90: /* SETO    rmb       */
 case 0x91: /* SETNO   rmb       */
 case 0x92: /* SETB    rmb       */
 case 0x93: /* SETNB   rmb       */
 case 0x94: /* SETZ    rmb       */
 case 0x95: /* SETNZ   rmb       */
 case 0x96: /* SETNA   rmb       */
 case 0x97: /* SETA    rmb       */
 case 0x98: /* SETS    rmb       */
 case 0x99: /* SETNS   rmb       */
 case 0x9A: /* SETP    rmb       */
 case 0x9B: /* SETNP   rmb       */
 case 0x9C: /* SETL    rmb       */
 case 0x9D: /* SETNL   rmb       */
 case 0x9E: /* SETNG   rmb       */
 case 0x9F: /* SETG    rmb       */
  {char setcc[8];
		op++;
        strcpy(setcc  ,"set");
        strcpy(setcc+3,jcmd[ opc & 0x0f ]+1);
        mne( setcc );
        modrm(op1buf,0,mod_rm,PTRTYPE);
  }
        break;
/*****************************************************************/
 case 0xA0: push("fs");break;
 case 0xA1: pop( "fs");break;
/*****************************************************************/
 case 0xA2: mne("cpuid");break;
 case 0xA3: /* BT      rmw,rw    */
		/*op++;*/
        mne("bt");
        modrmov(op2buf,op1buf,1);
/*        modrm(op1buf,1,mod_rm,PTRTYPE);
        op2(eregname[reg]);
  */
        break;

 case 0xA4: /* SHLD    rmw,rw,ib */
        mne("shld");
        modrmov(op2buf,op1buf,1);
        op2(",");
        immb();
        break;
 case 0xA5: /* SHLD    rmw,rw,CL */
        mne("shld");
        modrmov(op2buf,op1buf,1);
        op2(",cl");
        break;

 case 0xA6: /* CMPXCHG rmb,rb    */
        mne("cmpxchg");
		op++;
        modrm(op1buf,opc,mod_rm,PTRTYPE);
        op2(hregname[reg]);
        break;

 case 0xA7: /* CMPXCHG rmw,rw    */
        mne("cmpxchg");
		op++;
        modrm(op1buf,opc,mod_rm,PTRTYPE);
        op2(regname[reg]);
        break;

 case 0xA8: push("gs");break;/* PUSH    GS        */
 case 0xA9: pop( "gs");break;/* POP     GS        */
 case 0xAA: 
 		mne("rsm");rem("resume from SMI");
 		break;/* RSM               */
 case 0xAB: /* BTS     rmw,rw    */
        mne("bts");
		op++;
        modrm(op1buf,opc,mod_rm,PTRTYPE);
        op2(regname[reg]);
        break;

 case 0xAC: /* SHRD    rmw,rw,ib */
        mne("shrd");
        modrmov(op2buf,op1buf,1);
        op2(",");
        immb();
        break;
 case 0xAD: /* SHRD    rmw,rw,CL */
        mne("shrd");
        modrmov(op2buf,op1buf,1);
        op2(",cl");
        break;
 case 0xB0: /* CMPXCHG rmb,rb    */
        mne("cmpxchg");
		op++;
        modrm(op1buf,opc,mod_rm,PTRTYPE);
        op2(hregname[reg]);
        break;
 case 0xB1: /* CMPXCHG rmw,rw    */
        mne("cmpxchg");
		op++;
        modrm(op1buf,opc,mod_rm,PTRTYPE);
        op2(regname[reg]);
        break;
#if	!KNI
 case 0xAE: /* IMUL    rb,rmb    */
#endif
 case 0xAF: /* IMUL    rw,rmw    */
        mne("imul");
        modrmov(op1buf,op2buf,opc);
        break;
	
 case 0xB3: /* BTR     rmw,rw    */
        mne("btr");
        modrmov(op2buf,op1buf,1);
        break;

 case 0xB2: /* LSS     rw,md     */
 case 0xB4: /* LFS     rw,md     */
 case 0xB5: /* LGS     rw,md     */
        mnes(opc-0xb2,"lss","???","lfs","lgs");
        modrmov(op1buf,op2buf,1);
        break;

#if	0
 case 0xB6: /* MOVZX   rw,rmb    */
        op++;
        mne("movzx");
        strcpy(op1p,eregname[reg]);		/* ereg ! */
        modrm(op2p,opc,mod_rm,PTRTYPE); /*第２オペランドはr/m */
        break;
        
 case 0xB7: /* MOVZX   rd,rmw    */
        op++;
        mne("movzx");
        strcpy(op1p, regname[reg]);
        modrm(op2p,opc,mod_rm,PTRTYPE); /*第２オペランドはr/m */
        break;
#else
 case 0xB6: /* MOVZX   rw,rmb    */
 case 0xB7: /* MOVZX   rd,rmw    */
        op++;
        mne("movzx");
        if(opc==0xb6)   strcpy(op1p, regname[reg]);
        else            strcpy(op1p,eregname[reg]);
        modrm(op2p,opc,mod_rm,PTRTYPE); /*第２オペランドはr/m */
        break;
#endif

 case 0xBA: /* BT      rmw,ib    */
            /* BTS     rmw,ib    */
            /* BTR     rmw,ib    */
            /* BTC     rmw,ib    */
		switch(reg) {
			case 4:
				mne("bt") ;break;
			case 5:
				mne("bts");break;
			case 6:
				mne("btr");break;
			case 7:
				mne("btc");break;
			default:
				undis();return;
		}
        mod_rm=(*op++) & 0xff;
        modrm(op1buf,1,mod_rm,PTRTYPE);
        immb();
		break;
 case 0xBB: /* BTC     rmw,rw    */
        mne("btc");
        modrmov(op2buf,op1buf,1);
        break;

 case 0xBC: /* BSF     rw,rmw    */
        mne("bsf");
        modrmov(op1buf,op2buf,1);
        break;

 case 0xBD: /* BSR     rw,rmw    */
        mne("bsr");
        modrmov(op1buf,op2buf,1);
        break;

 case 0xBE: /* MOVSX   rw,rmb    */
 case 0xBF: /* MOVSX   rd,rmw    */
        op++;
        mne("movsx");
        if(opc==0xbe)   strcpy(op1p, regname[reg]);
        else            strcpy(op1p,eregname[reg]);
        modrm(op2p,opc,mod_rm,PTRTYPE); /*第２オペランドはr/m */
        break;

 case 0xC0: /* XADD    rmb,rb    */
 case 0xC1: /* XADD    rmw,rw    */
		op++;
        mne("xadd");
        modrm(op1buf,opc,mod_rm,PTRTYPE);
        op2(hregname[reg]);
        break;
 case 0xC7: /* CMPXCHG8B rmq,rd  */
        mne("cmpxchg8b");
		op++;
        modrm(op1buf,opc,mod_rm,PTRTYPE);
        op2(regname[reg]);
        break;
/*****************************************************************/
 case 0xC8: /* BSWAP   eax       */
 case 0xC9: /* BSWAP   ecx       */
 case 0xCA: /* BSWAP   edx       */
 case 0xCB: /* BSWAP   ebx       */
 case 0xCC: /* BSWAP   esp       */
 case 0xCD: /* BSWAP   ebp       */
 case 0xCE: /* BSWAP   esi       */
 case 0xCF: /* BSWAP   edi       */
        mne("bswap");
        op1(eregname[opc&7]);
        break;
/*****************************************************************/
#if	MMX
	/* P55Cで追加される予定のMMX関連の命令 */
 case 0x77:
        mne("emms");
        break;
 case 0x6b:
 case 0x63:
 case 0x67:
 case 0xfc:
 case 0xfe:
 case 0xec:
 case 0xed:
 case 0xdc:
 case 0xdd:
 case 0xfd:
 case 0xdb:
 case 0xdf:
 case 0x74:
 case 0x76:
 case 0x75:
 case 0x64:
 case 0x66:
 case 0x65:
 case 0xf5:
 case 0xe5:
 case 0xd5:
 case 0xeb:
 case 0xf2:
 case 0xf3:
 case 0xf1:
 case 0xe2:
 case 0xe1:
 case 0xd2:
 case 0xd3:
 case 0xd1:
 case 0xf8:
 case 0xfa:
 case 0xe8:
 case 0xe9:
 case 0xd8:
 case 0xd9:
 case 0xf9:
 case 0x68:
 case 0x6a:
 case 0x69:
 case 0x60:
 case 0x62:
 case 0x61:
 case 0xef:
		inst_mmx(opc);
		break;

 case 0x72:
 case 0x73:
 case 0x71:
		inst_mmx_shift(opc);
		break;
 case 0x6e:
 case 0x6f:
 case 0x7e:
 case 0x7f:
		inst_mmx_mov(opc);
		break;


#endif	/*MMX*/


#if	AMD3DNOW
	/* AMDの3DNow!(TM)命令群 */
 case 0x0d:
		{
			int prefetch_type;
			prefetch_type=(mod_rm>>3)&7;
	        switch(prefetch_type) {
	         case 1:
	        	mne("prefetchw");
	        	break;
	         case 0:
			 default:
	        	mne("prefetch");
	        	break;
	        }
			op++;
	        modrm(op1buf,opc,mod_rm,PTRTYPE);
    	    rem("3DNow!");
    	}
        break;
 case 0x0e:
        mne("femms");
    	    rem("3DNow!");
        break;
 case 0x0f:
		{
			//op++;		//２個目の 0fh を読み飛ばす.
    	    AMD3dnow();	
    	    rem("3DNow!");
		}
        break;

#endif	/*AMD3DNOW*/
/*****************************************************************/
#if	KNI
 case 0x10:
 case 0x11:
 case 0x12:
 case 0x13:
 case 0x14:
 case 0x15:
 case 0x16:
 case 0x17:
 case 0x18:
 case 0x28:
 case 0x29:
 case 0x2a:
 case 0x2b:
 case 0x2c:
 case 0x2d:
 case 0x2e:
 case 0x2f:
 case 0x50:
 case 0x51:
 case 0x52:
 case 0x53:
 case 0x54:
 case 0x55:
 case 0x56:
 case 0x57:
 case 0x58:
 case 0x59:
 case 0x5c:
 case 0x5d:
 case 0x5e:
 case 0x5f:
 case 0x70:
 case 0xae:
 case 0xc2:
 case 0xc4:
 case 0xc5:
 case 0xc6:
 case 0xd7:
 case 0xda:
 case 0xde:
 case 0xe0:
 case 0xe3:
 case 0xe4:
 case 0xe7:
 case 0xea:
 case 0xee:
 case 0xf6:
 case 0xf7:
   	    inst_kni(opc,0);
		break;
#endif	/*KNI*/
/*****************************************************************/
 default:
        undef();break;
        }
}


/*****************************************************************/
static	char *fpu_d8[]=
	{"fadd","fmul","fcom","fcomp","fsub","fsubr","fdiv","fdivr"};

/*****************************************************************/
static	char *fpu_d9[]=
	{"fld",UNDEF,"fst","fstp","fldenv","fldcw","fnstenv","fnstcw"};

static	char *fchs[]=
	{"fchs","fabs",UNDEF,UNDEF,"ftst","fxam",UNDEF,UNDEF,
       "fld1","fldl2t","fldl2e","fldpi","fldlg2","fldln2","fldz",UNDEF,
       "f2xm1","fyl2x","fptan","fpatan","fxtract","fprem1","fdecstp","fincstp",
       "fprem","fyl2xp1","fsqrt","fsincos","frndint","fscale","fsin","fcos"};

/*****************************************************************/
static	char *fpu_da[]=
	{"fiadd","fimul","ficom","ficomp","fisub","fisubr","fidiv","fidivr"};
static	char *fpu_da_c0[]=
	{"faddp","fmulp","fcomp" ,UNDEF,"fsubrp","fsubp","fdivrp","fdivp"};
static	char *fpu_da_c1[]=
	{"fadd" ,"fmul" ,UNDEF, "fcompp","fsubr" ,"fsubp","fdivrp","fdivp"};

/*****************************************************************/
static	char *fpu_db[]=
	{"fild",UNDEF,"fist","fistp",UNDEF,"fld",UNDEF,"fstp"};
static	char *fpu_db_c0[]=
	{"fneni","fndisi","fnclex","fninit","fsetpm"};

/*****************************************************************/
static	char *fpu_dc_c0[]=
	{"fadd","fmul","fcom","fcomp","fsubr","fsub","fdivr","fdiv"};
/*****************************************************************/
static	char *fpu_dd[]=
	{"fld",UNDEF,"fst","fst","frstor",UNDEF,"fnsave","fnstsw"};
static	char *fpu_dd_c0[]=
	{"ffree","fxch","fst","fstp","fucom","fucomp",UNDEF,UNDEF};

/*****************************************************************/
static	char *fpu_df[]=
	{"fild",UNDEF,"fist","fistp","fbld","fild","fbstp","fistp"};
static	char *fpu_df_c0[]=
	{"ffree","fxch","fst","fstp","fnstsw",UNDEF,UNDEF,UNDEF};

/*****************************************************************/

void	inst_FPU(int opc)
{
        int mod_rm;
        int mod;
        int reg;
        int rm;

        mod_rm=(*op++)& 0xff;
        mod   =(mod_rm>>6)&3;
        reg   =(mod_rm>>3)&7;
        rm    =(mod_rm   )&7;

	switch(opc) {
 case 0xd8: 
	if(mod!=3) {
		mne(fpu_d8[reg]);modrm(op1p,1,mod_rm,DWORDTYPE);
	}else{
		mne(fpu_d8[reg]);op1("st");
		PR2(op2p,"st(%d)",rm);
	}
	break;
 case 0xd9: 
	if(mod!=3) {
		mne(fpu_d9[reg]);
		switch(reg) {
		case 0x00:
		case 0x01:
		case 0x02:
		case 0x03:
			modrm(op1p,1,mod_rm,DWORDTYPE);break;
		case 0x04:
		case 0x06:
			modrm(op1p,1,mod_rm, PTRTYPE);break;
		case 0x05:
		case 0x07:
			modrm(op1p,1,mod_rm, PTRTYPE);break;
		}
	}else{
	 switch(mod_rm) {
	  case 0xc0:
	  case 0xc1:
	  case 0xc2:
	  case 0xc3:
	  case 0xc4:
	  case 0xc5:
	  case 0xc6:
	  case 0xc7:
		mne("fld");
		PR1(op1p,"st(%d)",rm);break;
	  case 0xc8:
	  case 0xca:
	  case 0xcb:
	  case 0xcc:
	  case 0xcd:
	  case 0xce:
	  case 0xcf:
		mne("fxch");
		PR1(op1p,"st(%d)",rm);break;
	  case 0xc9: mne("fxch");break;
	  case 0xd0: mne("fnop");break;
	  default:
	  	if(mod_rm <0xe0) undef();
	  	else{
	  		mne(fchs[mod_rm - 0xe0]);
	  	}
	  	break;
	 }
	}
	break;
	
 case 0xda: 
	if(mod!=3) {
		mne(fpu_da[reg]);modrm(op1p,1,mod_rm,PTRTYPE);
	}else{
		if(mod_rm==0xe9)
			mne("fucompp");
		else
			undef();
	}
	break;
 case 0xdb: 
	if(mod!=3) {
		mne(fpu_db[reg]);
		if(reg<4) {
			modrm(op1p,1,mod_rm,DWORDTYPE);
		}else{
			modrm(op1p,1,mod_rm,TBYTETYPE);
		}
	}else{
	  	if(mod_rm <0xe0) undef(); else
	  	if(mod_rm <0xe5) {
	  		mne(fpu_db_c0[mod_rm - 0xe0]);
	  	}else
	  			undef();
	  	break;
	}
	break;
	
 case 0xdc: 
	if(mod!=3) {
		mne(fpu_d8[reg]);modrm(op1p,1,mod_rm,DWORDTYPE);
	}else{
		mne(fpu_dc_c0[reg]);
		PR1(op1p,"st(%d)",rm);
		if((reg!=2)&&(reg!=3)) {
			op2("st");
		}
	}
	break;
 case 0xdd: 
	if(mod!=3) {
		mne(fpu_dd[reg]);
		switch(reg) {
		case 0x00:
		case 0x01:
		case 0x02:
		case 0x03:
			modrm(op1p,1,mod_rm,QWORDTYPE);break;
		case 0x04:
		case 0x06:
			modrm(op1p,1,mod_rm, PTRTYPE);break;
		case 0x05:
		case 0x07:
			modrm(op1p,1,mod_rm, PTRTYPE);break;
		}
	}else{
		mne(fpu_dd_c0[reg]);
		if(mod_rm!=0xe1) {
			PR1(op1p,"st(%d)",rm);
		}
	}
	break;
 case 0xde: 
	if(mod!=3) {
		mne(fpu_da[reg]);modrm(op1p,1,mod_rm,PTRTYPE);
	}else{
		if(rm!=1) {
			mne(fpu_da_c0[reg]);
			PR1(op1p,"st(%d)",rm);
			op2("st");
		}else{
			mne(fpu_da_c1[reg]);
		}
	}
	break;
 case 0xdf: 
	if(mod!=3) {
		mne(fpu_df[reg]);modrm(op1p,1,mod_rm,PTRTYPE);
	}else{
		if(mod_rm!=0xe0) {
			mne(fpu_df_c0[reg]);
			PR1(op1p,"st(%d)",rm);
		}else{
			mne(fpu_df_c0[reg]);
			op1("ax");
		}
	}
	break;
        }
}



