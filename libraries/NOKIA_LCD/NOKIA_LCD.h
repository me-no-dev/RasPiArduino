#ifndef NOKIA_LCD_h
#define NOKIA_LCD_h

#include "Arduino.h"


// Chip variants supported...
#define CHIP_PCD8544 0
#define CHIP_ST7576  1

class NOKIA_LCD {
  public:
    uint8_t width;
    uint8_t height;

	NOKIA_LCD(uint8_t mosi, uint8_t sclk, uint8_t cs, uint8_t dc, uint8_t rst);

	void begin(uint8_t model=CHIP_PCD8544);
	void setPower(bool on); // Control the display's power state...
	void setInverse(bool inverse); // Activate white-on-black mode (whole display)...
	void setCursor(uint8_t column, uint8_t line);
	void clear(); // Erase everything on the display...
	void clearLine();  // ...or just the current line

	void createChar(uint8_t chr, const uint8_t *glyph); // Assign a user-defined glyph (5x8) to an ASCII character (0-31)...

	void write(uint8_t chr);
	int write(const char *str);

	void drawBitmap(const uint8_t *data, uint8_t columns, uint8_t lines);
	void setPixel(uint8_t x, uint8_t y, bool value, bool write);
	void draw();


	void stop();
	void display();
	void noDisplay();
	void home(); // Place the cursor at the start of the current line...

  private:
	uint8_t mosiPin;
	uint8_t sckPin;
	uint8_t ssPin;
	uint8_t dcPin;
	uint8_t rstPin;

    uint8_t column;
    uint8_t line;
    const uint8_t *custom[' '];
    uint8_t lcd_buffer[504];

	void init();
    void send(uint8_t data);
    void sendData(uint8_t data);
    void sendCmd(uint8_t data);

	void writeByte(uint8_t column, uint8_t line, uint8_t data);
	void writeBuf(uint8_t column, uint8_t line, uint8_t *data, uint16_t len);
	void writeBuf(uint8_t *data); //full buffer

};
#endif
