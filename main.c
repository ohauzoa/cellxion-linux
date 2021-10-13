
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
#include "lv_libs/lv_audio.h"

#include "src/gui.h"
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


static Audio * sound;
static const lv_font_t * font_temp;

void sound_click(void)
{
    playSoundFromMemory(sound, SDL_MIX_MAXVOLUME);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
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

int thread_func(void *unused)
{
    int last_value = 0;

    while ( 1 ) {
		//for(int i=0; i<10; i++)add_data();
		add_data();
        SDL_Delay(100);
    }
    printf("Thread quitting\n");
    return(0);
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

int main(int argc, char **argv)
{
	setvbuf(stdout, NULL, _IONBF, 0); // Windows �� Eclipse�� Console â ���� �� ������ ���� ����
	setvbuf(stderr, NULL, _IONBF, 0);
	(void)argv; /*Unused*/
	SDL_Thread *thread;
	
	/*Initialize LVGL*/
	lv_init();

	/*Initialize the HAL (display, input devices, tick) for LVGL*/
	hal_init();
	lv_fs_stdio_init();
	lv_png_init();

    /*Init freetype library
    *Cache max 64 faces and 1 size*/
    lv_freetype_init(64, 1, 0);

    
    static lv_ft_info_t info;
    info.name = "../images/arial.ttf";
    info.weight = 64;
    info.style = FT_FONT_STYLE_NORMAL;


    if(lv_ft_font_init(&info)){
        font_temp = info.font;
    }
    else{
        font_temp = LV_FONT_DEFAULT;
    }


    /*Initialize the SDL*/
    SDL_Init(SDL_INIT_AUDIO);
    /* Init Simple-SDL2-Audio */
    initAudio();

    /* Play music and a sound */
    playMusic("../sounds/highlands.wav", SDL_MIX_MAXVOLUME);
    sound = createAudio("../sounds/door1.wav", 0, SDL_MIX_MAXVOLUME / 2);
    /* Override music, play another sound */
	//playSound("./sounds/door1.wav", SDL_MIX_MAXVOLUME / 2);
//    playMusic("./sounds/road.wav", SDL_MIX_MAXVOLUME);



	test();
    thread = SDL_CreateThread(thread_func, "chart", NULL);
    if ( thread == NULL ) {
        fprintf(stderr, "Unable to create thread: %s\n", SDL_GetError());
        return;
    }

//	SDL_CreateThread(comm_amp_thread, "amp", NULL);
//	SDL_CreateThread(comm_smps_thread, "smps", NULL);
//	SDL_CreateThread(comm_arduino_thread, "arduino", NULL);
//  treatment_create1();

//	lv_cellxion_start();

	
	while(1) {
		/* Periodically call the lv_task handler.
		 * It could be done in a timer interrupt or an OS task too.*/
		lv_timer_handler();
		usleep(5 * 1000);
	}

	return 0;
}

