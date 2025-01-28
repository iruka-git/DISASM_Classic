/**********************************************************************
 *  ＰＥ形式実行ファイルのヘッダーファイル
 **********************************************************************
 */
typedef struct FILE_HDR {
    Ushort  Machine;
    Ushort  NumberOfSections;
    Ulong   TimeDateStamp;
    Ulong   PointerToSymbolTable;
    Ulong   NumberOfSymbols;
    Ushort  SizeOfOptionalHeader;
    Ushort  Characteristics;
} FILE_HDR;


/**********************************************************************
 *  
 **********************************************************************
 */
typedef struct DATA_DIR {
    Ulong   VirtualAddress;
    Ulong   Size;
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
    Ulong   SizeOfCode;
    Ulong   SizeOfInitializedData;
    Ulong   SizeOfUninitializedData;
    Ulong   AddressOfEntryPoint;
    Ulong   BaseOfCode;
    Ulong   BaseOfData;

    /* NT additional fields */

    Ulong   ImageBase;
    Ulong   SectionAlignment;
    Ulong   FileAlignment;
    Ushort  MajorOperatingSystemVersion;
    Ushort  MinorOperatingSystemVersion;
    Ushort  MajorImageVersion;
    Ushort  MinorImageVersion;
    Ushort  MajorSubsystemVersion;
    Ushort  MinorSubsystemVersion;
    Ulong   Reserved1;
    Ulong   SizeOfImage;
    Ulong   SizeOfHeaders;
    Ulong   CheckSum;
    Ushort  Subsystem;
    Ushort  DllCharacteristics;
    Ulong   SizeOfStackReserve;
    Ulong   SizeOfStackCommit;
    Ulong   SizeOfHeapReserve;
    Ulong   SizeOfHeapCommit;
    Ulong   LoaderFlags;
    Ulong   NumberOfRvaAndSizes;
    DATA_DIR DataDir[16];
} OPT_HDR;

/**********************************************************************
 *  セクションヘッダー
 **********************************************************************
 */
typedef struct SECTION_HDR {
    Uchar   Name[8];
    Ulong   VirtualSize;
    Ulong   VirtualAddress;
    Ulong   SizeOfRawData;
    Ulong   PointerToRawData;
    Ulong   PointerToRelocations;
    Ulong   PointerToLinenumbers;
    Ushort  NumberOfRelocations;
    Ushort  NumberOfLinenumbers;
    Ulong   Characteristics;
} SECTION_HDR;

/* .edata の先頭 */
typedef struct EXPORT_DESC {
    Ulong   Characteristics;
    Ulong   TimeDateStamp;
    Ushort  MajorVersion;
    Ushort  MinorVersion;
    Ulong   Name;
    Ulong   Base;
    Ulong   NumberOfFunctions;
    Ulong   NumberOfNames;
    Ulong   AddressOfFunctions;
    Ulong   AddressOfNames;
    Ulong   AddressOfNameOrdinals;
} EXPORT_DESC;

/* .idata の先頭 */
typedef struct IMPORT_DESC {
    long	Characteristics;
    long	TimeDateStamp;
    long	ForwarderChain;
    long	Name;
    long	FirstThunk;
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


