#include <iostream>
#include <iomanip>
#include <cstdint>

using std::hex;
using std::dec;
using std::cout;
using std::endl;
using std::setfill;
using std::setw;

void dump(
		int16_t s16,
		uint16_t u16,
		int32_t s32,
		uint32_t u32
		)

{
	cout << setfill('0');
	cout << "s16=0x" << hex << setw(4) << s16 << " (" << dec << s16 << ')' << endl;
	cout << "u16=0x" << hex << setw(4) << u16 << " (" << dec << u16 << ')' << endl;
	cout << "s32=0x" << hex << setw(8) << s32 << " (" << dec << s32 << ')' << endl;
	cout << "u32=0x" << hex << setw(8) << u32 << " (" << dec << u32 << ')' << endl;

}

int main() {
	int16_t s16;
	uint16_t u16;

	int32_t s32;
	uint32_t u32;

	cout << "************************************************************" << endl;
	cout << "****** Set all to 0" << endl;
	s16 = 0;
	u16 = 0;
	s32 = 0;
	u32 = 0;
	dump(s16, u16, s32, u32);

	return 0;
}
