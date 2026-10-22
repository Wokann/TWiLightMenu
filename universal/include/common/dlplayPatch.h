#ifndef DLPLAY_PATCH_H
#define DLPLAY_PATCH_H

// RSA signature check patch for DSi DS Download Play (HNDA v256), ported from FlashMe
// https://gbatemp.net/threads/rsa-patch-for-dsi-download-play.538078/

// Returns the path to boot: a patched copy when the patch applies, otherwise srcPath
const char* dlplayGetBootPath(const char* srcPath, const char* patchedPath);

class CIniFile;

enum DlplayVirtualSlot1Error {
	DLPLAY_VIRTUAL_SLOT1_LOCAL_RECEIVER = -20,
	DLPLAY_VIRTUAL_SLOT1_ROM_MISSING = -21,
	DLPLAY_VIRTUAL_SLOT1_SAVE_MISSING = -22,
	DLPLAY_CONFIG_WRITE_FAILED = -23
};

// Configure the selected ROM's existing save. Leave the INI unchanged on error.
int dlplayConfigureSlot1(CIniFile& bootstrapini, bool rebootToSystemTitle);

#endif // DLPLAY_PATCH_H
