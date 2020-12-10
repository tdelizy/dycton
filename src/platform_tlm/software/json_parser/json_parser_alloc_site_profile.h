//automatically generated during each build, please see src/scripts/header_profile_gen.py
#define __APP_ALLOC_SITE_0 (4248508)
#define __APP_ALLOC_SITE_1 (4238212)
#define __APP_ALLOC_SITE_2 (4209172)
#define __APP_ALLOC_SITE_3 (4248540)
#define __APP_ALLOC_SITE_4 (4196428)
#define __APP_ALLOC_SITE_5 (4196712)
#define __APP_ALLOC_SITE_6 (4197792)
#define __APP_ALLOC_SITE_7 (4197488)
#define __APP_ALLOC_SITE_8 (4197232)
#define __APP_ALLOC_SITE_9 (4197252)
#define __APP_ALLOC_SITE_10 (4204256)
#define __APP_ALLOC_SITE_11 (4216860)
#define __APP_ALLOC_SITE_12 (4205840)
#define __APP_ALLOC_SITE_13 (4204388)
#define __APP_ALLOC_SITE_14 (4205472)
#define __APP_ALLOC_SITE_15 (4205156)
#define __APP_ALLOC_SITE_16 (4204224)
#define __APP_ALLOC_SITE_17 (4194424)
#define __APP_ALLOC_SITE_18 (4220556)
#define __APP_ALLOC_SITE_19 (4206092)
#define __APP_ALLOC_SITE_20 (4204356)
#define __APP_ALLOC_SITE_21 (4205920)
#define __APP_ALLOC_SITE_LENGTH (22)

// #define PROFILE_GARDS
#ifdef PROFILE_GARDS
	#define ASSERT_PROFILE_CONSISTENCY(a) do{ if(!(a == __APP_ALLOC_SITE_0 || a == __APP_ALLOC_SITE_1 || a == __APP_ALLOC_SITE_2 || a == __APP_ALLOC_SITE_3 || a == __APP_ALLOC_SITE_4 || a == __APP_ALLOC_SITE_5 || a == __APP_ALLOC_SITE_6 || a == __APP_ALLOC_SITE_7 || a == __APP_ALLOC_SITE_8 || a == __APP_ALLOC_SITE_9 || a == __APP_ALLOC_SITE_10 || a == __APP_ALLOC_SITE_11 || a == __APP_ALLOC_SITE_12 || a == __APP_ALLOC_SITE_13 || a == __APP_ALLOC_SITE_14 || a == __APP_ALLOC_SITE_15 || a == __APP_ALLOC_SITE_16 || a == __APP_ALLOC_SITE_17 || a == __APP_ALLOC_SITE_18 || a == __APP_ALLOC_SITE_19 || a == __APP_ALLOC_SITE_20 || a == __APP_ALLOC_SITE_21)){print("ERROR : caller @ not in profile !!");exit(a);} }while(0)
#else
	#define ASSERT_PROFILE_CONSISTENCY(a) do{}while(0)
#endif

#define GENERATED_SWITCH_PROFILE(addr, prof_len) do { \
uint32_t l = 0xFFFF;\
switch (addr) {\
case __APP_ALLOC_SITE_0:\
	l = 0;\
	break;\
case __APP_ALLOC_SITE_1:\
	l = 1;\
	break;\
case __APP_ALLOC_SITE_2:\
	l = 2;\
	break;\
case __APP_ALLOC_SITE_3:\
	l = 3;\
	break;\
case __APP_ALLOC_SITE_4:\
	l = 4;\
	break;\
case __APP_ALLOC_SITE_5:\
	l = 5;\
	break;\
case __APP_ALLOC_SITE_6:\
	l = 6;\
	break;\
case __APP_ALLOC_SITE_7:\
	l = 7;\
	break;\
case __APP_ALLOC_SITE_8:\
	l = 8;\
	break;\
case __APP_ALLOC_SITE_9:\
	l = 9;\
	break;\
case __APP_ALLOC_SITE_10:\
	l = 10;\
	break;\
case __APP_ALLOC_SITE_11:\
	l = 11;\
	break;\
case __APP_ALLOC_SITE_12:\
	l = 12;\
	break;\
case __APP_ALLOC_SITE_13:\
	l = 13;\
	break;\
case __APP_ALLOC_SITE_14:\
	l = 14;\
	break;\
case __APP_ALLOC_SITE_15:\
	l = 15;\
	break;\
case __APP_ALLOC_SITE_16:\
	l = 16;\
	break;\
case __APP_ALLOC_SITE_17:\
	l = 17;\
	break;\
case __APP_ALLOC_SITE_18:\
	l = 18;\
	break;\
case __APP_ALLOC_SITE_19:\
	l = 19;\
	break;\
case __APP_ALLOC_SITE_20:\
	l = 20;\
	break;\
case __APP_ALLOC_SITE_21:\
	l = 21;\
	break;\
}\
if(l < prof_len){\
	alloc_context = &(multi_heap_ctx[0]);\
	returnPointer = newlib_malloc_r(_impure_ptr, sz);\
	if (!returnPointer) {\
		ALLOC_INTERNAL_FAIL(0);\
		alloc_context = &(multi_heap_ctx[1]);\
		returnPointer = newlib_malloc_r(_impure_ptr, sz);\
	}\
}else{\
	alloc_context = &(multi_heap_ctx[1]);\
	returnPointer = newlib_malloc_r(_impure_ptr, sz);\
	if (!returnPointer) {\
		ALLOC_INTERNAL_FAIL(0);\
		alloc_context = &(multi_heap_ctx[0]);\
		returnPointer = newlib_malloc_r(_impure_ptr, sz);\
	}\
}\
}while(0)// we need to define these macro for every architecture, but simulation will fail for architectures whose profile is not computed.
#define GEN_SW_PROF_ENHANCED_A1(addr, prof_len) do { exit(8080); }while(0)
#define GEN_SW_PROF_ILP_A1(addr) do { exit(8080); }while(0)
#define GEN_SW_PROF_ENHANCED_A2(addr, prof_len) do { exit(8080); }while(0)
#define GEN_SW_PROF_ILP_A2(addr) do { exit(8080); }while(0)
#define GEN_SW_PROF_ENHANCED_A3(addr, prof_len) do { exit(8080); }while(0)
#define GEN_SW_PROF_ILP_A3(addr) do { exit(8080); }while(0)
#define GEN_SW_PROF_ENHANCED_A4(addr, prof_len) do { exit(8080); }while(0)
#define GEN_SW_PROF_ILP_A4(addr) do { exit(8080); }while(0)
#define GEN_SW_PROF_ENHANCED_A5(addr, prof_len) do { exit(8080); }while(0)
#define GEN_SW_PROF_ILP_A5(addr) do { exit(8080); }while(0)
