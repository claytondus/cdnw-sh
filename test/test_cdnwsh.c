#include "unity.h"
#include "blockdev.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_ShouldPass(void)
{
	TEST_ASSERT_TRUE(1==1);
}

void test_BlockdevInit(void)
{
	TEST_ASSERT_TRUE(test_blockdev() == 0);
}
