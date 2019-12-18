#include <stdlib.h>

// Print system error and exit
void error(char *msg)
{
	perror(msg);
	exit(1);
}
