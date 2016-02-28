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
	//blockdev_destroy();
}

TEST(fs, MkfsShouldComplete)
{
	TEST_ASSERT_TRUE(cnmkfs() == 0);
	//system("hd /tmp/fs.bin");
}

TEST(fs, MkdirShouldComplete)
{
	cnmkfs();
	cnmount();
	char dirname[20];
	strcpy(dirname, "test1");
	int8_t result = cnmkdir(dirname);

	system("hd /tmp/fs.bin");
	TEST_ASSERT_EQUAL_INT8(result,0);
}

