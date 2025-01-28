/*
 *	シンボルテーブルの管理
 *
 *	struct SYMBOL {
 *		long      adrs;	//番地
 *		char far *name;	//名前
 *	} SYMBOL;
 *
 *	init_symbol();							初期化
 *	reg_symbol(*symbol);					登録
 *	find_symbol_by_adrs(*symbol,address);	番地による検索
 *	find_symbol_by_name(*symbol,*name  );	名前による検索
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include <malloc.h>
#include "symbol.h"


/***  プロトタイプ宣言　***/
char far *segalloc(long size);
void init_symbol(void);
int adrs_hash(long adrs);
int name_hash(char far *name);
void entsym_by_adrs(SYMBOL far *symbol);
void entsym_by_name(SYMBOL far *symbol);
void reg_symbol(SYMBOL *symbol);
int find_symbol_by_adrs(SYMBOL *symbol,long address);
int find_symbol_by_name(SYMBOL *symbol,char *name );


SYMBOL far *symbuf;
char   far *namebuf;
SYMBOL far * far *namehash;
SYMBOL far * far *adrshash;

char   far *nameptr;
SYMBOL far *symptr;

#define	HASHMASK	0x3fff

char far *segalloc(long size)
{
	char far *p;
	char far *r;
	p=farmalloc(size+16L);
	if(p==NULL) {
		printf("fatal: can't alloc memory!\n");
		exit(1);
	}
	r=MK_FP(FP_SEG(p),0);
	return r;
}

void init_symbol()
{
	int i;
	symbuf  =(SYMBOL far *) segalloc(0x10000L);
	namebuf =(char   far *) segalloc(0x10000L);
	namehash=(SYMBOL far * far *) segalloc(0x10000L);
	adrshash=(SYMBOL far * far *) segalloc(0x10000L);

	nameptr =namebuf;
	symptr  =symbuf ;
	for(i=0;i<0x4000;i++) namehash[i]=NULL;
	for(i=0;i<0x4000;i++) adrshash[i]=NULL;
}

int	adrs_hash(long adrs)
{
	int h,m,l;
	h = adrs >> 28L;
	m = adrs >> 14L;
	l = adrs ;
	return (h+m+l) & HASHMASK;
}

int	name_hash(char far *name)
{
	int i;
	i=0;
	while(*name) {
		i *= 5;i += (*name++);
	}
	return (i & HASHMASK);
}

void entsym_by_adrs(SYMBOL far *symbol)
{
	int h;
	h = adrs_hash(symbol->adrs);
	while(adrshash[h]!=NULL) {h++;h&=HASHMASK;}
	adrshash[h] = symbol;
}

void entsym_by_name(SYMBOL far *symbol)
{
	int h;
	h = name_hash(symbol->name);
	while(namehash[h]!=NULL) {h++;h&=HASHMASK;}
	namehash[h] = symbol;
}

void reg_symbol(SYMBOL *symbol)
{
	symptr->adrs = symbol->adrs;
	symptr->name = nameptr;
		_fstrcpy(nameptr,symbol->name);
	nameptr += _fstrlen(symbol->name) + 1;
	entsym_by_adrs(symptr);
	entsym_by_name(symptr);
	symptr++;
}



/*
;	番地による検索
 */
int find_symbol_by_adrs(SYMBOL *symbol,long address)
{
	int h;
	h = adrs_hash(address);
	while( adrshash[h]!=NULL) {
		if(adrshash[h]->adrs == address) {
			*symbol = *adrshash[h];
			return 1;
		}
		h++;h&=HASHMASK;
	}
	return 0;
}
/*
;		名前による検索
 */
int find_symbol_by_name(SYMBOL *symbol,char *name )
{
	int h;
	h = name_hash(name);
	while( namehash[h]!=NULL) {
		if(_fstrcmp(namehash[h]->name,name) == 0) {
			*symbol = *namehash[h];
			return 1;
		}
		h++;h&=HASHMASK;
	}
	return 0;
}
