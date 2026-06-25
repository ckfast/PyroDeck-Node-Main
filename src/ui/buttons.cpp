#include "buttons.h"

ButtonHandler::ButtonHandler()
    : _leftState(HIGH), _centerState(HIGH), _rightState(HIGH),
      _lastLeftReading(HIGH), _lastCenterReading(HIGH), _lastRightReading(HIGH),
      _leftPressTime(0), _centerPressTime(0), _rightPressTime(0),
      _leftClicked(false), _centerClicked(false), _rightClicked(false),
      _lastDebounceLeft(0), _lastDebounceCentr(0), _lastDebounceRight(0)
{
}

void ButtonHandler::begin() {
    pinMode(UI_LEFT_PIN, INPUT_PULLUP);
    pinMode(UI_ENTER_PIN, INPUT_PULLUP);
    pinMode(UI_RIGHT_PIN, INPUT_PULLUP);

    _leftState = digitalRead(UI_LEFT_PIN);
    _centerState = digitalRead(UI_ENTER_PIN);
    _rightState = digitalRead(UI_RIGHT_PIN);
    _lastLeftReading = _leftState;
    _lastCenterReading = _centerState;
    _lastRightReading = _rightState;

    pinMode(UI_PWM, OUTPUT);
    analogWrite(UI_PWM, 128); // Default brightness at 50%
}

void ButtonHandler::update() {
    uint32_t now = millis();
    
    // LEFT button debounce and click detection
    uint8_t leftReading = digitalRead(UI_LEFT_PIN);
    if (leftReading != _lastLeftReading) {
        _lastDebounceLeft = now;
        _lastLeftReading = leftReading;
    }
    if ((now - _lastDebounceLeft) >= DEBOUNCE_TIME) {
        if (leftReading != _leftState) {
            _leftState = leftReading;
            if (_leftState == LOW) {
                _leftPressTime = now;
                _leftClicked = true;
            }
        }
    }
    
    // CENTER button debounce and click detection
    uint8_t centerReading = digitalRead(UI_ENTER_PIN);
    if (centerReading != _lastCenterReading) {
        _lastDebounceCentr = now;
        _lastCenterReading = centerReading;
    }
    if ((now - _lastDebounceCentr) >= DEBOUNCE_TIME) {
        if (centerReading != _centerState) {
            _centerState = centerReading;
            if (_centerState == LOW) {
                _centerPressTime = now;
                _centerClicked = true;
            }
        }
    }
    
    // RIGHT button debounce and click detection
    uint8_t rightReading = digitalRead(UI_RIGHT_PIN);
    if (rightReading != _lastRightReading) {
        _lastDebounceRight = now;
        _lastRightReading = rightReading;
    }
    if ((now - _lastDebounceRight) >= DEBOUNCE_TIME) {
        if (rightReading != _rightState) {
            _rightState = rightReading;
            if (_rightState == LOW) {
                _rightPressTime = now;
                _rightClicked = true;
            }
        }
    }
}

bool ButtonHandler::isLeftPressed() {
    return _leftState == LOW;
}

bool ButtonHandler::isCenterPressed() {
    return _centerState == LOW;
}

bool ButtonHandler::isRightPressed() {
    return _rightState == LOW;
}

bool ButtonHandler::wasLeftClicked() {
    if (_leftClicked) {
        _leftClicked = false;
        return true;
    }
    return false;
}

bool ButtonHandler::wasCenterClicked() {
    if (_centerClicked) {
        _centerClicked = false;
        return true;
    }
    return false;
}

bool ButtonHandler::wasRightClicked() {
    if (_rightClicked) {
        _rightClicked = false;
        return true;
    }
    return false;
}

void ButtonHandler::setLEDBrightness(uint8_t brightness) {
    analogWrite(UI_PWM, brightness);
}
