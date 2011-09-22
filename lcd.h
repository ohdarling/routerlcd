/**
 * HB12864M2A Operating Lib
 * @author  ohdarling88 <ohdarling88@gmail.com>
 * @date    2011.07.19
 */
 
void lcd_connect();
void lcd_disconnect();

void lcd_reset();
void lcd_clear();
void lcd_set_backlight(int open);

void lcd_draw_string(int x, int y, char *str);
void lcd_draw_image(int x, int y, int width, int height, unsigned char *data);
