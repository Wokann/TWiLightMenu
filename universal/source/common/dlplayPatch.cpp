#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>

#include "common/dlplayPatch.h"
#include "common/logging.h"
#include "common/inifile.h"
#include "common/systemdetails.h"
#include "common/twlmenusettings.h"

#define DLPLAY_SIZE 433088
#define DLPLAY_HEADER_SIZE 0x160 // Up to the header CRC, which differs between versions

// Both versions have the same code, only their strings differ
static const struct {
	const char* name;
	unsigned int originalCrc;
	unsigned int patchedCrc;
} dlplayVersions[] = {
	{"DSi", 0x5F5BFC6A, 0x3D505BFF},
	{"3DS", 0x57D48E69, 0x35DF29FC},
};
static const int dlplayVersionCount = sizeof(dlplayVersions) / sizeof(dlplayVersions[0]);

static const unsigned int patchOffsets[] = {0xD898, 0xDC44};
static const unsigned char patchCode[] = {0x01, 0x20, 0x70, 0x47}; // movs r0, #1; bx lr

static unsigned int crc32(const unsigned char* buf, size_t size) {
	static unsigned int table[256];
	if (table[1] == 0) {
		for (unsigned int i = 0; i < 256; i++) {
			unsigned int crc = i;
			for (int j = 0; j < 8; j++) {
				crc = (crc >> 1) ^ (0xEDB88320 & -(crc & 1));
			}
			table[i] = crc;
		}
	}

	unsigned int crc = 0xFFFFFFFF;
	for (size_t i = 0; i < size; i++) {
		crc = (crc >> 8) ^ table[(crc ^ buf[i]) & 0xFF];
	}
	return ~crc;
}

// Loads the first `size` bytes of the file into buf, if the file is DLPLAY_SIZE bytes
static bool loadDlplay(const char* path, unsigned char* buf, size_t size) {
	FILE* file = fopen(path, "rb");
	if (!file) {
		return false;
	}
	fseek(file, 0, SEEK_END);
	const bool sizeMatches = (ftell(file) == DLPLAY_SIZE);
	fseek(file, 0, SEEK_SET);
	const bool loaded = sizeMatches && (fread(buf, 1, size, file) == size);
	fclose(file);
	return loaded;
}

const char* dlplayGetBootPath(const char* srcPath, const char* patchedPath) {
	unsigned char* buf = (unsigned char*)malloc(DLPLAY_SIZE);
	if (!buf) {
		logPrint("DS Download Play RSA patch: Out of memory, launching unpatched\n");
		return srcPath;
	}

	// Reuse the patched copy from a previous launch, if it was made from this source
	unsigned char srcHeader[DLPLAY_HEADER_SIZE];
	if (loadDlplay(srcPath, srcHeader, sizeof(srcHeader)) && loadDlplay(patchedPath, buf, DLPLAY_SIZE)
	 && memcmp(buf, srcHeader, sizeof(srcHeader)) == 0) {
		const unsigned int crc = crc32(buf, DLPLAY_SIZE);
		for (int i = 0; i < dlplayVersionCount; i++) {
			if (crc == dlplayVersions[i].patchedCrc) {
				free(buf);
				logPrint("DS Download Play RSA patch: Using %s (%s)\n", patchedPath, dlplayVersions[i].name);
				return patchedPath;
			}
		}
	}

	int version = -1;
	if (loadDlplay(srcPath, buf, DLPLAY_SIZE)) {
		const unsigned int crc = crc32(buf, DLPLAY_SIZE);
		for (int i = 0; i < dlplayVersionCount; i++) {
			if (crc == dlplayVersions[i].originalCrc) {
				version = i;
				break;
			}
		}
	}
	if (version < 0) {
		free(buf);
		logPrint("DS Download Play RSA patch: Unsupported file, launching unpatched\n");
		return srcPath;
	}

	for (unsigned int offset : patchOffsets) {
		memcpy(buf + offset, patchCode, sizeof(patchCode));
	}

	bool written = false;
	if (crc32(buf, DLPLAY_SIZE) == dlplayVersions[version].patchedCrc) {
		FILE* file = fopen(patchedPath, "wb");
		if (file) {
			written = (fwrite(buf, 1, DLPLAY_SIZE, file) == DLPLAY_SIZE);
			fclose(file);
		}
	}
	free(buf);

	if (!written) {
		remove(patchedPath);
		logPrint("DS Download Play RSA patch: Failed to write %s, launching unpatched\n", patchedPath);
		return srcPath;
	}

	logPrint("DS Download Play RSA patch: Created %s (%s)\n", patchedPath, dlplayVersions[version].name);
	return patchedPath;
}

int dlplayConfigureSlot1(CIniFile& bootstrapini, bool rebootToSystemTitle) {
	const int mode = ms().dlplaySlot1Mode;
	if (mode < 0 || mode > 2) {
		return DLPLAY_VIRTUAL_SLOT1_ROM_MISSING;
	}
	if (mode != 0 && rebootToSystemTitle) {
		return DLPLAY_VIRTUAL_SLOT1_LOCAL_RECEIVER;
	}

	const std::string& romPath = ms().dlplayVirtualSlot1RomPath;
	std::string savePath;
	if (mode == 1 && !romPath.empty()) {
		const bool onFlashcard = romPath.compare(0, 5, "fat:/") == 0;
		const size_t slash = romPath.find_last_of('/');
		const size_t dot = romPath.find_last_of('.');
		if ((!onFlashcard && romPath.compare(0, 4, "sd:/") != 0)
		 || slash == std::string::npos || dot == std::string::npos || dot <= slash || dot+1 == romPath.size()
		 || ms().saveLocation < TWLSettings::ESavesFolder || ms().saveLocation > TWLSettings::ETWLMFolder
		 || access(romPath.c_str(), R_OK)) {
			return DLPLAY_VIRTUAL_SLOT1_ROM_MISSING;
		}

		FILE* file = fopen(romPath.c_str(), "rb");
		unsigned char header[0x40] = {};
		const bool valid = file && fread(header, 1, sizeof(header), file) == sizeof(header)
			&& (header[0x20] | header[0x21] | header[0x22] | header[0x23])
			&& (header[0x2C] | header[0x2D] | header[0x2E] | header[0x2F]);
		if (file) fclose(file);
		if (!valid) return DLPLAY_VIRTUAL_SLOT1_ROM_MISSING;

		const std::string settingsPath = std::string(onFlashcard ? "fat:/" : "sd:/")
			+ "_nds/TWiLightMenu/gamesettings/" + romPath.substr(slash+1) + ".ini";
		CIniFile gameSettings(settingsPath);
		const int saveNo = gameSettings.GetInt("GAMESETTINGS", "SAVE_NUMBER", 0);
		if (saveNo < 0 || saveNo > 9) return DLPLAY_VIRTUAL_SLOT1_SAVE_MISSING;

		std::string saveName = romPath.substr(slash+1, dot-slash-1) + ".sav";
		if (saveNo && ms().saveLocation != TWLSettings::EGamesFolder) {
			saveName += std::to_string(saveNo);
		}
		switch (ms().saveLocation) {
			case TWLSettings::ESavesFolder:
				savePath = romPath.substr(0, slash+1) + "saves/" + saveName;
				break;
			case TWLSettings::EGamesFolder:
				savePath = romPath.substr(0, slash+1) + saveName;
				break;
			case TWLSettings::ETWLMFolder:
				savePath = std::string(sys().isRunFromSD() ? "sd:/" : "fat:/") + "_nds/TWiLightMenu/saves/" + saveName;
				break;
		}
		if (access(savePath.c_str(), R_OK | W_OK)) {
			return DLPLAY_VIRTUAL_SLOT1_SAVE_MISSING;
		}
	}

	bootstrapini.SetInt("NDS-BOOTSTRAP", "DLP_SLOT1_MODE", mode == 0 ? 0 : 1);
	bootstrapini.SetString("NDS-BOOTSTRAP", "DLP_VIRTUAL_SLOT1_ROM_PATH", mode == 1 ? romPath : "");
	bootstrapini.SetString("NDS-BOOTSTRAP", "DLP_VIRTUAL_SLOT1_SAV_PATH", savePath);
	return 0;
}
