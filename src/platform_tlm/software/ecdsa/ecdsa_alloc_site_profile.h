//automatically generated during each build, please see src/scripts/header_profile_gen.py
#define __APP_ALLOC_SITE_0 (4285616)
#define __APP_ALLOC_SITE_1 (4234804)
#define __APP_ALLOC_SITE_2 (4232460)
#define __APP_ALLOC_SITE_3 (4204892)
#define __APP_ALLOC_SITE_4 (4285912)
#define __APP_ALLOC_SITE_5 (4209016)
#define __APP_ALLOC_SITE_6 (4312320)
#define __APP_ALLOC_SITE_7 (4317144)
#define __APP_ALLOC_SITE_LENGTH (8)

// #define PROFILE_GARDS
#ifdef PROFILE_GARDS
	#define ASSERT_PROFILE_CONSISTENCY(a) do{ if(!(a == __APP_ALLOC_SITE_0 || a == __APP_ALLOC_SITE_1 || a == __APP_ALLOC_SITE_2 || a == __APP_ALLOC_SITE_3 || a == __APP_ALLOC_SITE_4 || a == __APP_ALLOC_SITE_5 || a == __APP_ALLOC_SITE_6 || a == __APP_ALLOC_SITE_7)){print("ERROR : caller @ not in profile !!");exit(a);} }while(0)
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