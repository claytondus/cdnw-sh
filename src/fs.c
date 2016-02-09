#include <glib/gstdio.h>
#include <glib.h>

int main(int argc, char **argv)
{
	int creat_result = g_creat("fs.bin", 0666);
	if(creat_result == 0) {
		perror("Could not create fs.bin");
		exit(1);
	}
	GMappedFile* fs = g_mapped_file_new("fs.bin", 1, NULL);

}
