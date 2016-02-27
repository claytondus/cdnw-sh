#include <stdlib.h>
#include "blockdev.h"
#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP(blockdev);

TEST_SETUP(blockdev)
{

}

TEST_TEAR_DOWN(blockdev)
{
	blockdev_destroy();
}

TEST(blockdev, BlockdevAttachDetachShouldComplete)
{
	TEST_ASSERT_TRUE(blockdev_attach() == 0);
	TEST_ASSERT_TRUE(blockdev_detach() == 0);
}

TEST(blockdev, BlockdevCanReadWrite)
{
	TEST_ASSERT_TRUE(blockdev_attach() == 0);
	block* test_write_blk = (block*)malloc(BLOCK_SIZE);
	uint32_t* wblk_32 = (uint32_t*)test_write_blk;
	wblk_32[0] = 0xDEADBEEF;
	wblk_32[45] = 0xFEEDBEEF;
	TEST_ASSERT_TRUE(blk_write(1234, test_write_blk) == 0);
	free(test_write_blk);
	TEST_ASSERT_TRUE(blockdev_detach() == 0);

	TEST_ASSERT_TRUE(blockdev_attach() == 0);
	block* test_read_blk = (block*)malloc(BLOCK_SIZE);
	TEST_ASSERT_TRUE(blk_read(1234, test_read_blk) == 0);
	uint32_t* rblk_32 = (uint32_t*)test_read_blk;
	TEST_ASSERT_TRUE(rblk_32[0] == 0xDEADBEEF);
	TEST_ASSERT_TRUE(rblk_32[45] == 0xFEEDBEEF);
	free(test_read_blk);
	TEST_ASSERT_TRUE(blockdev_detach() == 0);
}

