

typedef unsigned char	Uchar ;
typedef unsigned short	Ushort;
typedef	unsigned int	Uint  ;
typedef	unsigned int	Uint ;


#define VBUFSIZE    0x40000		/* 出力ストリームバッファ */
#define	MMX			1
#define	AMD3DNOW	1
#define	KNI			1


/* OLD STYLE MS-DOS EXE HEADER */
typedef struct  EXE_OLD_HDR {
    char    mz[2];      /*+00 'MZ' */
    short   fragment;   /*+02 最終ページのバイト数。 */
    short   pages;      /*+04 ページ数 */
    short   relocations;/*+06 リロケーションテーブルの項目数 */
    short   reloc_size; /*+08 リロケーションテーブルのサイズ(16byte単位) */
    short   min_mem;    /*+0a 最小必要メモリー(16byte単位) */
    short   max_mem;    /*+0c 最大必要メモリー(16byte単位) */
    short   stackseg;   /*+0e スタックセグメントの位置(16byte単位) */
    short   init_sp;    /*+10 スタックポインタ初期値 */
    short   chack_sum;  /*+12  */
    short   init_ip;    /*+14 ＰＣ初期値 */
    short   init_cs;    /*+16 */
    short   reloc_offset;/*+18 リロケーションテーブルの位置 */
    short   overlay;    /*+1a オーバレイ番号（常駐部なら０） */
    short   rsv1[16];   /*+1c */
    int     ne_header;  /*+3c 'NE'ヘッダーの始まりのオフセット */
} EXE_OLD_HDR;
/* NE-HEADERの位置を割り出す方法： */
/* (1)OLD-HEADERのoffset 18h(reloc_offset)==0x0040であることを確かめる。 */
/* (2)ne_header にseekする。 */


/* WINDOWS 2.11 EXE HEADER */
typedef struct  EXE_NEW_HDR {
    char    ne[2];      /*+00 'NE' */
    char    ver;        /*+02 リンカーのバージョン */
    char    rev;        /*+03 */
    short   entrytable; /*+04 エントリーテーブルへのオフセット('NE'から) */
    short   entrysize;  /*+06 エントリーテーブルのバイト数 */
    int     crc_32;     /*+08 ＣＲＣ３２の値 */
    short   flag;       /*+0c 0000:NO AUTO  DATA */
                        /*    0001:SINGLE   DATA */
                        /*    0002:MULTIPLE DATA */
                        /*    2000:LINK ERR */
                        /*    8000: DLL */
    short   autoseg;    /*+0e 自動データセグメントの番号 */
    short   heapsize;   /*+10 ダイナミックヒープの初期サイズ */
    short   stacksize;  /*+12 スタックのサイズ(ss!=dsの時は0) */
    short   init_ip;    /*+14 */
    short   init_cs;    /*+16 */
    short   init_sp;    /*+18 */
    short   init_ss;    /*+1a */
    short   segentry;   /*+1c セグメントテーブルのエントリ数 */
    short   modentry;	/*+1e モジュール参照テーブルのエントリー数 */
    short   nrtablesize;/*+1e 非常駐名テーブルのバイト数 */

    short   seg_table;  /*+22 セグメントテーブルへのオフセット('NE'から) */
    short   res_table;  /*+24 リソース  テーブルへのオフセット('NE'から) */
    short   nam_table;  /*+26 常駐名    テーブルへのオフセット('NE'から) */
    short   ref_table;  /*+28 モジュール参照テーブルへのオフセット('NE'から) */
    short   imp_table;  /*+2a インポートテーブルへのオフセット('NE'から) */
    int     nrt_table;  /*+2c 非常駐名  テーブルへのオフセット(EXE先頭から) */
    short   movables;   /*+30 移動可能なエントリポイントの数 */
    short   align;      /*+32 論理セクタアラインメントのシフト数(2^nのn) */
    short   rsvsegs;    /*+34 予約セグメント数 */
    char    rsv[10];    /*+36〜3f */
} EXE_NEW_HDR;


typedef struct  SEGTABLE {
    Ushort  offset;     /*+00 論理セクタオフセット(byte)  0ならデータ無し */
    Ushort  size;       /*+02 セグメントサイズ(byte) 0なら64kB */
    short   flag;       /*+04 0000:CODE */
                        /*    0001:DATA */
                        /*    0007:TYPE_MASK */
                        /*    0010:MOVABLE */
                        /*    0040:PRELOAD */
                        /*    0100:RELOCINFO   リロケーション情報あり */
                        /*    F000:DISCARDABLE 廃棄可能 */
    Ushort  allocsize;  /*+06 最小割当サイズ(byte)=トータルサイズ  0なら64kB */
} SEGTABLE ;

typedef	struct	MOVABLE {/* flg==ffの時. ６バイト */
	Uchar	key;		/*	キーワード 01:EXPORTED 02:SHARED */
	Uchar	int3f[2];	/*  CD 3F */
	Uchar	segnum;
	Ushort	offset;
} MOVABLE;

typedef	struct	FIXED {	/* flg==xxの時. ３バイト */
	Uchar	key;		/*	キーワード 01:EXPORTED 02:SHARED */
/*	Ushort	offset;		 * segment fixup wordへのオフセット */
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

typedef	struct NRT {		/* 非常駐名を記憶するテーブル */
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

/* リロケーション情報の後半４バイトの構造 */
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

/* リロケーション情報８バイトの構造 */
/* データ最終番地に dw REL_COUNT; */
/* その後ろに REL_COUNT 個数のリロケーション・レコードが並ぶ */
typedef struct REL {		/* リロケーションテーブル */
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

#define F_INTERNALREF	0		/* 00 内部参照 */
#define	F_IMPORTORDINAL 1		/* 01 ＤＬＬ参照.エントリー序数で指定 */
#define	F_IMPORTNAME    2		/* 10 ＤＬＬ参照.エントリー名で指定 */
#define	F_OSFIXUP       3		/* 11 浮動小数点演算の命令をfixupする。 */
#define	F_ADDITIVE      4		/* 追加情報が有ることを示す。 */


#ifdef	LINUX
#define	CRLF	"\n"
#else
#define	CRLF	"\r\n"			/* 不幸なMS-DOSはテキストオープンすると遅い */
#endif							/* のでバイナリーで書くための文字列 */

char   *xstrdup(  char *s );
char   *xstrpdup( char *s );
char   *xmalloc( int size);
void	init_hash(void);
void	clr_hash(void);
void	clr_hash2(void);
void	insert_hash2(int val,int flg,char *obj);
void	insert_hash(int val,int flg,char *obj);
void	hash_report(void);
int	disasm_i386(char *buf,int start,int size,FILE *ofp);
void	spool_init(void);
void	pe_dump(void);
void	le_dump(void);
void	dump_out(FILE *fp,unsigned char *buf,int cutsize,int start);
void	code32_dump(int segnum,char *sectname,int off,int siz,int start,int size);
void	set_cpumode(int op,int ad);

/***  プロトタイプ宣言　***/
int		main(int argc,char **argv);
void 	raw386_dump(int ofs,int cpumode);
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
void	NEseek(int l);
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
void	loadbin(int off,int size);
void	cutbin(char *name,int size);
void	cuthex(char *name,int size);
void	rel_dump(int segnum,int off,int siz);
void	rel_print(REL *r);
int 	get_rword(int off);
void	rel_chain(REL *r);
void	disasmbin(char *name,int start,int size,char *hdrmsg);
void	gen_dummy_hdr(char *s,int l);
void	nrt_dump(Uchar *s);
int 	get_nrtnam(int val);
void	print_nrtname(int entnum,int seg,int off);
void	insert_label(void);
void	insert_fixup(void);
void	putrel(char *s,int ip);
void	touppers(char *s);
int		split_str(char *p,int c,char **args);

#define	ZZ	printf("%s : %d :ZZ\n",__FILE__,__LINE__);
