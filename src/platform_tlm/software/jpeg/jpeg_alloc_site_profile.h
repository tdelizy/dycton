//automatically generated during each build, please see src/scripts/header_profile_gen.py
#define __APP_ALLOC_SITE_0 (4311076)
#define __APP_ALLOC_SITE_1 (4321704)
#define __APP_ALLOC_SITE_2 (4316396)
#define __APP_ALLOC_SITE_3 (4309168)
#define __APP_ALLOC_SITE_4 (4317892)
#define __APP_ALLOC_SITE_5 (4313996)
#define __APP_ALLOC_SITE_LENGTH (6)

// #define PROFILE_GARDS
#ifdef PROFILE_GARDS
	#define ASSERT_PROFILE_CONSISTENCY(a) do{ if(!(a == __APP_ALLOC_SITE_0 || a == __APP_ALLOC_SITE_1 || a == __APP_ALLOC_SITE_2 || a == __APP_ALLOC_SITE_3 || a == __APP_ALLOC_SITE_4 || a == __APP_ALLOC_SITE_5)){print("ERROR : caller @ not in profile !!");exit(a);} }while(0)
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
}\
if(l < prof_len){\
	alloc_context = &(multi_heap_ctx[0]);\
	returnPointer = newlib_malloc_r(_impure_ptr, sz);\
	if (!returnPointer) {\
		ALLOC_INTERNAL_FAIL(0);\
	}\
}\
}while(0)


// stubs for now
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
