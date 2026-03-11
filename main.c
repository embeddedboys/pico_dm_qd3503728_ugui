// Copyright (c) 2024 embeddedboys developers

// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:

// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdbool.h>

#include "pico/time.h"
#include "pico/stdio.h"
#include "pico/stdlib.h"
#include "pico/stdio_uart.h"

#include "hardware/pll.h"
#include "hardware/vreg.h"
#include "hardware/clocks.h"
#include "hardware/timer.h"

#include "ili9488.h"
#include "ft6236.h"
#include "backlight.h"

#include "ugui/ugui.h"

#define ugui_area_get_size(x1, y1, x2, y2) ((x2 - x1 + 1) * (y2 - y1 + 1))
#define UGUI_VIDEO_MEMORY_LINES		   (10)
#define UGUI_VIDEO_MEMORY_SIZE		   (LCD_HOR_RES * UGUI_VIDEO_MEMORY_LINES)

static uint16_t ugui_video_memory[UGUI_VIDEO_MEMORY_SIZE];

static void ugui_test(bool loop);
static void ugui_test_matrix(bool loop);
static void matrix_init(void);
static void matrix_update(void);

static void ugui_pset(UG_S16 x, UG_S16 y, UG_COLOR color)
{
	ili9488_video_flush(x, y, x, y, &color, sizeof(color));
}

static UG_RESULT ugui_fill_frame(int x1, int y1, int x2, int y2, uint16_t color)
{
	uint32_t len = ugui_area_get_size(x1, y1, x2, y2);
	// printf("%s, %d, %d, %d, %d, %04x, len : %d\n", __func__, x1, y1, x2, y2,
	//        color, len);

	/* Load the color into video memory, prepare for transfer */
	for (int i = 0; i < UGUI_VIDEO_MEMORY_SIZE; i++)
		ugui_video_memory[i] = color;

	uint32_t remain = len;
	uint32_t chunk_size;

	while (remain) {
		chunk_size = MIN(remain, UGUI_VIDEO_MEMORY_SIZE);

		ili9488_video_flush(x1, y1, x2, y2, ugui_video_memory,
				    chunk_size * sizeof(color));

		y1 += UGUI_VIDEO_MEMORY_LINES;
		remain -= chunk_size;
	}

	return UG_RESULT_OK;
}

static void ugui_push_pixel(UG_COLOR color)
{
	ili9488_write_data(color);
}

static void *ugui_fill_area(int x1, int y1, int x2, int y2)
{
	ili9488_set_window(x1, y1, x2, y2);
	return ugui_push_pixel;
}

bool ugui_input_timer_callback(struct repeating_timer *t)
{
	static uint16_t last_x, last_y;

	if (ft6236_is_pressed()) {
		uint16_t x = ft6236_read_x();
		uint16_t y = ft6236_read_y();
		// printf("pressed at (%d, %d)\n", x, y);
		UG_TouchUpdate(x, y, TOUCH_STATE_PRESSED);

		last_x = x;
		last_y = y;
	} else {
		UG_TouchUpdate(last_x, last_y, TOUCH_STATE_RELEASED);
	}

	return true;
}

static void ugui_port_init(void)
{
	UG_GUI ugui;

	UG_Init(&ugui, ugui_pset, LCD_HOR_RES, LCD_VER_RES);

	// UG_DriverRegister(DRIVER_DRAW_LINE, ugui_draw_line);
	UG_DriverRegister(DRIVER_FILL_FRAME, ugui_fill_frame);
	UG_DriverRegister(DRIVER_FILL_AREA, ugui_fill_area);
}

int main(void)
{
	/* NOTE: DO NOT MODIFY THIS BLOCK */
#define CPU_SPEED_MHZ (DEFAULT_SYS_CLK_KHZ / 1000)
	if (CPU_SPEED_MHZ > 266 && CPU_SPEED_MHZ <= 360)
		vreg_set_voltage(VREG_VOLTAGE_1_20);
	else if (CPU_SPEED_MHZ > 360 && CPU_SPEED_MHZ <= 396)
		vreg_set_voltage(VREG_VOLTAGE_1_25);
	else if (CPU_SPEED_MHZ > 396)
		vreg_set_voltage(VREG_VOLTAGE_MAX);
	else
		vreg_set_voltage(VREG_VOLTAGE_DEFAULT);

	set_sys_clock_khz(CPU_SPEED_MHZ * 1000, true);
	clock_configure(clk_peri, 0, CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLK_SYS,
			CPU_SPEED_MHZ * MHZ, CPU_SPEED_MHZ * MHZ);
	stdio_uart_init_full(uart0, 115200, 16, 17);
	stdio_usb_init();

	printf("\n\n\nPICO DM QD3503728 Display Template\n");

	ili9488_driver_init();
	ft6236_driver_init();

	ili9488_fill(0x0000);

	sleep_ms(10);
	backlight_driver_init();
	backlight_set_level(100);
	printf("backlight set to 100%%\n");

	struct repeating_timer ugui_input_timer;
	add_repeating_timer_ms(33, ugui_input_timer_callback, NULL,
			       &ugui_input_timer);

	ugui_port_init();
	ugui_test(false);

	printf("going to loop, %lld\n", time_us_64() / 1000);
	for (;;) {
		UG_Update();
		// tight_loop_contents();
		// sleep_ms(200);
	}

	return 0;
}

struct ugui_test {
	u8 idx;
	const char *name;
	void (*func)(bool loop);
	bool hold;
};
#define DEFINE_UGUI_TEST(i, n, f, h) \
	{ .idx = i, .name = n, .func = f, .hold = h }
#define ARRAY_SIZE(arr) ((sizeof(arr) / sizeof(arr[0])))

static void ugui_test_color_fill(bool loop)
{
	uint16_t color[] = { C_RED,  C_ORANGE, C_YELLOW, C_GREEN,
			     C_BLUE, C_INDIGO, C_VIOLET };

	for (int i = 0; i < ARRAY_SIZE(color); i++) {
		UG_FillScreen(color[i]);
		sleep_ms(100);
	}
}

static void ugui_test_draw(bool loop)
{
	UG_FillScreen(C_BLACK);
	UG_SetForecolor(C_WHITE);
	UG_DrawLine(0, 0, 239, 239, C_RED);
	UG_DrawFrame(10, 10, 100, 100, C_RED);
	UG_FillFrame(120, 10, 200, 100, C_GREEN);
}

/* Calculate the grid size */
#define MAX_COLS (LCD_HOR_RES / 8)
#define MAX_ROWS (LCD_VER_RES / 8)

static void ugui_test_text(bool loop)
{
	UG_FillScreen(C_BLACK);
	UG_FontSelect(&FONT_8X8);
	UG_SetForecolor(C_WHITE);

	for (int timeout = 50; timeout > 0; timeout--) {
		char str_buf[2]; /* Buffer for a single character + null terminator */
		str_buf[1] = '\0'; /* Ensure string is null-terminated */

		for (int row = 0; row < MAX_ROWS; row++) {
			for (int col = 0; col < MAX_COLS; col++) {
				/* Generate a random printable ASCII character (from '!' to '~') */
				str_buf[0] = (char)((rand() % 94) + 33);

				/* Optional: Randomize the color for a 'Matrix' or 'Glitch' effect */
				UG_SetForecolor((UG_COLOR)(rand() & 0xFFFF));

				/* Calculate pixel coordinates for the current grid cell */
				UG_S16 x = (UG_S16)(col * 8);
				UG_S16 y = (UG_S16)(row * 8);

				/* Draw the character string at the calculated position */
				UG_PutString(x, y, str_buf);
			}
		}
	}
}

#define FONT_W	 8
#define FONT_H	 12
#define COLUMNS	 (LCD_HOR_RES / FONT_W)
#define RAIN_LEN 15

int drop_pos[COLUMNS];
int drop_speed[COLUMNS];

static void matrix_init(void)
{
	UG_FillScreen(C_BLACK);
	for (int i = 0; i < COLUMNS; i++) {
		drop_pos[i] = rand() % (LCD_VER_RES / FONT_H + RAIN_LEN);
		drop_speed[i] = (rand() % 3);
	}
}

static void matrix_update(void)
{
	/* Ensure the correct font is selected for drawing */
	UG_FontSelect(&FONT_8X12);

	for (int i = 0; i < COLUMNS; i++) {
		/* Basic speed control: skip update randomly to vary column speeds */
		if (rand() % 2 == 0)
			continue;

		/* --- 1. Erase the Tail --- */
		/* Calculate the position of the last character in the trail */
		int tail_row = drop_pos[i] - RAIN_LEN;
		int tail_y = tail_row * FONT_H;

		/* Coordinate clipping: Only erase if the tail is within visible screen bounds */
		if (tail_y >= 0 && tail_y <= (LCD_VER_RES - FONT_H)) {
			/* Overwrite the tail position with a black rectangle to clear it */
			UG_FillFrame(i * FONT_W, tail_y, (i + 1) * FONT_W - 1,
				     tail_y + FONT_H - 1, C_BLACK);
		}

		/* --- 2. Draw the Falling Head --- */
		int head_y = drop_pos[i] * FONT_H;

		/* Coordinate clipping: Only draw if the head is within visible screen bounds */
		if (head_y >= 0 && head_y <= (LCD_VER_RES - FONT_H)) {
			/* Select a random printable ASCII character */
			char random_char = (rand() % 94) + 33;

			/* Draw the leading character in bright Lime color */
			UG_PutChar(random_char, i * FONT_W, head_y, C_LIME,
				   C_BLACK);

			/*
			 * Visual polish: Change the previous head character to a darker green
			 * to create a fading trail effect
			 */
			int prev_y = (drop_pos[i] - 1) * FONT_H;
			if (prev_y >= 0 && prev_y <= (LCD_VER_RES - FONT_H)) {
				UG_PutChar((rand() % 94) + 33, i * FONT_W,
					   prev_y, C_GREEN, C_BLACK);
			}
		}

		/* --- 3. Update Position --- */
		/* Increment the row index to move the rain down */
		drop_pos[i]++;

		/*
		 * --- 4. Reset Cycle Logic ---
		 * Check if the entire trail (including the tail) has exited the bottom.
		 * This prevents characters from accumulating at the bottom edge.
		 */
		if (drop_pos[i] * FONT_H > LCD_VER_RES + (RAIN_LEN * FONT_H)) {
			/* Reset the column to start from the top again */
			drop_pos[i] = 0;
		}
	}
}

static void ugui_test_matrix(bool loop)
{
	int timeout = 3000;
	matrix_init();

	for (;;) {
		matrix_update();
		sleep_ms(40);

		if (loop)
			continue;

		timeout -= 40;

		if (timeout < 0)
			break;
	}
}

UG_WINDOW window;
UG_BUTTON button, button2;
UG_TEXTBOX textbox;
UG_OBJECT obj_buff[10];

void window_callback(UG_MESSAGE *msg)
{
	// printf("%s, type : %d, id : %d, event : %d\n", __func__, msg->type,
	//        msg->id, msg->event);
	if (msg->type == MSG_TYPE_OBJECT && msg->id == OBJ_TYPE_BUTTON) {
		if (msg->event == OBJ_EVENT_PRESSED) {
			printf("Button %d - '%s' pressed! \n", msg->sub_id,
			       UG_ButtonGetText(&window, msg->sub_id));
			if (msg->sub_id == BTN_ID_0)
				ugui_test(false);
			else if (msg->sub_id == BTN_ID_1)
				ugui_test_matrix(true);
			else
				;
		}
	}
}

static void ugui_test_window(bool loop)
{
	UG_WindowCreate(&window, obj_buff, 10, window_callback);
	UG_WindowSetTitleText(&window, "This is not a test");
	UG_WindowSetTitleTextColor(&window, C_WHITE);
	UG_WindowSetTitleTextFont(&window, &FONT_8X12);

	UG_TextboxCreate(&window, &textbox, TXB_ID_0, 20, 20, 400, 120);
	UG_TextboxSetText(
		&window, TXB_ID_0,
		"All tests have been completed.\nrun the tests again?");
	UG_TextboxSetFont(&window, TXB_ID_0, &FONT_12X16);
	UG_TextboxSetAlignment(&window, TXB_ID_0, ALIGN_CENTER);
	UG_TextboxSetForeColor(&window, TXB_ID_0, C_BLACK);
	UG_TextboxSetBackColor(&window, TXB_ID_0, C_WHITE);
	// UG_TextboxShow(&window, TXB_ID_0);

	UG_ButtonCreate(&window, &button, BTN_ID_0, 200, 240, 300, 280);
	UG_ButtonCreate(&window, &button2, BTN_ID_1, 340, 240, 440, 280);

	UG_ButtonSetFont(&window, BTN_ID_0, &FONT_8X12);
	UG_ButtonSetText(&window, BTN_ID_0, "Yes");

	UG_ButtonSetFont(&window, BTN_ID_1, &FONT_8X12);
	UG_ButtonSetText(&window, BTN_ID_1, "No");

	UG_WindowShow(&window);
}

static const struct ugui_test ugui_tests[] = {
	DEFINE_UGUI_TEST(0, "color fill", ugui_test_color_fill, false),
	DEFINE_UGUI_TEST(1, "draw", ugui_test_draw, false),
	DEFINE_UGUI_TEST(2, "text", ugui_test_text, false),
	DEFINE_UGUI_TEST(3, "matrix", ugui_test_matrix, false),
	DEFINE_UGUI_TEST(4, "window", ugui_test_window, false),
	{ /* sentinel */ },
};

static void ugui_test(bool loop)
{
	const struct ugui_test *test = ugui_tests;

	for (; test->func; test++) {
		printf("Running test %d -> '%s'\n", test->idx, test->name);
		test->func(false);
		sleep_ms(800);
	}
}
