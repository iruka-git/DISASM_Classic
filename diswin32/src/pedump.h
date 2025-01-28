/**********************************************************************
 *  �o�d�`�����s�t�@�C���̃w�b�_�[�t�@�C��
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
 *  �I�v�V���i���w�b�_�[
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
 *  �Z�N�V�����w�b�_�[
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

/* .edata �̐擪 */
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

/* .idata �̐擪 */
typedef struct IMPORT_DESC {
    long	Characteristics;
    long	TimeDateStamp;
    long	ForwarderChain;
    long	Name;
    long	FirstThunk;
} IMPORT_DESC;



/**********************************************************************
 *  �o�d�`���̃w�b�_�[�̑S�̍\��
 **********************************************************************
 */
typedef struct PE_HDR {
    char     Signature[4];	/* ������ 'PE'          */
    FILE_HDR hdr;		/* �t�@�C���w�b�_�[     */
    OPT_HDR  opt;		/* �I�v�V���i���w�b�_�[ */
} PE_HDR;

/**********************************************************************
  PE �t�H�[�}�b�g�̍\��

  (1) MS-DOS�I�[���h�w�b�_�[
  (2) 'PE'�w�b�_�[ (��L�̃w�b�_�[)
  (3) �Z�N�V�����w�b�_�[ x [�Z�N�V������]

 **********************************************************************
 */


