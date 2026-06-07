#include <cstdint>

enum class KeyState : uint8_t
{
    kDown, // 押されている
    kPressed, // 今フレームに押された
    kReleased // 今フレームに離された
};

namespace InputKey
{
    constexpr uint8_t kEsc = 0x01;
    constexpr uint8_t kNum1 = 0x02;
    constexpr uint8_t kNum2 = 0x03;
    constexpr uint8_t kNum3 = 0x04;
    constexpr uint8_t kNum4 = 0x05;
    constexpr uint8_t kNum5 = 0x06;
    constexpr uint8_t kNum6 = 0x07;
    constexpr uint8_t kNum7 = 0x08;
    constexpr uint8_t kNum8 = 0x09;
    constexpr uint8_t kNum9 = 0x0A;
    constexpr uint8_t kNum0 = 0x0B;
    constexpr uint8_t kMinus = 0x0C;
    constexpr uint8_t kEquals = 0x0D;
    constexpr uint8_t kBackspace = 0x0E;
    constexpr uint8_t kTab = 0x0F;
    constexpr uint8_t kQ = 0x10;
    constexpr uint8_t kW = 0x11;
    constexpr uint8_t kE = 0x12;
    constexpr uint8_t kR = 0x13;
    constexpr uint8_t kT = 0x14;
    constexpr uint8_t kY = 0x15;
    constexpr uint8_t kU = 0x16;
    constexpr uint8_t kI = 0x17;
    constexpr uint8_t kO = 0x18;
    constexpr uint8_t kP = 0x19;

    constexpr uint8_t kLeftBracket = 0x1A;
    constexpr uint8_t kRightBracket = 0x1B;
    constexpr uint8_t kEnter = 0x1C;
    constexpr uint8_t kLeftCtrl = 0x1D;

    constexpr uint8_t kA = 0x1E;
    constexpr uint8_t kS = 0x1F;
    constexpr uint8_t kD = 0x20;
    constexpr uint8_t kF = 0x21;
    constexpr uint8_t kG = 0x22;
    constexpr uint8_t kH = 0x23;
    constexpr uint8_t kJ = 0x24;
    constexpr uint8_t kK = 0x25;
    constexpr uint8_t kL = 0x26;

    constexpr uint8_t kSemicolon = 0x27;
    constexpr uint8_t kApostrophe = 0x28;
    constexpr uint8_t kGrave = 0x29;
    constexpr uint8_t kLeftShift = 0x2A;
    constexpr uint8_t kBackslash = 0x2B;
    constexpr uint8_t kZ = 0x2C;
    constexpr uint8_t kX = 0x2D;
    constexpr uint8_t kC = 0x2E;
    constexpr uint8_t kV = 0x2F;
    constexpr uint8_t kB = 0x30;
    constexpr uint8_t kN = 0x31;
    constexpr uint8_t kM = 0x32;
    constexpr uint8_t kComma = 0x33;
    constexpr uint8_t kPeriod = 0x34;
    constexpr uint8_t kSlash = 0x35;
    constexpr uint8_t kRightShift = 0x36;
    constexpr uint8_t kNumpadMultiply = 0x37;
    constexpr uint8_t kLeftAlt = 0x38;
    constexpr uint8_t kSpace = 0x39;
    constexpr uint8_t kCapsLock = 0x3A;

    constexpr uint8_t kF1 = 0x3B;
    constexpr uint8_t kF2 = 0x3C;
    constexpr uint8_t kF3 = 0x3D;
    constexpr uint8_t kF4 = 0x3E;
    constexpr uint8_t kF5 = 0x3F;
    constexpr uint8_t kF6 = 0x40;
    constexpr uint8_t kF7 = 0x41;
    constexpr uint8_t kF8 = 0x42;
    constexpr uint8_t kF9 = 0x43;
    constexpr uint8_t kF10 = 0x44;
    constexpr uint8_t kF11 = 0x57;
    constexpr uint8_t kF12 = 0x58;
    constexpr uint8_t kNumLock = 0x45;
    constexpr uint8_t kScrollLock = 0x46;
    constexpr uint8_t kNumpad7 = 0x47;
    constexpr uint8_t kNumpad8 = 0x48;
    constexpr uint8_t kNumpad9 = 0x49;
    constexpr uint8_t kNumpadMinus = 0x4A;
    constexpr uint8_t kNumpad4 = 0x4B;
    constexpr uint8_t kNumpad5 = 0x4C;
    constexpr uint8_t kNumpad6 = 0x4D;
    constexpr uint8_t kNumpadPlus = 0x4E;
    constexpr uint8_t kNumpad1 = 0x4F;
    constexpr uint8_t kNumpad2 = 0x50;
    constexpr uint8_t kNumpad3 = 0x51;
    constexpr uint8_t kNumpad0 = 0x52;
    constexpr uint8_t kNumpadPeriod = 0x53;
    constexpr uint8_t kNumpadEnter = 0x9C;
    constexpr uint8_t kRightCtrl = 0x9D;
    constexpr uint8_t kNumpadDivide = 0xB5;
    constexpr uint8_t kPrintScreen = 0xB7;
    constexpr uint8_t kRightAlt = 0xB8;
    constexpr uint8_t kPause = 0xC5;
    constexpr uint8_t kHome = 0xC7;
    constexpr uint8_t kArrowUp = 0xC8;
    constexpr uint8_t kPageUp = 0xC9;
    constexpr uint8_t kArrowLeft = 0xCB;
    constexpr uint8_t kArrowRight = 0xCD;
    constexpr uint8_t kEnd = 0xCF;
    constexpr uint8_t kArrowDown = 0xD0;
    constexpr uint8_t kPageDown = 0xD1;
    constexpr uint8_t kInsert = 0xD2;
    constexpr uint8_t kDelete = 0xD3;
    constexpr uint8_t kLeftWin = 0xDB;
    constexpr uint8_t kRightWin = 0xDC;
    constexpr uint8_t kApps = 0xDD;
}
