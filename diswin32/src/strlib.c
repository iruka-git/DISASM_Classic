#include  <stdio.h>
#include  <malloc.h>
#include  <string.h>
#include  <time.h>
#define SPOOLSIZE   0xff00L/* ������o�b�t�@ */


#ifdef	__LARGE__
#include  <process.h>		/* exit() */
#endif

#ifdef	FLAT
#include  <process.h>		/* exit() */
#endif

/***  �v���g�^�C�v�錾�@***/
void  spool_init(void);
char *xstrdup(char *s);
char *xstrpdup(char *s);
char *xmalloc(long size);
void  get_new_heap(void);
char *timedate_string(time_t lstamp);
char *dump_hex16(unsigned char *data,int mode);
int   printc1(int c);
int   is_kanji1(int c);
int   is_kanji2(int c);


#ifdef  LINUX
int stricmp(char *s,char *t)
{
    while(*s){
      if(toupper(*s)!=toupper(*t)) return -1;
      s++;t++;
    }
    return *t;
}
#endif

#ifdef  MINGW
int stricmp(const char *s,const char *t)
{
    while(*s){
      if(toupper(*s)!=toupper(*t)) return -1;
      s++;t++;
    }
    return *t;
}
#endif

char   *spool;                  /* ������v�[���̈�     */
char   *splp;					/* ������v�[���̈�̎g�p�|�C���^ */
/**********************************************************************
 *  
 **********************************************************************
 */
void	spool_init(void)
{
        splp=spool;
}
/**********************************************************************
 *  �b����t�H�[�}�b�g�̕�����𕶎���o�b�t�@�ɕۑ�����
 **********************************************************************
 */
char *xstrdup(char *s)
{
        char *p;

/*      printf("xstrdup(%s)=%lx",s,splp);
 */
        strcpy(splp,s);
        p=splp;
        splp+=strlen(p)+1;
        if(splp>=(spool+SPOOLSIZE-256)) {
        		get_new_heap();
        }
        return p;
}
/**********************************************************************
 *  �o�����������t�H�[�}�b�g�̕�����𕶎���o�b�t�@�ɕۑ�����
 **********************************************************************
 */
char *xstrpdup(char *s)
{
        int n;
        char *p;
        p=splp;

                        n = *s++;n&=0xff;
        while(n) {*splp++ = *s++;n--;}
        *splp++ = 0;

        if(splp>=(spool+SPOOLSIZE-256)) {
        		get_new_heap();
        }
        return p;
}

/**********************************************************************
 *  xmalloc(size)  �i�G���[�`�F�b�N�t�� farmalloc �j
 **********************************************************************
 */
char *xmalloc(long size)
{
        char *p;
#ifdef	__LARGE__
        p=farmalloc(size);if(p==NULL) {
                printf("�������[�m�ۂɎ��s���܂���.\n");
                exit(1);
        }
#else
        p=malloc(size);if(p==NULL) {
                printf("�������[�m�ۂɎ��s���܂���.\n");
                exit(1);
        }
#endif
        return p;
}

void	get_new_heap()
{
#ifdef	__LARGE__
        spool=farmalloc(SPOOLSIZE);if(spool==NULL) {
                printf("������o�b�t�@���I�[�o�[���܂���.\n");
                exit(1);
        }
#else
        spool=malloc(SPOOLSIZE);if(spool==NULL) {
                printf("������o�b�t�@���I�[�o�[���܂���.\n");
                exit(1);
        }
#endif
        splp=spool;
}

/**********************************************************************
 *  
 **********************************************************************
 */
static char buf[64];
char *timedate_string(time_t lstamp)
{
	char *p;
	p = ctime(&lstamp);
	
	strcpy(buf,"???");	//���̕�����.
	
	if(p!=NULL) {		// ctime�̖߂�l��NULL�łȂ�����.
		if(*p != 0) {	// �����񂪗�o�C�g�łȂ�����.
			strcpy(buf, p);buf[ strlen(buf)-1 ] = 0;	//�Ō�̂P���������.
		}
	}
	return &buf[0];
}


char *dump_hex16(unsigned char *data,int mode)
{
	static	int	sjis1=0;
	int i,c;
	char *p;
	unsigned long *l;
	unsigned char *b;

	p = buf;
	if(mode==1) {
		l=(unsigned long *)data;
		for(i=0;i<16;i+=4) {
			p += sprintf(p,"%08lx ",*l++);
		}
	}else{
		b=data;
		for(i=0;i<16;i++) {
			p += sprintf(p,"%02x ",*b++);
		}
	}

	for(i=0;i<16;i++) {
		c = *data++;
		if(sjis1) {		/*�������̕����������P�o�C�g��*/
			if(is_kanji2(c)) {
				*p++ = sjis1;			/* ���� */
				*p++ = c;
			}else{
				*p++ = printc1(sjis1);	/* ANK 2byte */
				*p++ = printc1(c);
			}
			sjis1=0;
		}else{
			if(is_kanji1(c)) {
				sjis1=c;
			}else{
				*p++ = printc1(c);
			}
		}
	}
	*p = 0;
	return buf;
}

int printc1(int c)
{
	if(  c< 0x20 )  c='.';
	if( (c>=0x7f ) && (c<0xa0) ) c='.';
	if( (c>=0xe0 ) ) c='.';
	return c;
}


int	is_kanji1(int c)
{
	if( (c>= 0x81)&&(c<0xa0) ) return(1);
	if( (c>= 0xe0)&&(c<0xf0) ) return(1);
	return(0);
}

int is_kanji2(int c)
{
	if( (c>= 0x40)&&(c<0x7f) ) return(1);
	if( (c>= 0x80)&&(c<0xfd) ) return(1);
	return(0);
}

/**********************************************************************
 *		������ p �� ���� c �ŕ������Aargs[] �z��ɓ����B
 *		������͕��f����邱�Ƃɒ��ӁB
 *		�󔒂��X�v���b�^�[�ɂȂ�悤�ɂ��Ă���̂Œ���
 **********************************************************************
 */
static	int	splitchr;
static	int	is_spc(int c)
{
	if(c==splitchr) return 1;
	if(c==' ') 		return 1;
	if(c=='\t') 	return 1;
	return(0);
}

int	split_str(char *p,int c,char **args)
{
	int argc=0;
	splitchr=c;


	while(1) {
		while( is_spc(*p) ) p++;
		if(*p==0) break;
		
		args[argc++]=p;		/* ���ڂ̐擪���� */

		while( *p ) {		/* ��؂蕶���܂œǂݐi�߂� */
			if(is_spc(*p))break;
			p++;
		}

		if(is_spc(*p)) {	/* ��؂蕶���ł���� */
			*p=0;p++;		/* NULL�Ő؂� */
		}
	}
	return argc;
}
