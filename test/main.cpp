#include <unistd.h>

#include <iostream>
#include <chrono>

#include "libmf/libmfwtimer.h"
#include "radio.h"

struct wtimer_desc wtimer_desc_1s;

void fn_wtimer_1s(struct wtimer_desc *desc)
{
	auto micros = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	char str[UINT8_MAX];
	snprintf(str, UINT8_MAX, "wtimer 1s callback, tick: %.6fs", (double)micros / 1000000);
	std::cout <<  str << std::endl;
	ScheduleTask(desc, 0, RELATIVE, SECONDS(1));
}

int main(int argc, char *argv[])
{
	std::cout << "start" << std::endl;

	radio_init();
	std::cout << "radio inited" << std::endl;

	ScheduleTask(&wtimer_desc_1s, fn_wtimer_1s, RELATIVE, SECONDS(1));
	std::cout << "schedules registered" << std::endl;

	std::cout << "loop started" << std::endl;
	while(1)
	{
		usleep(1000);
		wtimer_runcallbacks();
	}
}
