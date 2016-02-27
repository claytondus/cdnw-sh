#include "fs.h"
#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP(fs);

TEST_SETUP(fs)
{
	blockdev_attach();
}

TEST_TEAR_DOWN(fs)
{
	blockdev_detach();
	blockdev_destroy();
}

TEST(fs, MkfsShouldComplete)
{
	TEST_ASSERT_TRUE(mkfs() == 0);
	system("hexdump /tmp/fs.bin");
}



