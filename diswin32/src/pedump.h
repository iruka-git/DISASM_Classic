/**********************************************************************
 *  ＰＥ形式実行ファイルのヘッダーファイル
 **********************************************************************
 */
typedef struct FILE_HDR {
    Ushort  Machine;
    Ushort  NumberOfSections;
    Uint   TimeDateStamp;
    Uint   PointerToSymbolTable;
    Uint   NumberOfSymbols;
    Ushort  SizeOfOptionalHeader;
    Ushort  Characteristics;
} FILE_HDR;


/**********************************************************************
 *  
 **********************************************************************
 */
typedef struct DATA_DIR {
    Uint   VirtualAddress;
    Uint   Size;
} DATA_DIR;

/**********************************************************************
 *  オプショナルヘッダー
 **********************************************************************
 */
typedef struct OPT_HDR {
    /* Standard fields*/

    Ushort  Magic;
    Uchar   MajorLinkerVersion;
    Uchar   MinorLinkerVersion;
    Uint   SizeOfCode;
    Uint   SizeOfInitializedData;
    Uint   SizeOfUninitializedData;
    Uint   AddressOfEntryPoint;
    Uint   BaseOfCode;
    Uint   BaseOfData;

    /* NT additional fields */

    Uint   ImageBase;
    Uint   SectionAlignment;
    Uint   FileAlignment;
    Ushort  MajorOperatingSystemVersion;
    Ushort  MinorOperatingSystemVersion;
    Ushort  MajorImageVersion;
    Ushort  MinorImageVersion;
    Ushort  MajorSubsystemVersion;
    Ushort  MinorSubsystemVersion;
    Uint   Reserved1;
    Uint   SizeOfImage;
    Uint   SizeOfHeaders;
    Uint   CheckSum;
    Ushort  Subsystem;
    Ushort  DllCharacteristics;
    Uint   SizeOfStackReserve;
    Uint   SizeOfStackCommit;
    Uint   SizeOfHeapReserve;
    Uint   SizeOfHeapCommit;
    Uint   LoaderFlags;
    Uint   NumberOfRvaAndSizes;
    DATA_DIR DataDir[16];
} OPT_HDR;

/**********************************************************************
 *  セクションヘッダー
 **********************************************************************
 */
typedef struct SECTION_HDR {
    Uchar   Name[8];
    Uint   VirtualSize;
    Uint   VirtualAddress;
    Uint   SizeOfRawData;
    Uint   PointerToRawData;
    Uint   PointerToRelocations;
    Uint   PointerToLinenumbers;
    Ushort  NumberOfRelocations;
    Ushort  NumberOfLinenumbers;
    Uint   Characteristics;
} SECTION_HDR;

/* .edata の先頭 */
typedef struct EXPORT_DESC {
    Uint   Characteristics;
    Uint   TimeDateStamp;
    Ushort  MajorVersion;
    Ushort  MinorVersion;
    Uint   Name;
    Uint   Base;
    Uint   NumberOfFunctions;
    Uint   NumberOfNames;
    Uint   AddressOfFunctions;
    Uint   AddressOfNames;
    Uint   AddressOfNameOrdinals;
} EXPORT_DESC;

/* .idata の先頭 */
typedef struct IMPORT_DESC {
    int	Characteristics;
    int	TimeDateStamp;
    int	ForwarderChain;
    int	Name;
    int	FirstThunk;
} IMPORT_DESC;



/**********************************************************************
 *  ＰＥ形式のヘッダーの全体構造
 **********************************************************************
 */
typedef struct PE_HDR {
    char     Signature[4];	/* 文字列 'PE'          */
    FILE_HDR hdr;		/* ファイルヘッダー     */
    OPT_HDR  opt;		/* オプショナルヘッダー */
} PE_HDR;

/**********************************************************************
  PE フォーマットの構成

  (1) MS-DOSオールドヘッダー
  (2) 'PE'ヘッダー (上記のヘッダー)
  (3) セクションヘッダー x [セクション数]

 **********************************************************************
 */


