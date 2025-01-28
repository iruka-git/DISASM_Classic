/**********************************************************************
 *  �c�h�r���W���[������Ăяo�����R���[�`��
 **********************************************************************
 */
#include <stdio.h>
#include "diswin.h"

#define	CHK_EXTERNSYM	1


#if	CHK_EXTERNSYM
extern  int     segnum;                 /* �Z�O�����g�ԍ�       */
extern  int     filtermode;             /*                      */
extern  char   *modname[];              /* �Q�Ɩ�(string table) */
extern  char   *myself;                 /* �������������O       */
extern  char   *strtable;               /* �Q�Ɩ��e�[�u��       */
extern	int     adsize32def;

char *search_hash( long val,int flg);
char *search_hash2(long val,int flg);
int		search_api(char *s,int ordinal,char *namebuf);
void	sprintstr(char *t,char *s);

char *osfixuptbl[]={
        "?0",
        "FIARQQ, FJARQQ",
        "FISRQQ, FJSRQQ",
        "FICRQQ, FJCRQQ",
        "FIERQQ",
        "FIDRQQ",
        "FIWRQQ",
        "?7"
};

char   *sputs(char *p,char *s);

/**********************************************************************
 *  ���x������������\������i�t�@�C���ɒ��ڏo�͂���F�j
 **********************************************************************
 */
int	win_printlabel(long off,FILE *ofp)
{
    NRT *p;
    char *s;

    if(filtermode==0) return 0;

	if(adsize32def) {
        s=search_hash2(off,0);
        if(s!=NULL) {
                fprintf(ofp,"                %s:" CRLF,s);
                return 1;
        }
  		return 0;
	}else{
        p=(NRT *)search_hash2(off,segnum);
        if(p!=NULL) {
                fprintf(ofp,"                %s:" CRLF,p->name);
                return 1;
        }
  		return 0;
	}
}

/**********************************************************************
 *  �I�y�����h���O���V���{���ł���΁A���̖��O��Ԃ��B
 **********************************************************************
 */
int	win_printrel(off,buf)
char *buf;
{
        REL *r;
	char *bufp;
        char *modulename;
        char  entryname[128];

	*buf=0;
        if(filtermode==0) return 0;
		if(adsize32def)   return 0;
        
	bufp=buf;
        
        r=(REL *)search_hash(off,256);

        if(r!=NULL) {
/*            if(r->type != 3) return 0;   ** FAR_ADDR type�Ɍ��� */
	    switch(r->type) {
	     case T_LOBYTE  :
		bufp = sputs(bufp,"low ");
	     	break;
	     case T_SEGMENT :
		bufp = sputs(bufp,"seg ");
	     	break;
	     case T_OFFSET  : /*(16-bit offset)*/
		bufp = sputs(bufp,"offset ");
	     	break;
	     case T_FAR_ADDR: /*(32-bit pointer)*/
	     	break;
	     default:
	     	break;
	    }


            switch(r->flag & 3) {
                /************************************************/
                 case F_INTERNALREF   :
			if(r->type == T_SEGMENT) {
                	    sprintf(buf ,"seg_%02d"
                                ,r->i.i.segnum  /* �����Z�O�����g�ԍ� */
                	    );
			}else{
                	    sprintf(bufp,"seg_%02d:%04x"
                                ,r->i.i.segnum  /* �����Z�O�����g�ԍ� */
                                ,r->i.i.off             /* �����Z�O�����g�I�t�Z�b�g */
	                    );
        	        }
                        break;

                /************************************************/
                 case F_IMPORTORDINAL :
            entryname[0]=0;
            
            if(r->i.o.idx==0xff) {
                        modulename = myself;
            }else{
                        modulename = modname[r->i.o.idx-1];
                        search_api(modulename,r->i.o.ordinal,entryname);
            }

            if(entryname[0]) {
                sprintf(bufp,"%s.%d (%s)"
                ,modulename     /* �Q�ƃ��W���[���� */
                ,r->i.o.ordinal /* �G���g���[�̏��� */
                ,entryname
                );
            }else{
                sprintf(bufp,"%s.%d"
                ,modulename     /* �Q�ƃ��W���[���� */
                ,r->i.o.ordinal /* �G���g���[�̏��� */
                );
            }
                        break;

                /************************************************/
                 case F_IMPORTNAME    :

            if(r->i.o.idx==0xff) {
                        modulename = myself;
            }else{
                        modulename = modname[r->i.o.idx-1];
            }
                        sprintstr(entryname,&strtable[r->i.n.off]);
                sprintf(bufp,"%s.%s"
                                ,modulename     /* �Q�ƃ��W���[���� */
                                ,entryname
                        );
                break;
                /************************************************/
                 case F_OSFIXUP       :
                sprintf(bufp,"(OSFIXUP %s)",osfixuptbl[r->i.f.type]);
                break;
                }
        }
        return buf[0];
}

#else	/* CHK_EXTERNSYM */

/**********************************************************************
 *  EXTERNAL SYMBOL���������Ȃ��ꍇ
 **********************************************************************
 */

int	win_printlabel() {return 0;}
int	win_printrel()	 {return 0;}

#endif	/* CHK_EXTERNSYM */

