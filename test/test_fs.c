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

TEST(fs, MountUnmountShouldComplete)
{
	cnmkfs();
	cnmount();
	cnumount();
}

TEST(fs, MkdirShouldComplete)
{
	cnmkfs();
	cnmount();
	int8_t result = cnmkdir("test1");
	//debug("test1");
	//system("hd /tmp/fs.bin");
	TEST_ASSERT_EQUAL_INT8(result,0);
	result = cnmkdir("test1/test1a");
	//debug("test1/test1a");
	//system("hd /tmp/fs.bin");
	TEST_ASSERT_EQUAL_INT8(result,0);
	result = cnmkdir("test2");
	//debug("test2");
	//system("hd /tmp/fs.bin");
	TEST_ASSERT_EQUAL_INT8(result,0);
	result = cnmkdir("test1/test1b");
	//debug("test1/test1b");
	//system("hd /tmp/fs.bin");
	TEST_ASSERT_EQUAL_INT8(result,0);
	result = cnmkdir("test1/test1a/alpha");
	//debug("test1/test1a/alpha");
	//system("hd /tmp/fs.bin");
	TEST_ASSERT_EQUAL_INT8(result,0);
	result = cnmkdir("test1/test1a/alpha/beta");
	//debug("test1/test1a/alpha/beta");
	//system("hd /tmp/fs.bin");
	TEST_ASSERT_EQUAL_INT8(result,0);
	result = cnmkdir("test2/test2a");
	//debug("test2/test2a");
	//system("hd /tmp/fs.bin");
	TEST_ASSERT_EQUAL_INT8(result,0);
	result = cnmkdir("test2/test2a");
	//debug("test2/test2a  SHOULD FAIL");
	//system("hd /tmp/fs.bin");
	TEST_ASSERT_EQUAL_INT8(result,-1);
	result = cnmkdir("test2");
	//debug("test2  SHOULD FAIL");
	//system("hd /tmp/fs.bin");
	TEST_ASSERT_EQUAL_INT8(result,-1);
	cnumount();
}

TEST(fs, CdShouldComplete)
{
	cnmkfs();
	cnmount();
	int8_t result;
	result = cnmkdir("test1");
	TEST_ASSERT_EQUAL_INT8(result, 0);
	result = cncd("test1");
	TEST_ASSERT_EQUAL_INT8(result, 0);
	result = cnmkdir("test2");
	TEST_ASSERT_EQUAL_INT8(result, 0);
	result = cncd("test2");
	TEST_ASSERT_EQUAL_INT8(result, 0);
	system("hd /tmp/fs.bin");
	result = cncd("..");
	TEST_ASSERT_EQUAL_INT8(result, 0);
	result = cncd("/");
	TEST_ASSERT_EQUAL_INT8(result, 0);
	result = cncd("test1/test2");
	TEST_ASSERT_EQUAL_INT8(result, 0);
	result = cncd("/test1/test2");
	TEST_ASSERT_EQUAL_INT8(result, 0);
	cnumount();
}

TEST(fs, LsShouldComplete)
{
	cnmkfs();
	cnmount();
	int8_t result;
	result = cnmkdir("test1");
	result = cncd("test1");
	result = cnmkdir("test2");
	char lsbuf[4096];
	result = cnls("",lsbuf);
	debug("ls:\n%s\n", lsbuf);
	TEST_ASSERT_EQUAL_INT8(result, 0);
	char lsbuf2[4096];
	result = cnls("/",lsbuf2);
	debug("ls /:\n%s\n", lsbuf2);
	TEST_ASSERT_EQUAL_INT8(result, 0);
	char lsbuf3[4096];
	result = cnls("/test1",lsbuf3);
	debug("ls /test1:\n%s\n", lsbuf3);
	TEST_ASSERT_EQUAL_INT8(result, 0);
	char lsbuf4[4096];
	result = cnls("..",lsbuf4);
	debug("ls ..:\n%s\n", lsbuf4);
	TEST_ASSERT_EQUAL_INT8(result, 0);
	char lsbuf5[4096];
	result = cnls("test3",lsbuf5);
	debug("ls test3 (SHOULD FAIL):\n%s\n", lsbuf5);
	TEST_ASSERT_EQUAL_INT8(result, -1);
}

