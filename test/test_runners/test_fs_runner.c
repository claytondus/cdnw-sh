#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP_RUNNER(fs)
{
  //RUN_TEST_CASE(fs, MkfsShouldComplete);
  //RUN_TEST_CASE(fs, MountUnmountShouldComplete);
  //RUN_TEST_CASE(fs, MkdirShouldComplete);
  RUN_TEST_CASE(fs, CdShouldComplete);
}
