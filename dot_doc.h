#ifndef dot_doc_decoder
#define dot_doc_decoder

#define CFB_minor_version3E			(ut_WORD) 0x003E // All .doc files use this minor version.
#define CFB_major_version_3			(ut_WORD) 0x0003
#define CFB_major_version_4			(ut_WORD) 0x0004
#define little_endian_indication	(ut_WORD) 0xFFFE // FF FE represents little endian byte order. This applies to all DOC files
#define sector_size_indication_512	(ut_WORD) 0x0009
#define sector_size_indication_4096 (ut_WORD) 0x000C
#define mini_sector_shift			(ut_WORD) 0x0006 // Sector size of the Mini Stream. Will always be 00 06
#define free_sector					(ut_DWORD) 0xFFFFFFFF // Indication of free sectors
#define end_of_chain				(ut_DWORD) 0xFFFFFFFE // "Chain terminators"
#define fat_sect					(ut_DWORD) 0xFFFFFFFD // Sectors that contain storage for FAT sectors
#define word_bin_file_indicator     (ut_WORD) 0xA5EC
#define fib_version                 (ut_WORD) 0x00C1 // This should always be the value in the .doc/.docx binary file


/* CFB header signature. */
const ut_BYTE CFB_header_signature[8] = {0xD0, 0xCF, 0x11, 0xE0, 0xA1, 0xB1, 0x1A, 0xE1};

/* .doc binary header format. */
typedef struct dot_doc_header
{
	/* Should resemble: D0 Cf 11 E0 A1 B1 1A E1. */
	ut_BYTE		header_sig[8];

	/* 16 bytes of padding. */
	ut_BYTE		padding[16];

	/* CFB minor version. 
	 * Should be 3E 00.
	 * */
	ut_WORD		CFB_minor_version;

	/* CFB major version. 
	 * 03 00 indicating CFB major version 3 or 04 00 indicating CFB major version 4.
	 * All .doc files using any version of Word since 2003 should be based on CFB major version 3.
	 * */
	ut_WORD		CFB_major_version;

	/* CFB byte order indication.
	 * All CFB files use little-endian.
	 * Represented by FE FF.
	 * */
	ut_WORD		CFB_byte_order_indication;

	/* CFB sector size.
	 * 512 for CFB major version 3, 4096 for CFB major version 4.
	 * 09 00 for 512 bytes, 0C 00 for 4096 bytes.
	 * */
	ut_WORD		CFB_sector_size;

	/* CFB mini sector size.
	 * Should be 06 00. This specifies the sector size of the Mini Stream
	 * as a power of two.
	 * See https://wikileaks.org/ciav7p1/cms/files/%5BMS-CFB%5D.pdf, page 17.
	 * */
	ut_WORD		CFB_mini_sector_size;

	/* 6 more bytes of padding. */
	ut_BYTE		padding2[6];

	/* Number of Directory Sectors.
	 * For CFB major version 3, this must be zero.
	 * */
	ut_DWORD	directory_sectors;

	/* Number of FAT sectors in the compound file. */
	ut_DWORD	FAT_sectors;

	/* First directory sector location.
	 * Starting sector number for directory stream.
	 * A directory stream is an array of directory entries grouped into sectors.
	 * */
	ut_DWORD	sector_loc_for_first_dir;

	/* Transaction signature number.
	 * If file transactions are not implemented this must be all zeroes.
	 * */
	ut_DWORD	trans_sig_number;

	/* Mini stream cutoff size.
	 * Should be 00 10 00 00.
	 * */
	ut_DWORD	mini_stream_cutoff_size;

	/* First mini FAT sector location.
	 * Starting sector number for the mini FAT.
	 * Mini FAT is a File Allocation Table (FAT) structure for the mini stream used to allocate
	 * space in a small sector size.
	 * The sector sice for mini FAT is 64 bytes instead of 512 or 4096.
	 * */
	ut_DWORD	first_mini_FAT_sector_loc;

	/* Amount of mini FAT sectors. */
	ut_DWORD 	number_of_mini_FAT_sectors;

	/* Initial sector location for DIFAT. */
	ut_DWORD 	difat_first_sector_loc;

	/* Amount of DIFAT sectors. */
	ut_DWORD	difat_sectors;

	/* The remainder of the 512-byte header, which gets filled in with the value FF. */
	ut_DWORD 	FAT_sector_locations[0x6D];
} _dot_doc_header;

/* FibBase data.
 * This data comes right after `_dot_doc_header`.
 * */
typedef struct dot_doc_Fib
{
    /* This specifies that the binary file is a Word binary file.
     * Should always be EC A5 in binary.
     * */
    ut_WORD     fib_id;

    /* This specifies the version of the file format used.
     * Should always be C1 00 in binary.
     * */
    ut_WORD     fib_format_version;

    /* Ignored as specified by specification. */
    ut_BYTE     padding[2];

    /* Language ID (LID).
     * Probably always gonna be 09 04 (0x0409) in binary, idk. Specification be weird.
     * */
    ut_WORD     lang_ID;

    /* Offset of FIB for the document that contains all the AutoText.
     * If zero, there are no AutoText items attached.
     * */
    ut_WORD     auto_text_FIB_offset;

    /* Bit-values describing:
     * bit 0 - whether or not the document is a document template
     * bit 1 - specifies whether or not the document contains only AutoText
     * bit 2 - specifies the last save operation performed on the document was a incremental save operation
     * bit 3 - if set to zero, there should be no pictures in the document
     * bit 3-7 - if `fib_id` is 0x00D9 or greater, this value should be 0xF
     * */
    ut_BYTE     fib_bit_indications;

    /* Bit-values describing:
     * bit 0 - specifiec whether the document is encrypted or obfuscated
     * bit 1 - specifies the table stream to which the FIB refers to
     * bit 2 - specifies whether the document author recommended that the document be read-only
     * bit 4 - specifies whether the document has a write-reservation password
     * bit 5 - must be set to 1
     * bit 6 - specifies whether to override the language information and font that are specified in the paragraph style at istd 0
     * bit 7 - specifies whether the installation language of the application that created the document was an East Asian Language
     * bit 8 - if bit 0 is set, this specifies whether the document is obfuscated by using xor obfuscation
     * */
    ut_BYTE     fib_bit_indications2;

    /* Should always be 0x00BF or 0x00C1. */
    ut_WORD     fib_back;

    /* If bit 0 and bit 8 are set in `fib_bit_indications2`, this value specifies the
     * xor obfuscation password verifier.
     * If bit 0 is set and bit 8 is unset, this value specifies the size of the `EncryptionHeader` that
     * is stored at the beginning of the Table stream.
     * Else, this value will be zero.
     * */
    ut_DWORD    fib_key;

    /* Ignored. */
    ut_WORD     ignored;

    /* Reserved. */
    ut_WORD     reserved1;
    ut_WORD     reserved2;
    ut_DWORD    reserved3;
    ut_DWORD    reserved4;

    /* Count of 16-bit values corresponding to fibRgW.
     * Must be 0x000E.
     * */
    ut_WORD     fibrgw_csw;

    /* FibRgW97. Ignore it completely. */
    ut_BYTE     FibRgW97[28];

    /* Count of 32-bit values corresponding to fibRgLw.
     * Must be 0x0016.
     * */
    ut_WORD     fibrglw_cslw;

    /* Third section of the FIB. */
    ut_BYTE     fibrglw97[88];
} _dot_doc_Fib;

typedef struct dot_doc
{
    _dot_doc_header     *doc_header;
    _dot_doc_Fib        *doc_FIB;
} _dot_doc;

#endif