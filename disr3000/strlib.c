#include  <stdio.h>
#include  <stdlib.h>
#include  <malloc.h>
#include  <string.h>
#include  <time.h>
#define   SPOOLSIZE   0xff00L/* 文字列バッファ */
//#include  <process.h>		/* exit() */

/***  プロトタイプ宣言　***/
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

char   *spool;                  /* 文字列プール領域     */
char   *splp;					/* 文字列プール領域の使用ポインタ */
/**********************************************************************
 *  
 **********************************************************************
 */
void	spool_init(void)
{
        splp=spool;
}
/**********************************************************************
 *  Ｃ言語フォーマットの文字列を文字列バッファに保存する
 **********************************************************************
 */
char *xstrdup(char *s)
{
        char *p;

        strcpy(splp,s);
        p=splp;
        splp+=strlen(p)+1;
        if(splp>=(spool+SPOOLSIZE-256)) {
        		get_new_heap();
        }
        return p;
}
/**********************************************************************
 *  Ｐａｓｃａｌフォーマットの文字列を文字列バッファに保存する
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
 *  xmalloc(size)  （エラーチェック付の farmalloc ）
 **********************************************************************
 */
char *xmalloc(long size)
{
        char *p;
        p=malloc(size);if(p==NULL) {
                printf("メモリー確保に失敗しました.\n");
                exit(1);
        }
        return p;
}

void	get_new_heap()
{
        spool=malloc(SPOOLSIZE);if(spool==NULL) {
                printf("文字列バッファがオーバーしました.\n");
                exit(1);
        }
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
	
	strcpy(buf,"???");	//仮の文字列.
	
	if(p!=NULL) {		// ctimeの戻り値がNULLでないこと.
		if(*p != 0) {	// 文字列が零バイトでないこと.
			strcpy(buf, p);buf[ strlen(buf)-1 ] = 0;	//最後の１文字を削る.
		}
	}
	return &buf[0];
}


char *dump_hex16(unsigned char *data,int mode)
{
	static	int	sjis1=0;
	int i,c;
	char *p;
	unsigned int  *l;
	unsigned char *b;

	p = buf;
	if(mode==1) {
		l=(unsigned int *)data;
		for(i=0;i<16;i+=4) {
			p += sprintf(p,"%08x ",*l++);
		}
	}else{
		b=data;
		for(i=0;i<16;i++) {
			p += sprintf(p,"%02x ",*b++);
		}
	}

	for(i=0;i<16;i++) {
		c = *data++;
		if(sjis1) {		/*さっきの文字が漢字１バイト目*/
			if(is_kanji2(c)) {
				*p++ = sjis1;			/* 漢字 */
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
 *		文字列 p を 文字 c で分割し、args[] 配列に入れる。
 *		文字列は分断されることに注意。
 *		空白もスプリッターになるようにしているので注意
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
		
		args[argc++]=p;		/* 項目の先頭文字 */

		while( *p ) {		/* 区切り文字まで読み進める */
			if(is_spc(*p))break;
			p++;
		}

		if(is_spc(*p)) {	/* 区切り文字であれば */
			*p=0;p++;		/* NULLで切る */
		}
	}
	return argc;
}
