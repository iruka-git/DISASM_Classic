/*
MNEMONIC     OPERAND TYPES              Byte 1 2 Byte 3, [4]
---------------------------------------------------------------
PUNPCKLBW    register,memory/register   0F 60     mod-rm,[sib]
PUNPCKLWD    register,memory/register   0F 61     mod-rm,[sib]
PUNPCKLDQ    register,memory/register   0F 62     mod-rm,[sib]
PACKSSWB     register,memory/register   0F 63     mod-rm,[sib]
PCMPGTB      register,memory/register   0F 64     mod-rm,[sib]
PCMPGTW      register,memory/register   0F 65     mod-rm,[sib]
PCMPGTD      register,memory/register   0F 66     mod-rm,[sib]
PACKUSWB     register,memory/register   0F 67     mod-rm,[sib]
PUNPCKHBW    register,memory/register   0F 68     mod-rm,[sib]
PUNPCKHWD    register,memory/register   0F 69     mod-rm,[sib]
PUNPCKHDQ    register,memory/register   0F 6A     mod-rm,[sib]
PACKSSDW     register,memory/register   0F 6B     mod-rm,[sib]
MOVD         register,memory/iregister  0F 6E     mod-rm,[sib]
MOVQ         register,memory/register   0F 6F     mod-rm,[sib]
PSHIMW*      register, immediate        0F 71     mod-rm, imm
PSHIMD*      register, immediate        0F 72     mod-rm, imm
PSHIMQ*      register, immediate        0F 73     mod-rm, imm
PCMPEQB      register,memory/register   0F 74     mod-rm,[sib]
PCMPEQW      register,memory/register   0F 75     mod-rm,[sib]
PCMPEQD      register,memory/register   0F 76     mod-rm,[sib]
EMMS         None                       0F 77
MOVD         memory/iregister,register  0F 7E     mod-rm,[sib]
MOVQ         memory/register,register   0F 7F     mod-rm,[sib]
PSRLW        register,memory/register   0F D1     mod-rm,[sib]
PSRLD        register,memory/register   0F D2     mod-rm,[sib]
PSRLQ        register,memory/register   0F D3     mod-rm,[sib]
PMULLW       register,memory/register   0F D5     mod-rm,[sib]
PSUBUSB      register,memory/register   0F D8     mod-rm,[sib]
PSUBUSW      register,memory/register   0F D9     mod-rm,[sib]
PAND         register,memory/register   0F DB     mod-rm,[sib]
PADDUSB      register,memory/register   0F DC     mod-rm,[sib]
PADDUSW      register,memory/register   0F DD     mod-rm,[sib]
PANDN        register,memory/register   0F DF     mod-rm,[sib]
PSRAW        register,memory/register   0F E1     mod-rm,[sib]
PSRAD        register,memory/register   0F E2     mod-rm,[sib]
PMULHW       register,memory/register   0F E5     mod-rm,[sib]
PSUBSB       register,memory/register   0F E8     mod-rm,[sib]
PSUBSW       register,memory/register   0F E9     mod-rm,[sib]
POR          register,memory/register   0F EB     mod-rm,[sib]
PADDSB       register,memory/register   0F EC     mod-rm,[sib]
PADDSW       register,memory/register   0F ED     mod-rm,[sib]
PXOR         register,memory/register   0F EF     mod-rm,[sib]
PSLLW        register,memory/register   0F F1     mod-rm,[sib]
PSLLD        register,memory/register   0F F2     mod-rm,[sib]
PSLLQ        register,memory/register   0F F3     mod-rm,[sib]
PMADDWD      register,memory/register   0F F5     mod-rm,[sib]
PSUBB        register,memory/register   0F F8     mod-rm,[sib]
PSUBW        register,memory/register   0F F9     mod-rm,[sib]
PSUBD        register,memory/register   0F FA     mod-rm,[sib]
PADDB        register,memory/register   0F FC     mod-rm,[sib]
PADDW        register,memory/register   0F FD     mod-rm,[sib]
PADDD        register,memory/register   0F FE     mod-rm,[sib]
---------------------------------------------------------------
MNEMONIC     OPERAND TYPES              Byte 1 2 Byte 3, [4]


AMD 3D Now!

MNEMONIC     OPERAND TYPES              Byte 1 2 Byte 3, [4]
---------------------------------------------------------------
PAVGUSB		 							0F 0F mod-rm,BF
PFADD									0F 0F mod-rm,9E
PFSUB									0F 0F mod-rm,9A
PFSUBR									0F 0F mod-rm,AA
PFACC									0F 0F mod-rm,AE
PFCMPGE									0F 0F mod-rm,90
PFCMPGT									0F 0F mod-rm,A0
PFCMPEQ									0F 0F mod-rm,B0
PFMIN									0F 0F mod-rm,94
PFMAX									0F 0F mod-rm,A4
PI2FD									0F 0F mod-rm,0D
PF2ID									0F 0F mod-rm,1D
PFRCP									0F 0F mod-rm,96
PFRSQRT									0F 0F mod-rm,97
PFMUL									0F 0F mod-rm,B4
PFRCPIT1								0F 0F mod-rm,A6
PFRSQIT1								0F 0F mod-rm,A7
PFRCPIT2								0F 0F mod-rm,B6
PFMULHRW								0F 0F mod-rm,B7

FEMMS        None                       0F 0E 
PREFETCH                                0F 0D 
---------------------------------------------------------------
MNEMONIC     OPERAND TYPES              Byte 1 2 Byte 3, [4]
*/

char *mmx60_7f[32]={
	"punpcklbw",		/* 0F 60 */
	"punpcklwd",		/* 0F 61 */
	"punpckldq",		/* 0F 62 */
	"packsswb",			/* 0F 63 */
	"pcmpgtb",			/* 0F 64 */
	"pcmpgtw",			/* 0F 65 */
	"pcmpgtd",			/* 0F 66 */
	"packuswb",			/* 0F 67 */
	"punpckhbw",		/* 0F 68 */
	"punpckhwd",		/* 0F 69 */
	"punpckhdq",		/* 0F 6A */
	"packssdw",			/* 0F 6B */
	"",					/* 0F 6C */
	"",					/* 0F 6D */
	"movd",				/* 0F 6E */
	"movq",				/* 0F 6F */
	"",					/* 0F 70 */
	"pshimw",			/* 0F 71 */
	"pshimd",			/* 0F 72 */
	"pshimq",			/* 0F 73 */
	"pcmpeqb",			/* 0F 74 */
	"pcmpeqw",			/* 0F 75 */
	"pcmpeqd",			/* 0F 76 */
	"emms",				/* 0F 77 */
	"",					/* 0F 78 */
	"",					/* 0F 79 */
	"",					/* 0F 7A */
	"",					/* 0F 7B */
	"",					/* 0F 7C */
	"",					/* 0F 7D */
	"movd",				/* 0F 7E */
	"movq"				/* 0F 7F */
};
						
char *mmxd0_ff[48]={
	"",					/* 0F D0 */
	"psrlw",			/* 0F D1 */
	"psrld",			/* 0F D2 */
	"psrlq",			/* 0F D3 */
	"",					/* 0F D4 */
	"pmullw",			/* 0F D5 */
	"",					/* 0F D6 */
	"",					/* 0F D7 */
	"psubusb",			/* 0F D8 */
	"psubusw",			/* 0F D9 */
	"",					/* 0F DA */
	"pand",				/* 0F DB */
	"paddusb",			/* 0F DC */
	"paddusw",			/* 0F DD */
	"",					/* 0F DE */
	"pandn",			/* 0F DF */
	"",					/* 0F E0 */
	"psraw",			/* 0F E1 */
	"psrad",			/* 0F E2 */
	"",					/* 0F E3 */
	"",					/* 0F E4 */
	"pmulhw",			/* 0F E5 */
	"",					/* 0F E6 */
	"",					/* 0F E7 */
	"psubsb",			/* 0F E8 */
	"psubsw",			/* 0F E9 */
	"",					/* 0F EA */
	"por",				/* 0F EB */
	"paddsb",			/* 0F EC */
	"paddsw",			/* 0F ED */
	"",					/* 0F EE */
	"pxor",				/* 0F EF */
	"",					/* 0F F0 */
	"psllw",			/* 0F F1 */
	"pslld",			/* 0F F2 */
	"psllq",			/* 0F F3 */
	"",					/* 0F F4 */
	"pmaddwd",			/* 0F F5 */
	"",					/* 0F F6 */
	"",					/* 0F F7 */
	"psubb",			/* 0F F8 */
	"psubw",			/* 0F F9 */
	"psubd",			/* 0F FA */
	"",					/* 0F FB */
	"paddb",			/* 0F FC */
	"paddw",			/* 0F FD */
	"paddd",			/* 0F FE */
	""					/* 0F FF */
};

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
extern int     mmx;

// regname 0:REG 1:MMX 2:XMM
#define	R32	0
#define	MM	1
#define	XMM	2


void inst_mmx(int opc)
{
	if(opc>=0xd0) {
		mne( mmxd0_ff[opc - 0xd0] );
	} else if(opc>=0x80) {
		mne("?");
		return;
	} else if(opc>=0x60) {
		mne( mmx60_7f[opc - 0x60] );
	} else {
		mne("?");
		return;
	}
	modrmovmmm(op1buf,op2buf,MM,MM);
	return;
}

void inst_mmx_shift(int opc)
{
	int mod_rm;
	int reg;
	char opname[6]="ps   ";
	char *dir,*size;
	
	mod_rm=(*op++) & 0xff;
	reg=mod_rm & 0x07;
	if ((mod_rm&0xc0)!=0xc0) {
		mne("?");
		return;
	}
	mod_rm>>=3;
	switch(opc & 3) {
		case 0:	size="?";break;
		case 1:	size="w";break;
		case 2:	size="d";break;
		case 3:	size="q";break;
		}
	
	switch(mod_rm&7) {
		case 2: dir="rl";break;
		case 4: dir="ra";break;
		case 6: dir="ll";break;
		default:dir="??";break;
		}
	opname[2]=dir[0];
	opname[3]=dir[1];
	opname[4]=size[0];
	opname[5]=0;
	mne(opname);
	
	mmx=1;
	put_regname(op1buf,reg,1);
	immb();
	set_eop();
	mmx=0;
	return;
}

void inst_mmx_mov(int opc)
{
	char *mmxregbuf,*regbuf;
	int modrm,reg;
	
	if (opc & 0x10) {
		mmxregbuf = op2buf;
		regbuf = op1buf;
	} else {
		mmxregbuf = op1buf;
		regbuf = op2buf;
	}
	
	if(!(opc&0x01)) {
		mne("movd");
	} else {
		mne("movq");
		mmx=1;
	}
	
	modrm=*op;
	if ((modrm&0xc0) == 0xc0) {
		op++;
		put_regname(regbuf,modrm&0x07,1);
		mmx=1;
		modrm>>=3;
		put_regname(mmxregbuf,modrm&0x07,1);
        set_eop();
	} else {
		mmx=1;
		modrmov(mmxregbuf,regbuf,1);
	}
	mmx=0;
	return;
}

typedef	struct	{
	short code;
	char *inst;
} CODESTR;

CODESTR amdcode[]={
	{0xbf,"pavgusb"},
	{0x9e,"pfadd"},
	{0x9a,"pfsub"},
	{0xaa,"pfsubr"},
	{0xae,"pfacc"},
	{0x90,"pfcmpge"},
	{0xa0,"pfcmpgt"},
	{0xb0,"pfcmpeq"},
	{0x94,"pfmin"},
	{0xa4,"pfmax"},
	{0x0d,"pi2fd"},
	{0x1d,"pf2id"},
	{0x96,"pfrcp"},
	{0x97,"pfrsqrt"},
	{0xb4,"pfmul"},
	{0xa6,"pfrcpit1"},
	{0xa7,"pfrsqit1"},
	{0xb6,"pfrcpit2"},
	{0xb7,"pfmulhrw"},
	{-1  ,"???"}
};

void	AMD3dnow(void)
{
	extern	char    *op;    /* 現在の解析位置を指しているポインタ */
	CODESTR *t=amdcode;
	int opc;

//	mne("3dnow!");	/仮のニーモニック!

	modrmovmmm(op1buf,op2buf,MM,MM);
	
	opc = (*op++) & 0xff;

	while( t->code >= 0 ) {
		if(t->code == opc) break;
		t++;
	}
	mne( t->inst );

	return;
}
