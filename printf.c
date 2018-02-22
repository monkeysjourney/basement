#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
	unsigned char buf[128] = {0};
	buf[0] = '\n';
	buf[1] = '\r';
	printf("%s\n0x%x 0x%x\n", buf, '\n', '\r');
	
	sprintf(buf, "123456789");

	printf("%p -- %p -- %p\n", buf, strstr(buf, "23"), strstr(buf, "ab"));

	printf("%d\n", strncasecmp("123abc", "123ABC", strlen("123abc")));

	printf("%d\n", atoi("123456000"));
	printf("%d\n", atoi("a123456000"));

	
	return 0;
}
