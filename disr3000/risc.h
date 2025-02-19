void    dislmips_init();
void	pr_rem(cchar *format, ...);
void	set_ea(char *buf,long addr);
long	disasm_mips(char *buf,long start,long size,FILE *ofp);
long	disasm_sh(char *buf,long start,long size,FILE *ofp);
long	disasm_arm(char *buf,long start,long size,FILE *ofp);

/*
bfd_vma	bfd_getl32(const unsigned char *buffer);
bfd_vma	bfd_getb32(const unsigned char *buffer);
bfd_vma	bfd_getl16(const unsigned char *buffer);
bfd_vma	bfd_getb16(const unsigned char *buffer);
*/
