/**********************************************************************
 *  �k�d�`���̃t�@�C�����_���v����B
 **********************************************************************
 */
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include "diswin.h"
#include "pedump.h"


/***  �v���g�^�C�v�錾�@***/
void	pe_dump(void);
void	dump_out(FILE *fp,unsigned char *buf,long cutsize,long start);
void	code32_dump(int segnum,char *sectname,long off,long siz,long start,long size);
void	data32_cut(int segnum,char *sectname,long off,long siz,long start,long size);
void	data32_dump(int segnum,char *sectname,long off,long siz,long start,long size);
void	section_dump(void);
int		hint_dump(long rva,long size,long offset);
int		imports_dump(long offset,long size,long vadrs);
int		exports_dump(long offset,long size,long vadrs);
void	imp_exp_find(int f);

void	set_cpumode(int op,int ad);
void	set_exemode(int mode);


char	*xstrdup(char *s);
char    *timedate_string(long lstamp);


#define Read(buf,siz)   fread (buf,1,siz,ifp)
#define Rseek(ptr) fseek(ifp,ptr,0)
#define IsOpt(c) ((opt[ c & 0x7f ])!=NULL)
#define   Opt(c)   opt[ c & 0x7f ]
#define ifopt(c) if((opt[ c & 0x7f ])!=NULL)

#define	ASMCUTS 0x8000L

/**********************************************************************
 *  �d����������������
 **********************************************************************
 */
extern  FILE  *ifp;
extern  FILE  *ofp;
extern	char  *vbuf;
extern  char  *loadbuf;         /* �o�C�i���[�����[�h����o�b�t�@ */
extern	char  *opt[128];	/* �I�v�V�����������w�肳��Ă����炻�̕�����*/
			        /* ������������i�[�A�w��Ȃ����NULL	*/
extern	char   srcname[];
extern	char   binname[];
extern	char   hexname[];
extern	char   inpname[];
extern	int	   quietmode;
void NEseek(long);
void loadbin(long off,long size);
long disasm(char *buf,long start,long size,FILE *ofp);
void gen_dummy_hdr(char *s,long l);

extern	PE_HDR   pe;    /* PE�`���̃w�b�_�[   */

/**********************************************************************
 *  �v������
 **********************************************************************
 */
typedef struct OBJT {
    long   virtual_size;
    long   rel_base;
    long   flag;
    long   pagemap_idx;
    long   pagemap_ent;
    long   rsv;
} OBJT;



void NEread(long offset,void *buf,long len)
{
	NEseek(offset);
	Read(buf,len);
}
/**********************************************************************
 *  �k�d�@�c�t�l�o���C��
 **********************************************************************
 */



void	le_dump(void)
{
	OBJT	objt;
	long	le_pagesize;
	long	le_datapage;
	long	le_objt;
	long	le_objcnt;
	long	le_pagemap;
	long	offset;
	int i;

	NEread(0x0028L,&le_pagesize,sizeof(long) );
	NEread(0x0080L,&le_datapage,sizeof(long) );
	NEread(0x0040L,&le_objt    ,sizeof(long) );
	NEread(0x0044L,&le_objcnt  ,sizeof(long) );
	NEread(0x0048L,&le_pagemap ,sizeof(long) );
	
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
      	
	    set_cpumode(1,1);	/* �A�h���X�A�I�y�����h�T�C�Y���ɂR�Q������ */
		set_exemode(1);		/* LE MODE */
        code32_dump(i+1,"LE_CODE"       /*�Z�N�V������ */
                   ,offset				/*�t�@�C���̐擪����̃I�t�Z�b�g*/
                   ,objt.virtual_size	/* �u���b�N�T�C�Y */
                   ,objt.rel_base		/* ���z�A�h���X*/
                   ,objt.virtual_size	/* ���z�T�C�Y */
        );
	  }
		le_objt += sizeof(objt);
	}
	
}