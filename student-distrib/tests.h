#ifndef TESTS_H
#define TESTS_H

#define FREQ_TOO_SMALL 	1 		//frequency too small but still power of 2 for RTC TEST
#define	FREQ_TOO_BIG	2048	//frequency too big but still power of 2 for RTC test
#define	FREQ_NEG		-50		//Negative frequency for RTC test
#define	FREQ_NOT_POW	420		//Frequency not a power of 2 but not too big/small for RTC TEST

// test launcher
void launch_tests();

int rtc_test();
void rtc_helper(int i);

#endif /* TESTS_H */
