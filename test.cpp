#include <iostream>
#include <stdio.h>
#include <unistd.h>
using namespace std;

int main()
{

	execl("/bin/sh", "sh", "-c", "./NodeServer-start.sh &", (char *)0);

	return 0;


}
