/*
 *	シンボル検索結果を入れる構造体
 */

typedef struct SYMBOL {
	long	  adrs;		/* 論理アドレス  */
	char     *name;		/* シンボル名	 */
} SYMBOL;

#define	Ulong	unsigned long
int find_symbol_by_name(SYMBOL *symbol,char *name  );
int find_symbol_by_adrs(SYMBOL *symbol,long address);

#define	SYMLEN	48		/* シンボル名の長さの最大長 */

/***  プロトタイプ宣言　***/
void init_symbol(void);
int adrs_hash(long adrs);
int name_hash(char *name);
void entsym_by_adrs(SYMBOL *symbol);
void entsym_by_name(SYMBOL *symbol);
void reg_symbol(SYMBOL *symbol);
int find_symbol_by_adrs(SYMBOL *symbol,long address);
int find_symbol_by_name(SYMBOL *symbol,char *name );

