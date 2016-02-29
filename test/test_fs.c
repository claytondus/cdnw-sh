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

TEST(fs, CreatShouldComplete)
{
	cnmkfs();
	cnmount();
	int8_t result;
	dir_ptr* dir = cnopendir("");
	result = cncreat(dir, "file1.txt");
	result = cncreat(dir, "file2.txt");
	system("hd /tmp/fs.bin");
	char lsbuf[1024];
	memset(lsbuf, 0, 1024);
	cnls("",lsbuf);
	debug("ls after creat:\n%s",lsbuf);
	TEST_ASSERT_EQUAL_INT8(result, 0);
	cnumount();
}

TEST(fs, OpenShouldComplete)
{
	cnmkfs();
	cnmount();
	dir_ptr* dir = cnopendir("");
	int16_t fd1 = cnopen(dir, "file1.txt", FD_WRITE);
	int16_t fd2 = cnopen(dir, "file2.txt", FD_WRITE);
	system("hd /tmp/fs.bin");
	TEST_ASSERT_EQUAL_INT16(fd1, 0);
	TEST_ASSERT_EQUAL_INT16(fd2, 1);
	int16_t result4 = cnclose(fd2);
	int16_t result3 = cnclose(fd1);
	TEST_ASSERT_EQUAL_INT16(result3, 0);
	TEST_ASSERT_EQUAL_INT16(result4, 0);
	cnumount();
}

TEST(fs, SeekShouldComplete)
{
	cnmkfs();
	cnmount();
	dir_ptr* dir = cnopendir("");
	int16_t fd1 = cnopen(dir, "file1.txt", FD_WRITE);
	int8_t result = cnseek(fd1, 10000);
	system("hd /tmp/fs.bin");
	TEST_ASSERT_EQUAL_INT16(result, 0);
	cnumount();
}

TEST(fs, ReadWriteShouldComplete)
{
	cnmkfs();
	cnmount();
	cnmkdir("test1");
	cncd("test1");
	dir_ptr* dir = cnopendir(".");
	int16_t fd1 = cnopen(dir, "file1.txt", FD_WRITE);
	cnseek(fd1, 10000);
	size_t bytes_written = cnwrite((uint8_t*)"This is only a test.", 21, fd1);
	system("hd /tmp/fs.bin");
	TEST_ASSERT_TRUE(21 == bytes_written);
	cnseek(fd1, 10000);
	char readbuf[100];
	size_t bytes_read = cnread((uint8_t*)readbuf, 21, fd1);
	TEST_ASSERT_EQUAL_STRING("This is only a test.", readbuf);
	TEST_ASSERT_TRUE(21 == bytes_read);
	cnumount();
}

TEST(fs, RmdirShouldComplete)
{
	cnmkfs();
	cnmount();
	cnmkdir("test1");
	cnmkdir("test2");
	char lsbuf[512];
	cnls("",lsbuf);
	system("hd /tmp/fs.bin");
	debug("%s",lsbuf);
	int8_t result = cnrmdir("test1");
	char lsbuf2[512];
	cnls("",lsbuf2);
	debug("%s",lsbuf2);
	system("hd /tmp/fs.bin");
	TEST_ASSERT_EQUAL_INT8(0, result);
	cnumount();
}

TEST(fs, CatShouldComplete)
{
	cnmkfs();
	cnmkfs();
	cnmount();
	dir_ptr* dir = cnopendir(".");
	int16_t fd1 = cnopen(dir, "file1.txt", FD_WRITE);
	size_t bytes_written = cnwrite((uint8_t*)"This is only a test.\r\n\r\n", 24, fd1);
	cnwrite((uint8_t*)"This is also a test.\r\n\r\n\r\n",26,fd1);
	TEST_ASSERT_TRUE(24 == bytes_written);
	cnclose(fd1);
	char catbuf[512];
	memset(catbuf, 0, 512);
	int8_t result = cncat("file1.txt",catbuf);
	debug("%s",catbuf);
	system("hd /tmp/fs.bin");
	TEST_ASSERT_EQUAL_INT8(0, result);
	cnumount();
}
