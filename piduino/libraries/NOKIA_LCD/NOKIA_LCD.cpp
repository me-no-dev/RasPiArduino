#include "NOKIA_LCD.h"
#include <cstdarg>

#define STRING_STACK_LIMIT    120
// The 7-bit ASCII character set...
const uint8_t charset[][5] = {
  { 0x00, 0x00, 0x00, 0x00, 0x00 },  // 20 space
  { 0x00, 0x00, 0x5f, 0x00, 0x00 },  // 21 !
  { 0x00, 0x07, 0x00, 0x07, 0x00 },  // 22 "
  { 0x14, 0x7f, 0x14, 0x7f, 0x14 },  // 23 #
  { 0x24, 0x2a, 0x7f, 0x2a, 0x12 },  // 24 $
  { 0x23, 0x13, 0x08, 0x64, 0x62 },  // 25 %
  { 0x36, 0x49, 0x55, 0x22, 0x50 },  // 26 &
  { 0x00, 0x05, 0x03, 0x00, 0x00 },  // 27 '
  { 0x00, 0x1c, 0x22, 0x41, 0x00 },  // 28 (
  { 0x00, 0x41, 0x22, 0x1c, 0x00 },  // 29 )
  { 0x14, 0x08, 0x3e, 0x08, 0x14 },  // 2a *
  { 0x08, 0x08, 0x3e, 0x08, 0x08 },  // 2b +
  { 0x00, 0x50, 0x30, 0x00, 0x00 },  // 2c ,
  { 0x08, 0x08, 0x08, 0x08, 0x08 },  // 2d -
  { 0x00, 0x60, 0x60, 0x00, 0x00 },  // 2e .
  { 0x20, 0x10, 0x08, 0x04, 0x02 },  // 2f /
  { 0x3e, 0x51, 0x49, 0x45, 0x3e },  // 30 0
  { 0x00, 0x42, 0x7f, 0x40, 0x00 },  // 31 1
  { 0x42, 0x61, 0x51, 0x49, 0x46 },  // 32 2
  { 0x21, 0x41, 0x45, 0x4b, 0x31 },  // 33 3
  { 0x18, 0x14, 0x12, 0x7f, 0x10 },  // 34 4
  { 0x27, 0x45, 0x45, 0x45, 0x39 },  // 35 5
  { 0x3c, 0x4a, 0x49, 0x49, 0x30 },  // 36 6
  { 0x01, 0x71, 0x09, 0x05, 0x03 },  // 37 7
  { 0x36, 0x49, 0x49, 0x49, 0x36 },  // 38 8
  { 0x06, 0x49, 0x49, 0x29, 0x1e },  // 39 9
  { 0x00, 0x36, 0x36, 0x00, 0x00 },  // 3a :
  { 0x00, 0x56, 0x36, 0x00, 0x00 },  // 3b ;
  { 0x08, 0x14, 0x22, 0x41, 0x00 },  // 3c <
  { 0x14, 0x14, 0x14, 0x14, 0x14 },  // 3d =
  { 0x00, 0x41, 0x22, 0x14, 0x08 },  // 3e >
  { 0x02, 0x01, 0x51, 0x09, 0x06 },  // 3f ?
  { 0x32, 0x49, 0x79, 0x41, 0x3e },  // 40 @
  { 0x7e, 0x11, 0x11, 0x11, 0x7e },  // 41 A
  { 0x7f, 0x49, 0x49, 0x49, 0x36 },  // 42 B
  { 0x3e, 0x41, 0x41, 0x41, 0x22 },  // 43 C
  { 0x7f, 0x41, 0x41, 0x22, 0x1c },  // 44 D
  { 0x7f, 0x49, 0x49, 0x49, 0x41 },  // 45 E
  { 0x7f, 0x09, 0x09, 0x09, 0x01 },  // 46 F
  { 0x3e, 0x41, 0x49, 0x49, 0x7a },  // 47 G
  { 0x7f, 0x08, 0x08, 0x08, 0x7f },  // 48 H
  { 0x00, 0x41, 0x7f, 0x41, 0x00 },  // 49 I
  { 0x20, 0x40, 0x41, 0x3f, 0x01 },  // 4a J
  { 0x7f, 0x08, 0x14, 0x22, 0x41 },  // 4b K
  { 0x7f, 0x40, 0x40, 0x40, 0x40 },  // 4c L
  { 0x7f, 0x02, 0x0c, 0x02, 0x7f },  // 4d M
  { 0x7f, 0x04, 0x08, 0x10, 0x7f },  // 4e N
  { 0x3e, 0x41, 0x41, 0x41, 0x3e },  // 4f O
  { 0x7f, 0x09, 0x09, 0x09, 0x06 },  // 50 P
  { 0x3e, 0x41, 0x51, 0x21, 0x5e },  // 51 Q
  { 0x7f, 0x09, 0x19, 0x29, 0x46 },  // 52 R
  { 0x46, 0x49, 0x49, 0x49, 0x31 },  // 53 S
  { 0x01, 0x01, 0x7f, 0x01, 0x01 },  // 54 T
  { 0x3f, 0x40, 0x40, 0x40, 0x3f },  // 55 U
  { 0x1f, 0x20, 0x40, 0x20, 0x1f },  // 56 V
  { 0x3f, 0x40, 0x38, 0x40, 0x3f },  // 57 W
  { 0x63, 0x14, 0x08, 0x14, 0x63 },  // 58 X
  { 0x07, 0x08, 0x70, 0x08, 0x07 },  // 59 Y
  { 0x61, 0x51, 0x49, 0x45, 0x43 },  // 5a Z
  { 0x00, 0x7f, 0x41, 0x41, 0x00 },  // 5b [
  { 0x02, 0x04, 0x08, 0x10, 0x20 },  // 5c backslash
  { 0x00, 0x41, 0x41, 0x7f, 0x00 },  // 5d ]
  { 0x04, 0x02, 0x01, 0x02, 0x04 },  // 5e ^
  { 0x40, 0x40, 0x40, 0x40, 0x40 },  // 5f _
  { 0x00, 0x01, 0x02, 0x04, 0x00 },  // 60 `
  { 0x20, 0x54, 0x54, 0x54, 0x78 },  // 61 a
  { 0x7f, 0x48, 0x44, 0x44, 0x38 },  // 62 b
  { 0x38, 0x44, 0x44, 0x44, 0x20 },  // 63 c
  { 0x38, 0x44, 0x44, 0x48, 0x7f },  // 64 d
  { 0x38, 0x54, 0x54, 0x54, 0x18 },  // 65 e
  { 0x08, 0x7e, 0x09, 0x01, 0x02 },  // 66 f
  { 0x0c, 0x52, 0x52, 0x52, 0x3e },  // 67 g
  { 0x7f, 0x08, 0x04, 0x04, 0x78 },  // 68 h
  { 0x00, 0x44, 0x7d, 0x40, 0x00 },  // 69 i
  { 0x20, 0x40, 0x44, 0x3d, 0x00 },  // 6a j
  { 0x7f, 0x10, 0x28, 0x44, 0x00 },  // 6b k
  { 0x00, 0x41, 0x7f, 0x40, 0x00 },  // 6c l
  { 0x7c, 0x04, 0x18, 0x04, 0x78 },  // 6d m
  { 0x7c, 0x08, 0x04, 0x04, 0x78 },  // 6e n
  { 0x38, 0x44, 0x44, 0x44, 0x38 },  // 6f o
  { 0x7c, 0x14, 0x14, 0x14, 0x08 },  // 70 p
  { 0x08, 0x14, 0x14, 0x18, 0x7c },  // 71 q
  { 0x7c, 0x08, 0x04, 0x04, 0x08 },  // 72 r
  { 0x48, 0x54, 0x54, 0x54, 0x20 },  // 73 s
  { 0x04, 0x3f, 0x44, 0x40, 0x20 },  // 74 t
  { 0x3c, 0x40, 0x40, 0x20, 0x7c },  // 75 u
  { 0x1c, 0x20, 0x40, 0x20, 0x1c },  // 76 v
  { 0x3c, 0x40, 0x30, 0x40, 0x3c },  // 77 w
  { 0x44, 0x28, 0x10, 0x28, 0x44 },  // 78 x
  { 0x0c, 0x50, 0x50, 0x50, 0x3c },  // 79 y
  { 0x44, 0x64, 0x54, 0x4c, 0x44 },  // 7a z
  { 0x00, 0x08, 0x36, 0x41, 0x00 },  // 7b {
  { 0x00, 0x00, 0x7f, 0x00, 0x00 },  // 7c |
  { 0x00, 0x41, 0x36, 0x08, 0x00 },  // 7d }
  { 0x10, 0x08, 0x08, 0x10, 0x08 },  // 7e ~
  { 0x00, 0x00, 0x00, 0x00, 0x00 }   // 7f
};

NOKIA_LCD::NOKIA_LCD(uint8_t mosi, uint8_t sclk, uint8_t cs, uint8_t dc, uint8_t rst) : mosiPin(mosi), sckPin(sclk), ssPin(cs), dcPin(dc), rstPin(rst) {
	this->height = 48;
	this->width = 84;
	this->column = 0;
	this->line = 0;
}

void NOKIA_LCD::send(uint8_t data){
	uint8_t i = 8;
	while(i--){
		//halt(1);
		if ((data & (1 << i)) != 0){
			digitalWrite(mosiPin, HIGH);
		} else {
			digitalWrite(mosiPin, LOW);
		}
		digitalWrite(sckPin, HIGH);
		digitalWrite(sckPin, HIGH);
		digitalWrite(sckPin, LOW);
		digitalWrite(sckPin, LOW);
	}
}

void NOKIA_LCD::sendData(uint8_t data){
	digitalWrite(ssPin, LOW);//SS LOW
	digitalWrite(dcPin, HIGH);//dc high
	this->send(data);
	digitalWrite(ssPin, HIGH);//SS HIGH
}

void NOKIA_LCD::sendCmd(uint8_t data){
	digitalWrite(ssPin, LOW);//SS LOW
	digitalWrite(dcPin, LOW);//dc low
	this->send(data);
	digitalWrite(ssPin, HIGH);//SS HIGH
}

void NOKIA_LCD::writeByte(uint8_t column, uint8_t line, uint8_t data){
	digitalWrite(ssPin, LOW);//SS LOW
	digitalWrite(dcPin, LOW);//DC LOW
	this->send(0x80 | (column % this->width));
	this->send(0x40 | (line % (this->height >> 3)));
	digitalWrite(dcPin, HIGH);//DC HIGH
	this->send(data);
	digitalWrite(ssPin, HIGH);//SS HIGH
}

void NOKIA_LCD::writeBuf(uint8_t column, uint8_t line, uint8_t *data, uint16_t len){
	digitalWrite(ssPin, LOW);//SS LOW
	digitalWrite(dcPin, LOW);//CMD Mode
	this->send(0x80 | (column % this->width));
	this->send(0x40 | (line % (this->height >> 3)));
	digitalWrite(dcPin, HIGH);//DATA Mode
	while(len--) this->send(*data++);
	digitalWrite(ssPin, HIGH);//SS HIGH
}

void NOKIA_LCD::writeBuf(uint8_t *data){
	uint16_t len = this->width * (this->height >> 3);
	digitalWrite(ssPin, LOW);//SS LOW
	digitalWrite(dcPin, LOW);//DC LOW
	this->send(0x80);
	this->send(0x40);
	digitalWrite(dcPin, HIGH);//DC HIGH
	while(len--) this->send(*data++);
	digitalWrite(ssPin, HIGH);//SS HIGH
}

void NOKIA_LCD::setCursor(uint8_t column, uint8_t line){
	this->column = (column % this->width);
	this->line = (line % (this->height/8));

	digitalWrite(ssPin, LOW);//SS LOW
	digitalWrite(dcPin, LOW);//DC LOW
	this->send(0x80 | this->column);
	this->send(0x40 | this->line);
	digitalWrite(ssPin, HIGH);//SS HIGH
}

void NOKIA_LCD::clearLine(){
	this->column = 0;
	uint8_t chars = this->width;

	digitalWrite(ssPin, LOW);//SS LOW
	digitalWrite(dcPin, LOW);//DC LOW
	this->send(0x80);
	this->send(0x40 | this->line);
	digitalWrite(dcPin, HIGH);//DC HIGH
	while (chars--) this->send(0x00);
	digitalWrite(dcPin, LOW);//DC LOW
	this->send(0x80);
	this->send(0x40 | this->line);
	digitalWrite(ssPin, HIGH);//SS HIGH
}

void NOKIA_LCD::clear(){
	this->column = 0;
	this->line = 0;
	uint16_t chars = this->width * (this->height >> 3);

	digitalWrite(ssPin, LOW);//SS LOW
	digitalWrite(dcPin, LOW);//DC LOW
	this->send(0x80);
	this->send(0x40);
	digitalWrite(dcPin, HIGH);//DC HIGH
	while (chars--) this->send(0x00);
	digitalWrite(dcPin, LOW);//DC LOW
	this->send(0x80);
	this->send(0x40);
	digitalWrite(ssPin, HIGH);//SS HIGH
}

void NOKIA_LCD::begin(uint8_t model){
	pinMode(rstPin, OUTPUT);
	pinMode(ssPin, OUTPUT);
	pinMode(dcPin, OUTPUT);
	pinMode(mosiPin, OUTPUT);
	pinMode(sckPin, OUTPUT);
    //reset the controller
    digitalWrite(rstPin, HIGH);//reset high
    digitalWrite(ssPin, HIGH);//ss high
    digitalWrite(rstPin, LOW);//reset low
    delay(100);//delay
    digitalWrite(rstPin, HIGH);//reset high

    digitalWrite(ssPin, LOW);//SS LOW
    digitalWrite(dcPin, LOW);//DC LOW

    // Set the LCD parameters...
    this->send(0x21);  // extended instruction set control (H=1)
    this->send(0x13);  // bias system (1:48)

    if (model == CHIP_ST7576) {
        this->send(0xe0);  // higher Vop, too faint at default
        this->send(0x05);  // partial display mode
    } else {
        this->send(0xc2);  // default Vop (3.06 + 66 * 0.06 = 7V)
    }

    this->send(0x20);  // extended instruction set control (H=0)
    this->send(0x09);  // all display segments on

    // Clear RAM contents...
    digitalWrite(dcPin, HIGH);//DC HIGH
	uint16_t chars = this->width * (this->height >> 3);
    while (chars--) this->send(0x00);
    digitalWrite(dcPin, LOW);//DC LOW

    // Activate LCD...
    this->send(0x08);  // display blank
    this->send(0x0c);  // normal mode (0x0d = inverse mode)
    delay(100);

    // Place the cursor at the origin...
    this->send(0x80);
    this->send(0x40);
    digitalWrite(ssPin, HIGH);//SS HIGH
}



//Chars and Strings

void NOKIA_LCD::createChar(uint8_t chr, const uint8_t *glyph){
    // ASCII 0-31 only...
    if (chr >= ' ') return;
    this->custom[chr] = glyph;
}

void NOKIA_LCD::write(uint8_t chr){
    // ASCII 7-bit only...
    if (chr >= 0x80) return;

    const uint8_t *glyph;

    if (chr >= ' ') {
        // Regular ASCII characters are kept in flash to save RAM...
        glyph = charset[chr - ' '];
    } else {
        // Custom glyphs, on the other hand, are stored in RAM...
        if (this->custom[chr]) {
            glyph = this->custom[chr];
        } else {
            // Default to a space character if unset...
            glyph = charset[0];
        }
    }

    digitalWrite(ssPin, LOW);//SS LOW
    digitalWrite(dcPin, HIGH);//DC HIGH
    for (uint8_t i = 0; i < 5; i++) this->send(glyph[i]);// Output one column at a time...
    this->send(0x00); // One column between characters...
    digitalWrite(ssPin, HIGH);//SS HIGH

    this->column = (this->column + 6) % this->width; // Update the cursor position...

    if (this->column == 0) {
        this->line = (this->line + 1) % (this->height/8);
    }
}

int NOKIA_LCD::write(const char *str) {
    while (*str) this->write(*str ++);
    return 0;
}

//Pixel

void NOKIA_LCD::drawBitmap(const uint8_t *data, uint8_t columns, uint8_t lines){
    uint8_t scolumn = this->column;
    uint8_t sline = this->line;

    // The bitmap will be clipped at the right/bottom edge of the display...
    uint8_t mx = (scolumn + columns > this->width) ? (this->width - scolumn) : columns;
    uint8_t my = (sline + lines > this->height/8) ? (this->height/8 - sline) : lines;

    digitalWrite(ssPin, LOW);//SS LOW
    for (uint8_t y = 0; y < my; y++) {
    	digitalWrite(dcPin, LOW);//DC LOW
    	this->send(0x80 | scolumn);
    	this->send(0x40 | (sline + y));
    	digitalWrite(dcPin, HIGH);//DC HIGH
        for (uint8_t x = 0; x < mx; x++) {
            this->send(data[y * columns + x]);
        }
    }
    digitalWrite(dcPin, LOW);//DC LOW
    this->send(0x80 | (scolumn + columns));
    this->send(0x40 | sline);
    digitalWrite(ssPin, HIGH);//SS HIGH

    this->column = scolumn + columns;
    this->line = sline;
}


void NOKIA_LCD::setPixel(uint8_t x, uint8_t y, bool value, bool write = false){
  uint8_t column = (x % this->width);
  uint8_t line = ((y >> 3) % (this->height >> 3));
  uint16_t index = column + (line * this->width);
  if(value){
	  this->lcd_buffer[index] |= (1 << (y % 8));
  } else {
	  this->lcd_buffer[index] &= ~(1 << (y % 8));
  }

  if(write) this->writeByte(column, line, (uint8_t)this->lcd_buffer[index]);
}

void NOKIA_LCD::draw(){
	writeBuf((uint8_t *)this->lcd_buffer);
}


//Helpers

inline void NOKIA_LCD::display(){
    this->sendCmd(0x20);
}

inline void NOKIA_LCD::noDisplay(){
    this->sendCmd(0x24);
}

void NOKIA_LCD::setPower(bool on){
    this->sendCmd(on ? 0x20 : 0x24);
}

void NOKIA_LCD::setInverse(bool inverse){
    this->sendCmd(inverse ? 0x0d : 0x0c);
}

void NOKIA_LCD::home(){
    this->setCursor(0, this->line);
}

void NOKIA_LCD::stop(){
    this->clear();
    this->noDisplay();
}
