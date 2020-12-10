//automatically generated during each build, please see src/scripts/header_profile_gen.py
#define __APP_ALLOC_SITE_0 (4307816)
#define __APP_ALLOC_SITE_1 (4304724)
#define __APP_ALLOC_SITE_2 (4271396)
#define __APP_ALLOC_SITE_3 (4224312)
#define __APP_ALLOC_SITE_4 (4218444)
#define __APP_ALLOC_SITE_5 (4246468)
#define __APP_ALLOC_SITE_6 (4278108)
#define __APP_ALLOC_SITE_7 (4218464)
#define __APP_ALLOC_SITE_8 (4223564)
#define __APP_ALLOC_SITE_9 (4246688)
#define __APP_ALLOC_SITE_10 (4224328)
#define __APP_ALLOC_SITE_11 (4278096)
#define __APP_ALLOC_SITE_12 (4262652)
#define __APP_ALLOC_SITE_13 (4267308)
#define __APP_ALLOC_SITE_14 (4238700)
#define __APP_ALLOC_SITE_15 (4271384)
#define __APP_ALLOC_SITE_16 (4214720)
#define __APP_ALLOC_SITE_17 (4303228)
#define __APP_ALLOC_SITE_18 (4216576)
#define __APP_ALLOC_SITE_19 (4205324)
#define __APP_ALLOC_SITE_20 (4201072)
#define __APP_ALLOC_SITE_21 (4201052)
#define __APP_ALLOC_SITE_22 (4194568)
#define __APP_ALLOC_SITE_23 (4194556)
#define __APP_ALLOC_SITE_24 (4200952)
#define __APP_ALLOC_SITE_25 (4206036)
#define __APP_ALLOC_SITE_26 (4229236)
#define __APP_ALLOC_SITE_27 (4194604)
#define __APP_ALLOC_SITE_28 (4206056)
#define __APP_ALLOC_SITE_29 (4206092)
#define __APP_ALLOC_SITE_30 (4206076)
#define __APP_ALLOC_SITE_31 (4233172)
#define __APP_ALLOC_SITE_32 (4194616)
#define __APP_ALLOC_SITE_33 (4194580)
#define __APP_ALLOC_SITE_34 (4233208)
#define __APP_ALLOC_SITE_35 (4233032)
#define __APP_ALLOC_SITE_36 (4194628)
#define __APP_ALLOC_SITE_37 (4229932)
#define __APP_ALLOC_SITE_38 (4194592)
#define __APP_ALLOC_SITE_39 (4194656)
#define __APP_ALLOC_SITE_40 (4194668)
#define __APP_ALLOC_SITE_41 (4194640)
#define __APP_ALLOC_SITE_42 (4194680)
#define __APP_ALLOC_SITE_LENGTH (43)

// #define PROFILE_GARDS
#ifdef PROFILE_GARDS
	#define ASSERT_PROFILE_CONSISTENCY(a) do{ if(!(a == __APP_ALLOC_SITE_0 || a == __APP_ALLOC_SITE_1 || a == __APP_ALLOC_SITE_2 || a == __APP_ALLOC_SITE_3 || a == __APP_ALLOC_SITE_4 || a == __APP_ALLOC_SITE_5 || a == __APP_ALLOC_SITE_6 || a == __APP_ALLOC_SITE_7 || a == __APP_ALLOC_SITE_8 || a == __APP_ALLOC_SITE_9 || a == __APP_ALLOC_SITE_10 || a == __APP_ALLOC_SITE_11 || a == __APP_ALLOC_SITE_12 || a == __APP_ALLOC_SITE_13 || a == __APP_ALLOC_SITE_14 || a == __APP_ALLOC_SITE_15 || a == __APP_ALLOC_SITE_16 || a == __APP_ALLOC_SITE_17 || a == __APP_ALLOC_SITE_18 || a == __APP_ALLOC_SITE_19 || a == __APP_ALLOC_SITE_20 || a == __APP_ALLOC_SITE_21 || a == __APP_ALLOC_SITE_22 || a == __APP_ALLOC_SITE_23 || a == __APP_ALLOC_SITE_24 || a == __APP_ALLOC_SITE_25 || a == __APP_ALLOC_SITE_26 || a == __APP_ALLOC_SITE_27 || a == __APP_ALLOC_SITE_28 || a == __APP_ALLOC_SITE_29 || a == __APP_ALLOC_SITE_30 || a == __APP_ALLOC_SITE_31 || a == __APP_ALLOC_SITE_32 || a == __APP_ALLOC_SITE_33 || a == __APP_ALLOC_SITE_34 || a == __APP_ALLOC_SITE_35 || a == __APP_ALLOC_SITE_36 || a == __APP_ALLOC_SITE_37 || a == __APP_ALLOC_SITE_38 || a == __APP_ALLOC_SITE_39 || a == __APP_ALLOC_SITE_40 || a == __APP_ALLOC_SITE_41 || a == __APP_ALLOC_SITE_42)){print("ERROR : caller @ not in profile !!");exit(a);} }while(0)
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
case __APP_ALLOC_SITE_22:\
	l = 22;\
	break;\
case __APP_ALLOC_SITE_23:\
	l = 23;\
	break;\
case __APP_ALLOC_SITE_24:\
	l = 24;\
	break;\
case __APP_ALLOC_SITE_25:\
	l = 25;\
	break;\
case __APP_ALLOC_SITE_26:\
	l = 26;\
	break;\
case __APP_ALLOC_SITE_27:\
	l = 27;\
	break;\
case __APP_ALLOC_SITE_28:\
	l = 28;\
	break;\
case __APP_ALLOC_SITE_29:\
	l = 29;\
	break;\
case __APP_ALLOC_SITE_30:\
	l = 30;\
	break;\
case __APP_ALLOC_SITE_31:\
	l = 31;\
	break;\
case __APP_ALLOC_SITE_32:\
	l = 32;\
	break;\
case __APP_ALLOC_SITE_33:\
	l = 33;\
	break;\
case __APP_ALLOC_SITE_34:\
	l = 34;\
	break;\
case __APP_ALLOC_SITE_35:\
	l = 35;\
	break;\
case __APP_ALLOC_SITE_36:\
	l = 36;\
	break;\
case __APP_ALLOC_SITE_37:\
	l = 37;\
	break;\
case __APP_ALLOC_SITE_38:\
	l = 38;\
	break;\
case __APP_ALLOC_SITE_39:\
	l = 39;\
	break;\
case __APP_ALLOC_SITE_40:\
	l = 40;\
	break;\
case __APP_ALLOC_SITE_41:\
	l = 41;\
	break;\
case __APP_ALLOC_SITE_42:\
	l = 42;\
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
}while(0)

#define GEN_SW_PROF_ENHANCED_A1(addr, prof_len) do { \
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
case __APP_ALLOC_SITE_22:\
	l = 22;\
	break;\
case __APP_ALLOC_SITE_23:\
	l = 23;\
	break;\
case __APP_ALLOC_SITE_25:\
	l = 24;\
	break;\
case __APP_ALLOC_SITE_26:\
	l = 25;\
	break;\
case __APP_ALLOC_SITE_27:\
	l = 26;\
	break;\
case __APP_ALLOC_SITE_32:\
	l = 27;\
	break;\
case __APP_ALLOC_SITE_33:\
	l = 28;\
	break;\
case __APP_ALLOC_SITE_36:\
	l = 29;\
	break;\
case __APP_ALLOC_SITE_24:\
	l = 30;\
	break;\
case __APP_ALLOC_SITE_28:\
	l = 31;\
	break;\
case __APP_ALLOC_SITE_29:\
	l = 32;\
	break;\
case __APP_ALLOC_SITE_30:\
	l = 33;\
	break;\
case __APP_ALLOC_SITE_31:\
	l = 34;\
	break;\
case __APP_ALLOC_SITE_34:\
	l = 35;\
	break;\
case __APP_ALLOC_SITE_35:\
	l = 36;\
	break;\
case __APP_ALLOC_SITE_37:\
	l = 37;\
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
}while(0)

#define GEN_SW_PROF_ENHANCED_A3(addr, prof_len) do { \
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
case __APP_ALLOC_SITE_22:\
	l = 22;\
	break;\
case __APP_ALLOC_SITE_23:\
	l = 23;\
	break;\
case __APP_ALLOC_SITE_25:\
	l = 24;\
	break;\
case __APP_ALLOC_SITE_27:\
	l = 25;\
	break;\
case __APP_ALLOC_SITE_32:\
	l = 26;\
	break;\
case __APP_ALLOC_SITE_33:\
	l = 27;\
	break;\
case __APP_ALLOC_SITE_36:\
	l = 28;\
	break;\
case __APP_ALLOC_SITE_24:\
	l = 29;\
	break;\
case __APP_ALLOC_SITE_28:\
	l = 30;\
	break;\
case __APP_ALLOC_SITE_29:\
	l = 31;\
	break;\
case __APP_ALLOC_SITE_30:\
	l = 32;\
	break;\
case __APP_ALLOC_SITE_31:\
	l = 33;\
	break;\
case __APP_ALLOC_SITE_34:\
	l = 34;\
	break;\
case __APP_ALLOC_SITE_35:\
	l = 35;\
	break;\
case __APP_ALLOC_SITE_37:\
	l = 36;\
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
}while(0)

#define GEN_SW_PROF_ENHANCED_A2(addr, prof_len) do { \
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
case __APP_ALLOC_SITE_22:\
	l = 22;\
	break;\
case __APP_ALLOC_SITE_23:\
	l = 23;\
	break;\
case __APP_ALLOC_SITE_25:\
	l = 24;\
	break;\
case __APP_ALLOC_SITE_26:\
	l = 25;\
	break;\
case __APP_ALLOC_SITE_27:\
	l = 26;\
	break;\
case __APP_ALLOC_SITE_32:\
	l = 27;\
	break;\
case __APP_ALLOC_SITE_33:\
	l = 28;\
	break;\
case __APP_ALLOC_SITE_36:\
	l = 29;\
	break;\
case __APP_ALLOC_SITE_24:\
	l = 30;\
	break;\
case __APP_ALLOC_SITE_28:\
	l = 31;\
	break;\
case __APP_ALLOC_SITE_29:\
	l = 32;\
	break;\
case __APP_ALLOC_SITE_30:\
	l = 33;\
	break;\
case __APP_ALLOC_SITE_31:\
	l = 34;\
	break;\
case __APP_ALLOC_SITE_34:\
	l = 35;\
	break;\
case __APP_ALLOC_SITE_35:\
	l = 36;\
	break;\
case __APP_ALLOC_SITE_37:\
	l = 37;\
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
}while(0)

#define GEN_SW_PROF_ENHANCED_A5(addr, prof_len) do { \
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
case __APP_ALLOC_SITE_20:\
	l = 19;\
	break;\
case __APP_ALLOC_SITE_21:\
	l = 20;\
	break;\
case __APP_ALLOC_SITE_22:\
	l = 21;\
	break;\
case __APP_ALLOC_SITE_23:\
	l = 22;\
	break;\
case __APP_ALLOC_SITE_27:\
	l = 23;\
	break;\
case __APP_ALLOC_SITE_24:\
	l = 24;\
	break;\
case __APP_ALLOC_SITE_25:\
	l = 25;\
	break;\
case __APP_ALLOC_SITE_29:\
	l = 26;\
	break;\
case __APP_ALLOC_SITE_30:\
	l = 27;\
	break;\
case __APP_ALLOC_SITE_31:\
	l = 28;\
	break;\
case __APP_ALLOC_SITE_34:\
	l = 29;\
	break;\
case __APP_ALLOC_SITE_35:\
	l = 30;\
	break;\
case __APP_ALLOC_SITE_37:\
	l = 31;\
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
}while(0)

#define GEN_SW_PROF_ENHANCED_A4(addr, prof_len) do { \
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
case __APP_ALLOC_SITE_20:\
	l = 19;\
	break;\
case __APP_ALLOC_SITE_21:\
	l = 20;\
	break;\
case __APP_ALLOC_SITE_22:\
	l = 21;\
	break;\
case __APP_ALLOC_SITE_23:\
	l = 22;\
	break;\
case __APP_ALLOC_SITE_27:\
	l = 23;\
	break;\
case __APP_ALLOC_SITE_24:\
	l = 24;\
	break;\
case __APP_ALLOC_SITE_25:\
	l = 25;\
	break;\
case __APP_ALLOC_SITE_28:\
	l = 26;\
	break;\
case __APP_ALLOC_SITE_29:\
	l = 27;\
	break;\
case __APP_ALLOC_SITE_30:\
	l = 28;\
	break;\
case __APP_ALLOC_SITE_37:\
	l = 29;\
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
}while(0)

#define GEN_SW_PROF_ILP_A1(addr) do { \
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
case __APP_ALLOC_SITE_22:\
	l = 22;\
	break;\
case __APP_ALLOC_SITE_23:\
	l = 23;\
	break;\
case __APP_ALLOC_SITE_25:\
	l = 24;\
	break;\
case __APP_ALLOC_SITE_26:\
	l = 25;\
	break;\
case __APP_ALLOC_SITE_27:\
	l = 26;\
	break;\
case __APP_ALLOC_SITE_32:\
	l = 27;\
	break;\
case __APP_ALLOC_SITE_33:\
	l = 28;\
	break;\
case __APP_ALLOC_SITE_36:\
	l = 29;\
	break;\
}\
if(l < 0xFFFF){\
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
}while(0)



#define GEN_SW_PROF_ILP_A3(addr) do { \
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
case __APP_ALLOC_SITE_22:\
	l = 22;\
	break;\
case __APP_ALLOC_SITE_23:\
	l = 23;\
	break;\
case __APP_ALLOC_SITE_25:\
	l = 24;\
	break;\
case __APP_ALLOC_SITE_27:\
	l = 25;\
	break;\
case __APP_ALLOC_SITE_32:\
	l = 26;\
	break;\
case __APP_ALLOC_SITE_33:\
	l = 27;\
	break;\
case __APP_ALLOC_SITE_36:\
	l = 28;\
	break;\
}\
if(l < 0xFFFF){\
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
}while(0)



#define GEN_SW_PROF_ILP_A2(addr) do { \
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
case __APP_ALLOC_SITE_22:\
	l = 22;\
	break;\
case __APP_ALLOC_SITE_23:\
	l = 23;\
	break;\
case __APP_ALLOC_SITE_25:\
	l = 24;\
	break;\
case __APP_ALLOC_SITE_26:\
	l = 25;\
	break;\
case __APP_ALLOC_SITE_27:\
	l = 26;\
	break;\
case __APP_ALLOC_SITE_32:\
	l = 27;\
	break;\
case __APP_ALLOC_SITE_33:\
	l = 28;\
	break;\
case __APP_ALLOC_SITE_36:\
	l = 29;\
	break;\
}\
if(l < 0xFFFF){\
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
}while(0)



#define GEN_SW_PROF_ILP_A5(addr) do { \
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
case __APP_ALLOC_SITE_20:\
	l = 19;\
	break;\
case __APP_ALLOC_SITE_21:\
	l = 20;\
	break;\
case __APP_ALLOC_SITE_22:\
	l = 21;\
	break;\
case __APP_ALLOC_SITE_23:\
	l = 22;\
	break;\
case __APP_ALLOC_SITE_27:\
	l = 23;\
	break;\
}\
if(l < 0xFFFF){\
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
}while(0)



#define GEN_SW_PROF_ILP_A4(addr) do { \
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
case __APP_ALLOC_SITE_20:\
	l = 19;\
	break;\
case __APP_ALLOC_SITE_21:\
	l = 20;\
	break;\
case __APP_ALLOC_SITE_22:\
	l = 21;\
	break;\
case __APP_ALLOC_SITE_23:\
	l = 22;\
	break;\
case __APP_ALLOC_SITE_27:\
	l = 23;\
	break;\
}\
if(l < 0xFFFF){\
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
}while(0)

