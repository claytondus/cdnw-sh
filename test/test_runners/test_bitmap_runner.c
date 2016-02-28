#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP_RUNNER(bitmap)
{
  RUN_TEST_CASE(bitmap, TestCanSetClearBitmap);
  RUN_TEST_CASE(bitmap, TestClearBitmapDoesntAffectNearbyBits);
  RUN_TEST_CASE(bitmap, TestCanFindFreeBits);

}
