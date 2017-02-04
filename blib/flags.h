// File     : flags.h
// Purpose  : To provide a simple interface to standard C bit operators.
// Contains : class Flags
//
// Update Log -
//
// 20090609 - Begun
// 20110604 - Added Flags32, identical to Flags8 but using int instead of char


// prototypes
namespace blib
{
class  Flags8;
class  Flags32;
} // namespace blib


#ifndef FLAGS_CLASS_DEFINITIONS
#define FLAGS_CLASS_DEFINITIONS


#include "blib.h"      // blib globals


namespace blib
{


// Bit Masks used for bit manipulation of byte
#define  BIT8_MASK_0  0x01    // 00000001
#define  BIT8_MASK_1  0x02    // 00000010
#define  BIT8_MASK_2  0x04    // 00000100
#define  BIT8_MASK_3  0x08    // 00001000
#define  BIT8_MASK_4  0x10    // 00010000
#define  BIT8_MASK_5  0x20    // 00100000
#define  BIT8_MASK_6  0x40    // 01000000
#define  BIT8_MASK_7  0x80    // 10000000

// Bit Masks used for bit manipulation of 4 byte type (such as int)
#define  BIT32_MASK_00  0x00000001    // 00000000 00000000 00000000 00000001
#define  BIT32_MASK_01  0x00000002    // 00000000 00000000 00000000 00000010
#define  BIT32_MASK_02  0x00000004    // 00000000 00000000 00000000 00000100
#define  BIT32_MASK_03  0x00000008    // 00000000 00000000 00000000 00001000
#define  BIT32_MASK_04  0x00000010    // 00000000 00000000 00000000 00010000
#define  BIT32_MASK_05  0x00000020    // 00000000 00000000 00000000 00100000
#define  BIT32_MASK_06  0x00000040    // 00000000 00000000 00000000 01000000
#define  BIT32_MASK_07  0x00000080    // 00000000 00000000 00000000 10000000
#define  BIT32_MASK_08  0x00000100    // 00000000 00000000 00000001 00000000
#define  BIT32_MASK_09  0x00000200    // 00000000 00000000 00000010 00000000
#define  BIT32_MASK_10  0x00000400    // 00000000 00000000 00000100 00000000
#define  BIT32_MASK_11  0x00000800    // 00000000 00000000 00001000 00000000
#define  BIT32_MASK_12  0x00001000    // 00000000 00000000 00010000 00000000
#define  BIT32_MASK_13  0x00002000    // 00000000 00000000 00100000 00000000
#define  BIT32_MASK_14  0x00004000    // 00000000 00000000 01000000 00000000
#define  BIT32_MASK_15  0x00008000    // 00000000 00000000 10000000 00000000
#define  BIT32_MASK_16  0x00010000    // 00000000 00000001 00000000 00000000
#define  BIT32_MASK_17  0x00020000    // 00000000 00000010 00000000 00000000
#define  BIT32_MASK_18  0x00040000    // 00000000 00000100 00000000 00000000
#define  BIT32_MASK_19  0x00080000    // 00000000 00001000 00000000 00000000
#define  BIT32_MASK_20  0x00100000    // 00000000 00010000 00000000 00000000
#define  BIT32_MASK_21  0x00200000    // 00000000 00100000 00000000 00000000
#define  BIT32_MASK_22  0x00400000    // 00000000 01000000 00000000 00000000
#define  BIT32_MASK_23  0x00800000    // 00000000 10000000 00000000 00000000
#define  BIT32_MASK_24  0x01000000    // 00000001 00000000 00000000 00000000
#define  BIT32_MASK_25  0x02000000    // 00000010 00000000 00000000 00000000
#define  BIT32_MASK_26  0x04000000    // 00000100 00000000 00000000 00000000
#define  BIT32_MASK_27  0x08000000    // 00001000 00000000 00000000 00000000
#define  BIT32_MASK_28  0x10000000    // 00010000 00000000 00000000 00000000
#define  BIT32_MASK_29  0x20000000    // 00100000 00000000 00000000 00000000
#define  BIT32_MASK_30  0x40000000    // 01000000 00000000 00000000 00000000
#define  BIT32_MASK_31  0x80000000    // 10000000 00000000 00000000 00000000


// Class   : Flags8
// Purpose : To provide a simple interface to standard C bit operators.
//           Member 'flags' (being a char) provides 8 bit flags.
//           The methods abstract the code to treat these as type bool.
class Flags8
{
    private:
        char  flags;    // contains 8 bit flags

    public:
        // constructor
        Flags8(void) : flags(0) {}

        // inspectors
        bool flag0(void) const { return flags & BIT8_MASK_0; }
        bool flag1(void) const { return flags & BIT8_MASK_1; }
        bool flag2(void) const { return flags & BIT8_MASK_2; }
        bool flag3(void) const { return flags & BIT8_MASK_3; }
        bool flag4(void) const { return flags & BIT8_MASK_4; }
        bool flag5(void) const { return flags & BIT8_MASK_5; }
        bool flag6(void) const { return flags & BIT8_MASK_6; }
        bool flag7(void) const { return flags & BIT8_MASK_7; }
        bool flag(const int &i) const { return flags & (char)(1 << i); }
        bool operator[](const int& i) const { return flag( i ); }

        // mutators
        // XOR flips test bit, ignores others
        void flip_flag0(void) { flags ^= BIT8_MASK_0; }
        void flip_flag1(void) { flags ^= BIT8_MASK_1; }
        void flip_flag2(void) { flags ^= BIT8_MASK_2; }
        void flip_flag3(void) { flags ^= BIT8_MASK_3; }
        void flip_flag4(void) { flags ^= BIT8_MASK_4; }
        void flip_flag5(void) { flags ^= BIT8_MASK_5; }
        void flip_flag6(void) { flags ^= BIT8_MASK_6; }
        void flip_flag7(void) { flags ^= BIT8_MASK_7; }
        void flip_flag(const int& i) { flags ^= (char)(1 << i); }

        // OR turns test bit On, ignores others
        void turn_on_flag0(void) { flags |= BIT8_MASK_0; }
        void turn_on_flag1(void) { flags |= BIT8_MASK_1; }
        void turn_on_flag2(void) { flags |= BIT8_MASK_2; }
        void turn_on_flag3(void) { flags |= BIT8_MASK_3; }
        void turn_on_flag4(void) { flags |= BIT8_MASK_4; }
        void turn_on_flag5(void) { flags |= BIT8_MASK_5; }
        void turn_on_flag6(void) { flags |= BIT8_MASK_6; }
        void turn_on_flag7(void) { flags |= BIT8_MASK_7; }
        void turn_on_flag(const int& i) { flags |= (char)(1 << i); }

        // OR followed by XOR turns test bit Off, ignores others
        void turn_off_flag0(void) { flags = (flags | BIT8_MASK_0) ^ BIT8_MASK_0; }
        void turn_off_flag1(void) { flags = (flags | BIT8_MASK_1) ^ BIT8_MASK_1; }
        void turn_off_flag2(void) { flags = (flags | BIT8_MASK_2) ^ BIT8_MASK_2; }
        void turn_off_flag3(void) { flags = (flags | BIT8_MASK_3) ^ BIT8_MASK_3; }
        void turn_off_flag4(void) { flags = (flags | BIT8_MASK_4) ^ BIT8_MASK_4; }
        void turn_off_flag5(void) { flags = (flags | BIT8_MASK_5) ^ BIT8_MASK_5; }
        void turn_off_flag6(void) { flags = (flags | BIT8_MASK_6) ^ BIT8_MASK_6; }
        void turn_off_flag7(void) { flags = (flags | BIT8_MASK_7) ^ BIT8_MASK_7; }
        void turn_off_flag(const int& i)
        { char mask = (char)(1 << i); flags = (flags | mask) ^ mask; }
}; // class Flags8


// Class   : Flags32
// Purpose : To provide a simple interface to standard C bit operators.
//           Member 'flags' (being an int) provides 32 bit flags (on a 32 bit machine).
//           The methods abstract the code to treat these as type bool.
// Warning : If sizeof(int) != 4 then an int doesn't contain 32 bits,
//           and behavior is unpredicatable.
//           On many machines sizeof(int) == #-of-bytes-of-registar
//           so since most machines are 32 bit or larger this should not
//           cause bugs but simply cause greater memory use then necessary.
//           But if run on a 16 bit machine segmentation faults are expected.
class Flags32
{
    private:
        int  flags;    // contains 32 bit flags

    public:
        // constructor
        Flags32(void) : flags(0) {}

        // inspectors
        bool flag0(void) const  { return flags & BIT32_MASK_00; }
        bool flag1(void) const  { return flags & BIT32_MASK_01; }
        bool flag2(void) const  { return flags & BIT32_MASK_02; }
        bool flag3(void) const  { return flags & BIT32_MASK_03; }
        bool flag4(void) const  { return flags & BIT32_MASK_04; }
        bool flag5(void) const  { return flags & BIT32_MASK_05; }
        bool flag6(void) const  { return flags & BIT32_MASK_06; }
        bool flag7(void) const  { return flags & BIT32_MASK_07; }
        bool flag8(void) const  { return flags & BIT32_MASK_08; }
        bool flag9(void) const  { return flags & BIT32_MASK_09; }
        bool flag10(void) const { return flags & BIT32_MASK_10; }
        bool flag11(void) const { return flags & BIT32_MASK_11; }
        bool flag12(void) const { return flags & BIT32_MASK_12; }
        bool flag13(void) const { return flags & BIT32_MASK_13; }
        bool flag14(void) const { return flags & BIT32_MASK_14; }
        bool flag15(void) const { return flags & BIT32_MASK_15; }
        bool flag16(void) const { return flags & BIT32_MASK_16; }
        bool flag17(void) const { return flags & BIT32_MASK_17; }
        bool flag18(void) const { return flags & BIT32_MASK_18; }
        bool flag19(void) const { return flags & BIT32_MASK_19; }
        bool flag20(void) const { return flags & BIT32_MASK_20; }
        bool flag21(void) const { return flags & BIT32_MASK_21; }
        bool flag22(void) const { return flags & BIT32_MASK_22; }
        bool flag23(void) const { return flags & BIT32_MASK_23; }
        bool flag24(void) const { return flags & BIT32_MASK_24; }
        bool flag25(void) const { return flags & BIT32_MASK_25; }
        bool flag26(void) const { return flags & BIT32_MASK_26; }
        bool flag27(void) const { return flags & BIT32_MASK_27; }
        bool flag28(void) const { return flags & BIT32_MASK_28; }
        bool flag29(void) const { return flags & BIT32_MASK_29; }
        bool flag30(void) const { return flags & BIT32_MASK_30; }
        bool flag31(void) const { return flags & BIT32_MASK_31; }
        bool flag(const int &i) const { return flags & (int)(1 << i); }
        bool operator[](const int& i) const { return flag( i ); }

        // mutators
        // XOR flips test bit, ignores others
        void flip_flag0(void)  { flags ^= BIT32_MASK_00; }
        void flip_flag1(void)  { flags ^= BIT32_MASK_01; }
        void flip_flag2(void)  { flags ^= BIT32_MASK_02; }
        void flip_flag3(void)  { flags ^= BIT32_MASK_03; }
        void flip_flag4(void)  { flags ^= BIT32_MASK_04; }
        void flip_flag5(void)  { flags ^= BIT32_MASK_05; }
        void flip_flag6(void)  { flags ^= BIT32_MASK_06; }
        void flip_flag7(void)  { flags ^= BIT32_MASK_07; }
        void flip_flag8(void)  { flags ^= BIT32_MASK_08; }
        void flip_flag9(void)  { flags ^= BIT32_MASK_09; }
        void flip_flag10(void) { flags ^= BIT32_MASK_10; }
        void flip_flag11(void) { flags ^= BIT32_MASK_11; }
        void flip_flag12(void) { flags ^= BIT32_MASK_12; }
        void flip_flag13(void) { flags ^= BIT32_MASK_13; }
        void flip_flag14(void) { flags ^= BIT32_MASK_14; }
        void flip_flag15(void) { flags ^= BIT32_MASK_15; }
        void flip_flag16(void) { flags ^= BIT32_MASK_16; }
        void flip_flag17(void) { flags ^= BIT32_MASK_17; }
        void flip_flag18(void) { flags ^= BIT32_MASK_18; }
        void flip_flag19(void) { flags ^= BIT32_MASK_19; }
        void flip_flag20(void) { flags ^= BIT32_MASK_20; }
        void flip_flag21(void) { flags ^= BIT32_MASK_21; }
        void flip_flag22(void) { flags ^= BIT32_MASK_22; }
        void flip_flag23(void) { flags ^= BIT32_MASK_23; }
        void flip_flag24(void) { flags ^= BIT32_MASK_24; }
        void flip_flag25(void) { flags ^= BIT32_MASK_25; }
        void flip_flag26(void) { flags ^= BIT32_MASK_26; }
        void flip_flag27(void) { flags ^= BIT32_MASK_27; }
        void flip_flag28(void) { flags ^= BIT32_MASK_28; }
        void flip_flag29(void) { flags ^= BIT32_MASK_29; }
        void flip_flag30(void) { flags ^= BIT32_MASK_30; }
        void flip_flag31(void) { flags ^= BIT32_MASK_31; }
        void flip_flag(const int& i) { flags ^= (char)(1 << i); }

        // OR turns test bit On, ignores others
        void turn_on_flag0(void)  { flags |= BIT32_MASK_00; }
        void turn_on_flag1(void)  { flags |= BIT32_MASK_01; }
        void turn_on_flag2(void)  { flags |= BIT32_MASK_02; }
        void turn_on_flag3(void)  { flags |= BIT32_MASK_03; }
        void turn_on_flag4(void)  { flags |= BIT32_MASK_04; }
        void turn_on_flag5(void)  { flags |= BIT32_MASK_05; }
        void turn_on_flag6(void)  { flags |= BIT32_MASK_06; }
        void turn_on_flag7(void)  { flags |= BIT32_MASK_07; }
        void turn_on_flag8(void)  { flags |= BIT32_MASK_08; }
        void turn_on_flag9(void)  { flags |= BIT32_MASK_09; }
        void turn_on_flag10(void) { flags |= BIT32_MASK_10; }
        void turn_on_flag11(void) { flags |= BIT32_MASK_11; }
        void turn_on_flag12(void) { flags |= BIT32_MASK_12; }
        void turn_on_flag13(void) { flags |= BIT32_MASK_13; }
        void turn_on_flag14(void) { flags |= BIT32_MASK_14; }
        void turn_on_flag15(void) { flags |= BIT32_MASK_15; }
        void turn_on_flag16(void) { flags |= BIT32_MASK_16; }
        void turn_on_flag17(void) { flags |= BIT32_MASK_17; }
        void turn_on_flag18(void) { flags |= BIT32_MASK_18; }
        void turn_on_flag19(void) { flags |= BIT32_MASK_19; }
        void turn_on_flag20(void) { flags |= BIT32_MASK_20; }
        void turn_on_flag21(void) { flags |= BIT32_MASK_21; }
        void turn_on_flag22(void) { flags |= BIT32_MASK_22; }
        void turn_on_flag23(void) { flags |= BIT32_MASK_23; }
        void turn_on_flag24(void) { flags |= BIT32_MASK_24; }
        void turn_on_flag25(void) { flags |= BIT32_MASK_25; }
        void turn_on_flag26(void) { flags |= BIT32_MASK_26; }
        void turn_on_flag27(void) { flags |= BIT32_MASK_27; }
        void turn_on_flag28(void) { flags |= BIT32_MASK_28; }
        void turn_on_flag29(void) { flags |= BIT32_MASK_29; }
        void turn_on_flag30(void) { flags |= BIT32_MASK_30; }
        void turn_on_flag31(void) { flags |= BIT32_MASK_31; }
        void turn_on_flag(const int& i) { flags |= (char)(1 << i); }

        // OR followed by XOR turns test bit Off, ignores others
        void turn_off_flag0(void)  { flags = (flags | BIT32_MASK_00) ^ BIT32_MASK_00; }
        void turn_off_flag1(void)  { flags = (flags | BIT32_MASK_01) ^ BIT32_MASK_01; }
        void turn_off_flag2(void)  { flags = (flags | BIT32_MASK_02) ^ BIT32_MASK_02; }
        void turn_off_flag3(void)  { flags = (flags | BIT32_MASK_03) ^ BIT32_MASK_03; }
        void turn_off_flag4(void)  { flags = (flags | BIT32_MASK_04) ^ BIT32_MASK_04; }
        void turn_off_flag5(void)  { flags = (flags | BIT32_MASK_05) ^ BIT32_MASK_05; }
        void turn_off_flag6(void)  { flags = (flags | BIT32_MASK_06) ^ BIT32_MASK_06; }
        void turn_off_flag7(void)  { flags = (flags | BIT32_MASK_07) ^ BIT32_MASK_07; }
        void turn_off_flag8(void)  { flags = (flags | BIT32_MASK_08) ^ BIT32_MASK_08; }
        void turn_off_flag9(void)  { flags = (flags | BIT32_MASK_09) ^ BIT32_MASK_09; }
        void turn_off_flag10(void) { flags = (flags | BIT32_MASK_10) ^ BIT32_MASK_10; }
        void turn_off_flag11(void) { flags = (flags | BIT32_MASK_11) ^ BIT32_MASK_11; }
        void turn_off_flag12(void) { flags = (flags | BIT32_MASK_12) ^ BIT32_MASK_12; }
        void turn_off_flag13(void) { flags = (flags | BIT32_MASK_13) ^ BIT32_MASK_13; }
        void turn_off_flag14(void) { flags = (flags | BIT32_MASK_14) ^ BIT32_MASK_14; }
        void turn_off_flag15(void) { flags = (flags | BIT32_MASK_15) ^ BIT32_MASK_15; }
        void turn_off_flag16(void) { flags = (flags | BIT32_MASK_16) ^ BIT32_MASK_16; }
        void turn_off_flag17(void) { flags = (flags | BIT32_MASK_17) ^ BIT32_MASK_17; }
        void turn_off_flag18(void) { flags = (flags | BIT32_MASK_18) ^ BIT32_MASK_18; }
        void turn_off_flag19(void) { flags = (flags | BIT32_MASK_19) ^ BIT32_MASK_19; }
        void turn_off_flag20(void) { flags = (flags | BIT32_MASK_20) ^ BIT32_MASK_20; }
        void turn_off_flag21(void) { flags = (flags | BIT32_MASK_21) ^ BIT32_MASK_21; }
        void turn_off_flag22(void) { flags = (flags | BIT32_MASK_22) ^ BIT32_MASK_22; }
        void turn_off_flag23(void) { flags = (flags | BIT32_MASK_23) ^ BIT32_MASK_23; }
        void turn_off_flag24(void) { flags = (flags | BIT32_MASK_24) ^ BIT32_MASK_24; }
        void turn_off_flag25(void) { flags = (flags | BIT32_MASK_25) ^ BIT32_MASK_25; }
        void turn_off_flag26(void) { flags = (flags | BIT32_MASK_26) ^ BIT32_MASK_26; }
        void turn_off_flag27(void) { flags = (flags | BIT32_MASK_27) ^ BIT32_MASK_27; }
        void turn_off_flag28(void) { flags = (flags | BIT32_MASK_28) ^ BIT32_MASK_28; }
        void turn_off_flag29(void) { flags = (flags | BIT32_MASK_29) ^ BIT32_MASK_29; }
        void turn_off_flag30(void) { flags = (flags | BIT32_MASK_30) ^ BIT32_MASK_30; }
        void turn_off_flag31(void) { flags = (flags | BIT32_MASK_31) ^ BIT32_MASK_31; }
        void turn_off_flag(const int& i)
        { char mask = (char)(1 << i); flags = (flags | mask) ^ mask; }
}; // class Flags32


} // namespace blib


#endif // FLAGS_CLASS_DEFINITIONS

// flags.h
