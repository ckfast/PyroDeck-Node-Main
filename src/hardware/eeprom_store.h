 #ifndef EEPROM_STORE_H
 #define EEPROM_STORE_H

 #include <Arduino.h>

 struct PersistedSettings {
	 uint8_t nodeId;
	 uint8_t uiBrightness;
	 uint16_t firingTime;
	 uint16_t displaySleepTimeout;
 };

 class EEPROMStore {
 public:
	 static PersistedSettings loadSettings();
	 static void saveSettings(const PersistedSettings& settings);

 private:
	 struct StorageLayout {
		 uint16_t magic;
		 uint8_t version;
		 PersistedSettings settings;
		 uint8_t checksum;
	 };

	 static uint8_t calculateChecksum(const PersistedSettings& settings);
	 static PersistedSettings defaultSettings();
 };

 #endif
