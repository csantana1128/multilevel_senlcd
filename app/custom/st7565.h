#define ST7565_DRIVER_H

#define DISPLAY_SPI_ID SPI_1_ID
#define DISPLAY_CS_INDEX 0

#define DISPLAY_DC_PIN 8
#define DISPLAY_RST_PIN 6

#define LCD_WIDTH 128
#define LCD_HEIGHT 64
#define LCD_PAGES (LCD_HEIGHT / 8)


#define SPI1_CLK_PIN_DISPLAY 7
#define SPI1_IO0_PIN_DISPLAY 15
#define SPI1_IO1_PIN_DISPLAY 31
#define SPI1_MAX_CHIP_SELECT_PINS 1
#define SPI1_CS0_PIN_DISPLAY 9

#define SHARED_SPI_TX_BUFFER_SIZE 128

void display_clear(void);
void display_update(void);
void display_init(void);
void initialize_spi(void);
void display_temperature(int temperature_centi_degrees);
void display_test_on_button_press(void);