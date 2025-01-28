/**********************************************************************
 *  �A�h���X�n�b�V���̊Ǘ�
 **********************************************************************
 *	POOL1 �͊e�Z�O�����g���� FIXUP�A�h���X��ێ�
 *	POOL2 ��API DATABASE�����n�b�V��
 */
#include <stdio.h>
#include "diswin.h"

typedef struct HASH {
    struct HASH *next;
    char        *obj;
    long         val;
    int          flg;
} HASH;

#define HASHSIZE   0x4000
#define POOL1SIZE  0x8000L	/* �e�Z�O�����g���� FIXUP�A�h���X���n�b�V��*/
#define POOL2SIZE  0x40000L	/* API DATABASE�����n�b�V�� */
#define HS	sizeof(HASH)
static  HASH    *entry[HASHSIZE];
static  HASH    *entry2[HASHSIZE];
static  HASH    *pool;
static  HASH    *pool2;
static  HASH    *hp;
static  HASH    *hp2;


char   *xmalloc(long size);
void	clr_hash(void);
void	clr_hash2(void);

/**********************************************************************
 *  �A�h���X�n�b�V���̏������i�ŏ��̂P�񂾂��j
 **********************************************************************
 */
void	init_hash(void)
{
    pool  = (HASH *) xmalloc( POOL1SIZE );
    pool2 = (HASH *) xmalloc( POOL2SIZE );
    clr_hash();
    clr_hash2();
}

/**********************************************************************
 *  �A�h���X�n�b�V���̏������i�N���A�j
 **********************************************************************
 */
void	clr_hash(void)
{
    int i;
    for(i=0;i<HASHSIZE;i++) entry[i]=(HASH *)NULL;
    hp=&pool[1];
}

void	clr_hash2(void)
{
    int i;
    for(i=0;i<HASHSIZE;i++) entry2[i]=(HASH *)NULL;
    hp2=&pool2[1];
}

void	hash_report(void)
{
 printf("hash pool(1) used = 0x%x/0x%lx \n",(int)(hp -pool )*HS,POOL1SIZE);
 printf("hash pool(2) used = 0x%x/0x%lx \n",(int)(hp2-pool2)*HS,POOL2SIZE);
}

static	int	calc_hash(long	val)
{
    return val & (HASHSIZE-1);
}

/**********************************************************************
 *  �}��
 **********************************************************************
 */
void	insert_hash(long val,int flg,char *obj)
{
    int   hval;

    hp->val=val;
    hp->flg=flg;
    hp->obj=obj;
    
    hval=calc_hash(val);
        hp->next = entry[hval];
    entry[hval]  = hp;
    hp++;
}

void	insert_hash2(long val,int flg,char *obj)
{
    int   hval;

    hp2->val=val;
    hp2->flg=flg;
    hp2->obj=obj;
    
    hval=calc_hash(val);
        hp2->next = entry2[hval];
    entry2[hval]  = hp2;
    hp2++;
}

/**********************************************************************
 *  ����
 **********************************************************************
 */
char *search_hash(long val,int flg)
{
    int hval;
    HASH *h;
    hval=calc_hash(val);
    h = entry[hval];
    while(h != NULL) {
        if((h->val==val)&&
           (h->flg==flg)) {
            return h->obj;
        }
        h=h->next;
    }
    return NULL;
}

char *search_hash2(long val,int flg)
{
    int hval;
    HASH *h;
    hval=calc_hash(val);
    h = entry2[hval];
    while(h != NULL) {
        if((h->val==val)&&
           (h->flg==flg)) {
            return h->obj;
        }
        h=h->next;
    }
    return NULL;
}

/**********************************************************************
 *  �o�d ��������
 **********************************************************************
 */
#if	0
char *search_hash2_long(long l)
{
	return search_hash2(l,0);
}

void  insert_hash2_long(long l,char *obj)
{
	insert_hash2(l,0,obj);
}
#endif
/**********************************************************************
 *  
 **********************************************************************
 */
