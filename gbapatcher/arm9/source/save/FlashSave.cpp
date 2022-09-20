#include "common/tonccpy.h"
#include "find.h"
#include "Save.h"
#include "FlashSave.h"

extern u32 romSize;

static const u8 flash1M_V102_find1[48] = {
	0xaa,0x21,0x19,0x70,0x05,0x4a,0x55,0x21,0x11,0x70,0xb0,0x21,0x19,0x70,0xe0,0x21,
	0x09,0x05,0x08,0x70,0x70,0x47,0x55,0x55,0x00,0x0e,0xaa,0x2a,0x00,0x0e,0x30,0xb5,
	0x91,0xb0,0x68,0x46,0x00,0xf0,0xf3,0xf8,0x6d,0x46,0x01,0x35,0x06,0x4a,0xaa,0x20
};
static const u8 flash1M_V102_replace1[136] = {
	0x80,0x21,0x09,0x02,0x09,0x22,0x12,0x06,0x9f,0x44,0x11,0x80,0x03,0x49,0xc3,0x02,
	0xc9,0x18,0x11,0x80,0x70,0x47,0xfe,0xff,0xff,0x01,0x00,0x00,0x00,0x00,0x30,0xb5,
	0x91,0xb0,0x68,0x46,0x00,0xf0,0xf3,0xf8,0x6d,0x46,0x01,0x35,0x06,0x4a,0xaa,0x20,
	0x00,0x00,0x05,0x49,0x55,0x20,0x00,0x00,0x90,0x20,0x00,0x00,0x10,0xa9,0x03,0x4a,
	0x10,0x1c,0x08,0xe0,0x00,0x00,0x55,0x55,0x00,0x0e,0xaa,0x2a,0x00,0x0e,0x20,0x4e,
	0x00,0x00,0x08,0x88,0x01,0x38,0x08,0x80,0x08,0x88,0x00,0x28,0xf9,0xd1,0x0c,0x48,
	0x13,0x20,0x13,0x20,0x00,0x06,0x04,0x0c,0xe0,0x20,0x00,0x05,0x62,0x20,0x62,0x20,
	0x00,0x06,0x00,0x0e,0x04,0x43,0x07,0x49,0xaa,0x20,0x00,0x00,0x07,0x4a,0x55,0x20,
	0x00,0x00,0xf0,0x20,0x00,0x00,0x00,0x00
};
static const u8 flash1M_V102_find2[24] = {
	0x14,0x49,0xaa,0x24,0x0c,0x70,0x13,0x4b,0x55,0x22,0x1a,0x70,0x80,0x20,0x08,0x70,
	0x0c,0x70,0x1a,0x70,0x10,0x20,0x08,0x70
};
static const u8 flash1M_V102_replace2[24] = {
	0x0e,0x21,0x09,0x06,0xff,0x24,0x80,0x22,0x13,0x4b,0x52,0x02,0x01,0x3a,0x8c,0x54,
	0xfc,0xd1,0x00,0x00,0x00,0x00,0x00,0x00
};
static const u8 flash1M_V102_find3[22] = {
	0xaa,0x25,0x0d,0x70,0x13,0x4b,0x55,0x22,0x1a,0x70,0x80,0x20,0x08,0x70,0x0d,0x70,
	0x1a,0x70,0x30,0x20,0x20,0x70
};
static const u8 flash1M_V102_replace3[22] = {
	0xff,0x25,0x08,0x22,0x00,0x00,0x52,0x02,0x01,0x3a,0xa5,0x54,0xfc,0xd1,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00
};
static const u8 flash1M_V102_find4[12] = {
	0x22,0x70,0x09,0x4b,0x55,0x22,0x1a,0x70,0xa0,0x22,0x22,0x70
};
static const u8 flash1M_V102_replace4[12] = {
	0x00,0x00,0x09,0x4b,0x55,0x22,0x00,0x00,0xa0,0x22,0x00,0x00
};


static const u8 flash1M_V103_find1[98] = {
	0x05,0x4b,0xaa,0x21,0x19,0x70,0x05,0x4a,0x55,0x21,0x11,0x70,0xb0,0x21,0x19,0x70,
	0xe0,0x21,0x09,0x05,0x08,0x70,0x70,0x47,0x55,0x55,0x00,0x0e,0xaa,0x2a,0x00,0x0e,
	0x30,0xb5,0x91,0xb0,0x68,0x46,0x00,0xf0,0xf3,0xf8,0x6d,0x46,0x01,0x35,0x06,0x4a,
	0xaa,0x20,0x10,0x70,0x05,0x49,0x55,0x20,0x08,0x70,0x90,0x20,0x10,0x70,0x10,0xa9,
	0x03,0x4a,0x10,0x1c,0x08,0xe0,0x00,0x00,0x55,0x55,0x00,0x0e,0xaa,0x2a,0x00,0x0e,
	0x20,0x4e,0x00,0x00,0x08,0x88,0x01,0x38,0x08,0x80,0x08,0x88,0x00,0x28,0xf9,0xd1,
	0x0c,0x48
};
static const u8 flash1M_V103_replace1[138] = {
	0x05,0x4b,0x80,0x21,0x09,0x02,0x09,0x22,0x12,0x06,0x9f,0x44,0x11,0x80,0x03,0x49,
	0xc3,0x02,0xc9,0x18,0x11,0x80,0x70,0x47,0xfe,0xff,0xff,0x01,0x00,0x00,0x00,0x00,
	0x30,0xb5,0x91,0xb0,0x68,0x46,0x00,0xf0,0xf3,0xf8,0x6d,0x46,0x01,0x35,0x06,0x4a,
	0xaa,0x20,0x00,0x00,0x05,0x49,0x55,0x20,0x00,0x00,0x90,0x20,0x00,0x00,0x10,0xa9,
	0x03,0x4a,0x10,0x1c,0x08,0xe0,0x00,0x00,0x55,0x55,0x00,0x0e,0xaa,0x2a,0x00,0x0e,
	0x20,0x4e,0x00,0x00,0x08,0x88,0x01,0x38,0x08,0x80,0x08,0x88,0x00,0x28,0xf9,0xd1,
	0x0c,0x48,0x13,0x20,0x13,0x20,0x00,0x06,0x04,0x0c,0xe0,0x20,0x00,0x05,0x62,0x20,
	0x62,0x20,0x00,0x06,0x00,0x0e,0x04,0x43,0x07,0x49,0xaa,0x20,0x00,0x00,0x07,0x4a,
	0x55,0x20,0x00,0x00,0xf0,0x20,0x00,0x00,0x00,0x00
};
static const u8 flash1M_V103_find2[24] = {
	0x14,0x49,0xaa,0x24,0x0c,0x70,0x13,0x4b,0x55,0x22,0x1a,0x70,0x80,0x20,0x08,0x70,
	0x0c,0x70,0x1a,0x70,0x10,0x20,0x08,0x70
};
static const u8 flash1M_V103_replace2[24] = {
	0x0e,0x21,0x09,0x06,0xff,0x24,0x80,0x22,0x13,0x4b,0x52,0x02,0x01,0x3a,0x8c,0x54,
	0xfc,0xd1,0x00,0x00,0x00,0x00,0x00,0x00
};
static const u8 flash1M_V103_find3[22] = {
	0xaa,0x25,0x0d,0x70,0x14,0x4b,0x55,0x22,0x1a,0x70,0x80,0x20,0x08,0x70,0x0d,0x70,
	0x1a,0x70,0x30,0x20,0x20,0x70
};
static const u8 flash1M_V103_replace3[22] = {
	0xff,0x25,0x08,0x22,0x00,0x00,0x52,0x02,0x01,0x3a,0xa5,0x54,0xfc,0xd1,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00
};
static const u8 flash1M_V103_find4[12] = {
	0x10,0x70,0x0b,0x49,0x55,0x20,0x08,0x70,0xa0,0x20,0x10,0x70
};
static const u8 flash1M_V103_replace4[12] = {
	0x00,0x00,0x0b,0x49,0x55,0x20,0x00,0x00,0xa0,0x20,0x00,0x00
};
static const u8 flash1M_V103_find5[12] = {
	0x22,0x70,0x09,0x4b,0x55,0x22,0x1a,0x70,0xa0,0x22,0x22,0x70
};
static const u8 flash1M_V103_replace5[12] = {
	0x00,0x00,0x09,0x4b,0x55,0x22,0x00,0x00,0xa0,0x22,0x00,0x00
};


// FLASH512_V130, V131, and V133 have all the same patches.
// Patches are from gbatemp thread, but gbata doesn't actually support V133.
static const u8 flash512_V13X_find1[38] = {
	0xf0,0xb5,0xa0,0xb0,0x0d,0x1c,0x16,0x1c,0x1f,0x1c,0x03,0x04,0x1c,0x0c,0x0f,0x4a,
	0x10,0x88,0x0f,0x49,0x08,0x40,0x03,0x21,0x08,0x43,0x10,0x80,0x0d,0x48,0x00,0x68,
	0x01,0x68,0x80,0x20,0x80,0x02
};
static const u8 flash512_V13X_replace1[38] = {
	0x70,0xb5,0xa0,0xb0,0x00,0x03,0x40,0x18,0xe0,0x21,0x09,0x05,0x09,0x18,0x08,0x78,
	0x10,0x70,0x01,0x3b,0x01,0x32,0x01,0x31,0x00,0x2b,0xf8,0xd1,0x00,0x20,0x20,0xb0,
	0x70,0xbc,0x02,0xbc,0x08,0x47
};
static const u8 flash512_V13X_find2[8] = {
	0xff,0xf7,0x88,0xfd,0x00,0x04,0x03,0x0c
};
static const u8 flash512_V13X_replace2[8] = {
	0x1b,0x23,0x1b,0x02,0x32,0x20,0x03,0x43
};
static const u8 flash512_V13X_find3[8] = {
	0x70,0xb5,0x90,0xb0,0x15,0x4d,0x29,0x88
};
static const u8 flash512_V13X_find4[8] = {
	0x70,0xb5,0x46,0x46,0x40,0xb4,0x90,0xb0
};
static const u8 flash512_V13X_replace3_4[8] = {
	0x00,0xb5,0x00,0x20,0x02,0xbc,0x08,0x47
};
static const u8 flash512_V13X_find5[24] = {
	0xf0,0xb5,0x90,0xb0,0x0f,0x1c,0x00,0x04,0x04,0x0c,0x03,0x48,0x00,0x68,0x40,0x89,
	0x84,0x42,0x05,0xd3,0x01,0x48,0x41,0xe0
};
static const u8 flash512_V13X_replace5[42] = {
	0x7c,0xb5,0x90,0xb0,0x00,0x03,0x0a,0x1c,0xe0,0x21,0x09,0x05,0x09,0x18,0x01,0x23,
	0x1b,0x03,0x10,0x78,0x08,0x70,0x01,0x3b,0x01,0x32,0x01,0x31,0x00,0x2b,0xf8,0xd1,
	0x00,0x20,0x10,0xb0,0x7c,0xbc,0x02,0xbc,0x08,0x47
};


// Encompasses FLASH_V120 and V121.
static const u8 flash_V12X_find1[12] = {
	0x90,0xb5,0x93,0xb0,0x6f,0x46,0x39,0x1d,0x08,0x1c,0x00,0xf0
};
static const u8 flash_V12X_replace1[14] = {
	0x00,0xb5,0x3d,0x20,0x00,0x02,0x1f,0x21,0x08,0x43,0x02,0xbc,0x08,0x47
};
static const u8 flash_V12X_find2[35] = {
	0x80,0xb5,0x94,0xb0,0x6f,0x46,0x39,0x1c,0x08,0x80,0x38,0x1c,0x01,0x88,0x0f,0x29,
	0x04,0xd9,0x01,0x48,0x56,0xe0,0x00,0x00,0xff,0x80,0x00,0x00,0x23,0x48,0x23,0x49,
	0x0a,0x88,0x23
};
static const u8 flash_V12X_replace2[36] = {
	0x7c,0xb5,0x00,0x07,0x00,0x0c,0xe0,0x21,0x09,0x05,0x09,0x18,0x01,0x23,0x1b,0x03,
	0xff,0x20,0x08,0x70,0x01,0x3b,0x01,0x31,0x00,0x2b,0xfa,0xd1,0x00,0x20,0x7c,0xbc,
	0x02,0xbc,0x08,0x47
};
static const u8 flash_V12X_find3[42] = {
	0x80,0xb5,0x94,0xb0,0x6f,0x46,0x79,0x60,0x39,0x1c,0x08,0x80,0x38,0x1c,0x01,0x88,
	0x0f,0x29,0x03,0xd9,0x00,0x48,0x73,0xe0,0xff,0x80,0x00,0x00,0x38,0x1c,0x01,0x88,
	0x08,0x1c,0xff,0xf7,0x21,0xfe,0x39,0x1c,0x0c,0x31
};
static const u8 flash_V12X_replace3[42] = {
	0x7c,0xb5,0x90,0xb0,0x00,0x03,0x0a,0x1c,0xe0,0x21,0x09,0x05,0x09,0x18,0x01,0x23,
	0x1b,0x03,0x10,0x78,0x08,0x70,0x01,0x3b,0x01,0x32,0x01,0x31,0x00,0x2b,0xf8,0xd1,
	0x00,0x20,0x10,0xb0,0x7c,0xbc,0x08,0xbc,0x08,0x47
};

// Encompasses FLASH_V123, V124, V125, and V126.
// FIXME for FLASH_V125 and FLASH_V126: Medabots/Metarot and Super Monkey Ball Jr. (U) don't patch 1:1 with gbata.
static const u8 flash_V12Y_find1[8] = {
	0xff,0xf7,0xaa,0xff,0x00,0x04,0x03,0x0c
};
static const u8 flash_V12Y_replace1[8] = {
	0x1b,0x23,0x1b,0x02,0x32,0x20,0x03,0x43
};
static const u8 flash_V12Y_find2[6] = {
	0x70,0xb5,0x90,0xb0,0x15,0x4d
};
static const u8 flash_V12Y_replace2[4] = {
	0x00,0x20,0x70,0x47/*,0x15,0x4d*/
};
// Patch 3 differs from GBATemp tutorial.
// The added bytes at the end have significance.
static const u8 flash_V12Y_find3[9] = {
	0x70,0xb5,0x46,0x46,0x40,0xb4,0x90,0xb0,0x00
};
static const u8 flash_V12Y_replace3[4] = {
	0x00,0x20,0x70,0x47/*,0x40,0xb4,0x90,0xb0,0x00*/
};
static const u8 flash_V12Y_find4[38] = {
	0xf0,0xb5,0x90,0xb0,0x0f,0x1c,0x00,0x04,0x04,0x0c,0x0f,0x2c,0x04,0xd9,0x01,0x48,
	0x40,0xe0,0x00,0x00,0xff,0x80,0x00,0x00,0x20,0x1c,0xff,0xf7,0xd7,0xfe,0x00,0x04,
	0x05,0x0c,0x00,0x2d,0x35,0xd1
};
static const u8 flash_V12Y_replace4[38] = {
	0x70,0xb5,0x00,0x03,0x0a,0x1c,0xe0,0x21,0x09,0x05,0x41,0x18,0x01,0x23,0x1b,0x03,
	0x10,0x78,0x08,0x70,0x01,0x3b,0x01,0x32,0x01,0x31,0x00,0x2b,0xf8,0xd1,0x00,0x20,
	0x70,0xbc,0x02,0xbc,0x08,0x47
};


bool flash_patchV120(const save_type_t* type)
{
	u8* func1 = memsearch8((u8*)0x08000000, romSize, flash_V12X_find1, sizeof(flash_V12X_find1), true);
	if (!func1)
		return false;
	tonccpy(func1, &flash_V12X_replace1, sizeof(flash_V12X_replace1));

	u8* func2 = memsearch8((u8*)0x08000000, romSize, flash_V12X_find2, sizeof(flash_V12X_find2), true);
	if (!func2)
		return false;
	tonccpy(func2, &flash_V12X_replace2, sizeof(flash_V12X_replace2));

	u8* func3 = memsearch8((u8*)0x08000000, romSize, flash_V12X_find3, sizeof(flash_V12X_find3), true);
	if (!func3)
		return false;
	tonccpy(func3, &flash_V12X_replace3, sizeof(flash_V12X_replace3));

	return true;
}

bool flash_patchV123(const save_type_t* type)
{
	u8* func1 = memsearch8((u8*)0x08000000, romSize, flash_V12Y_find1, sizeof(flash_V12Y_find1), true);
	if (!func1)
		return false;
	tonccpy(func1, &flash_V12Y_replace1, sizeof(flash_V12Y_replace1));

	u8* func2 = memsearch8((u8*)0x08000000, romSize, flash_V12Y_find2, sizeof(flash_V12Y_find2), true);
	if (!func2)
		return false;
	tonccpy(func2, &flash_V12Y_replace2, sizeof(flash_V12Y_replace2));

	u8* func3 = memsearch8((u8*)0x08000000, romSize, flash_V12Y_find3, sizeof(flash_V12Y_find3), true);
	if (!func3)
		return false;
	tonccpy(func3, &flash_V12Y_replace3, sizeof(flash_V12Y_replace3));

	u8* func4 = memsearch8((u8*)0x08000000, romSize, flash_V12Y_find4, sizeof(flash_V12Y_find4), true);
	if (!func4)
		return false;
	tonccpy(func4, &flash_V12Y_replace4, sizeof(flash_V12Y_replace4));

	return true;
}

/*bool flash_patchV126(const save_type_t* type)
{
}*/

bool flash_patch512V130(const save_type_t* type)
{
	u32* romPos = (u32*)0x08000000;
	u32 curRomSize = romSize;
	u32 startSig[4] = {0};
	startSig[0] = *(u32*)0x08000000;
	startSig[1] = *(u32*)0x08000004;
	startSig[2] = *(u32*)0x08000008;
	startSig[3] = *(u32*)0x0800000C;

	for (int i = 0; i < 2; i++) {

	if (i != 0) {
		while (romPos < romPos+romSize) {
			// Look for another ROM in 2 in 1 game packs
			romPos += 0x100000;
			curRomSize -= 0x100000;
			if (curRomSize <= 0) break;
			if (romPos[0] == startSig[0]
			&& romPos[1] == startSig[1]
			&& romPos[2] == startSig[2]
			&& romPos[3] == startSig[3]) {
				break;
			}
		}

		if (romPos >= romPos+romSize) break;
	}

	u8* func1 = memsearch8((u8*)romPos, curRomSize, flash512_V13X_find1, sizeof(flash512_V13X_find1), true);
	if (!func1)
		return false;
	tonccpy(func1, &flash512_V13X_replace1, sizeof(flash512_V13X_replace1));

	u8* func2 = memsearch8((u8*)romPos, curRomSize, flash512_V13X_find2, sizeof(flash512_V13X_find2), true);
	if (!func2)
		return false;
	tonccpy(func2, &flash512_V13X_replace2, sizeof(flash512_V13X_replace2));

	u8* func3 = memsearch8((u8*)romPos, curRomSize, flash512_V13X_find3, sizeof(flash512_V13X_find3), true);
	if (!func3)
		return false;
	tonccpy(func3, &flash512_V13X_replace3_4, sizeof(flash512_V13X_replace3_4));

	u8* func4 = memsearch8((u8*)romPos, curRomSize, flash512_V13X_find4, sizeof(flash512_V13X_find4), true);
	if (!func4)
		return false;
	tonccpy(func4, &flash512_V13X_replace3_4, sizeof(flash512_V13X_replace3_4));

	u8* func5 = memsearch8((u8*)romPos, curRomSize, flash512_V13X_find5, sizeof(flash512_V13X_find5), true);
	if (!func5)
		return false;
	tonccpy(func5, &flash512_V13X_replace5, sizeof(flash512_V13X_replace5));

	}

	return true;
}

bool flash_patch1MV102(const save_type_t* type)
{
	u8* func1 = memsearch8((u8*)0x08000000, romSize, flash1M_V102_find1, sizeof(flash1M_V102_find1), true);
	if (!func1)
		return false;
	tonccpy(func1, &flash1M_V102_replace1, sizeof(flash1M_V102_replace1));

	u8* func2 = memsearch8((u8*)0x08000000, romSize, flash1M_V102_find2, sizeof(flash1M_V102_find2), true);
	if (!func2)
		return false;
	tonccpy(func2, &flash1M_V102_replace2, sizeof(flash1M_V102_replace2));

	u8* func3 = memsearch8((u8*)0x08000000, romSize, flash1M_V102_find3, sizeof(flash1M_V102_find3), true);
	if (!func3)
		return false;
	tonccpy(func3, &flash1M_V102_replace3, sizeof(flash1M_V102_replace3));

	u8* func4 = memsearch8((u8*)0x08000000, romSize, flash1M_V102_find4, sizeof(flash1M_V102_find4), true);
	if (!func4)
		return false;
	tonccpy(func4, &flash1M_V102_replace4, sizeof(flash1M_V102_replace4));

	return true;
}

bool flash_patch1MV103(const save_type_t* type)
{
	u8* func1 = memsearch8((u8*)0x08000000, romSize, flash1M_V103_find1, sizeof(flash1M_V103_find1), true);
	if (!func1)
		return false;
	tonccpy(func1, &flash1M_V103_replace1, sizeof(flash1M_V103_replace1));

	u8* func2 = memsearch8((u8*)0x08000000, romSize, flash1M_V103_find2, sizeof(flash1M_V103_find2), true);
	if (!func2)
		return false;
	tonccpy(func2, &flash1M_V103_replace2, sizeof(flash1M_V103_replace2));

	u8* func3 = memsearch8((u8*)0x08000000, romSize, flash1M_V103_find3, sizeof(flash1M_V103_find3), true);
	if (!func3)
		return false;
	tonccpy(func3, &flash1M_V103_replace3, sizeof(flash1M_V103_replace3));

	u8* func4 = memsearch8((u8*)0x08000000, romSize, flash1M_V103_find4, sizeof(flash1M_V103_find4), true);
	if (!func4)
		return false;
	tonccpy(func4, &flash1M_V103_replace4, sizeof(flash1M_V103_replace4));

	u8* func5 = memsearch8((u8*)0x08000000, romSize, flash1M_V103_find5, sizeof(flash1M_V103_find5), true);
	if (!func5)
		return false;
	tonccpy(func5, &flash1M_V103_replace5, sizeof(flash1M_V103_replace5));

	return true;
}
