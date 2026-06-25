 #ifndef CONTINUITY_H
 #define CONTINUITY_H

 #include <Arduino.h>

 #include "config/pins.h"

 class ContinuityController {
 public:
	 ContinuityController();

	 void begin();

	 void update();

	 bool hasContinuity(uint8_t channel) const;

	 uint32_t getContinuityMask() const;

 private:
	 uint32_t _continuityMask;

	 static bool isValidChannel(uint8_t channel);
 };

 #endif
