#ifndef error
#define error

enum error_type
{
        /* There was an invalid value in the signature. */
        invalid_doc_file_header_sig = 0xE0,
        /* Either a non-zero value, or the value was not 0xFF. */
        invalid_doc_file_padding = 0xE2,
        /* The CFB minor version was not 0x3E. */
        invalid_CFB_minor_version = 0xE4,
        /* The CFB major version was not 0x03 for major version 3 or 0x04 for major version 4. */
        invalid_CFB_major_version = 0xE6,
        /* The CFB little endian indication was not 0xFE 0xFF. */
        invalid_CFB_little_endian_indication = 0xE8,
        /* The CFB sector size indication was not 0x09 or 0x0C. */
        invalid_CFB_sector_size_indication = 0xEA,
        /* There was no error :D */
        no_error = 0x0
};

/* Specific error messages for `invalid_doc_file_header_sig`.
 * IDFHS - Invalid Doc File Header Signature.
 * */
ut_BYTE *new_IDFHS_err = ut_BYTE_PTR "\e[0;31mAn error occurred in the header of `%s`:\e[0;37m\n\tThe error occurred due to an invalid value in the 8-byte header signature.\n\tAt index %d of the 8-byte header signature, the value to be expected was \e[1;33m`%X`\e[0;37m. Instead, the program encountered \e[1;31m`%X`\e[0;37m.\n\tError Code: %X (%s)\n";
ut_BYTE *add_on_IDFHS_err = ut_BYTE_PTR "\n\t\e[0;31mException #%d:\e[0;37m\n\t\tThe error occurred in `%s` due to an invalid value in the 8-byte header signature.\n\t\tAt index %d of the 8-byte header signature, the value to be expected was \e[1;33m`%X`\e[0;37m. Instead, the program encountered \e[1;31m`%X`\e[0;37m.\n\t\tError Code: %X (%s)\n";

/* Specific error messages for `invalid_doc_file_padding`.
 * IDFP - Invalid Doc File Padding.
 * */
ut_BYTE *new_IDFP_err = ut_BYTE_PTR "\e[0;31mAn error occurred in the header of `%s`:\e[0;37m\n\tThe error occurred due to an invalid value in the 16-bytes of padding.\n\tThe 16-bytes of padding are supposed to be represented by \e[1;33m`0x00`\e[0;37m. Instead, the program encountered \e[1;31m`%X`\e[0;37m at index %d.\n\tError Code: %X (%s)\n";
ut_BYTE *add_on_IDFP_err = ut_BYTE_PTR "\n\t\e[0;31mException #%d:\e[0;37m\n\t\tThe error occurred in `%s` due to an invalid value in the 16-bytes of padding.\n\t\tThe 16-bytes of padding are supposed to be represented by \e[1;33m`0x00`\e[0;37m. Instead, the program encountered \e[1;31m`%X`\e[0;37m at index %d.\n\t\tError Code: %X (%s)\n";

/* Specific error message for `invalid_CFB_minor_version`.
 * CFBMV - CFB Minor Version.
 * */
ut_BYTE *new_CFBMV_err = ut_BYTE_PTR "\e[0;31mAn error occurred in the header of `%s`:\e[0;37m\n\tThe error occurred due to an invalid minor version value.\n\tThe value to be expected was \e[1;33m`3E 00`\e[0;37m. Instead, the program encountered \e[1;31m`%X`\e[0;37m.\n\tError Code: %X (%s)\n";

/* Specific error message for `invalid_CFB_major_version`.
 * CFBMJV - CFB Major Version.
 * */
ut_BYTE *new_CFBMJV_err = ut_BYTE_PTR "\e[0;31mAn error occurred in the header of `%s`:\e[0;37m\n\tThe error occurred due to an invalid major version value.\n\tThe value to be expected was either \e[1;33m`03 00`\e[0;37m or \e[1;33m`04 00`\e[0;37m.\n\tInstead, the program encountered \e[1;31m`%X`\e[0;37m.\n\tError Code: %X (%s)\n";

/* Specific error message for `invalid_CFB_little_endian_indication`.
 * LEI - Little Endian Indication.
 * */
ut_BYTE *new_LEI_err = ut_BYTE_PTR "\e[0;31mAn error occurred in the header of `%s`:\e[0;37m\n\tThe error occurred due to an invalid CFB little endian indication value.\n\tThe value to be expected was \e[1;33m`FE FF`\e[0;37m. Instead, the program encountered \e[1;31m`%X`\e[0;37m.\n\tError Code: %X (%s)\n";

/* Specific error message for `invalid_CFB_sector_size_indication`.
 * SSI - Sector Size Indication.
 * */
ut_BYTE *new_SSI_err = ut_BYTE_PTR "\e[0;31mAn error occurred in the header of `%s`:\e[0;37m\n\tThe error occurred due to an invalid CFB sector size indication value.\n\tThe value to be expected was either \e[1;33m`09 00`\e[0;37m or \e[1;33m`0C 00`\e[0;37m.\n\tInstead, the program encountered \e[1;31m`%X`\e[0;37m.\n\tError Code: %X (%s)\n";

ut_BYTE err_format[400];

typedef struct error_tracker
{
        enum error_type         current_error;
        enum error_type         previous_error;

        /* If the error is not the same as `previous_error`, this will be set to one.
         * If the error is the same as `previous_error`, this will continue to increment.
         * This will be set to 1 by default.
         * */
        ut_BYTE                 same_error_count;

        /* How many errors were encountered? */
        ut_WORD                 err_count;

        /* If this is false then the program will go ahead and exit with a failed status.
         * This will be set to true by default and, upon being set to false, will stay false so the program can
         * exit with an error.
         * */
        bool                    all_errors_fixed;
} _error_tracker;

static _error_tracker err_tracker = {
        .current_error = no_error,
        .previous_error = no_error,
        .same_error_count = 1,
        .err_count = 0,
        .all_errors_fixed = true
};

/* get_error_count - get the value of `same_error_count`. This will be used upon `is_same_error` returning true.
 *
 * Arguments: none
 *
 * Returns: byte value (`err_tracker.same_error_count`).
 *
 * */
ut_BYTE get_error_count()
{
        return err_tracker.same_error_count;
}

/* is_same_error - function that will be called to determine whether or not an error that is about to be raised is the same as the previous error.
 *
 * Arguments:
 *      enum error_type error_to_be_raised - the error that is going to be raised via `encountered_error`.
 *
 * Returns: true if `error_to_be_raised` is the same as `err_tracker.previous_error`, else false.
 *
 * */
bool is_same_error(enum error_type error_to_be_raised)
{
    if(err_tracker.err_count == 0)
		return false;

	if(err_tracker.previous_error == error_to_be_raised || err_tracker.current_error == error_to_be_raised)
		return true;
	
	return false;
}

/* get_error_name - get the name of the error.
 *
 * Arguments:
 *    enum error_type error - the error that is encountered
 *
 * Returns: the name of the error.
 *
 * */
ut_BYTE *get_error_name(enum error_type err)
{
    switch(err)
    {
        case invalid_doc_file_header_sig: return ut_BYTE_PTR "Invalid Header Signature";break;
        case invalid_doc_file_padding: return ut_BYTE_PTR "Invalid Padding";break;
        case invalid_CFB_minor_version: return ut_BYTE_PTR "Invalid CFB Minor Version";break;
        case invalid_CFB_major_version: return ut_BYTE_PTR "Invalid CFB Major Version";break;
        case invalid_CFB_little_endian_indication: return ut_BYTE_PTR "Invalid Little-Endian Indicator";break;
        case invalid_CFB_sector_size_indication: return ut_BYTE_PTR "Invalid CFB Sector Size Indication";break;
        default: break;
    }

    return "Unknown Error";
}

/* encountered_error - if an error is encountered, this function will be called.
 *
 * Arguments:
 *      enum error_type err_encountered - what error was encountered?
 *      ut_BYTE *err_msg - the error message to print.
 *      bool can_fix - if the error can be fixed by the program then this will be true. If this is false, `err_tracker.all_errors_fixed` will be set to false.
 *
 * Returns: nothing
 * */
void encountered_error(enum error_type err_encountered, bool can_fix, ut_BYTE *err_msg)
{
        /* If `err_tracker.current_error` does not represent `no_error`, assign `err_tracker.previous_error` to whatever the last error was
         * before `encountered_error` got called again.
         * */
        if(err_tracker.current_error != no_error)
            err_tracker.previous_error = err_tracker.current_error;

        /* Check to see if `err_tracker.err_count` is greater than zero. If it is, check to see if the error being encountered
         * is the same as the error that was encountered prior.
         * */
        if(err_tracker.err_count >= 1 && err_tracker.previous_error == err_encountered)
        {
                /* Increase the amount of same errors. */
                err_tracker.same_error_count++;

                /* Make sure `err_msg` is not NULL, then print. */
                dot_doc_assert(err_msg, "Internal program error.\n")
                printf("%s", err_msg);
                
                /* If `err_tracker.current_error` does not represent `err_encountered`, go ahead and assign the new error encountered to `err_encountered`.
                 * */
                if(err_tracker.current_error != err_encountered)
                    err_tracker.current_error = err_encountered;

                goto encountered_error_end;
        }

        printf("\n");
    
        err_tracker.current_error = err_encountered;
        dot_doc_assert(err_msg, "Internal program error.\n")
        printf("%s", err_msg);

        err_tracker.same_error_count = 1;

        encountered_error_end:
        if(!can_fix) err_tracker.all_errors_fixed = false;

        err_tracker.err_count++;
}

#endif
