enum opname {

	ADD,
	ADDC,
	ADDV,
	AND,
	ANDB,
	BF,
	BFS,
	BRA,
	BRAF,
	BSR,
	BSRF,
	BT,
	BTS,
	CLRMAC,
	CLRT,
	CMPEQ,
	CMPGE,
	CMPGT,
	CMPHI,
	CMPHS,
	CMPPL,
	CMPPZ,
	CMPSTR,
	DIV0S,
	DIV0U,
	DIV1,
	DT,
	DMULUL,
	DMULSL,
	EXTSB,
	EXTSW,
	EXTUB,
	EXTUW,
	JMP,
	JSR,
	LDC,
	LDCL,
	LDS,
	LDSL,
	MACW,
	MACL,
	MOV,
	MOVB,
	MOVL,
	MOVW,
	MOVA,
	MOVT,
	MULSW,
	MULUW,
	MULL,
	NEG,
	NEGC,
	NOP,
	NOT,
	OR,
	ORB,
	ROTCL,
	ROTCR,
	ROTL,
	ROTR,
	RTE,
	RTS,
	SETT,
	SHAL,
	SHAR,
	SHLL,
	SHLL2,
	SHLL8,
	SHLL16,
	SHLR,
	SHLR2,
	SHLR8,
	SHLR16,
	SLEEP,
	STC,
	STCL,
	STS,
	STSL,
	SUB,
	SUBC,
	SUBV,
	SWAPB,
	SWAPW,
	TASB,
	TRAPA,
	TST,
	TSTB,
	XOR,
	XORB,
	XTRCT,
	ILL,
	WORD,
	LONG,
	LONG2,
};

enum oprand {
No_op              ,
Imm                ,
Imm_at_R0_GBR      ,
Imm_R0             ,
Imm_Rn             ,
at_R0_Rm_Rn        ,
at_disp_GBR_R0     ,
PCrel4_R0          ,
PCrel2_R0          ,
PCrel1_R0          ,
PCrel4_Rn          ,
PCrel2_Rn          ,
PCrel1_Rn          ,
at_disp_Rm_R0      ,
at_disp_Rm_Rn      ,
atRmplus_atRnplus  ,
atRnplus_GBR       ,
atRnplus_MACH      ,
atRnplus_MACL      ,
atRnplus_PR        ,
atRmplus_Rn        ,
atRnplus_SR        ,
atRnplus_VBR       ,
atRm_Rn            ,
atRn               ,
GBR_atminusRn      ,
GBR_Rn             ,
MACH_atminusRn     ,
MACH_Rn            ,
MACL_atminusRn     ,
MACL_Rn            ,
PR_atminusRn       ,
PR_Rn              ,
R0_at_disp_GBR     ,
R0_at_disp_Rm      ,
Rm_at_R0_Rn        ,
Rm_at_disp_Rn      ,
Rm_atminusRn       ,
Rm_atRn            ,
Rn_GBR             ,
Rn_MACH            ,
Rn_MACL            ,
Rn_PR              ,
Rm_Rn              ,
Rn_SR              ,
Rn_VBR             ,
Rn                 ,
SR_atminusRn       ,
SR_Rn              ,
VBR_atminusRn      ,
VBR_Rn             ,
DISP               ,
BRANCH12           ,
ALL                ,

};

#define	OP( name , opr )  (name | (opr*256) )

#define	TFLAG	0x01
#define	TRAPA_0x1f 	(0xc31f)

enum mark {
	NOMARK=0,
	DW    =1,	/* .word */
	DD    =2,	/* .long */
	DD2   =3,	/* .longの２バイト目 */
};


/***  プロトタイプ宣言　***/
int get_opcode(int ins);
int op_0(void);
int op_1(void);
int op_2(void);
int op_3(void);
int op_4(void);
int op_5(void);
int op_6(void);
int op_7(void);
int op_8(void);
int op_9(void);
int op_a(void);
int op_b(void);
int op_c(void);
int op_d(void);
int op_e(void);
int op_f(void);
