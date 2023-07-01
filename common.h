#ifndef dot_doc_common
#define dot_doc_common
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

typedef char				nt_BYTE;
typedef unsigned char		ut_BYTE;
typedef signed char			st_BYTE;
#define nt_BYTE_PTR			(nt_BYTE *)
#define nt_BYTE_CPTR		(const nt_BYTE *)
#define ut_BYTE_PTR			(ut_BYTE *)
#define ut_BYTE_CPTR		(const ut_BYTE *)
#define st_BYTE_PTR			(st_BYTE *)
#define st_BYTE_CPTR		(const st_BYTE *)

typedef short				nt_WORD;
typedef unsigned short		ut_WORD;
typedef signed short		st_WORD;
#define nt_WORD_PTR     	(nt_WORD *)
#define nt_WORD_CPTR    	(const nt_WORD *)
#define ut_WORD_PTR			(ut_WORD *)
#define ut_WORD_CPTR		(const ut_WORD *)
#define st_WORD_PTR			(st_WORD *)
#define st_WORD_CPTR		(const st_WORD *)

typedef int					nt_DWORD;
typedef unsigned int		ut_DWORD;
typedef signed int			st_DWORD;
#define nt_DWORD_PTR		(nt_DWORD *)
#define nt_DWORD_CPTR		(const nt_DWORD *)
#define ut_DWORD_PTR		(ut_DWORD *)
#define ut_DWORD_CPTR		(const ut_DWORD *)
#define st_DWORD_PTR		(st_DWORD *)
#define st_DWORD_CPTR		(const st_DWORD *)

typedef long				nt_LBYTE;
typedef unsigned long		ut_LBYTE;
typedef signed long			st_LBYTE;
#define nt_LBYTE_PTR		(nt_LBYTE *)
#define nt_LBYTE_CPTR		(const nt_LBYTE *)
#define ut_LBYTE_PTR		(ut_LBYTE *)
#define ut_LBYTE_CPTR		(const ut_LBYTE *)
#define st_LBYTE_PTR		(st_LBYTE *)
#define st_LBYTE_CPTR		(const st_LBYTE *)

typedef long long			nt_LLBYTE;
typedef unsigned long long	ut_LLBYTE;
typedef signed long long	st_LLBYTE;
#define nt_LLBYTE_PTR		(nt_LLBYTE *)
#define nt_LLBYTE_CPTR		(const nt_LLBYTE *)
#define ut_LLBYTE_PTR		(ut_LLBYTE *)
#define ut_LLBYTE_CPTR		(const ut_LLBYTE *)
#define st_LLBYTE_PTR		(st_LLBYTE *)
#define st_LLBYTE_CPTR		(const st_LLBYTE *)

typedef ut_LLBYTE			ut_LSIZE;
#define ut_LSIZE_PTR		(ut_LSIZE *)
#define ut_LSIZE_CPTR		(const ut_LSIZE *)

typedef ut_BYTE				bool;
#define true				1
#define false				0

/* Colors for printing. */
#define red					ut_BYTE_CPTR "\e[0;31m"
#define yellow				ut_BYTE_CPTR "\e[0;33m"
#define green				ut_BYTE_CPTR "\e[0;32m"
#define bgreen				ut_BYTE_CPTR "\e[1;32m]"
#define white				ut_BYTE_CPTR "\e[0;37m"
#define reset				white

/* Custom asserts. */
#define dot_doc_error(msg, ...)			\
{						\
	fprintf(stderr, msg, ##__VA_ARGS__);	\
	exit(EXIT_FAILURE);			\
}
#define dot_doc_assert(cond, msg, ...)		\
	if(!(cond)) 				\
		dot_doc_error(msg, ##__VA_ARGS__)

#include "error.h"
#include "dot_doc.h"

#endif