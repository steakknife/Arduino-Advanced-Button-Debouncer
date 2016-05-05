#ifndef DEBOUNCE_H

#include "Arduino.h"
#include "Timer.h"
#include <vector>

static void debounce_handler(void);

class Debounce {
	friend static void debounce_handler(void);
public:
	Debounce(int pin, bool pullup, int press_interval /* ms */, int release_interval /* ms */, int check_interval /* ms */);
	Debounce(int pin,  /* true */  int press_interval /* ms */, int release_interval /* ms */, int check_interval /* ms */); // pullup default to true

	bool pressed() const; // is button pressed?
private:
	void setup(bool pullup);
	void reset_count();
	void check_handler(int ticks);

	static Timer timer;
	static std::vector<Debounce*> debouncers;

	bool _pressed;
	int _count;

	const int _pin; // which pin #
	const int _release_interval; 
	const int _check_interval; 
	const int _press_interval; 
};

#endif // DEBOUNCE_H
