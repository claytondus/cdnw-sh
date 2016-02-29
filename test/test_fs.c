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
	TEST_ASSERT_TRUE(cnmkfs() == 0);
	//system("hd /tmp/fs.bin");
}

TEST(fs, MkdirShouldComplete)
{
	cnmkfs();
	cnmount();
	int8_t result = cnmkdir("test1");
	debug("test1");
	system("hd /tmp/fs.bin");
	TEST_ASSERT_EQUAL_INT8(result,0);
	result = cnmkdir("test1/test1a");
	debug("test1/test1a");
	system("hd /tmp/fs.bin");
	TEST_ASSERT_EQUAL_INT8(result,0);
	result = cnmkdir("test2");
	debug("test2");
	system("hd /tmp/fs.bin");
	TEST_ASSERT_EQUAL_INT8(result,0);
	result = cnmkdir("test1/test1b");
	debug("test1/test1b");
	system("hd /tmp/fs.bin");
	TEST_ASSERT_EQUAL_INT8(result,0);
	result = cnmkdir("test1/test1a/alpha");
	debug("test1/test1a/alpha");
	system("hd /tmp/fs.bin");
	TEST_ASSERT_EQUAL_INT8(result,0);
	result = cnmkdir("test2/test2a");
	debug("test2/test2a");
	system("hd /tmp/fs.bin");
	TEST_ASSERT_EQUAL_INT8(result,0);
	result = cnmkdir("test2/test2a");
	debug("test2/test2a  SHOULD FAIL");
	system("hd /tmp/fs.bin");
	TEST_ASSERT_EQUAL_INT8(result,-1);
	result = cnmkdir("test2");
	debug("test2  SHOULD FAIL");
	system("hd /tmp/fs.bin");
	TEST_ASSERT_EQUAL_INT8(result,-1);


}

