  #include <cstdint>

  enum class KeyState : uint8_t
  {
      Down,        // 押されている
      Pressed,     // 今フレームに押された
      Released     // 今フレームに離された
  };

namespace Key
  {
      constexpr uint8_t ESC = 0x01;

      constexpr uint8_t NUM_1 = 0x02;
      constexpr uint8_t NUM_2 = 0x03;
      constexpr uint8_t NUM_3 = 0x04;
      constexpr uint8_t NUM_4 = 0x05;
      constexpr uint8_t NUM_5 = 0x06;
      constexpr uint8_t NUM_6 = 0x07;
      constexpr uint8_t NUM_7 = 0x08;
      constexpr uint8_t NUM_8 = 0x09;
      constexpr uint8_t NUM_9 = 0x0A;
      constexpr uint8_t NUM_0 = 0x0B;

      constexpr uint8_t MINUS = 0x0C;
      constexpr uint8_t EQUALS = 0x0D;
      constexpr uint8_t BACKSPACE = 0x0E;
      constexpr uint8_t TAB = 0x0F;

      constexpr uint8_t Q = 0x10;
      constexpr uint8_t W = 0x11;
      constexpr uint8_t E = 0x12;
      constexpr uint8_t R = 0x13;
      constexpr uint8_t T = 0x14;
      constexpr uint8_t Y = 0x15;
      constexpr uint8_t U = 0x16;
      constexpr uint8_t I = 0x17;
      constexpr uint8_t O = 0x18;
      constexpr uint8_t P = 0x19;

      constexpr uint8_t LBRACKET = 0x1A;
      constexpr uint8_t RBRACKET = 0x1B;
      constexpr uint8_t ENTER = 0x1C;
      constexpr uint8_t LCTRL = 0x1D;

      constexpr uint8_t A = 0x1E;
      constexpr uint8_t S = 0x1F;
      constexpr uint8_t D = 0x20;
      constexpr uint8_t F = 0x21;
      constexpr uint8_t G = 0x22;
      constexpr uint8_t H = 0x23;
      constexpr uint8_t J = 0x24;
      constexpr uint8_t K = 0x25;
      constexpr uint8_t L = 0x26;

      constexpr uint8_t SEMICOLON = 0x27;
      constexpr uint8_t APOSTROPHE = 0x28;
      constexpr uint8_t GRAVE = 0x29;
      constexpr uint8_t LSHIFT = 0x2A;
      constexpr uint8_t BACKSLASH = 0x2B;

      constexpr uint8_t Z = 0x2C;
      constexpr uint8_t X = 0x2D;
      constexpr uint8_t C = 0x2E;
      constexpr uint8_t V = 0x2F;
      constexpr uint8_t B = 0x30;
      constexpr uint8_t N = 0x31;
      constexpr uint8_t M = 0x32;

      constexpr uint8_t COMMA = 0x33;
      constexpr uint8_t PERIOD = 0x34;
      constexpr uint8_t SLASH = 0x35;
      constexpr uint8_t RSHIFT = 0x36;
      constexpr uint8_t NUMPAD_MULTIPLY = 0x37;
      constexpr uint8_t LALT = 0x38;
      constexpr uint8_t SPACE = 0x39;
      constexpr uint8_t CAPS_LOCK = 0x3A;

      constexpr uint8_t F1 = 0x3B;
      constexpr uint8_t F2 = 0x3C;
      constexpr uint8_t F3 = 0x3D;
      constexpr uint8_t F4 = 0x3E;
      constexpr uint8_t F5 = 0x3F;
      constexpr uint8_t F6 = 0x40;
      constexpr uint8_t F7 = 0x41;
      constexpr uint8_t F8 = 0x42;
      constexpr uint8_t F9 = 0x43;
      constexpr uint8_t F10 = 0x44;

      constexpr uint8_t NUM_LOCK = 0x45;
      constexpr uint8_t SCROLL_LOCK = 0x46;

      constexpr uint8_t NUMPAD_7 = 0x47;
      constexpr uint8_t NUMPAD_8 = 0x48;
      constexpr uint8_t NUMPAD_9 = 0x49;
      constexpr uint8_t NUMPAD_MINUS = 0x4A;
      constexpr uint8_t NUMPAD_4 = 0x4B;
      constexpr uint8_t NUMPAD_5 = 0x4C;
      constexpr uint8_t NUMPAD_6 = 0x4D;
      constexpr uint8_t NUMPAD_PLUS = 0x4E;
      constexpr uint8_t NUMPAD_1 = 0x4F;
      constexpr uint8_t NUMPAD_2 = 0x50;
      constexpr uint8_t NUMPAD_3 = 0x51;
      constexpr uint8_t NUMPAD_0 = 0x52;
      constexpr uint8_t NUMPAD_PERIOD = 0x53;

      constexpr uint8_t F11 = 0x57;
      constexpr uint8_t F12 = 0x58;

      constexpr uint8_t NUMPAD_ENTER = 0x9C;
      constexpr uint8_t RCTRL = 0x9D;
      constexpr uint8_t NUMPAD_DIVIDE = 0xB5;
      constexpr uint8_t PRINT_SCREEN = 0xB7;
      constexpr uint8_t RALT = 0xB8;

      constexpr uint8_t PAUSE = 0xC5;
      constexpr uint8_t HOME = 0xC7;
      constexpr uint8_t UP = 0xC8;
      constexpr uint8_t PAGE_UP = 0xC9;
      constexpr uint8_t LEFT = 0xCB;
      constexpr uint8_t RIGHT = 0xCD;
      constexpr uint8_t END = 0xCF;
      constexpr uint8_t DOWN = 0xD0;
      constexpr uint8_t PAGE_DOWN = 0xD1;
      constexpr uint8_t INSERT = 0xD2;
      constexpr uint8_t DELETE = 0xD3;

      constexpr uint8_t LWIN = 0xDB;
      constexpr uint8_t RWIN = 0xDC;
      constexpr uint8_t APPS = 0xDD;
  }