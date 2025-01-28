/*
 *	シンボル検索結果を入れる構造体
 */

typedef struct SYMBOL {
	long	  adrs;		/* 論理アドレス  */
	char far *name;		/* シンボル名	 */
} SYMBOL;

#define	Ulong	unsigned long
find_symbol_by_name(SYMBOL *symbol,char *name  );
find_symbol_by_adrs(SYMBOL *symbol,long address);

#define	SYMLEN	48		/* シンボル名の長さの最大長 */
