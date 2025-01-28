/*********************************************************
 *	�b����@�ȗ��w�b�_�[  Borland-C version. rev.2
 *********************************************************
 */
#include <stdio.h>
/*
 *	�����Ȃ��̌^���̏ȗ��`
 *	typedef�̏ꍇ�͔z���؂�Ƃ��Ƀ��[�h�A���C�����g�����̂�
 *	������#define�`�ɂ��Ă���܂��B
 */

/*
 *	�I�v�V����������`�F�b�N
 */
char  *opt[128];	/* �I�v�V�����������w�肳��Ă����炻�̕�����
			   ������������i�[�A�w��Ȃ����NULL	*/

#define Getopt(argc,argv)  \
 {int i;for(i=0;i<128;i++) opt[i]=NULL; \
   while( ( argc>1 )&&( *argv[1]=='-') ) \
    { opt[ argv[1][1] & 0x7f ] = &argv[1][2] ; argc--;argv++; } \
 }

#define IsOpt(c) ((opt[ c & 0x7f ])!=NULL)
#define   Opt(c)   opt[ c & 0x7f ]
#define ifopt(c) if((opt[ c & 0x7f ])!=NULL)

/*
 *	�Y�{����������
 */
#define Main main(int argc,char **argv){ Getopt(argc,argv);
#define End  exit(0);}

#define Seek(fp,ptr) fseek(fp,ptr,0)
#define Rseek(ptr) fseek(ifp,ptr,0)

/*
 *	�O���[�o���ϐ�
 */
FILE  *ifp;
FILE  *ofp;
FILE  *ifp2;
FILE  *ofp2;

/*
 *	�t���������i"������"�j
 */
#define Usage(string) void usage(void){printf(string);exit(1);}

/*
 *	�ȗ��^�t�@�C���A�N�Z�X
 */
#define Ropen(name) {ifp=fopen(name,"rb");if(ifp==NULL) \
{ printf("Fatal: can't open file:%s\n",name);exit(1);}  \
}

#define Wopen(name) {ofp=fopen(name,"wb");if(ofp==NULL) \
{ printf("Fatal: can't create file:%s\n",name);exit(1);}  \
}

#define Ropen2(name) {ifp2=fopen(name,"rb");if(ifp2==NULL) \
{ printf("Fatal: can't open file:%s\n",name);exit(1);}  \
}

#define Wopen2(name) {ofp2=fopen(name,"wb");if(ofp2==NULL) \
{ printf("Fatal: can't create file:%s\n",name);exit(1);}  \
}

#define Read(buf,siz)   fread (buf,1,siz,ifp)
#define Write(buf,siz)  fwrite(buf,1,siz,ofp)
#define Read2(buf,siz)  fread (buf,1,siz,ifp2)
#define Write2(buf,siz) fwrite(buf,1,siz,ofp2)

#define Rclose()  fclose(ifp)
#define Wclose()  fclose(ofp)
#define Rclose2()  fclose(ifp2)
#define Wclose2()  fclose(ofp2)

/*
 *	�g���q�t�����[�e�B���e�B
 *
 *	addext(name,"EXE") �̂悤�ɂ��Ďg��
 *	�����g���q���t���Ă����烊�l�[���ɂȂ�
 */
#ifdef  ADDEXT
void addext(char *name,char *ext)
{
	char *p,*q;
	p=name;q=NULL;
	while( *p ) {
		if ( *p == '.' ) return;
		p++;
	}
	if(q==NULL) q=p;
	/*if( (p-q) >= 4) return;  �Ȃ�bug���Ă���*/
	strcpy(q,".");
	strcpy(q+1,ext);
}

void renext(char *name,char *ext)
{
	char *p,*q;
	p=name;q=NULL;
	while( *p ) {
		if ( *p == '.' ) q=p;
		p++;
	}
	if(q==NULL) q=p;
	strcpy(q,".");
	strcpy(q+1,ext);
}

#endif



#ifdef GETLN
/*
 *	��s���̓��[�e�B���e�B
 */
int getln(char *buf,FILE *fp)
{
	int c;
	int l;
	l=0;
	while(1) {
		c=getc(fp);
		if(c==EOF)  return(EOF);/* EOF */
		if(c==0x0d) continue;
		if(c==0x0a) {
			*buf = 0;	/* EOL */
			return(0);	/* OK  */
		}
		*buf++ = c;l++;
		if(l>=255) {
			*buf = 0;
			return(1);	/* Too long line */
		}
	}
}
#endif
