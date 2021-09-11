enum MorseParts
{
  Dot = 0,
  Dash = 1,
  ERR = -1
};

enum PrintState
{
  IDENTIFY,
  PRINT_CHAR,
  WORD_WAIT,
  ON_WAIT,
  OFF_WAIT,
  COMPLETE
};

enum PrintErrors
{
  NO_ERROR = 0,
  INVAL_MORSE_BIT = 1,
  ESCAPE_RECEIVED = 2,
  INVAL_PRINT_STATE = 3
};

static const int UNIT_LENGTH_MS = 250;//ms

static const int DOT_DELAY_MS = 1 * UNIT_LENGTH_MS;
static const int DASH_DELAY_MS = 3 * UNIT_LENGTH_MS;
static const int PART_DELAY_MS = 1 * UNIT_LENGTH_MS;
static const int LETTER_DELAY_MS = 3 * UNIT_LENGTH_MS;
static const int WORD_DELAY_MS = 7 * UNIT_LENGTH_MS;

struct MorseLetter
{
  char num_parts;
  char parts;
};

const MorseLetter MORSE_LUT[] =
{
  { 0, 0b0 },// 0 - NUL
  { 0, 0b0 },// 1 - SOH
  { 0, 0b0 },// 2 - STX
  { 0, 0b0 },// 3 - ETX
  { 0, 0b0 },// 4 - EOT
  { 0, 0b0 },// 5 - ENQ
  { 0, 0b0 },// 6 - ACK
  { 0, 0b0 },// 7 - BEL
  { 0, 0b0 },// 8 - BS
  { 0, 0b0 },// 9 - HT
  { 0, 0b0 },// 10 - LF
  { 0, 0b0 },// 11 - VT
  { 0, 0b0 },// 12 - FF
  { 0, 0b0 },// 13 - CR
  { 0, 0b0 },// 14 - SO
  { 0, 0b0 },// 15 - SI
  { 0, 0b0 },// 16 - DLE
  { 0, 0b0 },// 17 - DC1
  { 0, 0b0 },// 18 - DC2
  { 0, 0b0 },// 19 - DC3
  { 0, 0b0 },// 20 - DC4
  { 0, 0b0 },// 21 - NAK
  { 0, 0b0 },// 22 - SYN
  { 0, 0b0 },// 23 - ETB
  { 0, 0b0 },// 24 - CAN
  { 0, 0b0 },// 25 - EM
  { 0, 0b0 },// 26 - SUB
  { 0, 0b0 },// 27 - ESC
  { 0, 0b0 },// 28 - FS
  { 0, 0b0 },// 29 - GS
  { 0, 0b0 },// 30 - RS
  { 0, 0b0 },// 31 - US
  { 0, 0b0 },// 32 - space
  { 0, 0b0 },// 33 - !
  { 0, 0b0 },// 34 - "
  { 0, 0b0 },// 35 - #
  { 0, 0b0 },// 36 - $
  { 0, 0b0 },// 37 - %
  { 0, 0b0 },// 38 - &
  { 0, 0b0 },// 39 - '
  { 0, 0b0 },// 40 - (
  { 0, 0b0 },// 41 - )
  { 0, 0b0 },// 42 - *
  { 0, 0b0 },// 43 - +
  { 0, 0b0 },// 44 - ,
  { 0, 0b0 },// 45 - -
  { 0, 0b0 },// 46 - .
  { 0, 0b0 },// 47 - /
  { 5, 0b11111 },// 48 - 0
  { 5, 0b11110 },// 49 - 1
  { 5, 0b11100 },// 50 - 2
  { 5, 0b11000 },// 51 - 3
  { 5, 0b10000 },// 52 - 4
  { 5, 0b00000 },// 53 - 5
  { 5, 0b00001 },// 54 - 6
  { 5, 0b00011 },// 55 - 7
  { 5, 0b00111 },// 56 - 8
  { 5, 0b01111 },// 57 - 9
  { 0, 0b0 },// 58 - :
  { 0, 0b0 },// 59 - ;
  { 0, 0b0 },// 60 - <
  { 0, 0b0 },// 61 - =
  { 0, 0b0 },// 62 - >
  { 0, 0b0 },// 63 - ?
  { 0, 0b0 },// 64 - @
  { 2, 0b10 },// 65 - A
  { 4, 0b0001 },// 66 - B
  { 4, 0b0101 },// 67 - C
  { 3, 0b001 },// 68 - D
  { 1, 0b0 },// 69 - E
  { 4, 0b0100 },// 70 - F
  { 3, 0b011 },// 71 - G
  { 4, 0b0000 },// 72 - H
  { 2, 0b00 },// 73 - I
  { 4, 0b1110 },// 74 - J
  { 3, 0b101 },// 75 - K
  { 4, 0b0010 },// 76 - L
  { 2, 0b11 },// 77 - M
  { 2, 0b01 },// 78 - N
  { 3, 0b111 },// 79 - O
  { 4, 0b0110 },// 80 - P
  { 4, 0b1011 },// 81 - Q
  { 3, 0b010 },// 82 - R
  { 3, 0b000 },// 83 - S
  { 1, 0b1 },// 84 - T
  { 3, 0b100 },// 85 - U
  { 4, 0b1000 },// 86 - V
  { 3, 0b110 },// 87 - W
  { 4, 0b1001 },// 88 - X
  { 4, 0b1101 },// 89 - Y
  { 4, 0b0011 },// 90 - Z
  { 0, 0b0 },// 91 - [
  { 0, 0b0 },// 92 - backslash
  { 0, 0b0 },// 93 - ]
  { 0, 0b0 },// 94 - ^
  { 0, 0b0 },// 95 - _
  { 0, 0b0 },// 96 - `
  { 2, 0b10 },// 97 - a
  { 4, 0b0001 },// 98 - b
  { 4, 0b0101 },// 99 - c
  { 3, 0b001 },// 100 - d
  { 1, 0b0 },// 101 - e
  { 4, 0b0100 },// 102 - f
  { 3, 0b011 },// 103 - g
  { 4, 0b0000 },// 104 - h
  { 2, 0b00 },// 105 - i
  { 4, 0b1110 },// 106 - j
  { 3, 0b101 },// 107 - k
  { 4, 0b0010 },// 108 - l
  { 2, 0b11 },// 109 - m
  { 2, 0b01 },// 110 - n
  { 3, 0b111 },// 111 - o
  { 4, 0b0110 },// 112 - p
  { 4, 0b1011 },// 113 - q
  { 3, 0b010 },// 114 - r
  { 3, 0b000 },// 115 - s
  { 1, 0b1 },// 116 - t
  { 3, 0b100 },// 117 - u
  { 4, 0b1000 },// 118 - v
  { 3, 0b110 },// 119 - w
  { 4, 0b1001 },// 120 - x
  { 4, 0b1101 },// 121 - y
  { 4, 0b0011 },// 122 - z
  { 0, 0b0 },// 123 - {
  { 0, 0b0 },// 124 - |
  { 0, 0b0 },// 125 - }
  { 0, 0b0 },// 126 - ~
  { 0, 0b0 },// 127 - DEL
  { 0, 0b0 },// 128 - 
  { 0, 0b0 },// 129 - 
  { 0, 0b0 },// 130 - 
  { 0, 0b0 },// 131 - 
  { 0, 0b0 },// 132 - 
  { 0, 0b0 },// 133 - 
  { 0, 0b0 },// 134 - 
  { 0, 0b0 },// 135 - 
  { 0, 0b0 },// 136 - 
  { 0, 0b0 },// 137 - 
  { 0, 0b0 },// 138 - 
  { 0, 0b0 },// 139 - 
  { 0, 0b0 },// 140 - 
  { 0, 0b0 },// 141 - 
  { 0, 0b0 },// 142 - 
  { 0, 0b0 },// 143 - 
  { 0, 0b0 },// 144 - 
  { 0, 0b0 },// 145 - 
  { 0, 0b0 },// 146 - 
  { 0, 0b0 },// 147 - 
  { 0, 0b0 },// 148 - 
  { 0, 0b0 },// 149 - 
  { 0, 0b0 },// 150 - 
  { 0, 0b0 },// 151 - 
  { 0, 0b0 },// 152 - 
  { 0, 0b0 },// 153 - 
  { 0, 0b0 },// 154 - 
  { 0, 0b0 },// 155 - 
  { 0, 0b0 },// 156 - 
  { 0, 0b0 },// 157 - 
  { 0, 0b0 },// 158 - 
  { 0, 0b0 },// 159 - 
  { 0, 0b0 },// 160 - 
  { 0, 0b0 },// 161 - 
  { 0, 0b0 },// 162 - 
  { 0, 0b0 },// 163 - 
  { 0, 0b0 },// 164 - 
  { 0, 0b0 },// 165 - 
  { 0, 0b0 },// 166 - 
  { 0, 0b0 },// 167 - 
  { 0, 0b0 },// 168 - 
  { 0, 0b0 },// 169 - 
  { 0, 0b0 },// 170 - 
  { 0, 0b0 },// 171 - 
  { 0, 0b0 },// 172 - 
  { 0, 0b0 },// 173 - 
  { 0, 0b0 },// 174 - 
  { 0, 0b0 },// 175 - 
  { 0, 0b0 },// 176 - 
  { 0, 0b0 },// 177 - 
  { 0, 0b0 },// 178 - 
  { 0, 0b0 },// 179 - 
  { 0, 0b0 },// 180 - 
  { 0, 0b0 },// 181 - 
  { 0, 0b0 },// 182 - 
  { 0, 0b0 },// 183 - 
  { 0, 0b0 },// 184 - 
  { 0, 0b0 },// 185 - 
  { 0, 0b0 },// 186 - 
  { 0, 0b0 },// 187 - 
  { 0, 0b0 },// 188 - 
  { 0, 0b0 },// 189 - 
  { 0, 0b0 },// 190 - 
  { 0, 0b0 },// 191 - 
  { 0, 0b0 },// 192 - 
  { 0, 0b0 },// 193 - 
  { 0, 0b0 },// 194 - 
  { 0, 0b0 },// 195 - 
  { 0, 0b0 },// 196 - 
  { 0, 0b0 },// 197 - 
  { 0, 0b0 },// 198 - 
  { 0, 0b0 },// 199 - 
  { 0, 0b0 },// 200 - 
  { 0, 0b0 },// 201 - 
  { 0, 0b0 },// 202 - 
  { 0, 0b0 },// 203 - 
  { 0, 0b0 },// 204 - 
  { 0, 0b0 },// 205 - 
  { 0, 0b0 },// 206 - 
  { 0, 0b0 },// 207 - 
  { 0, 0b0 },// 208 - 
  { 0, 0b0 },// 209 - 
  { 0, 0b0 },// 210 - 
  { 0, 0b0 },// 211 - 
  { 0, 0b0 },// 212 - 
  { 0, 0b0 },// 213 - 
  { 0, 0b0 },// 214 - 
  { 0, 0b0 },// 215 - 
  { 0, 0b0 },// 216 - 
  { 0, 0b0 },// 217 - 
  { 0, 0b0 },// 218 - 
  { 0, 0b0 },// 219 - 
  { 0, 0b0 },// 220 - 
  { 0, 0b0 },// 221 - 
  { 0, 0b0 },// 222 - 
  { 0, 0b0 },// 223 - 
  { 0, 0b0 },// 224 - 
  { 0, 0b0 },// 225 - 
  { 0, 0b0 },// 226 - 
  { 0, 0b0 },// 227 - 
  { 0, 0b0 },// 228 - 
  { 0, 0b0 },// 229 - 
  { 0, 0b0 },// 230 - 
  { 0, 0b0 },// 231 - 
  { 0, 0b0 },// 232 - 
  { 0, 0b0 },// 233 - 
  { 0, 0b0 },// 234 - 
  { 0, 0b0 },// 235 - 
  { 0, 0b0 },// 236 - 
  { 0, 0b0 },// 237 - 
  { 0, 0b0 },// 238 - 
  { 0, 0b0 },// 239 - 
  { 0, 0b0 },// 240 - 
  { 0, 0b0 },// 241 - 
  { 0, 0b0 },// 242 - 
  { 0, 0b0 },// 243 - 
  { 0, 0b0 },// 244 - 
  { 0, 0b0 },// 245 - 
  { 0, 0b0 },// 246 - 
  { 0, 0b0 },// 247 - 
  { 0, 0b0 },// 248 - 
  { 0, 0b0 },// 249 - 
  { 0, 0b0 },// 250 - 
  { 0, 0b0 },// 251 - 
  { 0, 0b0 },// 252 - 
  { 0, 0b0 },// 253 - 
  { 0, 0b0 },// 254 - 
  { 0, 0b0 }// 255 - 
};



MorseParts getNextMorseBit(const MorseLetter letter, int currentPart)
{
  if (currentPart >= letter.num_parts)
  {
    return MorseParts::ERR;
  }

  return (MorseParts)((letter.parts >> currentPart) & 1);
}

PrintErrors printStringMorse(const String& toMorse)
{
  PrintState currentState = PrintState::IDENTIFY;
  PrintErrors error = PrintErrors::NO_ERROR;

  int stringLen = toMorse.length();
  int currentPartIndex = 0;
  int currentCharIndex = 0;
  char currentChar = 0;
  MorseLetter currentLetter = { 0, 0 };
  int currentWait = 0;
  bool needsSpaceWait = false;
  bool letterComplete = false;

  while (currentState != PrintState::COMPLETE)
  {
    switch (currentState)
    {
    case PrintState::IDENTIFY:
      // Check if string is done
      if (currentCharIndex >= stringLen)
      {
        currentState = PrintState::COMPLETE;
        break;
      }

      // Get current letter
      currentChar = toMorse[currentCharIndex++];

      // Space Wait will be handled next iteration before
      // the next character
      if (currentChar == ' ')
      {
        currentState = WORD_WAIT;
        break;
      }

      // Convert Character to Morse Code
      currentLetter = MORSE_LUT[currentChar];

      // Check if non printable character
      if (currentLetter.num_parts == 0)
      {
        currentState = PrintState::COMPLETE;
        error = PrintErrors::ESCAPE_RECEIVED;
      }
      else
      {
        // go directly to print characters
        currentState = PrintState::PRINT_CHAR;
      }
      break;

    case PrintState::PRINT_CHAR:

      // check if done with character
      if (currentPartIndex >= currentLetter.num_parts)
      {
        currentState = PrintState::IDENTIFY;
        currentPartIndex = 0;
        break;
      }

      // Handle next letter part to print
      switch (getNextMorseBit(currentLetter, currentPartIndex++))
      {
      case MorseParts::Dash:
        currentState = PrintState::ON_WAIT;
        currentWait = DASH_DELAY_MS;
        
        break;

      case MorseParts::Dot:
        currentWait = DOT_DELAY_MS;
        currentState = PrintState::ON_WAIT;
        break;

      case MorseParts::ERR:
      default:
        error = PrintErrors::INVAL_MORSE_BIT;
        currentState = PrintState::COMPLETE;
        break;
      }
      break;

    case PrintState::WORD_WAIT:
      currentWait = WORD_DELAY_MS;
      currentState = PrintState::OFF_WAIT;
      break;

    case PrintState::ON_WAIT:
      digitalWrite(LED_BUILTIN, HIGH);
      delay(currentWait);
      currentState = PrintState::OFF_WAIT;
      break;

    case PrintState::OFF_WAIT:
      digitalWrite(LED_BUILTIN, LOW);
      
      if (needsSpaceWait)
      {
        needsSpaceWait = false;
      }
      else if (currentPartIndex >= currentLetter.num_parts)
      {
        currentWait = LETTER_DELAY_MS;
      }
      else
      {
        currentWait = PART_DELAY_MS;
      }

      delay(currentWait);
      currentState = PrintState::PRINT_CHAR;
      break;

    case PrintState::COMPLETE:
      break;

    default:
      error = PrintErrors::INVAL_PRINT_STATE;
      currentState = PrintState::COMPLETE;
      break;
    }
  }

  return error;
}

void writeString(const String& str)
{
  for(int i = 0; i < str.length(); ++i)
  {
    Serial.write(str[i]);
  }
}

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  writeString("Send String to Convert to Morse Code\n");
  // Loop until data available
  while(Serial.available() == 0) { delay(1); }
  // Read the string to convert
  String toMorse = Serial.readString();
  // convert and blink the code
  printStringMorse(toMorse);
}
