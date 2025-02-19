/**********************************************************************
 *	�o�b�`�̑f
 **********************************************************************
 */
#include  <alloc.h>
#include  <string.h>

#define   ADDEXT  1
#define   GETLN   1
#include  "std.h"

#define SPOOLSIZE   0xff00L/* ������o�b�t�@ */
#define	MAXSCR	1024
char	*script[MAXSCR];
int		maxscript=0;
extern	char   *spool;                  /* ������v�[���̈�     */
char *xstrdup(char *s);
char *xmalloc(int size);
char	buf[256];
char	body[256];

/**********************************************************************
 *  �g����
 **********************************************************************
 */
Usage(
	"*** �o�b�`�̑f ***\n"
	"�g����:\n"
	"> mkbat  script  filelist >batfile.bat\n"
	"# filelist�ɑ΂��ăX�N���v�g�����s����悤�ȃo�b�`�𐶐�����\n"
	"  �X�N���v�g���ňȉ��̃}�N�����g����\n"
	"  $$  =  filelist�ɋL�q���ꂽ�t�@�C�����̂���\n"
	"  $*  =  filelist�ɋL�q���ꂽ�t�@�C������g���q���Ȃ�������\n"
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
