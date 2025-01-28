

typedef unsigned char	Uchar ;
typedef unsigned short	Ushort;
typedef	unsigned long	Ulong ;


#define VBUFSIZE    0x40000		/* �o�̓X�g���[���o�b�t�@ */
#define	MMX			1
#define	AMD3DNOW	1
#define	KNI			1


/* OLD STYLE MS-DOS EXE HEADER */
typedef struct  EXE_OLD_HDR {
    char    mz[2];      /*+00 'MZ' */
    short   fragment;   /*+02 �ŏI�y�[�W�̃o�C�g���B */
    short   pages;      /*+04 �y�[�W�� */
    short   relocations;/*+06 �����P�[�V�����e�[�u���̍��ڐ� */
    short   reloc_size; /*+08 �����P�[�V�����e�[�u���̃T�C�Y(16byte�P��) */
    short   min_mem;    /*+0a �ŏ��K�v�������[(16byte�P��) */
    short   max_mem;    /*+0c �ő�K�v�������[(16byte�P��) */
    short   stackseg;   /*+0e �X�^�b�N�Z�O�����g�̈ʒu(16byte�P��) */
    short   init_sp;    /*+10 �X�^�b�N�|�C���^�����l */
    short   chack_sum;  /*+12  */
    short   init_ip;    /*+14 �o�b�����l */
    short   init_cs;    /*+16 */
    short   reloc_offset;/*+18 �����P�[�V�����e�[�u���̈ʒu */
    short   overlay;    /*+1a �I�[�o���C�ԍ��i�풓���Ȃ�O�j */
    short   rsv1[16];   /*+1c */
    long    ne_header;  /*+3c 'NE'�w�b�_�[�̎n�܂�̃I�t�Z�b�g */
} EXE_OLD_HDR;
/* NE-HEADER�̈ʒu������o�����@�F */
/* (1)OLD-HEADER��offset 18h(reloc_offset)==0x0040�ł��邱�Ƃ��m���߂�B */
/* (2)ne_header ��seek����B */


/* WINDOWS 2.11 EXE HEADER */
typedef struct  EXE_NEW_HDR {
    char    ne[2];      /*+00 'NE' */
    char    ver;        /*+02 �����J�[�̃o�[�W���� */
    char    rev;        /*+03 */
    short   entrytable; /*+04 �G���g���[�e�[�u���ւ̃I�t�Z�b�g('NE'����) */
    short   entrysize;  /*+06 �G���g���[�e�[�u���̃o�C�g�� */
    long    crc_32;     /*+08 �b�q�b�R�Q�̒l */
    short   flag;       /*+0c 0000:NO AUTO  DATA */
                        /*    0001:SINGLE   DATA */
                        /*    0002:MULTIPLE DATA */
                        /*    2000:LINK ERR */
                        /*    8000: DLL */
    short   autoseg;    /*+0e �����f�[�^�Z�O�����g�̔ԍ� */
    short   heapsize;   /*+10 �_�C�i�~�b�N�q�[�v�̏����T�C�Y */
    short   stacksize;  /*+12 �X�^�b�N�̃T�C�Y(ss!=ds�̎���0) */
    short   init_ip;    /*+14 */
    short   init_cs;    /*+16 */
    short   init_sp;    /*+18 */
    short   init_ss;    /*+1a */
    short   segentry;   /*+1c �Z�O�����g�e�[�u���̃G���g���� */
    short   modentry;	/*+1e ���W���[���Q�ƃe�[�u���̃G���g���[�� */
    short   nrtablesize;/*+1e ��풓���e�[�u���̃o�C�g�� */

    short   seg_table;  /*+22 �Z�O�����g�e�[�u���ւ̃I�t�Z�b�g('NE'����) */
    short   res_table;  /*+24 ���\�[�X  �e�[�u���ւ̃I�t�Z�b�g('NE'����) */
    short   nam_table;  /*+26 �풓��    �e�[�u���ւ̃I�t�Z�b�g('NE'����) */
    short   ref_table;  /*+28 ���W���[���Q�ƃe�[�u���ւ̃I�t�Z�b�g('NE'����) */
    short   imp_table;  /*+2a �C���|�[�g�e�[�u���ւ̃I�t�Z�b�g('NE'����) */
    long    nrt_table;  /*+2c ��풓��  �e�[�u���ւ̃I�t�Z�b�g(EXE�擪����) */
    short   movables;   /*+30 �ړ��\�ȃG���g���|�C���g�̐� */
    short   align;      /*+32 �_���Z�N�^�A���C�������g�̃V�t�g��(2^n��n) */
    short   rsvsegs;    /*+34 �\��Z�O�����g�� */
    char    rsv[10];    /*+36�`3f */
} EXE_NEW_HDR;


typedef struct  SEGTABLE {
    Ushort  offset;     /*+00 �_���Z�N�^�I�t�Z�b�g(byte)  0�Ȃ�f�[�^���� */
    Ushort  size;       /*+02 �Z�O�����g�T�C�Y(byte) 0�Ȃ�64kB */
    short   flag;       /*+04 0000:CODE */
                        /*    0001:DATA */
                        /*    0007:TYPE_MASK */
                        /*    0010:MOVABLE */
                        /*    0040:PRELOAD */
                        /*    0100:RELOCINFO   �����P�[�V������񂠂� */
                        /*    F000:DISCARDABLE �p���\ */
    Ushort  allocsize;  /*+06 �ŏ������T�C�Y(byte)=�g�[�^���T�C�Y  0�Ȃ�64kB */
} SEGTABLE ;

typedef	struct	MOVABLE {/* flg==ff�̎�. �U�o�C�g */
	Uchar	key;		/*	�L�[���[�h 01:EXPORTED 02:SHARED */
	Uchar	int3f[2];	/*  CD 3F */
	Uchar	segnum;
	Ushort	offset;
} MOVABLE;

typedef	struct	FIXED {	/* flg==xx�̎�. �R�o�C�g */
	Uchar	key;		/*	�L�[���[�h 01:EXPORTED 02:SHARED */
/*	Ushort	offset;		 * segment fixup word�ւ̃I�t�Z�b�g */
	Uchar	offset_l;
	Uchar	offset_h;
} FIXED;

typedef	struct	ENTRYTABLE {
	Uchar	cnt;
	Uchar	flg;		/* 00: no use */
						/* ff: movable  */
						/* xx: fixed segnum */
	union {
		MOVABLE	m;
		FIXED	f;
	} u;
} ENTRYTABLE;

typedef	struct NRT {		/* ��풓�����L������e�[�u�� */
	char *name;
	int   seg;
	int   off;
	int   val;
} NRT;

/* Windows API database */
typedef	struct API_DB {
	char *name;
	int   module_id;
	int   ordinal;
} API_DB;

/* �����P�[�V�������̌㔼�S�o�C�g�̍\�� */
typedef struct I_REF {
	Uchar  segnum;
	Uchar  dummy;
	Ushort off;
} I_REF;
typedef struct I_NAME {
	Ushort idx;
	Ushort off;
} I_NAME;
typedef struct I_ORDINAL {
	Ushort idx;
	Ushort ordinal;
} I_ORDINAL;
typedef struct I_OSFIXUP {
	Ushort type;
	Ushort dummy;
} I_OSFIXUP;

/* �����P�[�V�������W�o�C�g�̍\�� */
/* �f�[�^�ŏI�Ԓn�� dw REL_COUNT; */
/* ���̌��� REL_COUNT ���̃����P�[�V�����E���R�[�h������ */
typedef struct REL {		/* �����P�[�V�����e�[�u�� */
	Uchar  type;
	Uchar  flag;
	Ushort off;
	union  {
	  struct I_REF      i;
	  struct I_NAME     n;
	  struct I_ORDINAL  o;
	  struct I_OSFIXUP  f;
	} i;
} REL;

#define T_LOBYTE     0x00  
#define T_SEGMENT	 0x02  
#define T_FAR_ADDR   0x03  		/*(32-bit pointer)*/
#define T_OFFSET     0x05  		/*(16-bit offset)*/

#define F_INTERNALREF	0		/* 00 �����Q�� */
#define	F_IMPORTORDINAL 1		/* 01 �c�k�k�Q��.�G���g���[�����Ŏw�� */
#define	F_IMPORTNAME    2		/* 10 �c�k�k�Q��.�G���g���[���Ŏw�� */
#define	F_OSFIXUP       3		/* 11 ���������_���Z�̖��߂�fixup����B */
#define	F_ADDITIVE      4		/* �ǉ���񂪗L�邱�Ƃ������B */


#ifdef	LINUX
#define	CRLF	"\n"
#else
#define	CRLF	"\r\n"			/* �s�K��MS-DOS�̓e�L�X�g�I�[�v������ƒx�� */
#endif							/* �̂Ńo�C�i���[�ŏ������߂̕����� */

char   *xstrdup(  char *s );
char   *xstrpdup( char *s );
char   *xmalloc( long size);
void	init_hash(void);
void	clr_hash(void);
void	clr_hash2(void);
void	insert_hash2(long val,int flg,char *obj);
void	insert_hash(long val,int flg,char *obj);
void	hash_report(void);
long	disasm_i386(char *buf,long start,long size,FILE *ofp);
void	spool_init(void);
void	pe_dump(void);
void	le_dump(void);
void	dump_out(FILE *fp,unsigned char *buf,long cutsize,long start);
void	code32_dump(int segnum,char *sectname,long off,long siz,long start,long size);
void	set_cpumode(int op,int ad);

/***  �v���g�^�C�v�錾�@***/
int		main(int argc,char **argv);
void 	raw386_dump(long ofs,int cpumode);
void	old_exe_dump(void);
void	ne_dump(void);
void	sys_report(void);
void	touppers(char *s);
void	load_exemap(char *s);
void	read_exemap(char *s);
void	read_exemap32(char *s);
void	load_apilist(char *s);
void	read_apilist(char *s);
int		api_id(char *s);
void	insert_api(char *modname,int ordinal,char *entname);
int		search_api(char *s,int ordinal,char *namebuf);
void	MZchk(void);
void	NEseek(long l);
void	NEload(void);
void	NEchk(void);
void	SEGTload(void);
void	MODref(void);
void	printstr(char *s);
void	sprintstr(char *t,char *s);
void	dump(unsigned char *buf,int siz);
void	ENTdump(void);
void	pr_ent(int entnum,int seg,int off);
void	SEGTdump(void);
void	loadbin(long off,long size);
void	cutbin(char *name,long size);
void	cuthex(char *name,long size);
void	rel_dump(int segnum,long off,long siz);
void	rel_print(REL *r);
int 	get_rword(int off);
void	rel_chain(REL *r);
void	disasmbin(char *name,long start,long size,char *hdrmsg);
void	gen_dummy_hdr(char *s,long l);
void	nrt_dump(Uchar *s);
int 	get_nrtnam(int val);
void	print_nrtname(int entnum,int seg,int off);
void	insert_label(void);
void	insert_fixup(void);
void	putrel(char *s,int ip);
void	touppers(char *s);
int		split_str(char *p,int c,char **args);

#define	ZZ	printf("%s : %d :ZZ\n",__FILE__,__LINE__);
