/**********************************************************************
 *  �o�d�`���̃t�@�C�����_���v����B
 **********************************************************************
 */
#include  <stdio.h>
#include  <stdlib.h>
#include  <string.h>
#include "diswin.h"
#include "pedump.h"


/***  �v���g�^�C�v�錾�@***/
void	pe_dump(void);
void	dump_out(FILE *fp,unsigned char *buf,long cutsize,long start);
void	code32_dump(int segnum,char *sectname,long off,long siz,long start,long size);
void	data32_cut(int segnum,char *sectname,long off,long siz,long start,long size);
void	data32_dump(int segnum,char *sectname,long off,long siz,long start,long size);
void	section_dump(void);
int		hint_dump(long rva,long size,long offset);
int		imports_dump(long offset,long size,long vadrs);
int		exports_dump(long offset,long size,long vadrs);
void	imp_exp_find(int f);

void	set_cpumode(int op,int ad);


void	insert_hash2(long val,int flg,char *str);
char	*xstrdup(char *s);
char	*ModuleName;
char    *timedate_string(long lstamp);

extern	void	load_exemap(char *s);
extern	char    mapname[];            /* ���̓t�@�C����       */

#define Read(buf,siz)   fread (buf,1,siz,ifp)
#define Rseek(ptr) fseek(ifp,ptr,0)
#define IsOpt(c) ((opt[ c & 0x7f ])!=NULL)
#define   Opt(c)   opt[ c & 0x7f ]
#define ifopt(c) if((opt[ c & 0x7f ])!=NULL)

#define	ASMCUTS 0x8000L

/**********************************************************************
 *  �d����������������
 **********************************************************************
 */
extern  FILE  *ifp;
extern  FILE  *ofp;
extern	char  *vbuf;
extern  char  *loadbuf;         /* �o�C�i���[�����[�h����o�b�t�@ */
extern	char  *opt[128];	/* �I�v�V�����������w�肳��Ă����炻�̕�����*/
			        /* ������������i�[�A�w��Ȃ����NULL	*/
extern	char   srcname[];
extern	char   binname[];
extern	char   hexname[];
extern	char   inpname[];
extern	int	   quietmode;
void NEseek(long);
void loadbin(long off,long size);
void gen_dummy_hdr(char *s,long l);

long disasm_i386(char *buf,long start,long size,FILE *ofp);
long disasm_mips(char *buf,long start,long size,FILE *ofp);
long disasm_arm(char *buf,long start,long size,FILE *ofp);
long disasm_sh(char *buf,long start,long size,FILE *ofp);

#define MACHINE_I860              0x14d   // Intel 860.
#define MACHINE_I386              0x14c   // Intel 386.
#define MACHINE_R3000             0x162   // MIPS little-endian, 0540 big-endian
#define MACHINE_R4000             0x166   // MIPS little-endian
#define MACHINE_ALPHA             0x184   // Alpha_AXP
#define MACHINE_SH3	              0x1a2   // Hitachi-SH3
#define MACHINE_SH4	              0x1a6   // Hitachi-SH4
#define MACHINE_ARM	              0x1c0   // ARM

/**********************************************************************
 *  �v������
 **********************************************************************
 */
PE_HDR       pe;    /* PE�`���̃w�b�_�[   */
SECTION_HDR  sect;  /* �Z�N�V�����w�b�_�[ */
int			 dumpmode=0;

char *machine_name(int magic)
{
	switch(magic) {
	case MACHINE_I860  :   // Intel 860.
		return "i860";
	case MACHINE_I386  :   // Intel 386.
		return "i386";
	case MACHINE_R3000 :   // MIPS little-endian, 0540 big-endian
		return "R3000";
	case MACHINE_R4000 :   // MIPS little-endian
		return "R4000";
	case MACHINE_ALPHA :   // Alpha_AXP
		return "alpha";
	case MACHINE_SH3   :   // Hitachi SH3
		return "SH-3";
	case MACHINE_SH4   :   // Hitachi SH4
		return "SH-4";
	case MACHINE_ARM   :   // Alpha_AXP
		return "ARM";
	default:
		return "Unknown CPU";
	}
}
/**********************************************************************
 *  �e��@�t�A�Z���u��
 **********************************************************************
 */
#ifdef	DOS16
//	Turbo-C (DOS-generic)�ł́ARISC�֌W�������N���Ȃ�.
long disasm(char *buf,long start,long size,FILE *ofp)
{
		return disasm_i386(buf,start,size,ofp);
}
#else
long disasm(char *buf,long start,long size,FILE *ofp)
{
	switch(pe.hdr.Machine) {
	case MACHINE_R3000:
	case MACHINE_R4000:
		return disasm_mips(buf,start,size,ofp);
	case MACHINE_SH3:
	case MACHINE_SH4:
		return disasm_sh(buf,start,size,ofp);
	case MACHINE_ARM:
		return disasm_arm(buf,start,size,ofp);
	default:
	case MACHINE_I386:
		return disasm_i386(buf,start,size,ofp);
	}
}
#endif
/**********************************************************************
 *  �o�d�@�c�t�l�o���C��
 **********************************************************************
 */
void	pe_dump()
{
	static char	*subsys[]={	"Unknown",		/* 0*/
							"Native",		/* 1*/
							"Windows GUI",	/* 2*/
							"Windows CUI",	/* 3*/
							"???",			/* 4*/
							"OS2 CUI",		/* 5*/
							"???",			/* 6*/
							"POSIX CUI" 	/* 7*/	};
	static char	*datadirname[16]={
		"EXPORT",
		"IMPORT",
		"RESOURCE",
		"EXCEPTION",
		"SECURITY",
		"BASERELOC",
		"DEBUG",
		"COPYRIGHT",
		"GLOBALPTR(Mips GP reg)",
		"TLS",
		"LOAD_CONFIG",
		"",
		"",
		"",
		"",
		""
	};
    int i;
    char *dir_namep;
    
    if(IsOpt('l')) dumpmode=1;

    load_exemap(mapname);/* *.MAP�t�@�C�������݂���΂����ǂݍ��� */
    
    NEseek(0);
    Read(&pe,sizeof(pe));	/* PE�w�b�_�[�\����ǂݍ��� */
    if(quietmode) {
    	if(quietmode!=16)
	    	imp_exp_find(0);
    	return;
    }
    
    printf("Machine                 =0x%x (%s)\n" ,pe.hdr.Machine,machine_name(pe.hdr.Machine));
    printf("Number of sections      =0x%x\n" ,pe.hdr.NumberOfSections);
    printf("TimeDateStamp           =0x%lx (%s)\n",pe.hdr.TimeDateStamp,
    							   timedate_string(pe.hdr.TimeDateStamp));
    printf("Pointer to symbol table =0x%lx\n",pe.hdr.PointerToSymbolTable);
    printf("Number of symbols       =0x%lx\n",pe.hdr.NumberOfSymbols);
    printf("Size of optional header =0x%x\n" ,pe.hdr.SizeOfOptionalHeader);
    printf("Characteristics         =0x%x\n" ,pe.hdr.Characteristics);
    printf("\n");
    printf("Magic                   =0x%x\n" ,pe.opt.Magic                   );
    printf("linker version          =%d.%d\n",pe.opt.MajorLinkerVersion
                                             ,pe.opt.MinorLinkerVersion      );
    printf("Size of code            =0x%lx\n",pe.opt.SizeOfCode              );
    printf("Size of initialized data=0x%lx\n",pe.opt.SizeOfInitializedData   );
    printf("Size of uninitializedata=0x%lx\n",pe.opt.SizeOfUninitializedData);
    printf("Address of entry point  =0x%lx\n",pe.opt.AddressOfEntryPoint     );
    printf("Base of code            =0x%lx\n",pe.opt.BaseOfCode              );
    printf("Base of data            =0x%lx\n",pe.opt.BaseOfData              );
    printf("ImageBase               =0x%lx\n",pe.opt.ImageBase               );
    printf("Subsystem               =0x%x (%s)\n" ,pe.opt.Subsystem,
    										subsys[pe.opt.Subsystem	& 7]);

	//�f�[�^�f�B���N�g���̉��.
    printf("DataDirectory[16]=\n");

    for(i=0;i<pe.opt.NumberOfRvaAndSizes;i++) {
        dir_namep=datadirname[i];
        if(pe.opt.DataDir[i].Size==0) dir_namep="";

        printf("      adrs,size = %8lx,%8lx %s\n"
            ,pe.opt.DataDir[i].VirtualAddress
            ,pe.opt.DataDir[i].Size
            ,dir_namep
        );
    }

	imp_exp_find(1);	/* �C���|�[�g�E�e�[�u�����_���v���� */
    section_dump();		/* �Z�N�V�����E�w�b�_�[���_���v���� */
}



void dump_out(FILE *fp,unsigned char *buf,long cutsize,long start)
{
	char *dump_hex16(unsigned char *buf,int mode);
	int x,c;
	while(cutsize>0) {
		fprintf(fp,"%08lx   %s" CRLF,start,dump_hex16(buf,dumpmode));
		start   += 16;
		buf     += 16;
		cutsize -= 16;
	}
}

/**********************************************************************
 *  �R�[�h�Z�N�V�����Ȃ�t�A�Z���u�����N������
 **********************************************************************
 */
void code32_dump(int segnum,char *sectname,long off,long siz,long start,long size)
{
    char asmname[64];	/* *.asm */
    FILE *fp;		/* �t�@�C�� asmname �̃n���h�� */
    long cutsize,asmsize;

    sprintf(asmname,"%s%02d.asm",srcname,segnum);
    fp=fopen(asmname,"wb");
    if(fp==NULL) {
        printf("Fatal:Can't create file %s\n",asmname);exit(1);
    }else{
        printf("* Writing asmfile: %s  ...\n",asmname);
    }
    setvbuf(fp,vbuf,0,VBUFSIZE);
    fprintf(fp,"; ModuleName    : %s" CRLF,inpname);
    fprintf(fp,"; entry Address :   %06lx" CRLF,pe.opt.AddressOfEntryPoint);
    fprintf(fp,"; Base of code  :   %06lx" CRLF,pe.opt.BaseOfCode         );
    fprintf(fp,"; Base of data  :   %06lx" CRLF,pe.opt.BaseOfData         );
    fprintf(fp,"; ImageBase     : %08lx" CRLF,pe.opt.ImageBase          );
    fprintf(fp,CRLF    );
    fprintf(fp,"                        %s" CRLF,sectname);

#ifndef	DOS16
	dislmips_init();
#endif

	if(siz>size) siz=size;	/* �o�[�`�����T�C�Y�ɍ��킹�� */
    /* 32kB ���ǂݍ��݂Ȃ���t�A�Z���u������ */
    while(siz>0) {
        cutsize=siz;if(cutsize>=ASMCUTS) cutsize=ASMCUTS; /* 32k���� */
        loadbin(off,cutsize+256);	/* 256byte �]���ɓǂݍ��� */
        asmsize=disasm(loadbuf,start,cutsize,fp);
/*	printf("disasm start %lx size %lx\n",start,cutsize);*/
        off  += asmsize;
        siz  -= asmsize;
        start+= asmsize;
    }
    fclose(fp);
}

/**********************************************************************
 *  �f�[�^�Z�N�V�������J�b�g�o��
 **********************************************************************
 */
void data32_cut(int segnum,char *sectname,long off,long siz,long start,long size)
{
    char asmname[64];	/* *.asm */
    FILE *fp;		/* �t�@�C�� asmname �̃n���h�� */
    long cutsize,asmsize;
    static long dummy;
	dummy=size;			/* �[���Ӗ��͂Ȃ� */
	dummy=sectname[0];	/* �[���Ӗ��͂Ȃ� */
	sprintf(asmname,"%s%02d.bin",binname,segnum);

    fp=fopen(asmname,"wb");
    if(fp==NULL) {
            printf("Fatal:Can't create file %s\n",asmname);exit(1);
    }else{
            printf("* Writing binfile: %s  ...\n",asmname);
    }
    setvbuf(fp,vbuf,0,VBUFSIZE);

    /* 32kB ���ǂݍ��݂Ȃ���o�� */
    while(siz>0) {
        cutsize=siz;if(cutsize>=ASMCUTS) cutsize=ASMCUTS; /* 32k���� */
        loadbin(off,cutsize);
		fwrite(loadbuf,1,cutsize,fp);
        asmsize=cutsize;
        off  += asmsize;
        siz  -= asmsize;
        start+= asmsize;
    }
    fclose(fp);
}



void data32_dump(int segnum,char *sectname,long off,long siz,long start,long size)
{
    char asmname[64];	/* *.asm */
    FILE *fp;		/* �t�@�C�� asmname �̃n���h�� */
    long cutsize,asmsize,i;

    static long dummy;
	dummy=size;			/* �[���Ӗ��͂Ȃ� */
	
	strcpy(asmname,sectname);
	asmname[5]=0;

	if(!IsOpt('H')) {	// -H ���w�肷��ƑS�Z�N�V������HEX DUMP(�f�o�b�O��p)
		if( (stricmp(asmname,".data")!=0)&&
		    (stricmp(asmname,"DATA") !=0) ) return;
	}

	sprintf(asmname,"%s%02d.hex",hexname,segnum);
    fp=fopen(asmname,"wb");
    if(fp==NULL) {
            printf("Fatal:Can't create file %s\n",asmname);exit(1);
    }else{
            printf("* Writing hexfile: %s  ...\n",asmname);
    }
    setvbuf(fp,vbuf,0,VBUFSIZE);

    /* 32kB ���ǂݍ��݂Ȃ���o�� */
    while(siz>0) {
        cutsize=siz;if(cutsize>=ASMCUTS) cutsize=ASMCUTS; /* 32k���� */
        loadbin(off,cutsize);
		dump_out(fp,loadbuf,cutsize,start);
        asmsize=cutsize;
        off  += asmsize;
        siz  -= asmsize;
        start+= asmsize;
    }
    fclose(fp);
}

/**********************************************************************
 *  �Z�N�V�����w�b�_�[���_���v����B
 **********************************************************************
 */
void	section_dump()
{
    int i,j;
    char sectname[16];
    
    for(i=0;i<pe.hdr.NumberOfSections;i++) {
        NEseek( sizeof(pe)+sizeof(sect)*i );
        Read(&sect,sizeof(sect));
        for(j=0;j<8;j++) sectname[j]=sect.Name[j];
        sectname[8]=0;
        printf("section : %s\n",sectname);
        printf("    Virtual size          =%8lx\n",sect.VirtualSize);
        printf("    Virtual address       =%8lx\n",sect.VirtualAddress);
        printf("    Size of raw data      =%8lx\n",sect.SizeOfRawData);
        printf("    Pointer to raw data   =%8lx\n",sect.PointerToRawData);
        printf("    Pointer to relocations=%8lx\n",sect.PointerToRelocations);
        printf("    Pointer to linenumbers=%8lx\n",sect.PointerToLinenumbers);
        printf("    Number of relocations =%8d\n" ,sect.NumberOfRelocations);
        printf("    Number of linenumbers =%8d\n" ,sect.NumberOfLinenumbers);
        printf("    Characteristics       =%lx\n",sect.Characteristics);
      ifopt('s')
        if(sect.Characteristics & 0x0020) { /* .CODE */
		    set_cpumode(1,1);	/* �A�h���X�A�I�y�����h�T�C�Y���ɂR�Q������ */
            code32_dump(i+1,sectname         /*�Z�N�V������ */
                       ,sect.PointerToRawData/*�t�@�C���̐擪����̃I�t�Z�b�g*/
                       ,sect.SizeOfRawData   /* �u���b�N�T�C�Y */
                       ,sect.VirtualAddress  /* ���z�A�h���X*/
                          +pe.opt.ImageBase
                       ,sect.VirtualSize     /* ���z�T�C�Y */
            );
        }
      ifopt('b')
        data32_cut(i+1,sectname         /*�Z�N�V������ */
                       ,sect.PointerToRawData/*�t�@�C���̐擪����̃I�t�Z�b�g*/
                       ,sect.SizeOfRawData   /* �u���b�N�T�C�Y */
                       ,sect.VirtualAddress  /* ���z�A�h���X*/
                       ,sect.VirtualSize     /* ���z�T�C�Y */
        );
      ifopt('h')
        data32_dump(i+1,sectname         /*�Z�N�V������ */
                       ,sect.PointerToRawData/*�t�@�C���̐擪����̃I�t�Z�b�g*/
                       ,sect.SizeOfRawData   /* �u���b�N�T�C�Y */
                       ,sect.VirtualAddress+pe.opt.ImageBase  /* ���z�A�h���X*/
                       ,sect.VirtualSize     /* ���z�T�C�Y */
        );
    }
}

/**********************************************************************
 *  �C���|�[�g�e�[�u�����_���v����B
 **********************************************************************
 */
IMPORT_DESC *import;	/* �C���|�[�g�E�e�[�u���̃w�b�_�[ */
EXPORT_DESC *export;	/* �G�N�X�|�[�g�e�[�u���̃w�b�_�[ */
typedef	struct HINT {
	short	Hint;
	char	Name[2];
} HINT;

long 	impdelta;	/* import section �̉��z�A�h���X�E�I�t�Z�b�g */

int	hint_dump(long rva,long size,long offset)
{
	char labelbuf[128];
	char modname[128];
	long off;
	long *p;
	HINT *h;
	char *s;
	
/*	printf("#hint_dump(%lx %lx %lx)\n",rva,size,offset);	*/
	
	strcpy(modname,ModuleName);
	s=modname;while(*s) {
		if(*s=='.') *s=0;
		s++;
	}

#if	1
	if( (unsigned ) offset > (unsigned) size ) {
		printf("Fatal: .idata|Import Table Illegal! \n");
		return(-1);
	}
#endif

	p=(long *) (&loadbuf[offset]);

	while ( *p ) {
		if( *p & 0x80000000L) {
			 printf("   %6lx  %4u\n",rva,*p & 0xFFFF);
			sprintf(labelbuf,"%s.%u",modname,*p & 0xFFFF);
		}else{
			off = *p - impdelta;
#if	1
			if( (unsigned) off > (unsigned) size) {
				printf("Fatal: .idata|Import Table Illegal! \n");
				return(-1);
			}
#endif
			h = (HINT *) ( &loadbuf[ off ]);
			 printf("   %6lx  %4u:%s\n",rva,h->Hint, h->Name);
			sprintf(labelbuf,"%s.%s",modname, h->Name);
		}
		insert_hash2(rva,0,xstrdup(labelbuf));
		p++;
		rva+=4;
	}
	return(0);
}

/**********************************************************************
 *  .idata �Z�N�V�������_���v����
 **********************************************************************
 */
int	imports_dump(long offset,long size,long vadrs)
{
	int i;
	int descptr;
	unsigned long off;
	unsigned long import_off=0;

	impdelta = vadrs;

#if	0
	/* ���ʂ�IMPORT_DESC�� .idata�Z�N�V�����擪�ɂ���̂����E�E�E */
	import_off = pe.opt.DataDir[1].VirtualAddress - vadrs;
	if(import_off) {
		if(import_off > off) {
			printf("Fatal: .idata|Import Header Offset Illegal\n");
			return(-1);
		}
			printf("warning: .idata|Import Header Offset = %x\n",import_off);
	}
//	printf("#import_off=%lx\n",import_off);

#endif

	if(size >= 0x10000) size = 0x10000;	//import table�̃T�C�Y��64k�܂łɌ��肵�ă��[�h.
	loadbin(offset,size);

	import = (IMPORT_DESC *) &loadbuf[import_off];

#if	0
	printf("rva %lx\n",offset);
	printf("vadrs %lx\n",vadrs);
	dump(loadbuf,4096);
#endif

	printf("Import Table:\n");
	while ( (import->TimeDateStamp!=0) || (import->Name!=0) ) {
		off = import->Name - impdelta ;

#if	1
		if( off > (unsigned) size) {
			printf("Fatal: .idata|Import Header Illegal (%lx %lx %lx)! \n",off,import->Name,impdelta);
			return(-1);
		}
#endif

		ModuleName=&loadbuf[ (int)off ];
		printf("  %s:\n", ModuleName);
		printf("  Characteristics: %08lx\n", import->Characteristics);
 		printf("  TimeDateStamp:   %08lx (%s)\n", import->TimeDateStamp,
    							  timedate_string(import->TimeDateStamp));
 		printf("  ForwarderChain:  %08lx\n", import->ForwarderChain);
 		printf("  First thunk RVA: %08lx\n", import->FirstThunk);
 		printf("   thunk Ordinary: -- Import Function --\n");

		if(import->Characteristics) {
	 		if(hint_dump(
	 			pe.opt.ImageBase+import->FirstThunk,
	 			size,
	 			import->Characteristics - impdelta
	 			)!=0) return(-1);
		}else{
	 		if(hint_dump(
	 			pe.opt.ImageBase+import->FirstThunk,
	 			size,
	 			import->FirstThunk - impdelta
	 			)!=0) return(-1);
		}
		import++;
		
	}
	return(0);
}

/**********************************************************************
 *  .edata �Z�N�V�������_���v����
 **********************************************************************
 */
int	exports_dump(long offset,long size,long vadrs)
{
	int i;
	unsigned long off;
	long  *name;
	long  *functions;
	short *ordinals;
	char  *labelptr;

	impdelta = vadrs;
	loadbin(offset,size);
	export = (EXPORT_DESC *)loadbuf;

 if(!IsOpt('x')) {
	printf("Export Table:\n");
 }else{
	printf("Export Table of %s:\n",inpname);
 }
	off = export->Name - impdelta ;
	if( off > (unsigned) size) {
		printf("Fatal: .edata|Export Header Illegal! \n");
		return (-1);
	}
	ModuleName = &loadbuf[off];
 if(!IsOpt('x')) {
	printf("  %s:\n", ModuleName);
	printf("  Characteristics: %08lx\n", export->Characteristics);
	printf("  TimeDateStamp:   %08lx (%s)\n", export->TimeDateStamp,
    						  timedate_string(export->TimeDateStamp));
	printf("  Version:         %d.%d\n", export->MajorVersion,
									     export->MinorVersion);
 }
	functions =(long *)&loadbuf[export->AddressOfFunctions    -impdelta];
	ordinals  =(short*)&loadbuf[export->AddressOfNameOrdinals -impdelta];
	name      =(long *)&loadbuf[export->AddressOfNames        -impdelta];

 if(!IsOpt('x')) {
	printf("   EntryAdrs Ordn: -- Export Function --\n");
 }else{
 	char *p;
 	p=ModuleName;
 	while(*p) {
 		if(*p=='.') *p=0;
 		p++;
 	}
 }
 	for (i=0;i<export->NumberOfNames;i++) {
		labelptr = &loadbuf[*name - impdelta];

 if(!IsOpt('x')) {
		printf("   %08lx  %4lu:%s\n",
				*functions,
				*ordinals + export->Base,
				 labelptr
			  );
		insert_hash2(*functions + pe.opt.ImageBase,0,xstrdup(labelptr));
 }else{
		printf("  %s.%lu	%s\n",
				 ModuleName,
				*ordinals + export->Base,
				 labelptr
			  );
 }

		name++;
		ordinals++;
		functions++;
	}
	return (0);
}


/**********************************************************************
 *  ���z�A�h���X����|����ɁARVA���������Ă݂�.
 **********************************************************************
 */
int	find_RVAoffset(int vadrs,int *secsize)
{
    int i,j,off;
    for(i=0;i<pe.hdr.NumberOfSections;i++) {
        NEseek( sizeof(pe)+sizeof(sect)*i );
        Read(&sect,sizeof(sect));

        off = vadrs - sect.VirtualAddress;

        //���z�A�h���X���ݑł�.
		if((off>=0) && (off<sect.SizeOfRawData)) {
			*secsize = sect.SizeOfRawData - off;	//���̃Z�N�V�����̎c��o�C�g��.
			return sect.PointerToRawData + off;		//�����������z�A�h���X�ɑ΂���RVA.
		}
    }
	return 0;	//�^����ꂽ���z�A�h���X���܂�ł���Z�N�V������,
				//�����ł��Ȃ�����.
}
/**********************************************************************
 *  .idata .edata �Z�N�V������T���ă_���v����
 **********************************************************************
 */
void	imp_exp_find(int f)
{
    int i,j;
    int expidx=0;	//"EXPORT",
    int impidx=1;	//"IMPORT",
	int imp_PointerToRawData;	//IMPORT�e�[�u���̃t�@�C���I�t�Z�b�g
	int exp_PointerToRawData;	//EXPORT�e�[�u���̃t�@�C���I�t�Z�b�g
	int secsize;

	if(f) {
		imp_PointerToRawData = find_RVAoffset(pe.opt.DataDir[impidx].VirtualAddress,&secsize);
		
		if(imp_PointerToRawData)
		imports_dump(
			imp_PointerToRawData,
			secsize,
			pe.opt.DataDir[impidx].VirtualAddress	/* ���z�A�h���X*/
		);
    }
		exp_PointerToRawData = find_RVAoffset(pe.opt.DataDir[expidx].VirtualAddress,&secsize);
		if(exp_PointerToRawData)
		exports_dump(
			exp_PointerToRawData,
			pe.opt.DataDir[expidx].Size,
			pe.opt.DataDir[expidx].VirtualAddress	/* ���z�A�h���X*/
		);
}


/**********************************************************************
 *  .idata .edata �Z�N�V������T���ă_���v����i�����[�`���j
 **********************************************************************
 */
#if	0
void	imp_exp_find(int f)
{
    int i,j;
    char sectname[16];
    
    for(i=0;i<pe.hdr.NumberOfSections;i++) {
        NEseek( sizeof(pe)+sizeof(sect)*i );
        Read(&sect,sizeof(sect));
        for(j=0;j<8;j++) sectname[j]=sect.Name[j];
        sectname[6]=0;
		if(f)
        if(stricmp(sectname,".idata")==0){
			imports_dump(
				sect.PointerToRawData,
				sect.SizeOfRawData,
                sect.VirtualAddress  /* ���z�A�h���X*/
			);
        }
        if(stricmp(sectname,".edata")==0){
			exports_dump(
				sect.PointerToRawData,
				sect.SizeOfRawData,
                sect.VirtualAddress  /* ���z�A�h���X*/
			);
        }
    }
}
#endif

/**********************************************************************
 *  
 **********************************************************************
 */
