
#include <criterion/criterion.h>

#include "main.h"
//#include "log.h"


void	destroy_pools(void)
{
	munmap(metadata_pool, BIG_ALLOC_BASE);
	munmap(malloc_pool, BIG_ALLOC_BASE);
	metadata_pool = NULL;
	malloc_pool = NULL;
}

//List of functions to test
Test(secmalloc, init)
{
	//checking if values are correctly initialised
	cr_assert(started == FALSE);
	cr_assert(current_metadata_pool_size == 0);
	cr_assert(malloc_pool == NULL);
	cr_assert(alloc_offset == 0);
	cr_assert(current_allocated == 0);
	cr_assert(metadata_pool_size == 0);
	//ptr = malloc(0x10);
	cr_assert(!metadata_pool);
	//check if init set a metadata pool
	init_secmalloc();
	cr_assert(metadata_pool);
	cr_expect_not_null(metadata_pool);
	cr_expect_not_null(malloc_pool);
	//check if unmap is done correctly
	cr_expect(munmap(metadata_pool, BIG_ALLOC_BASE) == 0);
	cr_expect(munmap(malloc_pool, BIG_ALLOC_BASE) == 0);
	metadata_pool = NULL;
	malloc_pool = NULL;
}

Test(utils, destroy_pools)
{
	init_secmalloc();
	destroy_pools();
	cr_assert(!metadata_pool);
	cr_assert(!malloc_pool);
}

Test(secmalloc,RANDOM_CANARY)
{
	uint64_t canary1;
	uint64_t canary2;
	RANDOM_CANARY(&canary1);
	RANDOM_CANARY(&canary2);
	cr_assert(canary1 != canary2); //NEED correction -> invalid wright
}

Test(secmalloc,my_malloc)
{
	void *ptr;
	char test_str[16] = "abcdefghi\0";
	void *ptr2;
	void *ptr3;
	init_secmalloc();
	cr_expect_null(metadata_pool->flink);
	ptr = my_malloc(16);
	cr_expect_not_null(metadata_pool->flink);
	cr_expect_eq(metadata_pool->free, 0);
	cr_expect_eq(metadata_pool->size, 16);
	strcpy(ptr, test_str);
	cr_expect_null(strncmp(test_str, ptr, 16));
	ptr2 = my_malloc(32);
	cr_expect_eq(metadata_pool->flink->size, 32);
	ptr3 = my_malloc(next_hexa_base(40));
	cr_expect_eq(metadata_pool->flink->flink->size, next_hexa_base(40));
	my_free(ptr);
	destroy_pools();
}

Test(secmalloc, heap_overflow, .exit_code = 1)
{
	char test_str[20] = "AAAAAAAAAAAAAAAAAAA\0";
	init_secmalloc();
	void *ptr;
	ptr = my_malloc(next_hexa_base(4));
	strcpy(ptr, test_str);
	my_free(ptr);
	destroy_pools();
}

Test(secmalloc,my_free)
{
	//Malloc & free
	void *ptr;
	init_secmalloc();
	cr_expect_null(metadata_pool->flink);
	ptr = my_malloc(10);
	my_free(ptr);
	cr_expect_eq(metadata_pool->free, 1);
	destroy_pools();

	//Consecutive Mallocs & free
	init_secmalloc();
	void *ptr2;
	ptr = my_malloc(next_hexa_base(5));
	ptr2 = my_malloc(16);
	my_free(ptr);
	cr_expect_eq(metadata_pool->free, 1);
	cr_expect_eq(metadata_pool->flink->free, 0);
	cr_expect_eq(metadata_pool->size, next_hexa_base(5));
	my_free(ptr2);
	cr_expect_null(metadata_pool->flink->flink);
	cr_expect_eq(metadata_pool->size, next_hexa_base(5)+8+16); // next_hexa_base(5) (chunk_1) + 16 (chunk_2) + 8 (canary)
	destroy_pools();
}

Test(secmalloc, double_free, .exit_code = 1)
{
	init_secmalloc();
	void *ptr2;
	ptr2 = my_malloc(20);
	my_free(ptr2);
	my_free(ptr2);
	destroy_pools();
}

Test(secmalloc,my_calloc)
{
	char test_str[10] = "\0\0\0\0\0\0\0\0\0\0";
	void *ptr;
	init_secmalloc();
	ptr = my_calloc(10, 1);
	cr_expect_null(strncmp(ptr, test_str, 10));
	my_free(ptr);
	destroy_pools();
}

Test(secmalloc,my_realloc)
{
	void *ptr;
	void *ptr2;
	init_secmalloc();
	ptr = my_malloc(next_hexa_base(10));
	cr_expect_eq(metadata_pool->size, next_hexa_base(10));
	ptr2 = my_realloc(ptr, next_hexa_base(20));
	cr_expect_eq(metadata_pool->flink->size, next_hexa_base(20));
	my_free(ptr2); 
	destroy_pools();
} 