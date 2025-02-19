/*
0f 10		movups		
0f 11		movups		
0f 12		movlps		
0f 13		movlps		
0f 14		unpcklps	
0f 15		unpckhps	
0f 16		movhps		
0f 17		movhps		
0f 18		prefetchnta	
0f 18		prefetcht0	
0f 18		prefetcht1	
0f 18		prefetcht2	
0f 28		movaps		
0f 29		movaps		
0f 2a		cvtpi2ps	
0f 2b		movntps		
0f 2c		cvttps2pi	
0f 2d		cvtps2pi	
0f 2e		ucomiss		
0f 2f		comiss		
0f 50		movmskps	
0f 51		sqrtps		
0f 52		rsqrtps		
0f 53		rcpps		
0f 54		andps		
0f 55		andnps		
0f 56		orps		
0f 57		xorps		
0f 58		addps		
0f 59		mulps		
0f 5c		subps		
0f 5d		minps		
0f 5e		divps		
0f 5f		maxps		
0f 70		pshufw		
0f ae		fxsave		
0f ae		fxrstor		
0f ae		ldmxcsr		
0f ae		stmxcsr		
0f ae		sfence		
0f c2		cmpltps		
0f c2		cmpordps	
0f c2		cmpeqps		
0f c2		cmpleps		
0f c2		cmpunordps	
0f c2		cmpneqps	
0f c2		cmpnleps	
0f c4		pinsrw		
0f c5		pextrw		
0f c6		shufps		
0f d7		pmovmskb	
0f da		pminub		
0f de		pmaxub		
0f e0		pavgb		
0f e3		pavgw		
0f e4		pmulhuw		
0f e7		movntq		
0f ea		pminsw		
0f ee		pmaxsw		
0f f6		psadbw		
0f f7		maskmovq	
*/

#include <stdio.h>
#include <string.h>

void	modrm(char *s,int opc,int mod_rm,int ptrtype);
void	mne(char *s);
void	immb(void);
void	modrmov(char *s1,char *s2,int opc);
void	modrmovmmm(char *s1,char *s2,int mmm1,int mmm2);
void	put_regname(char *s,int reg,int w);
void	set_eop(void);
extern char    *op;
extern char    op1buf[];
extern char    op2buf[];
extern	char *op2p;     /* オペランド２ */
extern int     mmx;

//	extended attributes.
#define	________	0
#define	PREFETCH	1
#define	CMPXX_PS	2
#define	FXSAVE		3
#define	MEM_XMM		4
#define	MM_MEM		5
#define	MEM_MM		6
#define	REG_XMM		7	// r32,xmm
#define	XMM_MEM		8	// xmm,mm/mem
#define	REG_MEM		9	// r32,xmm/mem | mm,xmm/mem
#define	REG_MM		10	// r32,mm
#define	POSTBYTE	11	// xmm,xmm/mem,i8
#define	POSTBYT2	12	// mm,r32/mem,i8
#define	POSTBYT3	13	// r32,mm,i8
#define	POSTBYT4	14	// mm,mm/mem,i8
//
#define NONTYPE   0/* byte ptr */


// regname 0:REG 1:MMX 2:XMM
#define	R32	0
#define	MM	1
#define	XMM	2

typedef	struct	{
	unsigned char code;
	unsigned char attr;
	char *inst;
	char *inst_F3;
/*	char *comment;	*/
} LOOKUP;

LOOKUP knicode[]={
//CODE ATTR     INST         F3+INST        COMMENT .
{0x10,________,"movups"		,"movss"	/*,"Move unaligned parallel scalar"*/},
{0x11,MEM_XMM ,"movups"		,"movss"	/*,"Move unaligned parallel scalar"*/},
{0x12,________,"movlps"		,"movhlps"	/*,"Move low (qword) parallel scalar"*/},
{0x13,MEM_XMM ,"movlps"		,"movlps"	/*,"Move low (qword) parallel scalar"*/},
{0x14,________,"unpcklps"	,"unpcklps"	/*,"Unpack low (qword) parallel scalar"*/},
{0x15,________,"unpckhps"	,"unpckhps"	/*,"Unpack high (qword) parallel scalar"*/},
{0x16,________,"movhps"		,"movlhps"	/*,"Move high (qword) parallel scalar"*/},
{0x17,MEM_XMM ,"movhps"		,"movhps"	/*,"Move high (qword) parallel scalar"*/},
{0x18,PREFETCH,"prefetchnta","prefetchnta"/*,"Prefetch non-tagged (uncached) aligned"*/},
{0x18,PREFETCH,"prefetcht0"	,"prefetcht0"/*,"Prefetch tip 0 (tag line 0?)"*/},
{0x18,PREFETCH,"prefetcht1"	,"prefetcht1"/*,"Prefetch tip 1"*/},
{0x18,PREFETCH,"prefetcht2"	,"prefetcht2"/*,"Prefetch tip 2"*/},
{0x18,PREFETCH,"prefetch??"	,"prefetch??"/*,""*/},
{0x28,________,"movaps"		,"movaps"	/*,"Move aligned parallel scalar"*/},
{0x29,MEM_XMM ,"movaps"		,"movaps"	/*,"Move aligned parallel scalar"*/},
{0x2a,XMM_MEM ,"cvtpi2ps"	,"cvtsi2ss"	/*,"Convert parallel int to parallel scalar"*/},
{0x2b,MEM_XMM ,"movntps"	,"movntps"	/*,"Move non-tagged (uncached) parallel scalar"*/},
{0x2c,REG_MEM ,"cvttps2pi"	,"cvttss2si"/*,"Convert ? parallel scalar to parallel int"*/},
{0x2d,REG_MEM ,"cvtps2pi"	,"cvtss2si"	/*,"Convert parallel scalar to parallel int"*/},
{0x2e,________,"ucomiss"	,"ucomiss"	/*,""*/},
{0x2f,________,"comiss"		,"comiss"	/*,""*/},
{0x50,REG_XMM ,"movmskps"	,"movmskps"	/*,"Move mask parallel scalar"*/},
{0x51,________,"sqrtps"		,"sqrtss"	/*,"Square root parallel scalar"*/},
{0x52,________,"rsqrtps"	,"rsqrtss"	/*,"Reciprocal or square root parallel scalar"*/},
{0x53,________,"rcpps"		,"rcpss"	/*,"Reciprocal parallel scalar (very coarse)"*/},
{0x54,________,"andps"		,"andps"	/*,"And parallel scalar"*/},
{0x55,________,"andnps"		,"andnps"	/*,"And-Not parallel scalar"*/},
{0x56,________,"orps"		,"orps"		/*,"Or parallel scalar"*/},
{0x57,________,"xorps"		,"xorps"	/*,"Exclusive or parallel scalar"*/},
{0x58,________,"addps"		,"addss"	/*,"Add parallel scalar"*/},
{0x59,________,"mulps"		,"mulss"	/*,"Multiply parallel scalar"*/},
{0x5c,________,"subps"		,"subss"	/*,"Subtract parallel scalar"*/},
{0x5d,________,"minps"		,"minss"	/*,"Minimum parallel scalar"*/},
{0x5e,________,"divps"		,"divss"	/*,"Divide parallel scalar"*/},
{0x5f,________,"maxps"		,"maxss"	/*,"Maximum parallel scalar"*/},
{0x70,POSTBYT4,"pshufw"		,"pshufw"	/*,"Shuffle word parallel int"*/},
{0xae,FXSAVE  ,"fxsave"		,"fxsave"	/*,"Fast Extended Save"*/},
{0xae,FXSAVE  ,"fxrstor"	,"fxrstor"	/*,"Fast Extended Restore (FP/MMX/KNI context)"*/},
{0xae,FXSAVE  ,"ldmxcsr"	,"ldmxcsr"	/*,"LoaD Multimedia eXtended Control/Status Register"*/},
{0xae,FXSAVE  ,"stmxcsr"	,"stmxcsr"	/*,"STore Multimedia eXtended Control/Status Register"*/},
{0xae,FXSAVE  ,"fxsave??"	,"fxsave??"	/*,""*/},
{0xae,FXSAVE  ,"sfence"		,"sfence"	/*,""*/},
{0xc2,CMPXX_PS,"cmpeqps"	,"cmpeqss"	/*,"Compare Equal parallel scalar"*/},
{0xc2,CMPXX_PS,"cmpltps"	,"cmpltss"	/*,"Compare Less Than"*/},
{0xc2,CMPXX_PS,"cmpleps"	,"cmpless"	/*,"Compare Less than or Equal"*/},
{0xc2,CMPXX_PS,"cmpunordps"	,"cmpunordss"/*,"Compare Unordered"*/},
{0xc2,CMPXX_PS,"cmpneqps"	,"cmpneqss"	/*,"Compare Not Equal"*/},
{0xc2,CMPXX_PS,"cmpnltps"	,"cmpnltss"	/*,"Compare Not Less Than"*/},
{0xc2,CMPXX_PS,"cmpnleps"	,"cmpnless"	/*,"Compare Not Less than or Equal"*/},
{0xc2,CMPXX_PS,"cmpordps"	,"cmpordss"	/*,"Compare Ordered"*/},
{0xc2,CMPXX_PS,"cmp??ps"	,"cmp??ss"	/*,"Compare Ordered"*/},
{0xc4,POSTBYT2,"pinsrw"		,"pinsrw"	/*,"Insert word parallel int"*/},
{0xc5,POSTBYT3,"pextrw"		,"pextrw"	/*,"Extract word parallel int"*/},
{0xc6,POSTBYTE,"shufps"		,"shufps"	/*,"Shuffle parallel scalar"*/},
{0xd7,REG_MM  ,"pmovmskb"	,"pmovmskb"	/*,"Move mask parallel int"*/},
{0xda,MM_MEM  ,"pminub"		,"pminub"	/*,"Minimum unsigned-byte parallel int"*/},
{0xde,MM_MEM  ,"pmaxub"		,"pmaxub"	/*,"Maximum unsigned-byte parallel int"*/},
{0xe0,MM_MEM  ,"pavgb"		,"pavgb"	/*,"Average byte parallel int (non sign specific)"*/},
{0xe3,MM_MEM  ,"pavgw"		,"pavgw"	/*,"Average word parallel int"*/},
{0xe4,MM_MEM  ,"pmulhuw"	,"pmulhuw"	/*,"Multiply unsigned word parallel int"*/},
{0xe7,MEM_MM  ,"movntq"		,"movntq"	/*,"Move non-tagged (uncached) parallel int"*/},
{0xea,MM_MEM  ,"pminsw"		,"pminsw"	/*,"Minimum signed-word parallel int"*/},
{0xee,MM_MEM  ,"pmaxsw"		,"pmaxsw"	/*,"Maximum signed-word parallel int"*/},
{0xf6,MM_MEM  ,"psadbw"		,"psadbw"	/*,""*/},
{0xf7,MM_MEM  ,"maskmovq"	,"maskmovq"	/*,"Masked Move?"*/},
{0   ,________,"???"		,"???"		/*,""*/}
};



void inst_kni(int opc,int rep)
{
	LOOKUP *t=knicode;
	int cmp_xx;
	int mod_rm;
	int reg;

	while( t->code ) {
		if(t->code == opc) break;
		t++;
	}

	switch(t->attr) {
	  case CMPXX_PS:	// === cmpeqps,cmpltps,cmpleps,cmpunordps,cmpneqps,cmpnltps,cmpnleps,cmpordps
		modrmovmmm(op1buf,op2buf,XMM,XMM);
		cmp_xx = (*op++) & 0xff;

		if(cmp_xx >=8) cmp_xx = 8;	// check compare-ins range
		t+=cmp_xx;	// look ahead!
		
		if(rep == 0xf3) {
			mne(t->inst_F3);
		}else{
			mne(t->inst);
		}
		return;

	  case PREFETCH:	// === prefetchnta,prefetcht0,prefetcht1,prefetcht2
	  case FXSAVE:		// === fxsave,fxrstor,ldmxcsr,stmxcsr,sfence
		mod_rm=(*op++) & 0xff;
        reg   =( mod_rm >>3) & 7;

		if((reg==7)&&(t->attr==FXSAVE)) {	// === sfence
			t += 5;		// look ahead
			mne( t->inst );
			return;
		}

		if(reg>=4) reg=4;			// REGister field:range error
		t += reg;	// look ahead!
		
		mne( t->inst );
        modrm(op1buf,1,mod_rm,NONTYPE);   /*第２オペランドはr/m */
        set_eop();
		return;

	 case MEM_XMM:		// movups,movhps   mem,mm
		if(rep == 0xf3) {
			mne(t->inst_F3);
		}else{
			mne(t->inst);
		}
		modrmovmmm(op2buf,op1buf,XMM,XMM);	// mem,xmm
		return;

	 case MM_MEM:		// pminub,pmaxub,pminsw,pmaxsw mm,mem
		mne(t->inst);
		modrmovmmm(op1buf,op2buf,MM,MM);	// mm,mem
		return;

	 case MEM_MM:		// pminub,pmaxub,pminsw,pmaxsw mm,mem
		mne(t->inst);
		modrmovmmm(op2buf,op1buf,MM,MM);	// mem,mm
		return;

	 case XMM_MEM:		// cvtpi2ps,cvtsi2ss    xmm,mm/mem
		if(rep == 0xf3) {
			mne(t->inst_F3);
			modrmovmmm(op1buf,op2buf,XMM,R32);	// xmm,r32/mem
		}else{
			mne(t->inst);
			modrmovmmm(op1buf,op2buf,XMM,MM);	// xmm,mm/mem
		}
		return;

	 case REG_MEM:		// cvttps2pi,cvttss2si  r32,xmm/mem | mm,xmm/mem
	 					// cvtps2pi,cvtss2si    r32,xmm/mem | mm,xmm/mem
		if(rep == 0xf3) {
			mne(t->inst_F3);
			modrmovmmm(op1buf,op2buf,R32,XMM);	// r32,mm/mem
		}else{
			mne(t->inst);
			modrmovmmm(op1buf,op2buf, MM,XMM);	// mm,xmm/mem
		}
		return;

	 case REG_MM:		// pmovmskb r32,mm
		mne(t->inst);
		modrmovmmm(op1buf,op2buf, R32,MM);
		return;

	 case REG_XMM:		// movmskps
		mne(t->inst);
		modrmovmmm(op1buf,op2buf, R32,XMM);
		return;

	  case POSTBYTE:			// ===
		mne(t->inst);
		modrmovmmm(op1buf,op2buf,XMM,XMM);
		reg=(*op++) & 0xff;
		op2p += sprintf(op2p,",%d",reg);
		return;
		
	  case POSTBYT2:			// ===
		mne(t->inst);
		modrmovmmm(op1buf,op2buf,MM,R32);
		reg=(*op++) & 0xff;
		op2p += sprintf(op2p,",%d",reg);
		return;
		
	  case POSTBYT3:			// ===
		mne(t->inst);
		modrmovmmm(op1buf,op2buf,R32,MM);
		reg=(*op++) & 0xff;
		op2p += sprintf(op2p,",%d",reg);
		return;
		
	  case POSTBYT4:			// ===
		mne(t->inst);
		modrmovmmm(op1buf,op2buf,MM,MM);
		reg=(*op++) & 0xff;
		op2p += sprintf(op2p,",%d",reg);
		return;
		
	  default:			// === typical KNi pattern
		if(rep == 0xf3) {
			mne(t->inst_F3);
		}else{
			mne(t->inst);
		}
		//	rem("KNi");	
		//	rem( t->comment );

		modrmovmmm(op1buf,op2buf,XMM,XMM);
		return;
	}
}

