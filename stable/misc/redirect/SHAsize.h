#ifndef _SIZEDEF_H
#define _SIZEDEF_H 1

#define DATAMOD_ILP32  1


#ifdef DATAMOD_LP64 
	/** int Ϊ32λ��long = Pointer = 64λ */

	typedef unsigned char SZ_UBYTE1;
	typedef unsigned short SZ_UBYTE2;
	typedef signed char SZ_BYTE1;
	typedef signed short SZ_BYTE2;

	typedef float SZ_FLOAT;
	typedef double SZ_DOUBLE;
	typedef unsigned char SZ_UCHAR;
	typedef signed char SZ_CHAR;
	typedef unsigned short SZ_USHORT;
	typedef signed short SZ_SHORT;
	typedef signed short SZ_WCHAR;
	typedef unsigned short SZ_UWCHAR;

	typedef unsigned int SZ_UBYTE4;
	typedef signed int SZ_BYTE4;
	typedef unsigned long SZ_UBYTE8;
	typedef signed long SZ_BYTE8;

	typedef unsigned int SZ_UINT;
	typedef signed int SZ_INT;
	typedef unsigned long SZ_ULONG;
	typedef signed long SZ_LONG;

	typedef unsigned long SZ_UPTR;
	typedef signed long SZ_PTR;
	typedef unsigned long SZ_URESULT;
	typedef signed long SZ_RESULT;
	typedef unsigned long SZ_UT;
	typedef signed long SZ_T;

#elif DATAMOD_ILP64 	
	/*  int �� 64λ, long Ϊ 64λ */

	typedef unsigned char SZ_UBYTE1;
	typedef unsigned short SZ_UBYTE2;
	typedef signed char SZ_BYTE1;
	typedef signed short SZ_BYTE2;

	typedef float SZ_FLOAT;
	typedef double SZ_DOUBLE;
	typedef unsigned char SZ_UCHAR;
	typedef signed char SZ_CHAR;
	typedef unsigned short SZ_USHORT;
	typedef signed short SZ_SHORT;
	typedef signed short SZ_WCHAR;
	typedef unsigned short SZ_UWCHAR;

	typedef unsigned int SZ_UBYTE4;
	typedef signed int SZ_BYTE4;
	typedef unsigned long SZ_UBYTE8;
	typedef signed long SZ_BYTE8;

	typedef unsigned int SZ_UINT;
	typedef signed int SZ_INT;
	typedef unsigned long SZ_ULONG;
	typedef signed long SZ_LONG;

	typedef unsigned long SZ_UPTR;
	typedef signed long SZ_PTR;
	typedef unsigned long SZ_UT;
	typedef signed long SZ_T;

#elif DATAMOD_LLP64 
	/* int Ϊ 32λ��long Ϊ 32λ */

	typedef unsigned char SZ_UBYTE1;
	typedef unsigned short SZ_UBYTE2;
	typedef signed char SZ_BYTE1;
	typedef signed short SZ_BYTE2;

	typedef float SZ_FLOAT;
	typedef double SZ_DOUBLE;
	typedef unsigned char SZ_UCHAR;
	typedef signed char SZ_CHAR;
	typedef unsigned short SZ_USHORT;
	typedef signed short SZ_SHORT;
	typedef signed short SZ_WCHAR;
	typedef unsigned short SZ_UWCHAR;

	typedef unsigned int SZ_UBYTE4;
	typedef signed int SZ_BYTE4;
	typedef unsigned long long SZ_UBYTE8; /* ����ʵ���н��䶨����� */
	typedef signed long long SZ_BYTE8;

	typedef unsigned int SZ_UINT;
	typedef signed int SZ_INT;
	typedef unsigned long long SZ_ULONG;
	typedef signed long long SZ_LONG;
	
	typedef unsigned long long SZ_UPTR;
	typedef signed long long SZ_PTR;
	typedef unsigned long long SZ_UT;
	typedef signed long long SZ_T;

#elif DATAMOD_ILP32 
	/* int Ϊ 32λ, long Ϊ32 λ */
	typedef unsigned char SZ_UBYTE1;
	typedef unsigned short SZ_UBYTE2;
	typedef signed char SZ_BYTE1;
	typedef signed short SZ_BYTE2;

	typedef float SZ_FLOAT;
	typedef double SZ_DOUBLE;
	typedef unsigned char SZ_UCHAR;
	typedef signed char SZ_CHAR;
	typedef unsigned short SZ_USHORT;
	typedef signed short SZ_SHORT;
	typedef signed short SZ_WCHAR;
	typedef unsigned short SZ_UWCHAR;


	typedef unsigned int SZ_UBYTE4;
	typedef signed int SZ_BYTE4;
	typedef unsigned long long  SZ_UBYTE8; /* ����ʵ���н��䶨����� */
	typedef signed long long SZ_BYTE8;

	typedef unsigned int SZ_UINT;
	typedef signed int SZ_INT;
	typedef unsigned long long SZ_ULONG;
	typedef signed long long SZ_LONG;

	typedef unsigned int SZ_UPTR;
	typedef signed int SZ_PTR;
	typedef unsigned long SZ_UT;
	typedef signed long SZ_T;

#else 
	/* DATAMOD_LP32 int Ϊ 16λ */
	typedef unsigned char SZ_UBYTE1;
	typedef unsigned short SZ_UBYTE2;
	typedef signed char SZ_BYTE1;
	typedef signed short SZ_BYTE2;

	typedef float SZ_FLOAT;
	typedef double SZ_DOUBLE;
	typedef unsigned char SZ_UCHAR;
	typedef signed char SZ_CHAR;
	typedef unsigned short SZ_USHORT;
	typedef signed short SZ_SHORT;
	typedef signed short SZ_WCHAR;
	typedef unsigned short SZ_UWCHAR;

	typedef unsigned long SZ_UBYTE4;
	typedef signed long SZ_BYTE4;
	typedef unsigned long long SZ_UBYTE8; /* ����ʵ���н��䶨����� */
	typedef signed long long int SZ_BYTE8;

	typedef unsigned int SZ_UPTR;
	typedef signed int SZ_PTR;

	typedef unsigned long SZ_UINT;
	typedef signed long SZ_INT;
	typedef unsigned long long SZ_ULONG; /** 16λϵͳ���ܲ����õ���Ҳû������ڽ��ؼ��� */
	typedef signed long long SZ_LONG;
	typedef unsigned int SZ_UT;
	typedef signed int SZ_T;
#endif

#endif
