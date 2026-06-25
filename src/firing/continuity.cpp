 #include "continuity.h"

 ContinuityController::ContinuityController()
	 : _continuityMask(0) {
 }

 void ContinuityController::begin() {
	 for (uint8_t i = 0; i < sizeof(CONTINUITY_PINS); i++) {
		 pinMode(CONTINUITY_PINS[i], CONTINUITY_PIN_MODE);
	 }

	 update();
 }

 void ContinuityController::update() {
	 uint32_t newMask = 0;

	 for (uint8_t i = 0; i < sizeof(CONTINUITY_PINS); i++) {
		 if (digitalRead(CONTINUITY_PINS[i]) == CONTINUITY_ACTIVE_STATE) {
			 newMask |= (1UL << i);
		 }
	 }

	 _continuityMask = newMask;
 }

 bool ContinuityController::hasContinuity(uint8_t channel) const {
	 if (!isValidChannel(channel)) {
		 return false;
	 }

	 return (_continuityMask & (1UL << (channel - 1))) != 0;
 }

 uint32_t ContinuityController::getContinuityMask() const {
	 return _continuityMask;
 }

 bool ContinuityController::isValidChannel(uint8_t channel) {
	 return channel >= 1 && channel <= sizeof(CONTINUITY_PINS);
 }
