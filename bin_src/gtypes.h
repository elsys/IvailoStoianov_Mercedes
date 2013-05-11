
//  Header file for global types used everywhere 


// general types
typedef unsigned char	U8;
typedef signed char   	S8;
typedef unsigned int  	U16;
typedef signed int    	S16;
typedef unsigned long 	U32;
typedef signed long   	S32;

// macros for macro concatenation of 2 arguments
#define mCat2_(x,y)	x##y
#define mCat2(x,y) mCat2_(x,y)

// macros for macro concatenation of 3 arguments
#define mCat3_(x,y,z)	x##y##z
#define mCat3(x,y,z) mCat3_(x,y,z)


