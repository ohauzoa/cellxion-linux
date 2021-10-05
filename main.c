
/**
 * @file main
 *
 */

/*********************
 *      INCLUDES
 *********************/
#define _DEFAULT_SOURCE /* needed for usleep() */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define SDL_MAIN_HANDLED /*To fix SDL's "undefined reference to WinMain" issue*/
#include <SDL2/SDL.h>
#include "lvgl/lvgl.h"
#include "lv_drivers/display/monitor.h"
#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/indev/mouse.h"
#include "lv_drivers/indev/keyboard.h"
#include "lv_drivers/indev/mousewheel.h"
#include "lv_libs/lv_png.h"
#include "lv_libs/cellxion.h"
//#include "serialport.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void hal_init(void);
static int tick_thread(void *data);
void set_smps_sv(int sv);
void set_smps_si(int si);
void set_smps_si(int si);
void set_smps_power(int v);
static int comm_smps_thread(void);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *      VARIABLES
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

//static HANDLE comm_arduino;
//static HANDLE comm_smps;
//static HANDLE comm_amp;

static int flag_smps_power = 0;
static int flag_smps_voltage = 0;
static int flag_smps_current = 0;
static int smps_loop = 0;
static char smps_cmd[20];

static int amp_port_open = 0;
static int amp_loop = 0;
unsigned char msg[10];
char rx_matcher[20];


void check_sum(const char *buf, int cnt)
{
	int i;
	unsigned char EOR = 0x00;
	memset(msg, 0 ,20);
	for(i=0 ; i < cnt ; i++)
		msg[i] = *buf++;

	for(i=0 ; i < cnt ; i++)
		EOR ^=  msg[i];

	msg[cnt] = EOR;
	msg[cnt+1] = 0x03;
if(0){
	for(i=0;i<cnt+2;i++){
		printf("%02X ",msg[i]);
	}
	printf("\n");
}
}
void set_amp_port_open(void)
{
	//comm_amp = openSerialPort("COM7",B9600,one,off);
	//printf("amp port open ok!\n");
}

void set_amp_command(int cmd, int v)
{

}

static int comm_amp_thread(void)
{
}

static int comm_smps_thread(void)
{
}

void lexcial_buf(char *buf)
{
  char command = buf[1];
  int v1 = 0, v2 = 0;
  switch(command)
  {
    case 'S':
        v1 += (buf[2] - '0') * 1000;
        v1 += (buf[3] - '0') * 100;
        v1 += (buf[4] - '0') * 10;
        v1 += (buf[5] - '0') * 1;
        v2 += (buf[6] - '0') * 1000;
        v2 += (buf[7] - '0') * 100;
        v2 += (buf[8] - '0') * 10;
        v2 += (buf[9] - '0') * 1;
        printf("S->%d, %d\n", v1, v2);
        break;
   case 'R':
       v1 += (buf[2] - '0') * 1000;
       v1 += (buf[3] - '0') * 100;
       v1 += (buf[4] - '0') * 10;
       v1 += (buf[5] - '0') * 1;

       v2 += (buf[7] - '0') * 1000;
       v2 += (buf[8] - '0') * 100;
       v2 += (buf[9] - '0') * 10;
       v2 += (buf[10] - '0') * 1;
//       set_vswr_angle(0, v2);
//       set_vswr_angle(1, v1);
       printf("R->%d, %d\n", v1, v2);
      break;

   }
if(0){
  for(int i=0;i<12;i++){
		printf("%02X ",buf[i]);
	}
	printf("\n");
}
}


static int comm_arduino_thread(void)
{
}

void test_port(int sv1, int sv2)
{
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/



int main(int argc, char **argv)
{
	setvbuf(stdout, NULL, _IONBF, 0); // Windows �� Eclipse�� Console â ���� �� ������ ���� ����
	setvbuf(stderr, NULL, _IONBF, 0);
	(void)argv; /*Unused*/

	/*Initialize LVGL*/
	lv_init();

	/*Initialize the HAL (display, input devices, tick) for LVGL*/
	hal_init();
	lv_png_init();





//	SDL_CreateThread(comm_amp_thread, "amp", NULL);
//	SDL_CreateThread(comm_smps_thread, "smps", NULL);
//	SDL_CreateThread(comm_arduino_thread, "arduino", NULL);
//  treatment_create1();

	lv_cellxion_start();

	while(1) {
		/* Periodically call the lv_task handler.
		 * It could be done in a timer interrupt or an OS task too.*/
		lv_timer_handler();
		usleep(5 * 1000);
	}

	return 0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Initialize the Hardware Abstraction Layer (HAL) for the LVGL graphics
 * library
 */
static void hal_init(void)
{
	/* Use the 'monitor' driver which creates window on PC's monitor to simulate a display*/
	monitor_init();
	/* Tick init.
	* You have to call 'lv_tick_inc()' in periodically to inform LittelvGL about
	* how much time were elapsed Create an SDL thread to do this*/
	SDL_CreateThread(tick_thread, "tick", NULL);

	/*Create a display buffer*/
	static lv_disp_draw_buf_t disp_buf1;
	static lv_color_t buf1_1[MONITOR_HOR_RES * 1000];
	static lv_color_t buf1_2[MONITOR_HOR_RES * 1000];
	lv_disp_draw_buf_init(&disp_buf1, buf1_1, buf1_2, MONITOR_HOR_RES * 1000);

	/*Create a display*/
	static lv_disp_drv_t disp_drv;
	lv_disp_drv_init(&disp_drv); /*Basic initialization*/
	disp_drv.draw_buf = &disp_buf1;
	disp_drv.flush_cb = monitor_flush;
	disp_drv.hor_res = MONITOR_HOR_RES;
	disp_drv.ver_res = MONITOR_VER_RES;
	disp_drv.antialiasing = 1;

	lv_disp_t * disp = lv_disp_drv_register(&disp_drv);

	lv_theme_t * th = lv_theme_default_init(disp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), LV_THEME_DEFAULT_DARK, LV_FONT_DEFAULT);
	lv_disp_set_theme(disp, th);

	lv_group_t * g = lv_group_create();
	lv_group_set_default(g);

	/* Add the mouse as input device
	* Use the 'mouse' driver which reads the PC's mouse*/
	mouse_init();
	static lv_indev_drv_t indev_drv_1;
	lv_indev_drv_init(&indev_drv_1); /*Basic initialization*/
	indev_drv_1.type = LV_INDEV_TYPE_POINTER;

	/*This function will be called periodically (by the library) to get the mouse position and state*/
	indev_drv_1.read_cb = mouse_read;
	lv_indev_t *mouse_indev = lv_indev_drv_register(&indev_drv_1);

	keyboard_init();
	static lv_indev_drv_t indev_drv_2;
	lv_indev_drv_init(&indev_drv_2); /*Basic initialization*/
	indev_drv_2.type = LV_INDEV_TYPE_KEYPAD;
	indev_drv_2.read_cb = keyboard_read;
	lv_indev_t *kb_indev = lv_indev_drv_register(&indev_drv_2);
	lv_indev_set_group(kb_indev, g);
	mousewheel_init();
	static lv_indev_drv_t indev_drv_3;
	lv_indev_drv_init(&indev_drv_3); /*Basic initialization*/
	indev_drv_3.type = LV_INDEV_TYPE_ENCODER;
	indev_drv_3.read_cb = mousewheel_read;

	lv_indev_t * enc_indev = lv_indev_drv_register(&indev_drv_3);
	lv_indev_set_group(enc_indev, g);

	/*Set a cursor for the mouse*/
	LV_IMG_DECLARE(mouse_cursor_icon); /*Declare the image file.*/
	lv_obj_t * cursor_obj = lv_img_create(lv_scr_act()); /*Create an image object for the cursor */
	lv_img_set_src(cursor_obj, &mouse_cursor_icon);           /*Set the image source*/
	//  lv_indev_set_cursor(mouse_indev, cursor_obj);             /*Connect the image  object to the driver*/
}

/**
 * A task to measure the elapsed time for LVGL
 * @param data unused
 * @return never return
 */
static int tick_thread(void *data)
{
    (void)data;

    while(1) { 
        SDL_Delay(5);
        lv_tick_inc(5); /*Tell LittelvGL that 5 milliseconds were elapsed*/
    }

    return 0;
}


void set_smps_sv(int sv)
{
	memset(smps_cmd, 0, 20);
	sprintf(smps_cmd, "SV %02d.%1d\r\n", sv/10, sv%10);
//	LV_LOG_USER("%s", smps_cmd);
	flag_smps_voltage = 1;
	smps_loop = 15;
}

void set_smps_si(int si)
{
	memset(smps_cmd, 0, 20);
	sprintf(smps_cmd, "SI %02d.%1d\r\n", si/10, si%10);
//	LV_LOG_USER("%s", smps_cmd);
	flag_smps_current = 1;
	smps_loop = 35;
}

void set_smps_power(int v)
{
//	LV_LOG_USER("set_smps_power %d", v);
	if(v == 1){
		flag_smps_power = 1;
		smps_loop = 100;
	}
	else{
		flag_smps_power = 1;
		smps_loop = 0;
	}
}





