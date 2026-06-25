 #include "eeprom_store.h"

 #include <EEPROM.h>

 #include "config/settings.h"

 namespace {
 const uint16_t SETTINGS_MAGIC = 0x5044;
 const uint8_t SETTINGS_VERSION = 1;
 const int EEPROM_SETTINGS_ADDRESS = 0;
 }

 PersistedSettings EEPROMStore::loadSettings() {
	 StorageLayout stored = {};
	 EEPROM.get(EEPROM_SETTINGS_ADDRESS, stored);

	 PersistedSettings defaults = defaultSettings();
	 if (stored.magic != SETTINGS_MAGIC ||
		 stored.version != SETTINGS_VERSION ||
		 stored.checksum != calculateChecksum(stored.settings) ||
		 stored.settings.nodeId < MIN_NODE_ID ||
		 stored.settings.nodeId > MAX_NODE_ID) {
		 saveSettings(defaults);
		 return defaults;
	 }

	 return stored.settings;
 }

 void EEPROMStore::saveSettings(const PersistedSettings& settings) {
	 StorageLayout stored = {};
	 stored.magic = SETTINGS_MAGIC;
	 stored.version = SETTINGS_VERSION;
	 stored.settings = settings;
	 stored.checksum = calculateChecksum(settings);
	 EEPROM.put(EEPROM_SETTINGS_ADDRESS, stored);
 }

 uint8_t EEPROMStore::calculateChecksum(const PersistedSettings& settings) {
	 return static_cast<uint8_t>(settings.nodeId ^ settings.uiBrightness ^ (settings.firingTime & 0xFF) ^ (settings.displaySleepTimeout & 0xFF) ^ SETTINGS_VERSION);
 }

 PersistedSettings EEPROMStore::defaultSettings() {
	 PersistedSettings defaults = {};
	 defaults.nodeId = DEFAULT_NODE_ID;
	 defaults.uiBrightness = DEFAULT_UI_BRIGHTNESS;
	 defaults.firingTime = 1000;
	 defaults.displaySleepTimeout = 30000;
	 return defaults;
 }
