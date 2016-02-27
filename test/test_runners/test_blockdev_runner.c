#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP_RUNNER(blockdev)
{
  RUN_TEST_CASE(blockdev, BlockdevAttachDetachShouldComplete);
  RUN_TEST_CASE(blockdev, BlockdevCanReadWrite);
}
