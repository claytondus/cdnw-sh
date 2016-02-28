#include "bitmap.h"
#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP(bitmap);

TEST_SETUP(bitmap)
{
}

TEST_TEAR_DOWN(bitmap)
{
}

TEST(bitmap, TestCanSetClearBitmap)
{
	block bm;
	set_bitmap(&bm, 34);
	TEST_ASSERT_TRUE(read_bitmap(&bm, 34));
	clear_bitmap(&bm, 34);
	TEST_ASSERT_FALSE(read_bitmap(&bm, 34));
}

TEST(bitmap, TestClearBitmapDoesntAffectNearbyBits)
{
	block bm;
	set_bitmap(&bm, 4095);
	set_bitmap(&bm, 4094);
	set_bitmap(&bm, 4093);
	TEST_ASSERT_TRUE(read_bitmap(&bm, 4095));
	TEST_ASSERT_TRUE(read_bitmap(&bm, 4094));
	TEST_ASSERT_TRUE(read_bitmap(&bm, 4093));
	clear_bitmap(&bm, 4094);
	TEST_ASSERT_TRUE(read_bitmap(&bm, 4095));
	TEST_ASSERT_FALSE(read_bitmap(&bm, 4094));
	TEST_ASSERT_TRUE(read_bitmap(&bm, 4093));
}

TEST(bitmap, TestCanFindFreeBits)
{
	block bm;
	TEST_ASSERT_EQUAL_UINT16(find_free_bit(&bm),0);
	for(uint16_t i = 0; i <= 1053; i++)
	{
		set_bitmap(&bm,i);
	}
	TEST_ASSERT_EQUAL_UINT16(find_free_bit(&bm), 1054);
	clear_bitmap(&bm,317);
	TEST_ASSERT_EQUAL_UINT16(find_free_bit(&bm), 317);
	set_bitmap(&bm,317);
	clear_bitmap(&bm,407);
	TEST_ASSERT_EQUAL_UINT16(find_free_bit(&bm), 407);
}



