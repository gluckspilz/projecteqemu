/* -*- buffer-read-only: t -*-
 *
 *    opnames.h
 *
 *    Copyright (C) 1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006,
 *    2007, 2008 by Larry Wall and others
 *
 *    You may distribute under the terms of either the GNU General Public
 *    License or the Artistic License, as specified in the README file.
 *
 *
 * !!!!!!!   DO NOT EDIT THIS FILE   !!!!!!!
 *  This file is built by opcode.pl from its data.  Any changes made here
 *  will be lost!
 */

typedef enum opcode {
	OP_NULL		 = 0,
	OP_STUB		 = 1,
	OP_SCALAR	 = 2,
	OP_PUSHMARK	 = 3,
	OP_WANTARRAY	 = 4,
	OP_CONST	 = 5,
	OP_GVSV		 = 6,
	OP_GV		 = 7,
	OP_GELEM	 = 8,
	OP_PADSV	 = 9,
	OP_PADAV	 = 10,
	OP_PADHV	 = 11,
	OP_PADANY	 = 12,
	OP_PUSHRE	 = 13,
	OP_RV2GV	 = 14,
	OP_RV2SV	 = 15,
	OP_AV2ARYLEN	 = 16,
	OP_RV2CV	 = 17,
	OP_ANONCODE	 = 18,
	OP_PROTOTYPE	 = 19,
	OP_REFGEN	 = 20,
	OP_SREFGEN	 = 21,
	OP_REF		 = 22,
	OP_BLESS	 = 23,
	OP_BACKTICK	 = 24,
	OP_GLOB		 = 25,
	OP_READLINE	 = 26,
	OP_RCATLINE	 = 27,
	OP_REGCMAYBE	 = 28,
	OP_REGCRESET	 = 29,
	OP_REGCOMP	 = 30,
	OP_MATCH	 = 31,
	OP_QR		 = 32,
	OP_SUBST	 = 33,
	OP_SUBSTCONT	 = 34,
	OP_TRANS	 = 35,
	OP_SASSIGN	 = 36,
	OP_AASSIGN	 = 37,
	OP_CHOP		 = 38,
	OP_SCHOP	 = 39,
	OP_CHOMP	 = 40,
	OP_SCHOMP	 = 41,
	OP_DEFINED	 = 42,
	OP_UNDEF	 = 43,
	OP_STUDY	 = 44,
	OP_POS		 = 45,
	OP_PREINC	 = 46,
	OP_I_PREINC	 = 47,
	OP_PREDEC	 = 48,
	OP_I_PREDEC	 = 49,
	OP_POSTINC	 = 50,
	OP_I_POSTINC	 = 51,
	OP_POSTDEC	 = 52,
	OP_I_POSTDEC	 = 53,
	OP_POW		 = 54,
	OP_MULTIPLY	 = 55,
	OP_I_MULTIPLY	 = 56,
	OP_DIVIDE	 = 57,
	OP_I_DIVIDE	 = 58,
	OP_MODULO	 = 59,
	OP_I_MODULO	 = 60,
	OP_REPEAT	 = 61,
	OP_ADD		 = 62,
	OP_I_ADD	 = 63,
	OP_SUBTRACT	 = 64,
	OP_I_SUBTRACT	 = 65,
	OP_CONCAT	 = 66,
	OP_STRINGIFY	 = 67,
	OP_LEFT_SHIFT	 = 68,
	OP_RIGHT_SHIFT	 = 69,
	OP_LT		 = 70,
	OP_I_LT		 = 71,
	OP_GT		 = 72,
	OP_I_GT		 = 73,
	OP_LE		 = 74,
	OP_I_LE		 = 75,
	OP_GE		 = 76,
	OP_I_GE		 = 77,
	OP_EQ		 = 78,
	OP_I_EQ		 = 79,
	OP_NE		 = 80,
	OP_I_NE		 = 81,
	OP_NCMP		 = 82,
	OP_I_NCMP	 = 83,
	OP_SLT		 = 84,
	OP_SGT		 = 85,
	OP_SLE		 = 86,
	OP_SGE		 = 87,
	OP_SEQ		 = 88,
	OP_SNE		 = 89,
	OP_SCMP		 = 90,
	OP_BIT_AND	 = 91,
	OP_BIT_XOR	 = 92,
	OP_BIT_OR	 = 93,
	OP_NEGATE	 = 94,
	OP_I_NEGATE	 = 95,
	OP_NOT		 = 96,
	OP_COMPLEMENT	 = 97,
	OP_SMARTMATCH	 = 98,
	OP_ATAN2	 = 99,
	OP_SIN		 = 100,
	OP_COS		 = 101,
	OP_RAND		 = 102,
	OP_SRAND	 = 103,
	OP_EXP		 = 104,
	OP_LOG		 = 105,
	OP_SQRT		 = 106,
	OP_INT		 = 107,
	OP_HEX		 = 108,
	OP_OCT		 = 109,
	OP_ABS		 = 110,
	OP_LENGTH	 = 111,
	OP_SUBSTR	 = 112,
	OP_VEC		 = 113,
	OP_INDEX	 = 114,
	OP_RINDEX	 = 115,
	OP_SPRINTF	 = 116,
	OP_FORMLINE	 = 117,
	OP_ORD		 = 118,
	OP_CHR		 = 119,
	OP_CRYPT	 = 120,
	OP_UCFIRST	 = 121,
	OP_LCFIRST	 = 122,
	OP_UC		 = 123,
	OP_LC		 = 124,
	OP_QUOTEMETA	 = 125,
	OP_RV2AV	 = 126,
	OP_AELEMFAST	 = 127,
	OP_AELEM	 = 128,
	OP_ASLICE	 = 129,
	OP_AEACH	 = 130,
	OP_AKEYS	 = 131,
	OP_AVALUES	 = 132,
	OP_EACH		 = 133,
	OP_VALUES	 = 134,
	OP_KEYS		 = 135,
	OP_DELETE	 = 136,
	OP_EXISTS	 = 137,
	OP_RV2HV	 = 138,
	OP_HELEM	 = 139,
	OP_HSLICE	 = 140,
	OP_BOOLKEYS	 = 141,
	OP_UNPACK	 = 142,
	OP_PACK		 = 143,
	OP_SPLIT	 = 144,
	OP_JOIN		 = 145,
	OP_LIST		 = 146,
	OP_LSLICE	 = 147,
	OP_ANONLIST	 = 148,
	OP_ANONHASH	 = 149,
	OP_SPLICE	 = 150,
	OP_PUSH		 = 151,
	OP_POP		 = 152,
	OP_SHIFT	 = 153,
	OP_UNSHIFT	 = 154,
	OP_SORT		 = 155,
	OP_REVERSE	 = 156,
	OP_GREPSTART	 = 157,
	OP_GREPWHILE	 = 158,
	OP_MAPSTART	 = 159,
	OP_MAPWHILE	 = 160,
	OP_RANGE	 = 161,
	OP_FLIP		 = 162,
	OP_FLOP		 = 163,
	OP_AND		 = 164,
	OP_OR		 = 165,
	OP_XOR		 = 166,
	OP_DOR		 = 167,
	OP_COND_EXPR	 = 168,
	OP_ANDASSIGN	 = 169,
	OP_ORASSIGN	 = 170,
	OP_DORASSIGN	 = 171,
	OP_METHOD	 = 172,
	OP_ENTERSUB	 = 173,
	OP_LEAVESUB	 = 174,
	OP_LEAVESUBLV	 = 175,
	OP_CALLER	 = 176,
	OP_WARN		 = 177,
	OP_DIE		 = 178,
	OP_RESET	 = 179,
	OP_LINESEQ	 = 180,
	OP_NEXTSTATE	 = 181,
	OP_DBSTATE	 = 182,
	OP_UNSTACK	 = 183,
	OP_ENTER	 = 184,
	OP_LEAVE	 = 185,
	OP_SCOPE	 = 186,
	OP_ENTERITER	 = 187,
	OP_ITER		 = 188,
	OP_ENTERLOOP	 = 189,
	OP_LEAVELOOP	 = 190,
	OP_RETURN	 = 191,
	OP_LAST		 = 192,
	OP_NEXT		 = 193,
	OP_REDO		 = 194,
	OP_DUMP		 = 195,
	OP_GOTO		 = 196,
	OP_EXIT		 = 197,
	OP_METHOD_NAMED	 = 198,
	OP_ENTERGIVEN	 = 199,
	OP_LEAVEGIVEN	 = 200,
	OP_ENTERWHEN	 = 201,
	OP_LEAVEWHEN	 = 202,
	OP_BREAK	 = 203,
	OP_CONTINUE	 = 204,
	OP_OPEN		 = 205,
	OP_CLOSE	 = 206,
	OP_PIPE_OP	 = 207,
	OP_FILENO	 = 208,
	OP_UMASK	 = 209,
	OP_BINMODE	 = 210,
	OP_TIE		 = 211,
	OP_UNTIE	 = 212,
	OP_TIED		 = 213,
	OP_DBMOPEN	 = 214,
	OP_DBMCLOSE	 = 215,
	OP_SSELECT	 = 216,
	OP_SELECT	 = 217,
	OP_GETC		 = 218,
	OP_READ		 = 219,
	OP_ENTERWRITE	 = 220,
	OP_LEAVEWRITE	 = 221,
	OP_PRTF		 = 222,
	OP_PRINT	 = 223,
	OP_SAY		 = 224,
	OP_SYSOPEN	 = 225,
	OP_SYSSEEK	 = 226,
	OP_SYSREAD	 = 227,
	OP_SYSWRITE	 = 228,
	OP_EOF		 = 229,
	OP_TELL		 = 230,
	OP_SEEK		 = 231,
	OP_TRUNCATE	 = 232,
	OP_FCNTL	 = 233,
	OP_IOCTL	 = 234,
	OP_FLOCK	 = 235,
	OP_SEND		 = 236,
	OP_RECV		 = 237,
	OP_SOCKET	 = 238,
	OP_SOCKPAIR	 = 239,
	OP_BIND		 = 240,
	OP_CONNECT	 = 241,
	OP_LISTEN	 = 242,
	OP_ACCEPT	 = 243,
	OP_SHUTDOWN	 = 244,
	OP_GSOCKOPT	 = 245,
	OP_SSOCKOPT	 = 246,
	OP_GETSOCKNAME	 = 247,
	OP_GETPEERNAME	 = 248,
	OP_LSTAT	 = 249,
	OP_STAT		 = 250,
	OP_FTRREAD	 = 251,
	OP_FTRWRITE	 = 252,
	OP_FTREXEC	 = 253,
	OP_FTEREAD	 = 254,
	OP_FTEWRITE	 = 255,
	OP_FTEEXEC	 = 256,
	OP_FTIS		 = 257,
	OP_FTSIZE	 = 258,
	OP_FTMTIME	 = 259,
	OP_FTATIME	 = 260,
	OP_FTCTIME	 = 261,
	OP_FTROWNED	 = 262,
	OP_FTEOWNED	 = 263,
	OP_FTZERO	 = 264,
	OP_FTSOCK	 = 265,
	OP_FTCHR	 = 266,
	OP_FTBLK	 = 267,
	OP_FTFILE	 = 268,
	OP_FTDIR	 = 269,
	OP_FTPIPE	 = 270,
	OP_FTSUID	 = 271,
	OP_FTSGID	 = 272,
	OP_FTSVTX	 = 273,
	OP_FTLINK	 = 274,
	OP_FTTTY	 = 275,
	OP_FTTEXT	 = 276,
	OP_FTBINARY	 = 277,
	OP_CHDIR	 = 278,
	OP_CHOWN	 = 279,
	OP_CHROOT	 = 280,
	OP_UNLINK	 = 281,
	OP_CHMOD	 = 282,
	OP_UTIME	 = 283,
	OP_RENAME	 = 284,
	OP_LINK		 = 285,
	OP_SYMLINK	 = 286,
	OP_READLINK	 = 287,
	OP_MKDIR	 = 288,
	OP_RMDIR	 = 289,
	OP_OPEN_DIR	 = 290,
	OP_READDIR	 = 291,
	OP_TELLDIR	 = 292,
	OP_SEEKDIR	 = 293,
	OP_REWINDDIR	 = 294,
	OP_CLOSEDIR	 = 295,
	OP_FORK		 = 296,
	OP_WAIT		 = 297,
	OP_WAITPID	 = 298,
	OP_SYSTEM	 = 299,
	OP_EXEC		 = 300,
	OP_KILL		 = 301,
	OP_GETPPID	 = 302,
	OP_GETPGRP	 = 303,
	OP_SETPGRP	 = 304,
	OP_GETPRIORITY	 = 305,
	OP_SETPRIORITY	 = 306,
	OP_TIME		 = 307,
	OP_TMS		 = 308,
	OP_LOCALTIME	 = 309,
	OP_GMTIME	 = 310,
	OP_ALARM	 = 311,
	OP_SLEEP	 = 312,
	OP_SHMGET	 = 313,
	OP_SHMCTL	 = 314,
	OP_SHMREAD	 = 315,
	OP_SHMWRITE	 = 316,
	OP_MSGGET	 = 317,
	OP_MSGCTL	 = 318,
	OP_MSGSND	 = 319,
	OP_MSGRCV	 = 320,
	OP_SEMOP	 = 321,
	OP_SEMGET	 = 322,
	OP_SEMCTL	 = 323,
	OP_REQUIRE	 = 324,
	OP_DOFILE	 = 325,
	OP_HINTSEVAL	 = 326,
	OP_ENTEREVAL	 = 327,
	OP_LEAVEEVAL	 = 328,
	OP_ENTERTRY	 = 329,
	OP_LEAVETRY	 = 330,
	OP_GHBYNAME	 = 331,
	OP_GHBYADDR	 = 332,
	OP_GHOSTENT	 = 333,
	OP_GNBYNAME	 = 334,
	OP_GNBYADDR	 = 335,
	OP_GNETENT	 = 336,
	OP_GPBYNAME	 = 337,
	OP_GPBYNUMBER	 = 338,
	OP_GPROTOENT	 = 339,
	OP_GSBYNAME	 = 340,
	OP_GSBYPORT	 = 341,
	OP_GSERVENT	 = 342,
	OP_SHOSTENT	 = 343,
	OP_SNETENT	 = 344,
	OP_SPROTOENT	 = 345,
	OP_SSERVENT	 = 346,
	OP_EHOSTENT	 = 347,
	OP_ENETENT	 = 348,
	OP_EPROTOENT	 = 349,
	OP_ESERVENT	 = 350,
	OP_GPWNAM	 = 351,
	OP_GPWUID	 = 352,
	OP_GPWENT	 = 353,
	OP_SPWENT	 = 354,
	OP_EPWENT	 = 355,
	OP_GGRNAM	 = 356,
	OP_GGRGID	 = 357,
	OP_GGRENT	 = 358,
	OP_SGRENT	 = 359,
	OP_EGRENT	 = 360,
	OP_GETLOGIN	 = 361,
	OP_SYSCALL	 = 362,
	OP_LOCK		 = 363,
	OP_ONCE		 = 364,
	OP_CUSTOM	 = 365,
	OP_max		
} opcode;

#define MAXO 366
#define OP_phoney_INPUT_ONLY -1
#define OP_phoney_OUTPUT_ONLY -2


/* the OP_IS_(SOCKET|FILETEST) macros are optimized to a simple range
    check because all the member OPs are contiguous in opcode.pl
    <DATA> table.  opcode.pl verifies the range contiguity.  */

#define OP_IS_SOCKET(op)	\
	((op) >= OP_SEND && (op) <= OP_GETPEERNAME)

#define OP_IS_FILETEST(op)	\
	((op) >= OP_FTRREAD && (op) <= OP_FTBINARY)

#define OP_IS_FILETEST_ACCESS(op)	\
	((op) >= OP_FTRREAD && (op) <= OP_FTEEXEC)

/* ex: set ro: */
