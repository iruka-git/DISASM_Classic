/**********************************************************************
 *  ＬＥ形式のファイルをダンプする。
 **********************************************************************
 */
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include "diswin.h"
#include "pedump.h"


/***  プロトタイプ宣言　***/
void	pe_dump(void);
void	dump_out(FILE *fp,unsigned char *buf,int cutsize,int start);
void	code32_dump(int segnum,char *sectname,int off,int siz,int start,int size);
void	data32_cut(int segnum,char *sectname,int off,int siz,int start,int size);
void	data32_dump(int segnum,char *sectname,int off,int siz,int start,int size);
void	section_dump(void);
int		hint_dump(int rva,int size,int offset);
int		imports_dump(int offset,int size,int vadrs);
int		exports_dump(int offset,int size,int vadrs);
void	imp_exp_find(int f);

void	set_cpumode(int op,int ad);
void	set_exemode(int mode);


char	*xstrdup(char *s);
char    *timedate_string(int lstamp);


#define Read(buf,siz)   fread (buf,1,siz,ifp)
#define Rseek(ptr) fseek(ifp,ptr,0)
#define IsOpt(c) ((opt[ c & 0x7f ])!=NULL)
#define   Opt(c)   opt[ c & 0x7f ]
#define ifopt(c) if((opt[ c & 0x7f ])!=NULL)

#define	ASMCUTS 0x8000L

/**********************************************************************
 *  Ｅｘｔｅｒｎａｌｓ
 **********************************************************************
 */
extern  FILE  *ifp;
extern  FILE  *ofp;
extern	char  *vbuf;
extern  char  *loadbuf;         /* バイナリーをロードするバッファ */
extern	char  *opt[128];	/* オプション文字が指定されていたらその文字に*/
			        /* 続く文字列を格納、指定なければNULL	*/
extern	char   srcname[];
extern	char   binname[];
extern	char   hexname[];
extern	char   inpname[];
extern	int	   quietmode;
void NEseek(int);
void loadbin(int off,int size);
int disasm(char *buf,int start,int size,FILE *ofp);
void gen_dummy_hdr(char *s,int l);

extern	PE_HDR   pe;    /* PE形式のヘッダー   */

/**********************************************************************
 *  Ｗｏｒｋ
 **********************************************************************
 */
typedef struct OBJT {
    int   virtual_size;
    int   rel_base;
    int   flag;
    int   pagemap_idx;
    int   pagemap_ent;
    int   rsv;
} OBJT;



void NEread(int offset,void *buf,int len)
{
	NEseek(offset);
	Read(buf,len);
}
/**********************************************************************
 *  ＬＥ　ＤＵＭＰメイン
 **********************************************************************
 */



void	le_dump(void)
{
	OBJT	objt;
	int	le_pagesize;
	int	le_datapage;
	int	le_objt;
	int	le_objcnt;
	int	le_pagemap;
	int	offset;
	int i;

	NEread(0x0028L,&le_pagesize,sizeof(int) );
	NEread(0x0080L,&le_datapage,sizeof(int) );
	NEread(0x0040L,&le_objt    ,sizeof(int) );
	NEread(0x0044L,&le_objcnt  ,sizeof(int) );
	NEread(0x0048L,&le_pagemap ,sizeof(int) );
	
	printf("LE format\n");

	for(i=0;i<le_objcnt;i++) {
		NEread(le_objt, &objt ,sizeof(objt) );

		offset = le_datapage + (objt.pagemap_idx-1) * le_pagesize;

		printf("object table %d\n",i);
		printf("     virtual size    = 0x%lx\n",objt.virtual_size);
		printf("     relocation base = 0x%lx\n",objt.rel_base);
		printf("     file offset     = 0x%lx\n",offset);

      ifopt('s') {
      	pe.opt.ImageBase           =objt.rel_base;
      	
	    set_cpumode(1,1);	/* アドレス、オペランドサイズ共に３２ｂｉｔ */
		set_exemode(1);		/* LE MODE */
        code32_dump(i+1,"LE_CODE"       /*セクション名 */
                   ,offset				/*ファイルの先頭からのオフセット*/
                   ,objt.virtual_size	/* ブロックサイズ */
                   ,objt.rel_base		/* 仮想アドレス*/
                   ,objt.virtual_size	/* 仮想サイズ */
        );
	  }
		le_objt += sizeof(objt);
	}
	
}
