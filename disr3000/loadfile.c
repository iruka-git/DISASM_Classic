/*
 *   ptr = loadfile( "filename" );
 *------------------------------------------------------------------------
 *   ファイルをメモリーにロードする。
 *   メモリーは自前で確保し、その先頭アドレスを返す。
 *   失敗した場合は停止する。
 *------------------------------------------------------------------------
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <sys/types.h>
#include <sys/stat.h>
static	struct stat s;

int	sizefile(char *fname)
{
	int    st;
	st=stat(fname,&s);
	if(st) {
		return -1;
	}
	return s.st_size;
}

int	is_directory(char *fname)
{
	int    st;
	st=stat(fname,&s);
	if(st) {
		return 0;
	}
	if( s.st_mode & S_IFDIR ) return 1;
	return 0;
}

int loadfile_at(char *fname,char *buf,int size,int offset)
{
	int rc;
	FILE *fp=fopen(fname,"rb");
	if(fp==NULL) {
		printf("Fatal: can't open file:%s\n",fname);
		return 0;
	}
	if(offset) fseek(fp,offset,0);
	
	rc = fread(buf , 1,size,fp);
	fclose(fp);

	return rc;
}

char *loadfile(char *fname)
{
	int    st;
	FILE  *fp;
	char  *p;
	
	st=stat(fname,&s);
	if(st) {
		printf("file(%s) not found(err=%d).\n",fname,st);
		exit(1);
	}
	p=malloc(s.st_size+2560);
	if(p==NULL) {
		printf("fatal:memory exhausted.\n");
		exit(1);
	}
	fp=fopen(fname,"rb");
	if(fp==NULL) {
		printf("Fatal: can't open file:%s\n",fname);
		exit(1);
	}
	if( fread(p,1,s.st_size,fp)!=(unsigned )s.st_size ) {
		fclose(fp);
		exit(1);
	}
	fclose(fp);
	
	p[s.st_size]=0;
	p[s.st_size+1]=0;
	p[s.st_size+2]=0;
	p[s.st_size+3]=0;

	return p;
}

int	loadsize(void)
{
	return s.st_size;
}

int isDir(char *path)
{
	static	struct stat s;
	int st=stat(path,&s);
	if( st != 0) {
		return -1; // ディレクトリは存在しない.
	}
	if( S_ISDIR( s.st_mode )) {
		return 1;  // ディレクトリである.
	}
	return 0;      // ディレクトリではない.
}
