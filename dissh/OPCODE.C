#include "sh7000.h"
char *opstring[] = {
	"add"		,
	"addc"		,
	"addv"		,
	"and"		,
	"and.b"		,
	"bf"		,
	"bf.s"		,
	"bra"		,
	"braf"		,
	"bsr"		,
	"bsrf"		,
	"bt"		,
	"bt.s"		,
	"clrmac"	,
	"clrt"		,
	"cmp/eq"	,
	"cmp/ge"	,
	"cmp/gt"	,
	"cmp/hi"	,
	"cmp/hs"	,
	"cmp/pl"	,
	"cmp/pz"	,
	"cmp/str"	,
	"div0s"		,
	"div0u"		,
	"div1"		,
	"dt"		,
	"dmulu.l"	,
	"dmuls.l"	,
	"exts.b"	,
	"exts.w"	,
	"extu.b"	,
	"extu.w"	,
	"jmp"		,
	"jsr"		,
	"ldc"		,
	"ldc.l"		,
	"lds"		,
	"lds.l"		,
	"mac.w"		,
	"mac.l"		,
	"mov"		,
	"mov.b"		,
	"mov.l"		,
	"mov.w"		,
	"mova"		,
	"movt"		,
	"muls"		,
	"mulu"		,
	"mul.l"		,
	"neg"		,
	"negc"		,
	"nop"		,
	"not"		,
	"or"		,
	"or.b"		,
	"rotcl"		,
	"rotcr"		,
	"rotl"		,
	"rotr"		,
	"rte"		,
	"rts"		,
	"sett"		,
	"shal"		,
	"shar"		,
	"shll"		,
	"shll2"		,
	"shll8"		,
	"shll16"	,
	"shlr"		,
	"shlr2"		,
	"shlr8"		,
	"shlr16"	,
	"sleep"		,
	"stc"		,
	"stc.l"		,
	"sts"		,
	"sts.l"		,
	"sub"		,
	"subc"		,
	"subv"		,
	"swap.b"	,
	"swap.w"	,
	"tas.b"		,
	"trapa"		,
	"tst"		,
	"tst.b"		,
	"xor"		,
	"xor.b"		,
	"xtrct"		,
	".word"	,	/*	".data.w"	*/
	".word"		,	/*	".data.w"	*/
	".long"		,	/*	".data.l"	*/
	""			,	/*	".data.l"	*/
	""
};

char opdelay[] = {
		' ',/*	"add"		*/
		' ',/*	"addc"		*/
		' ',/*	"addv"		*/
		' ',/*	"and"		*/
		' ',/*	"and.b"		*/
		' ',/*	"bf"		*/
		's',/*	"bf.s"		*/
		's',/*	"bra"		*/
		's',/*	"braf"		*/
		's',/*	"bsr"		*/
		's',/*	"bsrf"		*/
		' ',/*	"bt"		*/
		's',/*	"bt.s"		*/
		' ',/*	"clrmac"	*/
		' ',/*	"clrt"		*/
		' ',/*	"cmp/eq"	*/
		' ',/*	"cmp/ge"	*/
		' ',/*	"cmp/gt"	*/
		' ',/*	"cmp/hi"	*/
		' ',/*	"cmp/hs"	*/
		' ',/*	"cmp/pl"	*/
		' ',/*	"cmp/pz"	*/
		' ',/*	"cmp/str"	*/
		' ',/*	"div0s"		*/
		' ',/*	"div0u"		*/
		' ',/*	"div1"		*/
		' ',/*	"dt"		*/
		' ',/*	"dmulu.l"	*/
		' ',/*	"dmuls.l"	*/
		' ',/*	"exts.b"	*/
		' ',/*	"exts.w"	*/
		' ',/*	"extu.b"	*/
		' ',/*	"extu.w"	*/
		's',/*	"jmp"		*/
		's',/*	"jsr"		*/
		' ',/*	"ldc"		*/
		' ',/*	"ldc.l"		*/
		' ',/*	"lds"		*/
		' ',/*	"lds.l"		*/
		' ',/*	"mac.w"		*/
		' ',/*	"mac.l"		*/
		' ',/*	"mov"		*/
		' ',/*	"mov.b"		*/
		' ',/*	"mov.l"		*/
		' ',/*	"mov.w"		*/
		' ',/*	"mova"		*/
		' ',/*	"movt"		*/
		' ',/*	"muls"		*/
		' ',/*	"mulu"		*/
		' ',/*	"mul.l"		*/
		' ',/*	"neg"		*/
		' ',/*	"negc"		*/
		' ',/*	"nop"		*/
		' ',/*	"not"		*/
		' ',/*	"or"		*/
		' ',/*	"or.b"		*/
		' ',/*	"rotcl"		*/
		' ',/*	"rotcr"		*/
		' ',/*	"rotl"		*/
		' ',/*	"rotr"		*/
		's',/*	"rte"		*/
		's',/*	"rts"		*/
		' ',/*	"sett"		*/
		' ',/*	"shal"		*/
		' ',/*	"shar"		*/
		' ',/*	"shll"		*/
		' ',/*	"shll2"		*/
		' ',/*	"shll8"		*/
		' ',/*	"shll16"	*/
		' ',/*	"shlr"		*/
		' ',/*	"shlr2"		*/
		' ',/*	"shlr8"		*/
		' ',/*	"shlr16"	*/
		' ',/*	"sleep"		*/
		' ',/*	"stc"		*/
		' ',/*	"stc.l"		*/
		' ',/*	"sts"		*/
		' ',/*	"sts.l"		*/
		' ',/*	"sub"		*/
		' ',/*	"subc"		*/
		' ',/*	"subv"		*/
		' ',/*	"swap.b"	*/
		' ',/*	"swap.w"	*/
		' ',/*	"tas.b"		*/
		' ',/*	"trapa"		*/
		' ',/*	"tst"		*/
		' ',/*	"tst.b"		*/
		' ',/*	"xor"		*/
		' ',/*	"xor.b"		*/
		' ',/*	"xtrct"		*/
		' ',/*	".word"		*/
		' ',/*	".word"		*/
		' ',/*	".word"		*/
		' ',/*	".word"		*/
		' ' /*	".word"		*/
};

char  *operands[]={
/*  opc(16) = ra(4) | rn(4) | rm(4) | rx(4)                            */
/*            $a      $n      $m      $x                               */
/*                            $d(8)      disp                          */
/*                            $i(8)      immediate (sign-extend 32bit) */
/*                            $g(8)      gbr offset(8bit)              */
/*                    $b(12)             branch offset                 */
/*------“WŠJ•¶Žš—ñ ----------------‚…‚Ž‚•‚–¼----------“WŠJŒ`----------*/
        "",                     /* No_op               no op           */
        "#$i",                  /* Imm                 #imm            */
        "#$i,@(r0,gbr)",        /* Imm_at_R0_GBR       #imm,@(R0,GBR)  */
        "#$i,r0",               /* Imm_R0              #imm,R0         */
        "#$i,$n",               /* Imm_Rn              #imm,Rn         */
        "@(r0,$m),$n",          /* at_R0_Rm_Rn         @(R0,Rm),Rn     */
        "@($g,gbr),r0",         /* at_disp_GBR_R0      @(disp,GBR),R0  */
        "$4,r0",                /* PCrel4_R0           @(disp,PC),R0   */
        "$2,r0",                /* PCrel2_R0           @(disp,PC),R0   */
        "$1,r0",                /* PCrel1_R0           @(disp,PC),R0   */
        "$4,$n",                /* PCrel4_Rn           @(disp,PC),Rn   */
        "$2,$n",                /* PCrel2_Rn           @(disp,PC),Rn   */
        "$1,$n",                /* PCrel1_Rn           @(disp,PC),Rn   */
        "@($x,$m),r0",          /* at_disp_Rm_R0       @(disp,Rm),R0   */
        "@($x,$m),$n",          /* at_disp_Rm_Rn       @(disp,Rm),Rn   */
        "@$m+,@$n+",            /* atRmplus_atRnplus   @Rm+,@Rn+       */
        "@$n+,gbr",             /* atRnplus_GBR        @Rm+,GBR        */
        "@$n+,mach",            /* atRnplus_MACH       @Rm+,MACH       */
        "@$n+,macl",            /* atRnplus_MACL       @Rm+,MACL       */
        "@$n+,pr",              /* atRnplus_PR         @Rm+,PR         */
        "@$m+,$n",              /* atRmplus_Rn         @Rm+,Rn         */
        "@$n+,sr",              /* atRnplus_SR         @Rm+,SR         */
        "@$n+,vbr",             /* atRnplus_VBR        @Rm+,VBR        */
        "@$m,$n",               /* atRm_Rn             @Rm,Rn          */
        "@$n",                  /* atRn                @Rn             */
        "gbr,@-$n",             /* GBR_atminusRn       GBR,@-Rn        */
        "gbr,$n",               /* GBR_Rn              GBR,Rn          */
        "mach,@-$n",            /* MACH_atminusRn      MACH,@-Rn       */
        "mach,$n",              /* MACH_Rn             MACH,Rn         */
        "macl,@-$n",            /* MACL_atminusRn      MACL,@-Rn       */
        "macl,$n",              /* MACL_Rn             MACL,Rn         */
        "pr,@-$n",              /* PR_atminusRn        PR,@-Rn         */
        "pr,$n",                /* PR_Rn               PR,Rn           */
        "r0,@($g,gbr)",         /* R0_at_disp_GBR      R0,@(disp,GBR)  */
        "r0,@($x,$m)",          /* R0_at_disp_Rm       R0,@(disp,Rn)   */
        "$m,@(r0,$n)",          /* Rm_at_R0_Rn         Rm,@(R0,Rn)     */
        "$m,@($x,$n)",          /* Rm_at_disp_Rn       Rm,@(disp,Rn)   */
        "$m,@-$n",              /* Rm_atminusRn        Rm,@-Rn         */
        "$m,@$n",               /* Rm_atRn             Rm,@Rn          */
        "$n,gbr",               /* Rn_GBR              Rm,GBR          */
        "$n,mach",              /* Rn_MACH             Rm,MACH         */
        "$n,macl",              /* Rn_MACL             Rm,MACL         */
        "$n,pr",                /* Rn_PR               Rm,PR           */
        "$m,$n",                /* Rm_Rn               Rm,Rn           */
        "$n,sr",                /* Rn_SR               Rm,SR           */
        "$n,vbr",               /* Rn_VBR              Rm,VBR          */
        "$n",                   /* Rn                  Rn              */
        "sr,@-$n",              /* SR_atminusRn        SR,@-Rn         */
        "sr,$n",                /* SR_Rn               SR,Rn           */
        "vbr,@-$n",             /* VBR_atminusRn       VBR,@-Rn        */
        "vbr,$n",               /* VBR_Rn              VBR,Rn          */
        "$d",                   /* DISP                disp            */
        "$b",                   /* BRANCH12            disp            */
        "$$",                   /* ALL                 disp            */
        ""
};

int      ra,rn,rm,rx,opc;
long     disp,imm;

int	get_opcode(ins)
{
	opc=ins;
	ra=(ins>>12) & 0x0f;
	rn=(ins>> 8) & 0x0f;
	rm=(ins>> 4) & 0x0f;
	rx=(ins    ) & 0x0f;
	disp=imm=ins    & 0xff;
	
	switch(ra) {
		case 0x0:	return op_0();
		case 0x1:	return op_1();
		case 0x2:	return op_2();
		case 0x3:	return op_3();
		case 0x4:	return op_4();
		case 0x5:	return op_5();
		case 0x6:	return op_6();
		case 0x7:	return op_7();
		case 0x8:	return op_8();
		case 0x9:	return op_9();
		case 0xa:	return op_a();
		case 0xb:	return op_b();
		case 0xc:	return op_c();
		case 0xd:	return op_d();
		case 0xe:	return op_e();
		case 0xf:	return op_f();
	}
	return 0;
}

int	op_0()
{
	switch(opc) {
		case  /*(%00001000)*/ 0x0008:	return CLRT;
		case  /*(%00001001)*/ 0x0009:	return NOP;
		case  /*(%00001011)*/ 0x000b:	return RTS;
		case  /*(%00011000)*/ 0x0018:	return SETT;
		case  /*(%00011001)*/ 0x0019:	return DIV0U;
		case  /*(%00011011)*/ 0x001b:	return SLEEP;
		case  /*(%00101000)*/ 0x0028:	return CLRMAC;
		case  /*(%00101011)*/ 0x002b:	return RTE;
	}
	switch(disp) {
		case  /*(%00000010)*/ 0x0002:	return OP(STC, SR_Rn);
		case  /*(%00000011)*/ 0x0003:	return OP(BSRF,Rn);
		case  /*(%00001010)*/ 0x000a:	return OP(STS, MACH_Rn);
		case  /*(%00010010)*/ 0x0012:	return OP(STC, GBR_Rn);
		case  /*(%00011010)*/ 0x001a:	return OP(STS, MACL_Rn);
		case  /*(%00100010)*/ 0x0022:	return OP(STC, VBR_Rn);
		case  /*(%00100011)*/ 0x0023:	return OP(BRAF,Rn);
		case  /*(%00101001)*/ 0x0029:	return OP(MOVT,Rn);
		case  /*(%00101010)*/ 0x002a:	return OP(STS ,PR_Rn);
	}
	switch(rx) {
		case  /*(%0100)*/ 0x0004:	return OP(MOVB,Rm_at_R0_Rn);
		case  /*(%0101)*/ 0x0005:	return OP(MOVW,Rm_at_R0_Rn);
		case  /*(%0110)*/ 0x0006:	return OP(MOVL,Rm_at_R0_Rn);
		case  /*(%0111)*/ 0x0007:	return OP(MULL,Rm_Rn);
		case  /*(%1100)*/ 0x000c:	return OP(MOVB,at_R0_Rm_Rn);
		case  /*(%1101)*/ 0x000d:	return OP(MOVW,at_R0_Rm_Rn);
		case  /*(%1110)*/ 0x000e:	return OP(MOVL,at_R0_Rm_Rn);
		case  /*(%1111)*/ 0x000f:	return OP(MACL,atRmplus_atRnplus);
	}
	return OP(ILL,ALL);
}

int	op_1()
{
	return OP(MOVL,Rm_at_disp_Rn);
}
int	op_2()
{
	switch(rx) {
		case   /*(%0000)*/ 0x0000:	return OP(MOVB ,Rm_atRn);
		case   /*(%0001)*/ 0x0001:	return OP(MOVW ,Rm_atRn);
		case   /*(%0010)*/ 0x0002:	return OP(MOVL ,Rm_atRn);
		case   /*(%0011)*/ 0x0003:	return OP(ILL,ALL);
		case   /*(%0100)*/ 0x0004:	return OP(MOVB ,Rm_atminusRn);
		case   /*(%0101)*/ 0x0005:	return OP(MOVW ,Rm_atminusRn);
		case   /*(%0110)*/ 0x0006:	return OP(MOVL ,Rm_atminusRn);
		case   /*(%0111)*/ 0x0007:	return OP(DIV0S,Rm_Rn);
		case   /*(%1000)*/ 0x0008:	return OP(TST  ,Rm_Rn);
		case   /*(%1001)*/ 0x0009:	return OP(AND  ,Rm_Rn);
		case   /*(%1010)*/ 0x000a:	return OP(XOR  ,Rm_Rn);
		case   /*(%1011)*/ 0x000b:	return OP(OR   ,Rm_Rn);
		case   /*(%1100)*/ 0x000c:	return OP(CMPSTR,Rm_Rn);
		case   /*(%1101)*/ 0x000d:	return OP(XTRCT,Rm_Rn);
		case   /*(%1110)*/ 0x000e:	return OP(MULUW,Rm_Rn);
		case   /*(%1111)*/ 0x000f:	return OP(MULSW,Rm_Rn);
	}
	return OP(ILL,ALL);
}
int	op_3()
{
	switch(rx) {
		case   /*(%0000)*/ 0x0000:	return OP(CMPEQ,Rm_Rn);
		case   /*(%0001)*/ 0x0001:	return OP(ILL,ALL);
		case   /*(%0010)*/ 0x0002:	return OP(CMPHS,Rm_Rn);
		case   /*(%0011)*/ 0x0003:	return OP(CMPGE,Rm_Rn);
		case   /*(%0100)*/ 0x0004:	return OP(DIV1 ,Rm_Rn);
		case   /*(%0101)*/ 0x0005:	return OP(DMULUL,Rm_Rn);
		case   /*(%0110)*/ 0x0006:	return OP(CMPHI,Rm_Rn);
		case   /*(%0111)*/ 0x0007:	return OP(CMPGT,Rm_Rn);
		case   /*(%1000)*/ 0x0008:	return OP(SUB  ,Rm_Rn);
		case   /*(%1001)*/ 0x0009:	return OP(ILL,ALL);
		case   /*(%1010)*/ 0x000a:	return OP(SUBC ,Rm_Rn);
		case   /*(%1011)*/ 0x000b:	return OP(SUBV ,Rm_Rn);
		case   /*(%1100)*/ 0x000c:	return OP(ADD  ,Rm_Rn);
		case   /*(%1101)*/ 0x000d:	return OP(DMULSL,Rm_Rn);
		case   /*(%1110)*/ 0x000e:	return OP(ADDC ,Rm_Rn);
		case   /*(%1111)*/ 0x000f:	return OP(ADDV ,Rm_Rn);
	}
	return OP(ILL,ALL);
}
int	op_4()
{
	switch(disp) {
		case  /*(%00000000)*/ 0x0000:	return OP(SHLL, Rn);
		case  /*(%00000001)*/ 0x0001:	return OP(SHLR, Rn);
		case  /*(%00000010)*/ 0x0002:	return OP(STSL, MACH_atminusRn);
		case  /*(%00000011)*/ 0x0003:	return OP(STCL, SR_atminusRn);
		case  /*(%00000100)*/ 0x0004:	return OP(ROTL, Rn);
		case  /*(%00000101)*/ 0x0005:	return OP(ROTR, Rn);
		case  /*(%00000110)*/ 0x0006:	return OP(LDSL, Rn_MACH);
		case  /*(%00000111)*/ 0x0007:	return OP(LDCL, atRnplus_SR);
		case  /*(%00001000)*/ 0x0008:	return OP(SHLL2,Rn);
		case  /*(%00001001)*/ 0x0009:	return OP(SHLR2,Rn);
		case  /*(%00001010)*/ 0x000a:	return OP(LDS , Rn_MACH);
		case  /*(%00001011)*/ 0x000b:	return OP(JSR , atRn  );
		case  /*(%00001110)*/ 0x000e:	return OP(LDC , Rn_SR  );

		case  /*(%00010000)*/ 0x0010:	return OP(DT  , Rn    );
		case  /*(%00010001)*/ 0x0011:	return OP(CMPPZ,Rn);
		case  /*(%00010010)*/ 0x0012:	return OP(STSL,MACL_atminusRn);
		case  /*(%00010011)*/ 0x0013:	return OP(STCL,GBR_atminusRn);
		case  /*(%00010100)*/ 0x0014:	return OP(ILL,ALL);
		case  /*(%00010101)*/ 0x0015:	return OP(CMPPL,Rn);
		case  /*(%00010110)*/ 0x0016:	return OP(LDSL,atRnplus_MACL);
		case  /*(%00010111)*/ 0x0017:	return OP(LDCL,atRnplus_GBR);
		case  /*(%00011000)*/ 0x0018:	return OP(SHLL8,Rn);
		case  /*(%00011001)*/ 0x0019:	return OP(SHLR8,Rn);
		case  /*(%00011010)*/ 0x001a:	return OP(LDS,  Rn_MACL);
		case  /*(%00011011)*/ 0x001b:	return OP(TASB, atRn);
		case  /*(%00011100)*/ 0x001c:	
		case  /*(%00011101)*/ 0x001d:	return OP(ILL,ALL);
		case  /*(%00011110)*/ 0x001e:	return OP(LDC,Rn_GBR);
		
		case  /*(%00100000)*/ 0x0020:	return OP(SHAL,  Rn    );
		case  /*(%00100001)*/ 0x0021:	return OP(SHAR  ,Rn);
		case  /*(%00100010)*/ 0x0022:	return OP(STSL,  PR_atminusRn);
		case  /*(%00100011)*/ 0x0023:	return OP(STCL  ,VBR_atminusRn);
		case  /*(%00100100)*/ 0x0024:	return OP(ROTCL ,Rn);
		case  /*(%00100101)*/ 0x0025:	return OP(ROTCR ,Rn);
		case  /*(%00100110)*/ 0x0026:	return OP(LDSL  ,atRnplus_PR  );
		case  /*(%00100111)*/ 0x0027:	return OP(LDCL  ,atRnplus_VBR);
		case  /*(%00101000)*/ 0x0028:	return OP(SHLL16,Rn);
		case  /*(%00101001)*/ 0x0029:	return OP(SHLR16,Rn);
		case  /*(%00101010)*/ 0x002a:	return OP(LDS ,  Rn_PR);
		case  /*(%00101011)*/ 0x002b:	return OP(JMP  , atRn);
		case  /*(%00101100)*/ 0x002c:	
		case  /*(%00101101)*/ 0x002d:	return OP(ILL,ALL);
		case  /*(%00101110)*/ 0x002e:	return OP(LDC,Rn_VBR);
	}
	switch(rx) {
		case  /*(%1111)*/ 0x000f:	return OP(MACW,atRmplus_atRnplus);
	}
	return OP(ILL,ALL);
}
int	op_5()
{
	return OP(MOVL,at_disp_Rm_Rn);
}
int	op_6()
{
	switch(rx) {
		case   /*(%0000)*/ 0x0000:	return OP(MOVB ,atRm_Rn);
		case   /*(%0001)*/ 0x0001:	return OP(MOVW ,atRm_Rn);
		case   /*(%0010)*/ 0x0002:	return OP(MOVL ,atRm_Rn);
		case   /*(%0011)*/ 0x0003:	return OP(MOV  ,Rm_Rn);
		case   /*(%0100)*/ 0x0004:	return OP(MOVB ,atRmplus_Rn);
		case   /*(%0101)*/ 0x0005:	return OP(MOVW ,atRmplus_Rn);
		case   /*(%0110)*/ 0x0006:	return OP(MOVL ,atRmplus_Rn);
		case   /*(%0111)*/ 0x0007:	return OP(NOT  ,Rm_Rn);
		case   /*(%1000)*/ 0x0008:	return OP(SWAPB,Rm_Rn);
		case   /*(%1001)*/ 0x0009:	return OP(SWAPW,Rm_Rn);
		case   /*(%1010)*/ 0x000a:	return OP(NEGC ,Rm_Rn);
		case   /*(%1011)*/ 0x000b:	return OP(NEG  ,Rm_Rn);
		case   /*(%1100)*/ 0x000c:	return OP(EXTUB,Rm_Rn);
		case   /*(%1101)*/ 0x000d:	return OP(EXTUW,Rm_Rn);
		case   /*(%1110)*/ 0x000e:	return OP(EXTSB,Rm_Rn);
		case   /*(%1111)*/ 0x000f:	return OP(EXTSW,Rm_Rn);
	}
	return OP(ILL,ALL);
}
int	op_7()
{
	return OP(ADD,Imm_Rn);
}
int	op_8()
{
	switch(rn) {
		case   /*(%0000)*/ 0x0000:	return OP(MOVB,R0_at_disp_Rm);
		case   /*(%0001)*/ 0x0001:	return OP(MOVW,R0_at_disp_Rm);
		case   /*(%0010)*/ 0x0002:	
		case   /*(%0011)*/ 0x0003:	return OP(ILL,ALL);
		case   /*(%0100)*/ 0x0004:	return OP(MOVB,at_disp_Rm_R0);
		case   /*(%0101)*/ 0x0005:	return OP(MOVW,at_disp_Rm_R0);
		case   /*(%0110)*/ 0x0006:	
		case   /*(%0111)*/ 0x0007:	return OP(ILL,ALL);
		case   /*(%1000)*/ 0x0008:	return OP(CMPEQ,Imm_R0);
		case   /*(%1001)*/ 0x0009:	return OP(BT  ,DISP);
		case   /*(%1010)*/ 0x000a:	return OP(ILL ,ALL);
		case   /*(%1011)*/ 0x000b:	return OP(BF  ,DISP);
		case   /*(%1100)*/ 0x000c:	return OP(ILL ,ALL);
		case   /*(%1101)*/ 0x000d:	return OP(BTS ,DISP);
		case   /*(%1110)*/ 0x000e:	return OP(ILL,ALL);
		case   /*(%1111)*/ 0x000f:	return OP(BFS ,DISP);
	}
	return OP(ILL,ALL);
}
int	op_9()
{
	return OP(MOVW,PCrel2_Rn);
}
int	op_a()
{
	return OP(BRA,BRANCH12);
}
int	op_b()
{
	return OP(BSR,BRANCH12);
}
int	op_c()
{
	switch(rn) {
		case   /*(%0000)*/ 0x0000:	return OP(MOVB,R0_at_disp_GBR);
		case   /*(%0001)*/ 0x0001:	return OP(MOVW,R0_at_disp_GBR);
		case   /*(%0010)*/ 0x0002:	return OP(MOVL,R0_at_disp_GBR);
		case   /*(%0011)*/ 0x0003:	return OP(TRAPA,Imm);
		case   /*(%0100)*/ 0x0004:	return OP(MOVB,at_disp_GBR_R0);
		case   /*(%0101)*/ 0x0005:	return OP(MOVW,at_disp_GBR_R0);
		case   /*(%0110)*/ 0x0006:	return OP(MOVL,at_disp_GBR_R0);
		case   /*(%0111)*/ 0x0007:	return OP(MOVA,PCrel4_R0);
		case   /*(%1000)*/ 0x0008:	return OP(TST ,Imm_R0);
		case   /*(%1001)*/ 0x0009:	return OP(AND ,Imm_R0);
		case   /*(%1010)*/ 0x000a:	return OP(XOR ,Imm_R0);
		case   /*(%1011)*/ 0x000b:	return OP(OR  ,Imm_R0);
		case   /*(%1100)*/ 0x000c:	return OP(TSTB,Imm_at_R0_GBR);
		case   /*(%1101)*/ 0x000d:	return OP(ANDB,Imm_at_R0_GBR);
		case   /*(%1110)*/ 0x000e:	return OP(XORB,Imm_at_R0_GBR);
		case   /*(%1111)*/ 0x000f:	return OP(ORB ,Imm_at_R0_GBR);
	}
	return OP(ILL,ALL);
}
int	op_d()
{
	return OP(MOVL,PCrel4_Rn);
}
int	op_e()
{
	return OP(MOV,Imm_Rn);
}
int	op_f()
{
	return OP(ILL,ALL);
}







