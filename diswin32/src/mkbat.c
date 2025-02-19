/**********************************************************************
 *	バッチの素
 **********************************************************************
 */
#include  <alloc.h>
#include  <string.h>

#define   ADDEXT  1
#define   GETLN   1
#include  "std.h"

#define SPOOLSIZE   0xff00L/* 文字列バッファ */
#define	MAXSCR	1024
char	*script[MAXSCR];
int		maxscript=0;
extern	char   *spool;                  /* 文字列プール領域     */
char *xstrdup(char *s);
char *xmalloc(int size);
char	buf[256];
char	body[256];

/**********************************************************************
 *  使い方
 **********************************************************************
 */
Usage(
	"*** バッチの素 ***\n"
	"使い方:\n"
	"> mkbat  script  filelist >batfile.bat\n"
	"# filelistに対してスクリプトを実行するようなバッチを生成する\n"
	"  スクリプト内で以下のマクロが使える\n"
	"  $$  =  filelistに記述されたファイルそのもの\n"
	"  $*  =  filelistに記述されたファイルから拡張子を省いたもの\n"
)

Main
	Getopt(argc,argv);
	if(argc<3) usage();
	
	spool = xmalloc( SPOOLSIZE );
	spool_init();
	
	getscript(argv[1]);
	applyscript(argv[2]);
End



getscript(char *s)
{
	FILE *ifp;
	
	Ropen(s);
	maxscript=0;
	while(getln(buf,ifp)!=EOF) {
		script[maxscript++]=xstrdup(buf);
	}
	Rclose();
}

applyscript(char *s)
{
	FILE *ifp;
	
	Ropen(s);
	while(getln(buf,ifp)!=EOF) {
		if(*buf) {
			strcpy(body,buf);
			cutext(body);
			apply1file();
		}
	}
	Rclose();
}

apply1line(char *s)
{
	while(	*s) {
		if(	*s=='$') {
			if(s[1]=='$') {
				s++;printf("%s",buf);
			}else
			if(s[1]=='*') {
				s++;printf("%s",body);
			}
		}else{
			putchar(*s);
		}
		s++;
	}
	printf("\n");
}

apply1file()
{
	int i;
	for(i=0;i<maxscript;i++) {
		apply1line(script[i]);
	}
}


cutext(char *s)
{
	while(*s) {
		if(*s=='.') *s=0;
		s++;
	}
}
