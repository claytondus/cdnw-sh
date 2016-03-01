#include "unity.h"
#include "unity_fixture.h"

TEST_GROUP_RUNNER(fs)
{
  //RUN_TEST_CASE(fs, MkfsShouldComplete);
  //RUN_TEST_CASE(fs, MountUnmountShouldComplete);
  //RUN_TEST_CASE(fs, MkdirShouldComplete);
  //RUN_TEST_CASE(fs, CdShouldComplete);
  //RUN_TEST_CASE(fs, LsShouldComplete);
  //RUN_TEST_CASE(fs, CreatShouldComplete);
  //RUN_TEST_CASE(fs, OpenShouldComplete);
  //RUN_TEST_CASE(fs, SeekShouldComplete);
	RUN_TEST_CASE(fs, ReadWriteShouldComplete);
	//RUN_TEST_CASE(fs, RmdirShouldComplete);
	//RUN_TEST_CASE(fs, CatShouldComplete);
	//RUN_TEST_CASE(fs, ImportExportShouldComplete);
}
