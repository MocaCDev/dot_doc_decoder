#include "common.h"

int main(int args, char *argv[])
{
	dot_doc_assert(args > 1, "Expected file as input.\n")
	printf("%ld\n", sizeof(_dot_doc_header));
	beginning:
	/* Open and make sure the file exists. */
	FILE *dot_doc_file = fopen(argv[1], "rb");

	/* If this results in true, the file `dot_doc_file` will be closed and reopened
	 * with `wb` to write new contents if the contents that were invalid while checking
	 * were fixable.
	 * */
	bool got_fixed = false;

	dot_doc_assert(dot_doc_file, "The file `%s` does not exist.\n", argv[1])

	/* Get the file size. */
	fseek(dot_doc_file, 0, SEEK_END);
	ut_LSIZE dot_doc_filesize = ftell(dot_doc_file);
	fseek(dot_doc_file, 0, SEEK_SET);

	/* Make sure there is data in the file. */
	dot_doc_assert(dot_doc_filesize > 1, "The file `%s` is empty.\n", argv[1])
	
	/* Allocate memory to store header. */
	_dot_doc_header *doc_header = calloc(1, sizeof(*doc_header));
	_dot_doc *doc = calloc(1, sizeof(*doc));
	doc->doc_header = calloc(1, sizeof(*doc->doc_header));
	doc->doc_FIB = calloc(1, sizeof(*doc->doc_FIB));
	_dot_doc_Fib *fib = calloc(1, sizeof(*fib));

	fread(doc_header, 1, sizeof(_dot_doc_header), dot_doc_file);
	fread(fib, 1, sizeof(_dot_doc_Fib), dot_doc_file);

	/* Make sure the heading signature is correct.
	 * D0 CF 11 E0 A1 B1 1A E1
	 * */
	ut_BYTE i = 0;
	for(; i < sizeof(doc_header->header_sig)/sizeof(doc_header->header_sig[0]); i++)
	{
		if(!(doc_header->header_sig[i] == CFB_header_signature[i]))
		{
			if(is_same_error(invalid_doc_file_header_sig))
			{
				sprintf(nt_BYTE_PTR err_format, nt_BYTE_CPTR add_on_IDFHS_err,
					get_error_count(),
					argv[1], i + 1, CFB_header_signature[i], doc_header->header_sig[i],
					invalid_doc_file_header_sig, get_error_name(invalid_doc_file_header_sig));
				encountered_error(invalid_doc_file_header_sig, true, err_format);
				goto finish1;
			}
			
			sprintf(nt_BYTE_PTR err_format, nt_BYTE_CPTR new_IDFHS_err,
				argv[1], i + 1, CFB_header_signature[i], doc_header->header_sig[i],
				invalid_doc_file_header_sig, get_error_name(invalid_doc_file_header_sig));
			encountered_error(invalid_doc_file_header_sig, true, err_format);

			finish1:
			doc_header->header_sig[i] = CFB_header_signature[i];
			if(!got_fixed) got_fixed = true;
		}
	}
	
	/* Make sure the 16 bytes of padding are zero and that there is no other
	 * value besides zero.
	 * */
	for(i = 0; i < sizeof(doc_header->padding)/sizeof(doc_header->padding[0]); i++)
	{
		if(!(doc_header->padding[i] == 0))
		{
			/* Check to see if the error we are about to raise is the same as the
			 * previous one.
			 * If it is, format the `add_on_IDFP_err` pointer.
			 * */
			if(is_same_error(invalid_doc_file_padding))
			{
				sprintf(nt_BYTE_PTR err_format, nt_BYTE_CPTR add_on_IDFP_err,
					get_error_count(),
					argv[1], (ut_BYTE) doc_header->padding[i], i + 1,
					invalid_doc_file_padding, get_error_name(invalid_doc_file_padding));
				encountered_error(invalid_doc_file_padding, true, err_format);

				goto finish2;
			}
			
			/* If it is not the same error, format the `new_IDFP_err` pointer. */
			sprintf(nt_BYTE_PTR err_format, nt_BYTE_CPTR new_IDFP_err,
				argv[1], (ut_BYTE) doc_header->padding[i], i + 1,
				invalid_doc_file_padding, get_error_name(invalid_doc_file_padding));
			encountered_error(invalid_doc_file_padding, true, err_format);

			finish2:
			doc_header->padding[i] = 0;
			if(!got_fixed) got_fixed = true;
		}
	}

	/* Check the CFB minor version.
	 * Should always be 3E 00.
	 * */
	if(!(doc_header->CFB_minor_version == CFB_minor_version3E))
	{
		sprintf(nt_BYTE_PTR err_format, nt_BYTE_CPTR new_CFBMV_err,
			argv[1], doc_header->CFB_minor_version,
			invalid_CFB_minor_version, get_error_name(invalid_CFB_minor_version));
		encountered_error(invalid_CFB_minor_version, true, err_format);

		doc_header->CFB_minor_version = CFB_minor_version3E;

		if(!got_fixed) got_fixed = true;
	}

	/* Check the CFB major version.
	 * This should be either 03 00 or 04 00.
	 * */
	if(!(doc_header->CFB_major_version == CFB_major_version_3) &&
		!(doc_header->CFB_major_version == CFB_major_version_4))
	{
		sprintf(nt_BYTE_PTR err_format, nt_BYTE_CPTR new_CFBMJV_err,
			argv[1], doc_header->CFB_major_version,
			invalid_CFB_major_version, get_error_name(invalid_CFB_major_version));
		encountered_error(invalid_CFB_major_version, true, err_format);

		/* Check the sector size, if it is valid.
		 * If it is, assign the major version accordingly.
		 * */
		if(doc_header->CFB_sector_size == sector_size_indication_512) { doc_header->CFB_major_version = CFB_major_version_3; goto finish4; }
		if(doc_header->CFB_sector_size == sector_size_indication_4096) { doc_header->CFB_major_version = CFB_major_version_4; goto finish4; }

		/* If CFB_sector_size is inalid, default ot major version 3. */
		doc_header->CFB_major_version = CFB_major_version_3;

		finish4:
		if(!got_fixed) got_fixed = true;
	}

	/* Check the CFB little-endian indicator.
	 * Should always be FE FF.
	 * */
	if(!(doc_header->CFB_byte_order_indication == little_endian_indication))
	{
		sprintf(nt_BYTE_PTR err_format, nt_BYTE_CPTR new_LEI_err,
			argv[1], doc_header->CFB_byte_order_indication,
			invalid_CFB_little_endian_indication, get_error_name(invalid_CFB_little_endian_indication));
		encountered_error(invalid_CFB_little_endian_indication, true, err_format);

		doc_header->CFB_byte_order_indication = little_endian_indication;

		if(!got_fixed) got_fixed = true;
	}

	/* Check the CFB sector size indication.
	 * This should either be 09 00 or 0C 00 depending on the major version.
	 * If the CFB major version is 3, then this value should be 09 00.
	 * If the CFB major version is 4, then this value should be 0C 00.
	 * */
	switch(doc_header->CFB_major_version)
	{
		case CFB_major_version_3: {
			if(!(doc_header->CFB_sector_size == sector_size_indication_512)) goto raise_sector_indication_err;
			
			goto finalize;
			break;
		}
		case CFB_major_version_4: {
			if(!(doc_header->CFB_sector_size == sector_size_indication_4096)) goto raise_sector_indication_err;
			
			goto finalize;
			break;
		}
		default: dot_doc_error("Unknown Major Version.\n");break;
	}
	
	/* This code only runs via the `goto` statement in the above switch statement. */
	raise_sector_indication_err:
	{
		sprintf(nt_BYTE_PTR err_format, nt_BYTE_CPTR new_SSI_err,
			argv[1], doc_header->CFB_sector_size,
			invalid_CFB_sector_size_indication, get_error_name(invalid_CFB_sector_size_indication));
		encountered_error(invalid_CFB_sector_size_indication, true, err_format);

		/* If `doc_header->CFB_major_version` represents Major Version 4, then set this
		 * to `sector_size_indication4096` else set it to `sector_size_indication_512`.
		 * */
		if(doc_header->CFB_major_version == CFB_major_version_4)
		{
			doc_header->CFB_sector_size = sector_size_indication_4096;

			goto finish3;
		}

		doc_header->CFB_sector_size = sector_size_indication_512;

		finish3:
		if(!got_fixed) got_fixed = true;
	}

	finalize:
	if(got_fixed)
	{
		/* If anything got fixed.. close the file, reopen in `wb` mode and write new
		 * contents.
		 * */
		fclose(dot_doc_file);
		dot_doc_file = fopen(argv[1], "wb");
		dot_doc_assert(dot_doc_file, "Error opening `%s` in Write Byte (WB) mode.\n", argv[1])

		fwrite(doc_header, 1, sizeof(_dot_doc_header), dot_doc_file);
		fclose(dot_doc_file);

		free(doc_header);
		doc_header = NULL;
		got_fixed = false;
		goto beginning;
	}

	printf("\n\nFile: %s.\n\tHeader Signature: \e[0;95m", argv[1]);
	for(i = 0; i < 8; i++)
	{
		if(i != 7) printf("%X ", doc_header->header_sig[i]);
		else printf("%X\e[0;37m\n", doc_header->header_sig[i]);
	}
	printf("\tCFB Minor Version: \e[0;95m0x%X (%s)\e[0;37m\n", doc_header->CFB_minor_version, doc_header->CFB_minor_version == 0x3E ? "Minor Version 3E" : "Unknown Minor Version");
	printf("\tCFB Major Version: \e[0;95m0x%X (%s)\e[0;37m\n", doc_header->CFB_major_version, doc_header->CFB_major_version == 0x03 ? "Major Version 3" : "Major Version 4");
	printf("\tCFB Byte Order Indication: \e[0;95m0x%X (%s)\e[0;37m\n", doc_header->CFB_byte_order_indication, doc_header->CFB_byte_order_indication == little_endian_indication ? "Little Endian" : "Unknown Byte Order");
	printf("\tCFB Sector Size Indication: \e[0;95m0x%X (%s)\e[0;37m\n", doc_header->CFB_sector_size, doc_header->CFB_sector_size == 0x09 ? "512 Byte Sector" : "4096 Byte Sector");
	printf("\tCFB Mini Sector Size: \e[0;95m0x%X\e[0;37m\n", doc_header->CFB_mini_sector_size);
	printf("\tNumber Of FAT Sectors: \e[0;95m0x%X\e[0;37m\n", doc_header->FAT_sectors);
	printf("\tSector Location For First Directory: \e[0;95m0x%X\e[0;37m\n", doc_header->sector_loc_for_first_dir);
	printf("\tTransaction Signature Number: \e[0;95m0x%X (%s)\e[0;37m\n", doc_header->trans_sig_number, doc_header->trans_sig_number == 0 ? "File Transactions Not Implemented" : "File Transactions Implemented");
	printf("\tMini Stream Cutoff Size: \e[0;95m0x%X\e[0;37m\n", doc_header->mini_stream_cutoff_size);
	printf("\tFirst Minit FAT Sector Location: \e[0;95m0x%X\e[0;37m\n", doc_header->first_mini_FAT_sector_loc);
	printf("\tNumber Of Mini FAT Sectors: \e[0;95m0x%X\e[0;37m\n", doc_header->number_of_mini_FAT_sectors);
	printf("\tDIFAT First Sector Location: \e[0;95m0x%X\e[0;37m\n", doc_header->difat_first_sector_loc);
	printf("\tNumber Of DIFAT Sectors: \e[0;95m0x%X\e[0;37m\n", doc_header->difat_sectors);
	printf("\tFAT Sector Locations: \e[0;95m");
	for(i = 0; i < 0x6D; i++)
	{

		if(doc_header->FAT_sector_locations[i] != 0x00000000 &&
		   doc_header->FAT_sector_locations[i + 1] != 0xFFFFFFFF)
			printf("0x%X, ", doc_header->FAT_sector_locations[i]);
		
		if(doc_header->FAT_sector_locations[i + 1] == 0xFFFFFFFF && doc_header->FAT_sector_locations[i] != 0x00000000)
			printf("0x%X\e[0;37m\n\n", doc_header->FAT_sector_locations[i]);

		if(doc_header->FAT_sector_locations[i] == 0xFFFFFFFF || doc_header->FAT_sector_locations[i + 1] == 0xFFFFFFFF) break;
	}


	printf("\n\n%X\n", fib->fib_id);
	free(doc_header);
	fclose(dot_doc_file);	
	return 0;
}
