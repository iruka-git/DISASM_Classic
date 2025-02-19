/*
 *	1024バイト分の先読みデータマークを管理する
 */
#include "sh7000.h"

#define MASK 1023
static	char	markbuf[MASK+1];


/***  プロトタイプ宣言　***/
void init_mark(void);
int  get_mark(long current,long next);
int  set_mark(long current,long next,int flag);

void init_mark()
{
	int i;
	for(i=0;i<(MASK+1);i++) markbuf[i]=NOMARK;
}

int	get_mark(long current,long next)
{
	if( ( next - current )> MASK ) return NOMARK;
	return markbuf[next & MASK];
}

int	set_mark(long current,long next,int flag)
{
	if( ( next - current )> MASK ) return -1;
	markbuf[next & MASK] = flag;
	return 0;
}
