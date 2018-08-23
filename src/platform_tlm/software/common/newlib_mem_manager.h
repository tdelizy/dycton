


void* newlib_malloc_r(struct _reent *reent_ptr, size_t bytes);
void newlib_free_r(struct _reent *reent_ptr, void* mem);
void* newlib_calloc_r(struct _reent *reent_ptr, size_t n, size_t elem_size);
