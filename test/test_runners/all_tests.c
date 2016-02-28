#include "unity_fixture.h"

static void RunAllTests(void)
{
  //RUN_TEST_GROUP(blockdev);
  RUN_TEST_GROUP(fs);
  //RUN_TEST_GROUP(bitmap);
}

int main(int argc, const char * argv[])
{
  return UnityMain(argc, argv, RunAllTests);
}
