#include <cstdint>

/**
 * @brief KeyStateのデータと処理をまとめる型。
 */
enum class KeyState : uint8_t
{
    kDown,     // 押されている。//
    kPressed,  // 今フレームに押された。///
    kReleased  // 今フレームに離された。///
};

// 入力で使うキー番号。内部で Windows の仮想キーコードへ変換される。///
namespace InputKey
{
    inline constexpr uint8_t kEsc = 0x01;
    inline constexpr uint8_t kNum1 = 0x02;
    inline constexpr uint8_t kNum2 = 0x03;
    inline constexpr uint8_t kNum3 = 0x04;
    inline constexpr uint8_t kNum4 = 0x05;
    inline constexpr uint8_t kNum5 = 0x06;
    inline constexpr uint8_t kNum6 = 0x07;
    inline constexpr uint8_t kNum7 = 0x08;
    inline constexpr uint8_t kNum8 = 0x09;
    inline constexpr uint8_t kNum9 = 0x0A;
    inline constexpr uint8_t kNum0 = 0x0B;
    inline constexpr uint8_t kMinus = 0x0C;
    inline constexpr uint8_t kEquals = 0x0D;
    inline constexpr uint8_t kBackspace = 0x0E;
    inline constexpr uint8_t kTab = 0x0F;
    inline constexpr uint8_t kQ = 0x10;
    inline constexpr uint8_t kW = 0x11;
    inline constexpr uint8_t kE = 0x12;
    inline constexpr uint8_t kR = 0x13;
    inline constexpr uint8_t kT = 0x14;
    inline constexpr uint8_t kY = 0x15;
    inline constexpr uint8_t kU = 0x16;
    inline constexpr uint8_t kI = 0x17;
    inline constexpr uint8_t kO = 0x18;
    inline constexpr uint8_t kP = 0x19;

    inline constexpr uint8_t kLeftBracket = 0x1A;
    inline constexpr uint8_t kRightBracket = 0x1B;
    inline constexpr uint8_t kEnter = 0x1C;
    inline constexpr uint8_t kLeftCtrl = 0x1D;

    inline constexpr uint8_t kA = 0x1E;
    inline constexpr uint8_t kS = 0x1F;
    inline constexpr uint8_t kD = 0x20;
    inline constexpr uint8_t kF = 0x21;
    inline constexpr uint8_t kG = 0x22;
    inline constexpr uint8_t kH = 0x23;
    inline constexpr uint8_t kJ = 0x24;
    inline constexpr uint8_t kK = 0x25;
    inline constexpr uint8_t kL = 0x26;

    inline constexpr uint8_t kSemicolon = 0x27;
    inline constexpr uint8_t kApostrophe = 0x28;
    inline constexpr uint8_t kGrave = 0x29;
    inline constexpr uint8_t kLeftShift = 0x2A;
    inline constexpr uint8_t kBackslash = 0x2B;
    inline constexpr uint8_t kZ = 0x2C;
    inline constexpr uint8_t kX = 0x2D;
    inline constexpr uint8_t kC = 0x2E;
    inline constexpr uint8_t kV = 0x2F;
    inline constexpr uint8_t kB = 0x30;
    inline constexpr uint8_t kN = 0x31;
    inline constexpr uint8_t kM = 0x32;
    inline constexpr uint8_t kComma = 0x33;
    inline constexpr uint8_t kPeriod = 0x34;
    inline constexpr uint8_t kSlash = 0x35;
    inline constexpr uint8_t kRightShift = 0x36;
    inline constexpr uint8_t kNumpadMultiply = 0x37;
    inline constexpr uint8_t kLeftAlt = 0x38;
    inline constexpr uint8_t kSpace = 0x39;
    inline constexpr uint8_t kCapsLock = 0x3A;

    inline constexpr uint8_t kF1 = 0x3B;
    inline constexpr uint8_t kF2 = 0x3C;
    inline constexpr uint8_t kF3 = 0x3D;
    inline constexpr uint8_t kF4 = 0x3E;
    inline constexpr uint8_t kF5 = 0x3F;
    inline constexpr uint8_t kF6 = 0x40;
    inline constexpr uint8_t kF7 = 0x41;
    inline constexpr uint8_t kF8 = 0x42;
    inline constexpr uint8_t kF9 = 0x43;
    inline constexpr uint8_t kF10 = 0x44;
    inline constexpr uint8_t kF11 = 0x57;
    inline constexpr uint8_t kF12 = 0x58;
    inline constexpr uint8_t kNumLock = 0x45;
    inline constexpr uint8_t kScrollLock = 0x46;
    inline constexpr uint8_t kNumpad7 = 0x47;
    inline constexpr uint8_t kNumpad8 = 0x48;
    inline constexpr uint8_t kNumpad9 = 0x49;
    inline constexpr uint8_t kNumpadMinus = 0x4A;
    inline constexpr uint8_t kNumpad4 = 0x4B;
    inline constexpr uint8_t kNumpad5 = 0x4C;
    inline constexpr uint8_t kNumpad6 = 0x4D;
    inline constexpr uint8_t kNumpadPlus = 0x4E;
    inline constexpr uint8_t kNumpad1 = 0x4F;
    inline constexpr uint8_t kNumpad2 = 0x50;
    inline constexpr uint8_t kNumpad3 = 0x51;
    inline constexpr uint8_t kNumpad0 = 0x52;
    inline constexpr uint8_t kNumpadPeriod = 0x53;
    inline constexpr uint8_t kNumpadEnter = 0x9C;
    inline constexpr uint8_t kRightCtrl = 0x9D;
    inline constexpr uint8_t kNumpadDivide = 0xB5;
    inline constexpr uint8_t kPrintScreen = 0xB7;
    inline constexpr uint8_t kRightAlt = 0xB8;
    inline constexpr uint8_t kPause = 0xC5;
    inline constexpr uint8_t kHome = 0xC7;
    inline constexpr uint8_t kArrowUp = 0xC8;
    inline constexpr uint8_t kPageUp = 0xC9;
    inline constexpr uint8_t kArrowLeft = 0xCB;
    inline constexpr uint8_t kArrowRight = 0xCD;
    inline constexpr uint8_t kEnd = 0xCF;
    inline constexpr uint8_t kArrowDown = 0xD0;
    inline constexpr uint8_t kPageDown = 0xD1;
    inline constexpr uint8_t kInsert = 0xD2;
    inline constexpr uint8_t kDelete = 0xD3;
    inline constexpr uint8_t kLeftWin = 0xDB;
    inline constexpr uint8_t kRightWin = 0xDC;
    inline constexpr uint8_t kApps = 0xDD;
}
