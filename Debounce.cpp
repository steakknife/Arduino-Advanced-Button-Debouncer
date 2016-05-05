#define __ASSERT_USE_STDERR

#include "Arduino.h"
#include "Debounce.h"

#include <assert.h>

// handle diagnostic informations given by assertion and abort program execution:
void __assert(const char *__func, const char *__file, int __lineno, const char *__sexp) {
	// transmit diagnostic informations through serial link. 
	Serial.println(__func);
	Serial.println(__file);
	Serial.println(__lineno, DEC);
	Serial.println(__sexp);
	Serial.flush();
	// abort program execution.
	abort();
}

Debounce::Debounce(int pin, bool pullup, int press_interval, int release_interval, int check_interval) :
	_pin(pin), _release_interval(release_interval), _press_interval(press_interval), _check_interval(check_interval)
{
	setup(pullup);
}

Debounce::Debounce(int pin, /* true */ int press_interval, int release_interval, int check_interval) :
	_pin(pin), _release_interval(release_interval), _press_interval(press_interval), _check_interval(check_interval)
{
	setup(true);
}

void Debounce::setup(bool pullup) {
	assert(_press_interval > 0);
	assert(_release_interval > 0);
	assert(_check_interval > 0);
	assert(_press_interval % _check_interval == 0);
	assert(_release_interval % _check_interval == 0);

	_pressed = false;
	_count = 0;

	pinMode(_pin, pullup ? INPUT_PULLUP : INPUT);
	reset_count();
	debouncers.push_back(this);

	static bool handler_installed = false;
	if (!handler_installed) {
		handler_installed = true;
		timer.every(1, debounce_handler); // every 1 ms
	}
}

bool Debounce::pressed() const {
	return _pressed;
}

void Debounce::reset_count() {
	if (_pressed) // was pressed, now not pressed
		_count = _release_interval / _check_interval;
	else // was not pressed, now pressed
		_count = _press_interval / _check_interval;
}

void Debounce::check_handler(const int ticks) {
	if ((ticks % _check_interval) != 0)
		return; // timer not ready to check
	const bool raw_pressed = digitalRead(_pin);
	if (raw_pressed != _pressed) { // pin change
		if (--_count)
			return; // timer not yet expired
		_pressed = raw_pressed;
	}
	reset_count();
}

static void debounce_handler(void) {
	static int ticks = 0; // ms
	for (Debouncers::iterator debounce = Debounce::debouncers.begin(); debounce != Debounce::debouncers.end(); ++debounce)
		(*debounce)->check_handler(ticks);
	++ticks;
}
