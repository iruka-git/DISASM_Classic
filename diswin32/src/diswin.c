/**********************************************************************
 *  �c�h�r�v�h�m�i�v�������������p�t�A�Z���u���j
 **********************************************************************
 *  Borland-C++ 4.0J/3.1 or Turbo-C 2.0  LARGE MODEL 
 *    Compile option= -O -ml -w-rvl -w-pro
 *
 *  �g�����F
 *      DISWIN -Option filename[.EXE .DLL]
 *  �I�v�V�����F
 *   -b    �Z�O�����g�����ꂽ�o�C�i���[���o�͂���
 *   -s    �t�A�Z���u���\�[�X�t�@�C�����o�͂���
 *   -u    �t�A�Z���u�����X�g��啶���ŏo��
 *   -t    �G���g���[�e�[�u����\�����郂�[�h
 *   -r    �����P�[�V��������\�����郂�[�h
 *   -x    �d�w�o�n�q�s���ꂽ�G���g���[�̃��X�g�̂ݕ\������
 *   -v    verbose mode
 *
 *  ���ӁF
 *      ���[�W���f�����O��.
 */
#define   ADDEXT  1
#define   GETLN   1

#include  <malloc.h>
#include  <string.h>
#include  <ctype.h>
#include  "std.h"
#include  "diswin.h"

#ifdef	GO32
char environs[]="go32";
#endif

#ifdef	__LARGE__
#include  <process.h>		/* exit() */
char environs[]="DOS Generic";
#endif

#ifdef	FLAT
#include  <process.h>		/* exit() */
char environs[]="bcc32 Win32 Console";
#endif

#ifdef	MINGW
char environs[]="Mingw Win32 Console";
#endif

#ifdef	MSVC
char environs[]="MSVC++ Win32 Console";
#endif

#ifdef  LINUX
char environs[]="Linux";
#endif

char versions[] = "0.19";


/**********************************************************************
 *  �o�b�t�@�T�C�Y���̒�`
 **********************************************************************
 */
#define REFSIZE     128    /* �Q�Ɩ��e�[�u�� */
#define NRTMAX      1024   /* ��풓���e�[�u��*/
#define NRTSIZE     (NRTMAX*sizeof(NRT) )    /* ��풓���e�[�u��*/
#define CUTSIZE     0x2000 /* �o�C�i���[�J�b�^�[���g�p����o�b�t�@ */
#define LOADSIZE    0x4000 /* �o�C�i�����[�_����x�Ƀ��[�h�\�ȃT�C�Y */
#define RELSIZE     0xc000L/* �����P�[�V���������L������o�b�t�@ */
#define SPOOLSIZE   0xff00L/* ������o�b�t�@ */
#define	API_DBMAX	20000   /* diswin.api �̓o�^�� */

#define API_DBSIZE (sizeof(API_DB)*API_DBMAX)/* API Database �o�b�t�@ */
#define RELMAX     (RELSIZE/sizeof(REL))/* �����P�[�V�����o�b�t�@�̍ő�� */
#define ifnq        if(quietmode==0)
/**********************************************************************
 *  ���L���[�N�G���A
 **********************************************************************
 */
EXE_OLD_HDR oldhdr;             /* OLD EXE Header       */
EXE_NEW_HDR ne;                 /* Windows EXE Header   */
SEGTABLE    segt[128];           /* SEGMENT TABLE        */

char    inpname[80];            /* ���̓t�@�C����       */
char    mapname[80];            /* ���̓t�@�C����       */
char   *cutbuf;                 /* �o�C�i���[�J�b�^�̃o�b�t�@ */
extern	char   *spool;          /* ������v�[���̈�     */
extern	char   *splp;			/* ������v�[���̈�̎g�p�|�C���^ */
Ushort  refname[REFSIZE];       /* �Q�Ɩ�(file offset)  */
char   *modname[REFSIZE];       /* �Q�Ɩ�(string table) */
char   *strtable;				/* ������e�[�u��(2048byte) */
API_DB *api_db;					/* API Database�e�[�u���̃A�h���X */
int     api_dbmax=0;			/* API Database�e�[�u���̓o�^�� */
char   *api_module[256];        /* API Database �̃��W���[�����e�[�u�� */
int     api_modmax;        		/* API Database �̃��W���[�������� */


int     segnum;                 /* �Z�O�����g�ԍ�       */
NRT     *nrt;					/* ��풓�����L������e�[�u�� */
int     nrtmax=0;               /* ��풓�����L������e�[�u���̗v�f�� */

char    nulstr[]="";            /* �m�t�k�k������ */
char   *myself="(unknown)";     /* �������������O */
char   *loadbuf;                /* �o�C�i���[�����[�h����o�b�t�@ */
REL    *relbuf;                 /* �����P�[�V��������ێ� */
char   *vbuf;					/* setvbuf()���g���ē��o�͂������� */
int     relmax;					/* Win16:�����P�[�V�������̃o�C�g�� */
int     uppermode=0;			/* 1:�S�đ啶���ŏo�͂���       */
int     quietmode=0;			/* 1:�Ïl 16:16only 32:32only	*/
int     filtermode=1;			/* 1:�\�[�X�����₷�����鏈�������� */
int     commentmode=1;			/* 1:�K���ȃR�����g������     */
char    apiname[80]="DISWIN.API";	/* API�f�[�^�x�[�X�̃t�@�C���� */
char   *search_hash(long val,int flg);
char   *search_hash2(long val,int flg);

char    srcname[64]="seg_";		/* �t�A�Z���u�����X�g    �̃t�@�C����prefix */
char    binname[64]="seg_";		/* �Z�O�����g�����t�@�C���̃t�@�C����prefix */
char    hexname[64]="seg_";		/* �P�U�i�_���v���X�g    �̃t�@�C����prefix */

/**********************************************************************
 *  �֐��v���g�^�C�v
 **********************************************************************
 */

/**********************************************************************
 *  �g����
 **********************************************************************
 */
void usage(void) {
	printf(
        "*****  DisWin Ver %s (%s)  *****\n",
			versions,
			environs
        );
	printf(
        "Usage: diswin -option filename[.exe .dll]\n"
        "option:\n"
        "    -b<name>  generate binary segment\n"
        "    -s<name>  generate source segment\n"
        "    -h<name>  generate hex dump (.data section)\n"
        "      -l      (long word dump)\n"
        "    -u    list upper case\n"
        "    -t    display entry table\n"
        "    -r    display relocation table\n"
        "    -x    display only export table\n"
        "    -x16    display only win16 export table\n"
        "    -x32    display only win32 export table\n"
        "    -c<adrs> input 80286 binary code\n"
        "    -d<adrs> input 80386 binary code\n"
        "    -e    input old style msdos-exe code\n"
        "    -v    verbose\n"
        "    -n    supress comment\n"
	);
	exit(1);
}

/**********************************************************************
 *  ��������
 **********************************************************************
 */
int		main(int argc,char **argv)
{
        strcpy(apiname,argv[0]);
        renext(apiname,"api");

        Getopt(argc,argv);

        if(argc<2) usage();

        cutbuf  =           xmalloc( CUTSIZE   );
        vbuf    =           xmalloc( VBUFSIZE  );
        spool   =           xmalloc( SPOOLSIZE );
        loadbuf =           xmalloc( 0x10000L  );
        strtable=           xmalloc( 2048      );
        relbuf  = (REL    *)xmalloc( RELSIZE   );
        api_db  = (API_DB *)xmalloc( API_DBSIZE);
        nrt     = (NRT    *)xmalloc( NRTSIZE   );

        spool_init();
        init_hash();

        strcpy(inpname,argv[1]);addext(inpname,"exe");
        strcpy(mapname,argv[1]);renext(mapname,"map");

        ifopt('f') filtermode=0;
        ifopt('u') uppermode=1;
        ifopt('n') commentmode=0;
        ifopt('x') {
        	quietmode=1;
			sscanf(Opt('x'),"%d",&quietmode);
        }
        ifopt('s') {
                if(*Opt('s')) strcpy(srcname,Opt('s'));
                load_apilist(apiname);	/* DISWIN.API��ǂݍ���*/
        }
        ifopt('b') {
                if(*Opt('b')) strcpy(binname,Opt('b'));
        }
        ifopt('h') {
                if(*Opt('h')) strcpy(hexname,Opt('h'));
        }
        Ropen(inpname);
        ifopt('c') {
        		long ofs=0;
        		sscanf(Opt('c'),"%lx",&ofs);
        		raw386_dump(ofs,286);
        		Rclose();
		        exit(0);
        }
        ifopt('d') {
        		long ofs=0;
        		sscanf(Opt('d'),"%lx",&ofs);
        		raw386_dump(ofs,386);
        		Rclose();
        		exit(0);
        }
        ifopt('e') {
                MZchk();    /* OLD EXE HEADER��ǂݏo��     */
        		old_exe_dump();
        		Rclose();
        		exit(0);
        }
        
        MZchk();        /* OLD EXE HEADER��ǂݏo��         */
        NEload();       /* Windows New EXE HEADER��ǂݍ��� */
		if( ne.ne[1] != 'E' ) {
			printf("fatal:unknown executable format!\n");
			exit(1);
		}
		
        if( ne.ne[0]=='P') {
            pe_dump();  /* Windows PE �`��(Win32)           */
        }else
        if( ne.ne[0]=='L') {
            le_dump();  /* Windows LE �`��(VxD)             */
        }else{
		  if(quietmode!=32)
            ne_dump();  /* Windows NE �`��(Win16)           */
        }

        Rclose();

#if	0
		ifnq {
	        sys_report();
    	    hash_report();
    	}
#endif
        return 0;
}

void	raw386_dump(long ofs,int cpumode)
{
	long filesize=0;

	while( getc(ifp) != EOF ) filesize++;	/*���񂵂ȕ��@�Ńt�@�C���T�C�Y�𓾂�..*/
	Rseek(0);
	if(cpumode!=386){
		set_cpumode(0,0);
	}else{
		set_cpumode(1,1);
	}
	code32_dump(1,"code",0,filesize,ofs,filesize);
}

void	old_exe_dump(void)
{
	long filesize;
	long exeofs;

	exeofs = oldhdr.reloc_size;
	exeofs <<= 4;

	filesize = oldhdr.pages;
	filesize <<= 9;		/* 1Page = 512byte */

	if(oldhdr.fragment) {
		filesize -= ( 0x200 - oldhdr.fragment  );
	}

	set_cpumode(0,0);
	code32_dump(1,"code",exeofs,filesize,0,filesize);
}

void	ne_dump()
{
        NEchk();        /* Windows EXE HEADER ��ǂݍ��݃`�F�b�N        */
        SEGTload();     /* �Z�O�����g�e�[�u����ǂݍ���                 */
        MODref();       /* �Q�Ɩ��e�[�u����ǂݍ���                     */
        ENTdump();      /* �G���g���[�e�[�u����\������                 */
        insert_label(); /* �G���g���[���x�����n�b�V���ɓo�^����         */
        load_exemap(mapname);/* *.MAP�t�@�C�������݂���΂����ǂݍ��� */
        ifnq SEGTdump();/* �Z�O�����g�e�[�u����\������                 */
}

void	sys_report()
{
#ifdef	__LARGE__
        printf("\n");
        printf("spool used=0x%x bytes\n", splp-spool );
        printf("core left =%ld bytes\n",farcoreleft() );
#endif
}

void	touppers(char *s)
{
        if(uppermode) {
                while(*s) {*s=toupper(*s);s++;}
        }
}

static	int	str_cmpn(char *s,char *t)
{
	int n = strlen(t);
	return strncmp(s,t,n);
}

void	load_exemap(s)
char *s;
{
        char buf[255];
        int  flg=0;
        FILE *fp;

        fp=fopen(s,"rb");
        if(fp==NULL) return;

        while(getln(buf,fp)!=EOF) {
			if(str_cmpn(buf,"  Address         Publics by Value              Rva+Base")==0) flg=2;
			else
            if(str_cmpn(buf,"  Address         Publics by Value")==0) flg=1;

            if( buf[0]=='L') flg=0;
            
            if((flg!=0)&&(buf[0]==' ')&&(buf[5]==':')) {
				if(flg==2) {
	                read_exemap32(buf);
	      		}else{
	                read_exemap(buf);
	      		}
            }
			//printf("%d %s\n",flg,buf);
        }
        fclose(fp);

}

void	read_exemap(s)
char *s;
{
        int seg,off,c;
        char *str;

        c=0;
        sscanf(s+1,"%x%c%x",&seg,&c,&off);

        if( (c==':')&&(seg!=0) ) {
                api_db[api_dbmax].module_id= 0;
                api_db[api_dbmax].ordinal  = 0;
                api_db[api_dbmax].name     = xstrdup( s+17 );
                
                insert_hash2(off,seg,(char *)&api_db[api_dbmax]);
				ifopt('Z') printf("map %d:%04x %s [%d]\n",seg,off,api_db[api_dbmax].name );
                api_dbmax++;
                if(api_dbmax >= API_DBMAX) {
    	            printf("fatal:API database full (map file)\n");
	                exit(1);
                }
        }
}

//
//	��������󔒋�؂�ŕ�������.
//
static	char *exemap_symcut(char *buf,int *off)
{
	char cutbuf[1024];
	char *args[256];
	int	  argn;

	strcpy(cutbuf,buf);
	argn = split_str(cutbuf,' ',args);

	if(argn<2) return "";

	if(strlen(args[1]) >= 32) { args[1][32] = 0; }	//�����񒷂���32byte�ɐ���.

	if(argn>=3) {
		sscanf(args[2],"%x",off);
	}

	return strdup(args[1]);	//�Q�Ԗڂ̕�����=symbol�� ��Ԃ�.
}

//
//	PE���[�h�ł�map�t�@�C���ǂݍ���.
//
void	read_exemap32(s)
char *s;
{
        int seg,off,c;
        char *str;

        c=0;
        sscanf(s+1,"%x%c%x",&seg,&c,&off);

        if( (c==':')&&(seg!=0) ) {
                insert_hash2(off,0,(char *) exemap_symcut(s,&off));
                
				ifopt('Z') printf("map %d:%04x %s\n",seg,off,api_db[api_dbmax].name );
                api_dbmax++;
                if(api_dbmax >= API_DBMAX) {
    	            printf("fatal:API database full (map file)\n");
	                exit(1);
                }
        }
}


void	load_apilist(char *s)
{
        char buf[255];
        FILE *fp;

        fp=fopen(s,"rb");
        if(fp==NULL) return;

        while(getln(buf,fp)!=EOF) {
            if((buf[0]==' ')&&(buf[1]!=' ')) {
                read_apilist(buf+1);
            }
        }
        fclose(fp);
}

void	read_apilist(char *s)
{
        char *modname;
        char *ordstr;
        char *entname;
        int   ordinal;
        
        modname = s;
        while(*s) { if(*s=='.') break; s++;}
        if(*s!='.') return;
        *s++ = 0;

        ordstr = s;
        while(*s) { if(*s=='\t') break; s++;}
                    if(*s!='\t') return;
        *s++ = 0;
        sscanf(ordstr,"%d",&ordinal);
        
        while(*s) { if(*s!='\t') break; s++;}   /* �^�u�X�L�b�v */
        entname = s;
        
        if( (*s!=0)&&(*s!='(') )
                insert_api(modname,ordinal,entname);
}

int		api_id(char *s)
{
        if(api_modmax!=0) {
                if(strcmp(s,api_module[api_modmax-1])==0) {
                        return api_modmax-1;
                }
        }

        api_module[api_modmax++] = xstrdup( s );
        if(api_modmax>=256) {
                printf("fatal:API database full (module name)\n");
                exit(1);
        }
        return api_modmax-1;
}

void	insert_api(char *modname,int ordinal,char *entname)
{
        api_db[api_dbmax].module_id= api_id( modname );
        api_db[api_dbmax].ordinal  = ordinal;
        api_db[api_dbmax].name     = xstrdup( entname );
        insert_hash2( ordinal
                ,257+api_db[api_dbmax].module_id
                ,(char *)   &api_db[api_dbmax]
        );
ifopt('a')
printf("api %s.%d %s [%d]\n",modname,ordinal,entname,api_db[api_dbmax].module_id);

        api_dbmax++;
        if(api_dbmax >= API_DBMAX) {
            printf("fatal:API database full (entry max)\n");
            exit(1);
        }
}

int		search_api(char *s,int ordinal,char *namebuf)
{
        int i;
        API_DB *db;

        for(i=0;i<api_modmax;i++) {
                if(strcmp(s,api_module[i])==0) {
                        db = (API_DB *)search_hash2(ordinal,257+i);
                        if(db != NULL) {
                                strcpy(namebuf,db->name);
                                return 1;
                        }
                }
        }
        *namebuf=0;
        return 0;
}

/**********************************************************************
 *  �d�w�d�@�n�k�c�@�g�d�`�c�d�q���`�F�b�N
 **********************************************************************
 */
void	MZchk()
{
        Read(&oldhdr,sizeof(oldhdr));

        if( (oldhdr.mz[0]=='M') && (oldhdr.mz[1]=='Z') ) return ;
        if( (oldhdr.mz[0]=='Z') && (oldhdr.mz[1]=='M') ) return ;

        printf("fatal:EXE OLD HEADER ('MZ') not found.\n");
        exit(1);
}

/**********************************************************************
 *  �m�d�@�g�d�`�c�d�q�܂ŃV�[�N����B
 **********************************************************************
 */
void	NEseek(long l)
{
        char    buf[32];

        if( oldhdr.relocations == 0x0040 ) {
                printf("fatal:Windows EXE HEADER ('NE') not found.\n");
                exit(1);
        }
        Rseek(oldhdr.ne_header+l);
}

void	NEload(void)
{
	NEseek(0);
    Read(&ne,sizeof(ne));
}
/**********************************************************************
 *  �m�����d�w�d�w�b�_�[��ǂݍ���Ń`�F�b�N����B
 **********************************************************************
 */
void	NEchk(void)
{
        char    name[128];
        if( (ne.ne[0]=='N') && (ne.ne[1]=='E') ) {
                printf("filename    : %s\n",inpname);   /* �t�@�C���� */
                printf("link version: %d.%d\n",ne.ver,ne.rev);
          if(ne.flag & 0x8000) {
                printf("exe type    : library\n");      /* �c�k�k */
          }else{
                printf("exe type    : program\n");      /* �d�w�d */
          }
                printf("start addres: seg_%02x:%04x\n",ne.init_cs,ne.init_ip);
                Rseek(ne.nrt_table);
                Read(cutbuf,CUTSIZE);
                printf("description : ");printstr(cutbuf);/* ���� */
                printf("\n");
                nrt_dump(cutbuf);             /* ��풓���e�[�u�� */

                NEseek(ne.nam_table);           /* �m�d�w�b�_�[   */
                Read(cutbuf,CUTSIZE);
                printf("module name : ");printstr(cutbuf);/*���W���[����*/
                printf("\n");
                myself = xstrpdup(cutbuf);
                nrt_dump(cutbuf);             /* ��풓���e�[�u�� */
                return ;
        }
        printf("fatal:EXE NEW HEADER ('NE') not found.\n");
        exit(1);
}

/**********************************************************************
 *  �Z�O�����g�e�[�u�������[�h����
 **********************************************************************
 */
void	SEGTload(void)
{
        NEseek(ne.seg_table);
        Read(segt,ne.segentry * sizeof(segt[0]) );
}
/**********************************************************************
 *  ���W���[�����t�@�����X�e�[�u�������[�h����B
 **********************************************************************
 */
void	MODref(void)
{
        int     i,len,ofs;
        Uchar *p;
        
        NEseek(ne.ref_table);
          Read(refname,REFSIZE );
        NEseek(ne.imp_table);
          Read(strtable,2048);

        printf("module reference:\n");
        for(i=0;i<ne.modentry;i++) {
                ofs=refname[i];
                modname[i] = xstrpdup(&strtable[ofs]);

                printf("   %2d:",i+1);
                printstr(&strtable[ofs]);
                printf("\n");
        }
}

void	printstr(char *s)
{
        int len;
        len=*s++;
        while(len) {
                putchar(*s);s++;
                len--;
        }
}

void	sprintstr( char *t,char *s)
{
        int len;
        len=*s++;
        while(len) {
                *t++ = *s++;
                len--;
        }
        *t = 0;
}


/**********************************************************************
 *  �������[�_���v
 **********************************************************************
 */
void	dump(buf,siz)
unsigned char *buf;
{
        while(siz) {
                printf(" %02x",*buf++);
                siz--;
        }
        printf("\n");
}

/**********************************************************************
 *  �G���g���[�e�[�u����\������
 **********************************************************************
 */
void	ENTdump()
{
        int i,n,id;
        int entnum=1;
        Ushort	 offset;
        Uchar   *p;
        MOVABLE *m;
        FIXED   *f;

        NEseek(ne.entrytable);
        Read(cutbuf,CUTSIZE);
        p=(Uchar *)cutbuf;

        printf("entry table:\n");

        while(1) {
                n=*p++; /*      �G���g���[�� */
                if(n==0) break;
                id=*p++;
                switch(id) {
                 case 0x00:     /* NO USE */
                        entnum += n;break;
                 case 0xff:     /* MOVABLE */
                        for(i=0;i<n;i++) {
                                m=(MOVABLE *)p;p+=6;
                                pr_ent(entnum++,m->segnum,m->offset);
                        }
                        break;
                 default:
                        for(i=0;i<n;i++) {
                                f=(FIXED *)p;p+=3;

                                /*offset�����o�[�͊�A���C�����g�Ȃ̂�*/
                                offset = (f->offset_l)|(f->offset_h<<8);

                                pr_ent(entnum++,id,offset);
                        }
                        break;
                }
        }
}

void	pr_ent(int entnum,int seg,int off)
{
    ifopt('x') {
        printf(" %s.%d\t",myself,entnum);
        print_nrtname(entnum,seg,off);
        printf("\n");
    }else{
        printf("%5d ",entnum);
        printf("%3d:%04x "  ,seg,off);
        print_nrtname(entnum,seg,off);
        printf("\n");
    }
}
/**********************************************************************
 *  �Z�O�����g�e�[�u����\������
 **********************************************************************
 */
void	SEGTdump()
{
        int i,flg;
        Ushort align;
        long off,siz;
        char *segname;
        char *segnamet[]={
                "CODE","DATA","SEG2","SEG3",
                "SEG4","SEG5","SEG6","SEG7"
        };
        char  cutname[80];

        align = 1<<ne.align;
        printf("segment table:  offset   size\n");
        for(i=0;i<ne.segentry;i++) {
                clr_hash();
                flg = segt[i].flag &7;
                segname = segnamet[ flg ];
                off=(long) segt[i].offset * align;
                siz=(long) segt[i].size;
                segnum=i+1;
/*              if(siz==0) siz=0x10000L;        */
                printf("  seg_%02d      %6lx %6lx %s\n"
                        ,segnum,off,siz,segname
                );

            loadbin(off,siz);

            if( flg == 0 ) { /* CODE SEGMENT�ł��� */
                rel_dump(segnum,off,siz);/* �����P�[�V�����e�[�u���_���v */
                insert_fixup();
            }

ifopt('b')      { /* �K�v�Ȃ�Z�O�����g�𕪗����ċt�A�Z���u������ */
                sprintf(cutname,"%s%02d.bin",binname,segnum);/* seg_xx.bin */
                cutbin (cutname,siz);
                }
            if( flg == 0 ) {/* CODE SEGMENT�ł��� */
ifopt('s')		{
                sprintf(cutname,"%s%02d.asm",srcname,segnum);/*seg_xx.asm*/

                disasmbin(cutname,0L,siz,NULL);/*�t�A�Z���u�����ʂ��o�͂��� */
				}
            }else{
ifopt('h')      { /* add hex dump */
                sprintf(cutname,"%s%02d.hex",hexname,segnum);/* seg_xx.hex */
                cuthex (cutname,siz);
                }
            }
        }
}
/**********************************************************************
 *  �o�C�i���[��؂�o��
 **********************************************************************
 */
void	loadbin(long off,long size)
{
        unsigned len;
        char *p;
        
        Rseek(off);
        p=loadbuf;      /* loadbuf�ɂ܂Ƃ߂ēǂ߂�Ηǂ���.   */
        /* �T�C�Y��64k�ɋ߂��Ȃ�Ɠǂ߂Ȃ��̂�16k�P�ʂŕ������[�h����. */
        while(size) {
                len=LOADSIZE;if(len>size) len=size;
                Read(  p,len);
                p    +=  len;
                size -=  len;
        }
}

/**********************************************************************
 *  �o�C�i���[��؂�o��
 **********************************************************************
 */
void	cutbin(char *name,long size)
{
        unsigned len;
        char *p;
        
        Wopen(name);
#if	0	/*�_�~�[��EXE�w�b�_�[��t���������ꍇ�� 1 �ɂ���*/
        gen_dummy_hdr(cutbuf,size);
                Write(cutbuf,0x200);
#endif

        p=loadbuf;      /* loadbuf�ɂ܂Ƃ߂ēǂ߂�Ηǂ���.   */
        /* �T�C�Y��64k�ɋ߂��Ȃ�Ɠǂ߂Ȃ��̂�16k�P�ʂŕ������[�h����. */
        while(size) {
                len=LOADSIZE;if(len>size) len=size;
                Write( p,len);
                p    +=  len;
                size -=  len;
        }
        Wclose();
}

void	cuthex(char *name,long size)
{
        unsigned len;
        unsigned long start=0;
        char *p;
        
        Wopen(name);
        p=loadbuf;      /* loadbuf�ɂ܂Ƃ߂ēǂ߂�Ηǂ���.   */
        /* �T�C�Y��64k�ɋ߂��Ȃ�Ɠǂ߂Ȃ��̂�16k�P�ʂŕ���dump����. */
        while(size) {
                len=LOADSIZE;if(len>size) len=size;
				dump_out(ofp,p,len,start);
                p    +=  len;
                start+=  len;
                size -=  len;
        }
        Wclose();
}

/**********************************************************************
 *  �����P�[�V��������\������
 **********************************************************************
 */
void	rel_dump(int segnum,long off,long siz)  /* �����P�[�V�����e�[�u���_���v */
{
        int i;
        short m;
        REL  *r;
        
ifopt('r')printf("relocation table seg_%02d:\n",segnum);

        Rseek(off+siz);
        Read(&m, sizeof(short));
        Read(relbuf,sizeof(REL)*m);relmax=m;
        r = relbuf;
ifopt('r')  printf(" ;type flag offset name.ordinal\n");

        for(i=0;i<m;i++) {
                rel_print(r);
            /* �����P�[�V�����`�F�C����H�� */
            if((r->flag & F_ADDITIVE)==0) rel_chain(r);
            r++;
        }
}

/**********************************************************************
 *  �����P�[�V����������\������
 **********************************************************************
 */
void	rel_print(REL *r)
{
        char *modulename;
        char *funcstr;
        if(!IsOpt('r')) return;

        switch(r->flag & 3) {

         case F_INTERNALREF   :
                printf("  %02x   %02x   %04x    %d:%x\n"
                        ,r->type
                        ,r->flag
                        ,r->off
                        ,r->i.i.segnum  /* �����Z�O�����g�ԍ� */
                        ,r->i.i.off     /* �����Z�O�����g�I�t�Z�b�g */
                );
                break;

         case F_IMPORTORDINAL :
                if(r->i.o.idx==0xff) {
                        modulename = myself;
                }else{
                        modulename = modname[r->i.o.idx-1];
                }
                printf("  %02x   %02x   %04x    %s.%d\n"
                        ,r->type
                        ,r->flag
                        ,r->off
                        ,modulename     /* �Q�ƃ��W���[���� */
                        ,r->i.o.ordinal /* �G���g���[�̏��� */
                );
                break;
         case F_IMPORTNAME    :
                if(r->i.o.idx==0xff) {
                        modulename = myself;
                }else{
                        modulename = modname[r->i.o.idx-1];
                }
                funcstr=&strtable[r->i.n.off];  /* �Ăт������e�[�u�� */
                printf("  %02x   %02x   %04x    %s."
                        ,r->type
                        ,r->flag
                        ,r->off
                        ,modulename     /* �Q�ƃ��W���[���� */
                );
                printstr(funcstr);      /* �Ăяo�����e�[�u�� */
                printf( "\n" );
                break;
         case F_OSFIXUP       :
                printf("  %02x   %02x   %04x    OS_FIXUP %d\n"
                        ,r->type
                        ,r->flag
                        ,r->off
                        ,r->i.f.type    /* FIXUP�̃^�C�v */
                );
                break;
        }

}

get_rword(off)
{
        int h,l;
        l=loadbuf[off]   & 0xff;
        h=loadbuf[off+1] & 0xff;
        return l|(h<<8);
}

void	rel_chain(REL *r)
{
        unsigned int w;
        REL rc;

/*        if(r->type != 3) return;      */

        rc = *r;
        while(1) {
                w = get_rword(rc.off);if(w == 0xffff) break;
                rc.off = w;
                relbuf[relmax++] = rc;
                rel_print(&rc);
                if(relmax>=RELMAX) {
                        printf("�����P�[�V�����E�`�F�C���Ɏ��s���܂���.\n");
                        exit(1);
                }
        }
}

void	disasmbin(char *name,long start,long size,char *hdrmsg)
{
        char oname[128];
        char cmd[128];
        FILE *fp;

        strcpy(oname,name);
        fp=fopen(oname,"wb");
        if(fp==NULL) {
                printf("fatal:Can't create file %s\n",oname);exit(1);
        }
        setvbuf(fp,vbuf,0,VBUFSIZE);
	if(hdrmsg!=NULL) fprintf(fp,"%s" CRLF,hdrmsg);
        disasm_i386(loadbuf,start,size,fp);
        fclose(fp);
}
/**********************************************************************
 *  
 **********************************************************************
 */
char    dummy_hdr[16]={
         'M', 'Z',0x04,0x00,0x02,0x00,0x00,0x00,
        0x20,0x00,0x01,0x00,0xff,0xff,0x00,0x00,
};

#if	0
void	gen_dummy_hdr(s,l)
char *s;
long l;
{
        int i;
        EXE_OLD_HDR *h;
        
        h=(EXE_OLD_HDR *)s;
        for(i=0;i<0x200;i++) {
                s[i]=0;
        }
        for(i=0;i<16;i++) {
                s[i]=dummy_hdr[i];
        }
        h->fragment = l & 0x1ff;
        h->pages    = l / 0x200 +1;
        if(h->fragment) h->pages ++;
}
#endif
/**********************************************************************
 *  ��풓���e�[�u���i�G���g���[���x���j��\������B
 **********************************************************************
 */
void	nrt_dump(s)
Uchar *s;
{
        int l;
        int entnum;

        l = *s;
        s+= 1+l+2;

        while(*s) {
                l=*s;
                entnum = s[l+1] + (s[l+2]<<8);
                nrt[nrtmax].name = xstrpdup(s);
                nrt[nrtmax].val  = entnum;
                    nrtmax++;
                if( nrtmax >= NRTMAX ) {
                    printf("��풓�e�[�u�����I�[�o�[���܂���.\n");
                    exit(1);
                }
ifopt('t') {
                printf("   %5d: ",entnum);
                printstr(s);
                printf("\n");
           }
                s+=l+3;
        }
}

int		get_nrtnam(val)
{
        int i;
        for(i=0;i<nrtmax;i++) {
                if(val == nrt[i].val) return i;
        }
        return -1;
}

void	print_nrtname(entnum,seg,off)
{
        int i;
        i=get_nrtnam(entnum);
        if(i == -1) printf("(unknown)");
        else{
                printf("%s",nrt[i].name);
                nrt[i].seg=seg;
                nrt[i].off=off;
        }
}


/**********************************************************************
 *  
 **********************************************************************
 */
void	insert_label()
{
        int i;
        for(i=0;i<nrtmax;i++) {
                insert_hash2(nrt[i].off,nrt[i].seg,(char *)&nrt[i]);
        }
}

void	insert_fixup()
{
        int i;
        REL *r;
        r=relbuf;
        for(i=0;i<relmax;i++) {
                insert_hash(r->off,256,(char *)r);
                r++;
        }
}

void	putrel(s,ip)
char *s;
{
        int i;
        REL *r;
        char *modulename;

        ip++;
        r=relbuf;
        for(i=0;i<relmax;i++) {
                if(ip == r->off) {
                    if(r->i.o.idx==0xff) {
                                modulename = myself;
                    }else{
                                modulename = modname[r->i.o.idx-1];
                    }
                    sprintf(s,"%s.%d"
                        ,modulename
                        ,r->i.o.ordinal
                    );
                    return;
                }
                r++;
        }
}

/**********************************************************************
        �e�Z�O�����g�f�[�^�̌��ɕt������郊���P�[�V�������
                           PER SEGMENT DATA

        �ŏ��� dw �����P�[�V�����e�[�u����   ������B
        ���̌��Ɉȉ��̂W�o�C�g���A���������B
 **********************************************************************
+00     DB  Source type.
            0Fh = SOURCE_MASK
            00h = LOBYTE
            02h = SEGMENT
            03h = FAR_ADDR (32-bit pointer)
            05h = OFFSET (16-bit offset)

+01     DB  Flags byte.
            03h = TARGET_MASK     Flag byte�̉��ʂQ�r�b�g�Ŏ�ނ����܂�B
            00h = INTERNALREF           00 �����Q��
            01h = IMPORTORDINAL     01 �c�k�k�Q��.�G���g���[�����Ŏw��
            02h = IMPORTNAME        10 �c�k�k�Q��.�G���g���[���Ŏw��
            03h = OSFIXUP           11 ���������_���Z�̖��߂�fixup����B
            04h = ADDITIVE        ���Z��񂪗L�邱�Ƃ������B

+02     DW  Offset within this segment of the source chain.

        type:   �ȉ��̂S�ʂ�
-----------------------------------------------------------------------
        INTERNALREF
+04         DB  Segment number for a fixed segment, or 0FFh for a
                movable segment.
+05         DB  0
+06         DW  Offset into segment if fixed segment, or ordinal
                number index into Entry Table if movable segment.
-----------------------------------------------------------------------
        IMPORTNAME
+04         DW  Index into module reference table for the imported
                module.
+06         DW  Offset within Imported Names Table to procedure name
                string.
-----------------------------------------------------------------------
        IMPORTORDINAL
+04         DW  Index into module reference table for the imported
                module.
+06         DW  Procedure ordinal number.
-----------------------------------------------------------------------
        OSFIXUP
+04         DW  Operating system fixup type.
                Floating-point fixups.
                0001h = FIARQQ, FJARQQ
                0002h = FISRQQ, FJSRQQ
                0003h = FICRQQ, FJCRQQ
                0004h = FIERQQ
                0005h = FIDRQQ
                0006h = FIWRQQ
+06         DW  0
-----------------------------------------------------------------------
 */
/**********************************************************************
 *  
 **********************************************************************
 */
