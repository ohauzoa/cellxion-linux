/*
 * patient.c
 *
 *  Created on: 2021. 8. 25.
 *      Author: ForYou
 */


#include "../lvgl/lvgl.h"
#include <stdio.h>
#include <stdlib.h>
#include "cellxion.h"

#include "lv_freetype.h"
#include "lv_rlottie.h"


/**********************
 *  STATIC PROTOTYPES
 **********************/
static void btn_event_cb(lv_event_t * e);
static void ta_event_cb(lv_event_t * e);
static void calendar_event_cb(lv_event_t * e);
static void birthday_event_cb(lv_event_t * e);
static void draw_part_event_cb(lv_event_t * e);
static void patient_create(lv_obj_t * parent);
static void set_angle(void * img, int32_t v);
static void treatment_timer(lv_timer_t * timer);
static void treatment_create(lv_obj_t * parent);
static void passwd_btn_event_cb(lv_event_t * e);
static void passwd_panel_create(lv_obj_t * parent);
static void setting_anim_cb(void * var, int32_t v);
static void setting_event_cb(lv_event_t *e);
static void drag_event_handler(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/
static const lv_font_t * font_timer;
static const lv_font_t * font_large;
static const lv_font_t * font_normal;
static const lv_font_t * font_temp;

static lv_obj_t * tv;
static lv_obj_t * passwd;
static lv_obj_t * setting_cont;
static lv_obj_t * calendar;
static lv_obj_t * calendar_header;
static lv_style_t style_text_muted;
static lv_style_t style_title;
static lv_style_t style_icon;
static lv_style_t style_bullet;
static lv_style_t style_timer;
static lv_style_t style_sel;
static lv_style_t style_roller;


static lv_obj_t * panel_vswr;
static lv_obj_t * img_needle1;
static lv_obj_t * img_needle2;
static lv_obj_t * img_needle3;


static lv_obj_t * obj_system_arc;
static lv_obj_t * obj_system_time;
static lv_obj_t * obj_system_status;
static lv_obj_t * obj_currently_delivered_power;
static lv_obj_t * obj_delivered_energy;

static int downcount;
static int init_vswr = 0;


static lv_obj_t * spinbox_voltage;
static lv_obj_t * spinbox_amplitude;
static lv_obj_t * spinbox_current;

static lv_obj_t * spinbox_sv1;
static lv_obj_t * spinbox_sv2;

static void lv_spinbox_amplitude_increment_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_SHORT_CLICKED || code  == LV_EVENT_LONG_PRESSED_REPEAT) {
        lv_spinbox_increment(spinbox_amplitude);
        int v = lv_spinbox_get_value(spinbox_amplitude);
//        set_smps_sv(v);
    }
}

static void lv_spinbox_amplitude_decrement_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_SHORT_CLICKED || code == LV_EVENT_LONG_PRESSED_REPEAT) {
    	lv_spinbox_decrement(spinbox_amplitude);
        int v = lv_spinbox_get_value(spinbox_amplitude);
//        set_smps_sv(v);
    }
}

static void execute_amplitude_event_cb(lv_event_t * e)
{
//    LV_LOG_USER("Clicked");

    int v1 = lv_spinbox_get_value(spinbox_amplitude);
    lv_obj_t * btn = lv_event_get_target(e);
    lv_obj_t * label = lv_obj_get_child(btn, 0);
    lv_label_set_text_fmt(label, "%03d", v1);
    //set_amp_command(0, v1);
    //SDL_Delay(40);
    set_amp_command(10, v1);
//    test_port(v1, v2);
}


static void lv_spinbox_voltage_increment_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_SHORT_CLICKED || code  == LV_EVENT_LONG_PRESSED_REPEAT) {
        lv_spinbox_increment(spinbox_voltage);
        int v = lv_spinbox_get_value(spinbox_voltage);
        set_smps_sv(v);
    }
}

static void lv_spinbox_voltage_decrement_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_SHORT_CLICKED || code == LV_EVENT_LONG_PRESSED_REPEAT) {
    	lv_spinbox_decrement(spinbox_voltage);
        int v = lv_spinbox_get_value(spinbox_voltage);
        set_smps_sv(v);
    }
}

static void lv_spinbox_current_increment_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_SHORT_CLICKED || code  == LV_EVENT_LONG_PRESSED_REPEAT) {
        lv_spinbox_increment(spinbox_current);
        int v = lv_spinbox_get_value(spinbox_current);
        set_smps_si(v);
    }
}

static void lv_spinbox_current_decrement_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_SHORT_CLICKED || code == LV_EVENT_LONG_PRESSED_REPEAT) {
    	lv_spinbox_decrement(spinbox_current);
        int v = lv_spinbox_get_value(spinbox_current);
        set_smps_si(v);
    }
}

static void execute_smps_event_cb(lv_event_t * e)
{
	//    LV_LOG_USER("Clicked");
	static int flag = 0;
    lv_obj_t * btn = lv_event_get_target(e);
    lv_obj_t * label = lv_obj_get_child(btn, 0);
    if(flag){
    	lv_label_set_text(label, "POWER ON");
    	flag = 0;
    }
    else{
    	lv_label_set_text(label, "POWER 0FF");
    	flag = 1;
    }
    set_smps_power(flag);
}



static void move_servo(void)
{
    int v1 = lv_spinbox_get_value(spinbox_sv1);
    int v2 = lv_spinbox_get_value(spinbox_sv2);
    test_port(v1, v2);
}

static void lv_spinbox_sv1_increment_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_SHORT_CLICKED || code  == LV_EVENT_LONG_PRESSED_REPEAT) {
        lv_spinbox_increment(spinbox_sv1);
        move_servo();
    }
}

static void lv_spinbox_sv1_decrement_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_SHORT_CLICKED || code == LV_EVENT_LONG_PRESSED_REPEAT) {
    	lv_spinbox_decrement(spinbox_sv1);
    	move_servo();
    }
}

static void lv_spinbox_sv2_increment_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_SHORT_CLICKED || code  == LV_EVENT_LONG_PRESSED_REPEAT) {
        lv_spinbox_increment(spinbox_sv2);
        move_servo();
    }
}

static void lv_spinbox_sv2_decrement_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_SHORT_CLICKED || code == LV_EVENT_LONG_PRESSED_REPEAT) {
    	lv_spinbox_decrement(spinbox_sv2);
    	move_servo();
    }
}

static void execute_servo_event_cb(lv_event_t * e)
{
//    LV_LOG_USER("Clicked");

    int v1 = lv_spinbox_get_value(spinbox_sv1);
    int v2 = lv_spinbox_get_value(spinbox_sv2);
    lv_obj_t * btn = lv_event_get_target(e);
    lv_obj_t * label = lv_obj_get_child(btn, 0);
    lv_label_set_text_fmt(label, "%03d %03d", v1, v2);
    test_port(v1, v2);
}


static void setting_create(lv_obj_t * parent)
{

	setting_cont = lv_obj_create(parent);

//    lv_obj_remove_style_all(setting_cont);
    lv_obj_set_flex_flow(setting_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(setting_cont, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_add_flag(setting_cont, LV_OBJ_FLAG_FLOATING);

    lv_obj_t * panel = lv_obj_create(setting_cont);
    lv_obj_set_size(panel, 1700, 900);
//    lv_obj_set_y(panel, -10);

    static lv_style_t style_pr;
    lv_style_init(&style_pr);

    lv_style_set_bg_opa(&style_pr, 255);
    lv_style_set_bg_color(&style_pr, lv_color_make(255, 255, 255));
    lv_style_set_border_width(&style_pr, 2);
    lv_style_set_border_color(&style_pr, lv_color_make(230, 230, 230));

    //    lv_style_reset(&style_pr);
    lv_obj_add_style(panel, &style_pr, 0);
    lv_obj_add_flag(setting_cont, LV_OBJ_FLAG_HIDDEN);

    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_radius(&style, 3);

	LV_IMG_DECLARE(img_setting);
	lv_obj_t * img_set = lv_img_create(panel);
	lv_img_set_src(img_set, &img_setting);
	lv_img_set_zoom(img_set, 140);
	lv_obj_set_pos(img_set, 0, -5);

	lv_obj_t * lbl_setting_title = lv_label_create(panel);
	lv_label_set_text(lbl_setting_title, "Adjust the Matcher v2");
	lv_obj_add_style(lbl_setting_title, &style_title, 0);
	lv_obj_set_pos(lbl_setting_title, 50, 5);

///////////////////////////////////////////////////////////////////////


//    lv_obj_t* r2 = lv_rlottie_create_from_file(panel, 500, 300, "./lv_libs/orange-circle.json");
    //lv_obj_t* r3 = lv_rlottie_create_from_file(lv_scr_act(), 200, 200, "./lv_libs/lv_rlottie.gif");






    lv_obj_t* lbl_voltage = lv_label_create(panel);
    lv_label_set_text(lbl_voltage, "Voltage");
    lv_obj_add_style(lbl_voltage, &style_text_muted, 0);
    lv_obj_set_pos(lbl_voltage, 10, 265);

    spinbox_voltage = lv_spinbox_create(panel);
    lv_obj_add_style(spinbox_voltage, &style, 0);
    //
    lv_spinbox_set_range(spinbox_voltage, 400, 480);
    lv_spinbox_set_digit_format(spinbox_voltage, 3, 2);
    lv_spinbox_step_prev(spinbox_voltage);
    lv_spinbox_set_pos(spinbox_voltage, 0);
    lv_obj_set_width(spinbox_voltage, 60);
    lv_obj_set_pos(spinbox_voltage, 50, 300);
    lv_spinbox_set_value(spinbox_voltage, 480);

    lv_coord_t h_voltage = lv_obj_get_height(spinbox_voltage);

    lv_obj_t * btn_voltage = lv_btn_create(panel);
    lv_obj_set_size(btn_voltage, h_voltage, h_voltage);
    lv_obj_align_to(btn_voltage, spinbox_voltage, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
    lv_obj_set_style_bg_img_src(btn_voltage, LV_SYMBOL_PLUS, 0);
    lv_obj_add_event_cb(btn_voltage, lv_spinbox_voltage_increment_event_cb, LV_EVENT_ALL,  NULL);
    lv_obj_add_style(btn_voltage, &style, 0);
    lv_obj_add_style(btn_voltage, &style_sel, 0);

    btn_voltage = lv_btn_create(panel);
    lv_obj_set_size(btn_voltage, h_voltage, h_voltage);
    lv_obj_align_to(btn_voltage, spinbox_voltage, LV_ALIGN_OUT_LEFT_MID, -5, 0);
    lv_obj_set_style_bg_img_src(btn_voltage, LV_SYMBOL_MINUS, 0);
    lv_obj_add_event_cb(btn_voltage, lv_spinbox_voltage_decrement_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_style(btn_voltage, &style, 0);
    lv_obj_add_style(btn_voltage, &style_sel, 0);


    lv_obj_t* lbl_current = lv_label_create(panel);
    lv_label_set_text(lbl_current, "Current");
    lv_obj_add_style(lbl_current, &style_text_muted, 0);
    lv_obj_set_pos(lbl_current, 210, 265);


    spinbox_current = lv_spinbox_create(panel);
    lv_obj_add_style(spinbox_current, &style, 0);
    ///
    lv_spinbox_set_range(spinbox_current, 80, 100);
    lv_spinbox_set_digit_format(spinbox_current, 3, 2);
    lv_spinbox_step_prev(spinbox_current);
    lv_spinbox_set_pos(spinbox_current, 0);
    lv_obj_set_width(spinbox_current, 60);
    lv_obj_set_pos(spinbox_current, 255, 300);
    lv_spinbox_set_value(spinbox_current, 100);

    lv_coord_t h_current = lv_obj_get_height(spinbox_current);

    lv_obj_t * btn_current = lv_btn_create(panel);
    lv_obj_set_size(btn_current, h_current, h_current);
    lv_obj_align_to(btn_current, spinbox_current, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
    lv_obj_set_style_bg_img_src(btn_current, LV_SYMBOL_PLUS, 0);
    lv_obj_add_event_cb(btn_current, lv_spinbox_current_increment_event_cb, LV_EVENT_ALL,  NULL);
    lv_obj_add_style(btn_current, &style, 0);
    lv_obj_add_style(btn_current, &style_sel, 0);

    btn_current = lv_btn_create(panel);
    lv_obj_set_size(btn_current, h_current, h_current);
    lv_obj_align_to(btn_current, spinbox_current, LV_ALIGN_OUT_LEFT_MID, -5, 0);
    lv_obj_set_style_bg_img_src(btn_current, LV_SYMBOL_MINUS, 0);
    lv_obj_add_event_cb(btn_current, lv_spinbox_current_decrement_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_style(btn_current, &style, 0);
    lv_obj_add_style(btn_current, &style_sel, 0);

    lv_obj_t * btn_execute_smps = lv_btn_create(panel);     /*Add a button the current screen*/
    lv_obj_set_pos(btn_execute_smps, 440, 298);                            /*Set its position*/
    lv_obj_set_size(btn_execute_smps, 200, 50);                          /*Set its size*/
    lv_obj_add_event_cb(btn_execute_smps, execute_smps_event_cb, LV_EVENT_CLICKED, NULL);           /*Assign a callback to the button*/
    lv_obj_add_flag(btn_execute_smps, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_add_style(btn_execute_smps, &style, 0);
    lv_obj_add_style(btn_execute_smps, &style_sel, 0);

    lv_obj_t * label_execute_smps = lv_label_create(btn_execute_smps);          /*Add a label to the button*/
    lv_label_set_text(label_execute_smps, "Execute SMPS");                     /*Set the labels text*/
    lv_obj_center(label_execute_smps);

//////////////////////////////////////////////////////////////////////////////////


    lv_obj_t* lbl_amp = lv_label_create(panel);
    lv_label_set_text(lbl_amp, "Amplitude");
    lv_obj_add_style(lbl_amp, &style_text_muted, 0);
    lv_obj_set_pos(lbl_amp, 10, 465);

    spinbox_amplitude = lv_spinbox_create(panel);
    lv_obj_add_style(spinbox_amplitude, &style, 0);
    //
    lv_spinbox_set_range(spinbox_amplitude, 1, 99);
    lv_spinbox_set_digit_format(spinbox_amplitude, 2, 0);
    lv_spinbox_step_prev(spinbox_amplitude);
    lv_spinbox_set_pos(spinbox_amplitude, 0);
    lv_obj_set_width(spinbox_amplitude, 60);
    lv_obj_set_pos(spinbox_amplitude, 50, 500);
    lv_spinbox_set_value(spinbox_amplitude, 0);
    lv_spinbox_set_value(spinbox_amplitude, 40);

    lv_coord_t h_amplitude = lv_obj_get_height(spinbox_amplitude);

    lv_obj_t * btn_amplitude = lv_btn_create(panel);
    lv_obj_set_size(btn_amplitude, h_amplitude, h_amplitude);
    lv_obj_align_to(btn_amplitude, spinbox_amplitude, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
    lv_obj_set_style_bg_img_src(btn_amplitude, LV_SYMBOL_PLUS, 0);
    lv_obj_add_event_cb(btn_amplitude, lv_spinbox_amplitude_increment_event_cb, LV_EVENT_ALL,  NULL);
    lv_obj_add_style(btn_amplitude, &style, 0);
    lv_obj_add_style(btn_amplitude, &style_sel, 0);

    btn_amplitude = lv_btn_create(panel);
    lv_obj_set_size(btn_amplitude, h_amplitude, h_amplitude);
    lv_obj_align_to(btn_amplitude, spinbox_amplitude, LV_ALIGN_OUT_LEFT_MID, -5, 0);
    lv_obj_set_style_bg_img_src(btn_amplitude, LV_SYMBOL_MINUS, 0);
    lv_obj_add_event_cb(btn_amplitude, lv_spinbox_amplitude_decrement_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_style(btn_amplitude, &style, 0);
    lv_obj_add_style(btn_amplitude, &style_sel, 0);


    lv_obj_t * btn_execute_amplitude = lv_btn_create(panel);     /*Add a button the current screen*/
    lv_obj_set_pos(btn_execute_amplitude, 440, 498);                            /*Set its position*/
    lv_obj_set_size(btn_execute_amplitude, 200, 50);                          /*Set its size*/
    lv_obj_add_event_cb(btn_execute_amplitude, execute_amplitude_event_cb, LV_EVENT_CLICKED, NULL);           /*Assign a callback to the button*/
//    lv_obj_add_flag(btn_execute_amplitude, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_add_style(btn_execute_amplitude, &style, 0);
    lv_obj_add_style(btn_execute_amplitude, &style_sel, 0);

    lv_obj_t * label_execute_amplitude = lv_label_create(btn_execute_amplitude);          /*Add a label to the button*/
    lv_label_set_text(label_execute_amplitude, "Execute Amp.");                     /*Set the labels text*/
    lv_obj_center(label_execute_amplitude);



    ////////////////////////////////////////////////////////////////////////////////////



    lv_obj_t* lbl_spin1 = lv_label_create(panel);
    lv_label_set_text(lbl_spin1, "Servo 1");
    lv_obj_add_style(lbl_spin1, &style_text_muted, 0);
    lv_obj_set_pos(lbl_spin1, 10, 665);

    spinbox_sv1 = lv_spinbox_create(panel);
    lv_spinbox_set_range(spinbox_sv1, 0, 180);
    lv_spinbox_set_digit_format(spinbox_sv1, 3, 0);
    lv_spinbox_step_prev(spinbox_sv1);
    lv_spinbox_set_pos(spinbox_sv1, 0);
    lv_obj_set_width(spinbox_sv1, 60);
    lv_obj_set_pos(spinbox_sv1, 50, 700);
    lv_obj_add_style(spinbox_sv1, &style, 0);
    lv_spinbox_set_value(spinbox_sv1, 51);

    lv_coord_t h_sv1 = lv_obj_get_height(spinbox_sv1);

    lv_obj_t * btn_sv1 = lv_btn_create(panel);
    lv_obj_set_size(btn_sv1, h_sv1, h_sv1);
    lv_obj_align_to(btn_sv1, spinbox_sv1, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
    lv_obj_set_style_bg_img_src(btn_sv1, LV_SYMBOL_PLUS, 0);
    lv_obj_add_event_cb(btn_sv1, lv_spinbox_sv1_increment_event_cb, LV_EVENT_ALL,  NULL);
    lv_obj_add_style(btn_sv1, &style, 0);
    lv_obj_add_style(btn_sv1, &style_sel, 0);

    btn_sv1 = lv_btn_create(panel);
    lv_obj_set_size(btn_sv1, h_sv1, h_sv1);
    lv_obj_align_to(btn_sv1, spinbox_sv1, LV_ALIGN_OUT_LEFT_MID, -5, 0);
    lv_obj_set_style_bg_img_src(btn_sv1, LV_SYMBOL_MINUS, 0);
    lv_obj_add_event_cb(btn_sv1, lv_spinbox_sv1_decrement_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_style(btn_sv1, &style, 0);
    lv_obj_add_style(btn_sv1, &style_sel, 0);


    lv_obj_t* lbl_spin2 = lv_label_create(panel);
    lv_label_set_text(lbl_spin2, "Servo 2");
    lv_obj_add_style(lbl_spin2, &style_text_muted, 0);
    lv_obj_set_pos(lbl_spin2, 210, 665);

    spinbox_sv2 = lv_spinbox_create(panel);
    lv_spinbox_set_range(spinbox_sv2, 0, 180);
    lv_spinbox_set_digit_format(spinbox_sv2, 3, 0);
    lv_spinbox_step_prev(spinbox_sv2);
    lv_spinbox_set_pos(spinbox_sv2, 0);
    lv_obj_set_width(spinbox_sv2, 60);
    lv_obj_set_pos(spinbox_sv2, 255, 700);
    lv_obj_add_style(spinbox_sv2, &style, 0);
    lv_spinbox_set_value(spinbox_sv2, 126);

    lv_coord_t h_sv2 = lv_obj_get_height(spinbox_sv2);

    lv_obj_t * btn_sv2 = lv_btn_create(panel);
    lv_obj_set_size(btn_sv2, h_sv2, h_sv2);
    lv_obj_align_to(btn_sv2, spinbox_sv2, LV_ALIGN_OUT_RIGHT_MID, 5, 0);
    lv_obj_set_style_bg_img_src(btn_sv2, LV_SYMBOL_PLUS, 0);
    lv_obj_add_event_cb(btn_sv2, lv_spinbox_sv2_increment_event_cb, LV_EVENT_ALL,  NULL);
    lv_obj_add_style(btn_sv2, &style, 0);
    lv_obj_add_style(btn_sv2, &style_sel, 0);

    btn_sv2 = lv_btn_create(panel);
    lv_obj_set_size(btn_sv2, h_sv2, h_sv2);
    lv_obj_align_to(btn_sv2, spinbox_sv2, LV_ALIGN_OUT_LEFT_MID, -5, 0);
    lv_obj_set_style_bg_img_src(btn_sv2, LV_SYMBOL_MINUS, 0);
    lv_obj_add_event_cb(btn_sv2, lv_spinbox_sv2_decrement_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_style(btn_sv2, &style, 0);
    lv_obj_add_style(btn_sv2, &style_sel, 0);




    lv_obj_t * btn_execute_servo = lv_btn_create(panel);     /*Add a button the current screen*/
    lv_obj_set_pos(btn_execute_servo, 440, 698);                            /*Set its position*/
    lv_obj_set_size(btn_execute_servo, 200, 50);                          /*Set its size*/
    lv_obj_add_event_cb(btn_execute_servo, execute_servo_event_cb, LV_EVENT_CLICKED, NULL);           /*Assign a callback to the button*/
    lv_obj_add_style(btn_execute_servo, &style, 0);
    lv_obj_add_style(btn_execute_servo, &style_sel, 0);

    lv_obj_t * label_execute_sevo = lv_label_create(btn_execute_servo);          /*Add a label to the button*/
    lv_label_set_text(label_execute_sevo, "Execute Servo");                     /*Set the labels text*/
    lv_obj_center(label_execute_sevo);

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////


    lv_obj_t* r2 = lv_rlottie_create_from_file(panel, 100, 100, "../images/card.json");
    lv_obj_set_pos(r2,  100, 100);

    lv_obj_t* r3 = lv_rlottie_create_from_file(panel, 100, 100, "../images/orange-circle.json");
    lv_obj_set_pos(r3,  200, 200);

    lv_obj_t* r4 = lv_rlottie_create_from_file(panel, 100, 100, "../images/test1.json");
    lv_obj_set_pos(r4,  300, 100);

    panel_vswr = lv_obj_create(panel);
    lv_obj_set_height(panel_vswr, LV_SIZE_CONTENT);
    lv_obj_set_size(panel_vswr, 800, 800);
    lv_obj_add_style(panel_vswr, &style, 0);
    lv_obj_add_style(panel_vswr, &style_sel, 0);
    lv_obj_set_pos(panel_vswr,  750, 20);

    LV_IMG_DECLARE(patient_s);
    lv_obj_t * img_treatment = lv_img_create(panel_vswr);
    lv_img_set_src(img_treatment, "../images/png_decoder_test.png");
//    lv_img_set_src(img_treatment, &patient_s);
    lv_obj_set_pos(img_treatment,0,5);


    lv_obj_t * panel1_title = lv_label_create(panel_vswr);
    lv_label_set_text(panel1_title, "VSWR MONITOR");
    lv_obj_add_style(panel1_title, &style_title, 0);
    lv_obj_set_x(panel1_title, 30);






    LV_IMG_DECLARE(img_vswr);
    LV_IMG_DECLARE(img_needle);
    LV_IMG_DECLARE(img_needle_g);
    LV_IMG_DECLARE(img_needle_r);

    lv_obj_t * img1 = lv_img_create(panel_vswr);
    lv_img_set_src(img1, &img_vswr);
    lv_obj_set_height(img1, 430);
    lv_obj_set_pos(img1, 105, 80);
//    lv_obj_align(img1, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t* lbl_forword = lv_label_create(panel_vswr);
    lv_label_set_text(lbl_forword, "forword");
    lv_obj_align_to(lbl_forword, img1, LV_ALIGN_TOP_LEFT, 40, 70);
//    lv_obj_set_pos(lbl_forword, 130, 120);

    lv_obj_t* lbl_reflected = lv_label_create(panel_vswr);
    lv_label_set_text(lbl_reflected, "reflected");
    lv_obj_align_to(lbl_reflected, img1, LV_ALIGN_TOP_RIGHT, -40, 70);
//    lv_obj_set_pos(lbl_reflected, 510, 120);



    img_needle1 = lv_img_create(panel_vswr);
    lv_img_set_src(img_needle1, &img_needle_r);
//    lv_obj_align_to(img_needle1, img1, LV_ALIGN_BOTTOM_RIGHT, -125, -175);
    lv_obj_align_to(img_needle1, img1, LV_ALIGN_BOTTOM_RIGHT, -125, -5);
    lv_img_set_pivot(img_needle1, 11, 380);    /*Rotate around the top left corner*/
//    lv_obj_set_size(img_needle1, 34, 450);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, img_needle1);
//    lv_anim_set_exec_cb(&a, set_angle);
//    lv_anim_set_values(&a, 0, 100);
    lv_anim_set_time(&a, 5000);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&a);
    lv_anim_set_values(&a, 2870, 3530);
    lv_anim_set_playback_time(&a, 5000);
    lv_anim_start(&a);

    img_needle2 = lv_img_create(panel_vswr);
    lv_img_set_src(img_needle2, &img_needle_r);
    lv_obj_align_to(img_needle2, img1, LV_ALIGN_BOTTOM_LEFT, 125, -5);
    lv_img_set_pivot(img_needle2, 11, 380);    /*Rotate around the top left corner*/
//    lv_obj_set_size(img_needle2, 34, 450);

    lv_anim_t b;
    lv_anim_init(&b);
    lv_anim_set_var(&b, img_needle2);
//    lv_anim_set_exec_cb(&b, set_angle);
//    lv_anim_set_values(&b, 0, 3600);
    lv_anim_set_time(&b, 3000);
    lv_anim_set_repeat_count(&b, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&b);
    lv_anim_set_values(&b, 730, 60);
    lv_anim_set_playback_time(&b, 1000);
    lv_anim_start(&b);

    init_vswr = 1;

/*

    img_needle3 = lv_img_create(panel_vswr);
    lv_img_set_src(img_needle3, &img_needle_r);
    lv_obj_align_to(img_needle3, img1, LV_ALIGN_BOTTOM_RIGHT, -125, -5);
    lv_img_set_pivot(img_needle3, 11, 380);
//    lv_obj_set_size(img_needle3, 34, 450);

    lv_anim_t c;
    lv_anim_init(&c);
    lv_anim_set_var(&c, img_needle3);
    lv_anim_set_exec_cb(&c, set_angle);
//    lv_anim_set_values(&c, 0, 3600);
    lv_anim_set_time(&c, 4000);
    lv_anim_set_repeat_count(&c, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&c);
    lv_anim_set_values(&c, 2900, 3500);
    lv_anim_set_playback_time(&c, 4000);
    lv_anim_start(&c);

//    lv_img_set_zoom(img1, 200);
//    lv_img_set_zoom(img_needle1, 200);
//    lv_img_set_zoom(img_needle2, 200);
*/














////////////////////////////////////////////////////////////////////////////////////////////////

    lv_obj_t * btn = lv_btn_create(parent);
    lv_obj_add_flag(btn, LV_OBJ_FLAG_FLOATING | LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_style_bg_color(btn, lv_color_white(), LV_STATE_CHECKED);
    lv_obj_set_style_pad_all(btn, 10, 0);
    lv_obj_set_style_radius(btn, LV_RADIUS_CIRCLE, 0);
    lv_obj_add_event_cb(btn, setting_event_cb, LV_EVENT_ALL, setting_cont);
    lv_obj_set_style_shadow_width(btn, 0, 0);
    lv_obj_set_style_bg_img_src(btn, LV_SYMBOL_SETTINGS, 0);
    lv_obj_add_style(btn, &style_sel, 0);

	lv_obj_set_size(btn, LV_DPX(80), LV_DPX(80));
	lv_obj_align(btn, LV_ALIGN_RIGHT_MID, -LV_DPX(15), -LV_DPX(15));
//    lv_obj_add_event_cb(btn, drag_event_handler, LV_EVENT_PRESSING, NULL);

}



void lv_cellxion_start(void)
{
    /*Init freetype library
    *Cache max 64 faces and 1 size*/
    lv_freetype_init(64, 1, 0);

    /*Create a font*/
    static lv_ft_info_t info;
    info.name = "images/arial.ttf";
    info.weight = 64;
    info.style = FT_FONT_STYLE_NORMAL;


    if(lv_ft_font_init(&info)){
        font_temp = info.font;
    }
    else{
        font_temp = LV_FONT_DEFAULT;
    }


    /*Create a label with the new style*/
    //lv_obj_t * label = lv_label_create(lv_scr_act());
    //lv_obj_add_style(label, &style, 0);
    //lv_label_set_text(label, "Hello world");




    font_large = LV_FONT_DEFAULT;
    font_normal = LV_FONT_DEFAULT;

    lv_coord_t tab_h;
	tab_h = 80;

	font_timer     =  &lv_font_montserrat_48;
	font_large     =  &lv_font_montserrat_24;
	font_normal    =  &lv_font_montserrat_16;

#if LV_USE_THEME_DEFAULT
    lv_theme_default_init(NULL, lv_palette_main(LV_PALETTE_INDIGO), lv_palette_main(LV_PALETTE_RED), LV_THEME_DEFAULT_DARK, font_normal);
#endif

    lv_style_init(&style_timer);
    lv_style_set_text_font(&style_timer, font_timer);

    lv_style_init(&style_roller);
    lv_style_set_text_font(&style_roller, font_temp);

    lv_style_init(&style_text_muted);
    lv_style_set_text_opa(&style_text_muted, LV_OPA_60);

    lv_style_init(&style_title);
    lv_style_set_text_font(&style_title, font_large);

    lv_style_init(&style_icon);
    lv_style_set_text_color(&style_icon, lv_theme_get_color_primary(NULL));
    lv_style_set_text_font(&style_icon, font_large);

    lv_style_init(&style_bullet);
    lv_style_set_border_width(&style_bullet, 0);
    lv_style_set_radius(&style_bullet, LV_RADIUS_CIRCLE);

    /*A style to make the selected option larger*/
    lv_style_init(&style_sel);
    lv_style_set_text_font(&style_sel, &lv_font_montserrat_22);
    lv_style_set_bg_opa(&style_sel, LV_OPA_COVER);
    lv_style_set_bg_color(&style_sel, lv_palette_lighten(LV_PALETTE_GREY, 3));
    lv_style_set_bg_grad_color(&style_sel, lv_palette_main(LV_PALETTE_GREY));
    lv_style_set_bg_grad_dir(&style_sel, LV_GRAD_DIR_VER);




    tv = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, tab_h);

    lv_obj_set_style_text_font(lv_scr_act(), font_normal, 0);

	lv_obj_t * tab_btns = lv_tabview_get_tab_btns(tv);
	lv_obj_set_style_pad_left(tab_btns, LV_HOR_RES / 2, 0);
	lv_obj_t * logo = lv_img_create(tab_btns);
	LV_IMG_DECLARE(img_medicoson);
	lv_img_set_src(logo, &img_medicoson);
	lv_obj_align(logo, LV_ALIGN_LEFT_MID, -LV_HOR_RES / 2 + 25, 0);
	lv_obj_add_style(tab_btns, &style_sel, 0);

	lv_obj_t * label = lv_label_create(tab_btns);
	lv_obj_add_style(label, &style_title, 0);
	lv_label_set_text(label, "cellxion");
	lv_obj_align_to(label, logo, LV_ALIGN_OUT_RIGHT_TOP, 10, -4);

	label = lv_label_create(tab_btns);
	lv_label_set_text(label, " matcher v2");
	lv_obj_add_style(label, &style_text_muted, 0);
	lv_obj_align_to(label, logo, LV_ALIGN_OUT_RIGHT_BOTTOM, 5, 3);

    lv_obj_t * t1 = lv_tabview_add_tab(tv, "Patient");


 //   lv_obj_add_style(t1, &style_sel, 0);

//	lv_obj_t * t1_btns = lv_tabview_get_tab_btns(t1);
//	lv_obj_set_style_pad_left(t1_btns, 0, 0);
//	lv_obj_t * t1_logo = lv_img_create(t1_btns);
//	LV_IMG_DECLARE(patient_data);
//	lv_img_set_src(t1_logo, &patient_data);
//	lv_obj_align(t1_logo, LV_ALIGN_CENTER, 0, 0);


    lv_obj_t * t2 = lv_tabview_add_tab(tv, "Treatment");
//    lv_obj_t * t3 = lv_tabview_add_tab(tv, "Analytics");

    patient_create(t1);
    treatment_create(t2);
//    shop_create(t3);
    lv_tabview_set_act(tv, 1, 1);
    setting_create(tv);
//    passwd_panel_create(lv_scr_act());

}



/**********************
 *   STATIC FUNCTIONS
 **********************/
static void setting_anim_cb(void * var, int32_t v)
{
    lv_obj_t * obj = var;
    lv_coord_t max_w = lv_obj_get_width(lv_obj_get_parent(obj)) - LV_DPX(20);
    lv_coord_t max_h = lv_obj_get_height(lv_obj_get_parent(obj)) - LV_DPX(20);
    lv_coord_t w;

	w = lv_map(v, 0, 256, LV_DPX(60), max_w);
	lv_obj_set_width(obj, w);
	lv_obj_set_height(obj, max_h);
	lv_obj_align(obj, LV_ALIGN_RIGHT_MID, - LV_DPX(10),  - LV_DPX(10));

	if(v > 1){
		lv_obj_clear_flag(setting_cont, LV_OBJ_FLAG_HIDDEN);
	}
	else{
		lv_obj_add_flag(setting_cont, LV_OBJ_FLAG_HIDDEN);
	}
}


static void setting_event_cb(lv_event_t *e)
{
    if(lv_event_get_code(e) == LV_EVENT_CLICKED) {
        lv_obj_t * setting_cont = lv_event_get_user_data(e);
        if(lv_obj_get_width(setting_cont) < LV_HOR_RES / 2) {
            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, setting_cont);
            lv_anim_set_exec_cb(&a, setting_anim_cb);
            lv_anim_set_values(&a, 0, 256);
            lv_anim_set_time(&a, 200);
            lv_anim_start(&a);
        } else {
            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, setting_cont);
            lv_anim_set_exec_cb(&a, setting_anim_cb);
            lv_anim_set_values(&a, 256, 0);
            lv_anim_set_time(&a, 200);
            lv_anim_start(&a);
        }
    }
}

static void drag_event_handler(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);

    lv_indev_t * indev = lv_indev_get_act();
    lv_point_t vect;
    lv_indev_get_vect(indev, &vect);

    lv_coord_t x = lv_obj_get_x(obj) + vect.x;
    lv_coord_t y = lv_obj_get_y(obj) + vect.y;
    lv_obj_set_pos(obj, x, y);
}

static void roller_event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_VALUE_CHANGED) {
        char buf[32];
        lv_roller_get_selected_str(obj, buf, sizeof(buf));
        LV_LOG_USER("Selected value: %s", buf);
    }
}











static void add_step_event_cb(lv_event_t * e)
{
    LV_LOG_USER("add_step_event_cb Clicked");
    sound_click();
}




static void treatment_create(lv_obj_t * parent)
{
    
    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_ROW_WRAP);
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_radius(&style, 3);

    //  Create the patient panel
	lv_obj_t * panel_patient = lv_obj_create(parent);
	lv_obj_set_height(panel_patient, LV_SIZE_CONTENT);
	lv_obj_add_style(panel_patient, &style, 0);

	LV_IMG_DECLARE(patient_s);
	lv_obj_t * img_patient = lv_img_create(panel_patient);
	lv_img_set_src(img_patient, &patient_s);

	lv_obj_t * lbl_patient_title = lv_label_create(panel_patient);
	lv_label_set_text(lbl_patient_title, "PATIENT DATA");
	lv_obj_add_style(lbl_patient_title, &style_title, 0);

    lv_obj_t * user_firstname_label = lv_label_create(panel_patient);
    lv_label_set_text(user_firstname_label, "Firstname");
    lv_obj_add_style(user_firstname_label, &style_text_muted, 0);

    lv_obj_t * user_firstname = lv_label_create(panel_patient);
	lv_label_set_text(user_firstname, "Maximilian Benjamin");
	lv_obj_add_style(user_firstname, &style_title, 0);

    lv_obj_t * user_surname_label = lv_label_create(panel_patient);
    lv_label_set_text(user_surname_label, "Surname");
    lv_obj_add_style(user_surname_label, &style_text_muted, 0);

    lv_obj_t * user_surname = lv_label_create(panel_patient);
	lv_label_set_text(user_surname, "Mustermann-Musterfrau");
	lv_obj_add_style(user_surname, &style_title, 0);

    lv_obj_t * user_date_of_birth_label = lv_label_create(panel_patient);
    lv_label_set_text(user_date_of_birth_label, "Date of Birth");
    lv_obj_add_style(user_date_of_birth_label, &style_text_muted, 0);

    lv_obj_t * user_date_of_birth = lv_label_create(panel_patient);
	lv_label_set_text(user_date_of_birth, "23.04.1950");
	lv_obj_add_style(user_date_of_birth, &style_title, 0);

    lv_obj_t * user_weight_label = lv_label_create(panel_patient);
    lv_label_set_text(user_weight_label, "Weight");
    lv_obj_add_style(user_weight_label, &style_text_muted, 0);

    lv_obj_t * user_weight = lv_label_create(panel_patient);
	lv_label_set_text(user_weight, "82 kg");
	lv_obj_add_style(user_weight, &style_title, 0);

    lv_obj_t * user_height_label = lv_label_create(panel_patient);
    lv_label_set_text(user_height_label, "Height");
    lv_obj_add_style(user_height_label, &style_text_muted, 0);

    lv_obj_t * user_height = lv_label_create(panel_patient);
	lv_label_set_text(user_height, "180 cm");
	lv_obj_add_style(user_height, &style_title, 0);

    lv_obj_t * user_gender_label = lv_label_create(panel_patient);
    lv_label_set_text(user_gender_label, "Gender");
    lv_obj_add_style(user_gender_label, &style_text_muted, 0);

    lv_obj_t * user_gender = lv_label_create(panel_patient);
	lv_label_set_text(user_gender, "mail");
	lv_obj_add_style(user_gender, &style_title, 0);

    lv_obj_t * user_pmcf_label = lv_label_create(panel_patient);
    lv_label_set_text(user_pmcf_label, "PMCF");
    lv_obj_add_style(user_pmcf_label, &style_text_muted, 0);

    lv_obj_t * user_pmcf = lv_label_create(panel_patient);
	lv_label_set_text(user_pmcf, "False");
	lv_obj_add_style(user_pmcf, &style_title, 0);






    //  Create the effect panel
	lv_obj_t * panel_effect = lv_obj_create(parent);
	lv_obj_set_height(panel_effect, LV_SIZE_CONTENT);
	lv_obj_add_style(panel_effect, &style, 0);

	LV_IMG_DECLARE(patient_s);
	lv_obj_t * img_effect = lv_img_create(panel_effect);
	lv_img_set_src(img_effect, &patient_s);

	lv_obj_t * lbl_effect_title = lv_label_create(panel_effect);
	lv_label_set_text(lbl_effect_title, "SIDE EFFECTS LAST SESSION");
	lv_obj_add_style(lbl_effect_title, &style_title, 0);

    lv_obj_t * effect1_label = lv_label_create(panel_effect);
    lv_label_set_text(effect1_label, "Hautrotungen");
    lv_obj_add_style(effect1_label, &style_text_muted, 0);

    lv_obj_t * effect2_label = lv_label_create(panel_effect);
    lv_label_set_text(effect2_label, "Verbrennungen Grad 1");
    lv_obj_add_style(effect2_label, &style_text_muted, 0);

    lv_obj_t * effect3_label = lv_label_create(panel_effect);
    lv_label_set_text(effect3_label, "Verbrennungen Grad 2");
    lv_obj_add_style(effect3_label, &style_text_muted, 0);

    lv_obj_t * effect4_label = lv_label_create(panel_effect);
    lv_label_set_text(effect4_label, "Verbrennungen Grad 3");
    lv_obj_add_style(effect4_label, &style_text_muted, 0);

    lv_obj_t * effect5_label = lv_label_create(panel_effect);
    lv_label_set_text(effect5_label, "Fettgewebsfibrosen");
    lv_obj_add_style(effect5_label, &style_text_muted, 0);

    lv_obj_t * effect6_label = lv_label_create(panel_effect);
    lv_label_set_text(effect6_label, "Krerislaufbeeintrachtigungen");
    lv_obj_add_style(effect6_label, &style_text_muted, 0);

    lv_obj_t * effect7_label = lv_label_create(panel_effect);
    lv_label_set_text(effect7_label, "Muskelzuckungen");
    lv_obj_add_style(effect7_label, &style_text_muted, 0);

    lv_obj_t * effect8_label = lv_label_create(panel_effect);
    lv_label_set_text(effect8_label, "Sensorische Irritationen");
    lv_obj_add_style(effect8_label, &style_text_muted, 0);




//////////// Create new session //////////////////////////
    lv_obj_t * panel_cerate = lv_obj_create(parent);
    lv_obj_set_height(panel_cerate, LV_SIZE_CONTENT);
//    lv_obj_set_size(panel_cerate, 600, 600);
    lv_obj_add_style(panel_cerate, &style, 0);

    LV_IMG_DECLARE(patient_s);
    lv_obj_t * img_create = lv_img_create(panel_cerate);
    lv_img_set_src(img_create, &patient_s);

    lv_obj_t * lbl_create_title = lv_label_create(panel_cerate);
    lv_label_set_text(lbl_create_title, "CRAETE NEW SESSION");
    lv_obj_add_style(lbl_create_title, &style_title, 0);

    lv_obj_t * lbl_create_sub_title = lv_label_create(panel_cerate);
    lv_label_set_text(lbl_create_sub_title, "Lungenmetastasen");
    lv_obj_add_style(lbl_create_sub_title, &style_title, 0);

//    lv_obj_t * lbl_step_title = lv_label_create(panel_cerate);
//    lv_label_set_text(lbl_step_title, "Step              Duration             Power                  cooling");
//    lv_obj_add_style(lbl_step_title, &style_title, 0);

//    lv_obj_t * lbl_minutes_title = lv_label_create(panel_cerate);
//    lv_label_set_text(lbl_minutes_title, "                                     [Minutes]                               [W]");
//    lv_obj_add_style(lbl_minutes_title, &style_text_muted, 0);

    lv_obj_t * table = lv_table_create(panel_cerate);
//    lv_obj_add_style(table, &style_title, 0);
    lv_table_set_cell_value(table, 0, 0, "Step    ");
    lv_table_set_cell_value(table, 0, 1, "Duration");
    lv_table_set_cell_value(table, 0, 2, "Power");
    lv_table_set_cell_value(table, 0, 3, "cooling");
    lv_table_set_col_width(table, 0, 160);
    lv_table_set_col_width(table, 1, 160);
    lv_table_set_col_width(table, 2, 160);
    lv_table_set_col_width(table, 3, 350);

    int i;
    for(i=1;i<7;i++){

        lv_table_set_cell_value(table, i, 0, "000");
        lv_table_set_cell_value(table, i, 1, "10 Minutes");
        lv_table_set_cell_value(table, i, 2, "10 Watt");
        lv_table_set_cell_value(table, i, 3, "*****   *****   *****   *****");
    }

    /*Set a smaller height to the table. It'll make it scrollable*/
    lv_obj_set_height(table, 350);
    lv_obj_center(table);

    /*Add an event callback to to apply some custom drawing*/
    lv_obj_add_event_cb(table, draw_part_event_cb, LV_EVENT_DRAW_PART_BEGIN, NULL);



    const char * opts_minutes = " 1 min\n 2 min\n 3 min\n 4 min\n 5 min\n 6 min\n 7 min\n 8 min\n 9 min\n10 min";

    /*A roller on the left with left aligned text, and custom width*/
    lv_obj_t * roller_minutes = lv_roller_create(panel_cerate);
    lv_roller_set_options(roller_minutes, opts_minutes, LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(roller_minutes, 2);
    lv_obj_set_width(roller_minutes, 200);
    lv_obj_add_style(roller_minutes, &style_roller, LV_PART_SELECTED);
    lv_obj_add_style(roller_minutes, &style_roller, LV_PART_MAIN);
    lv_obj_set_style_text_align(roller_minutes, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_align(roller_minutes, LV_ALIGN_LEFT_MID, 10, 0);
    lv_obj_add_event_cb(roller_minutes, roller_event_handler, LV_EVENT_ALL, NULL);
    lv_roller_set_selected(roller_minutes, 5, LV_ANIM_OFF);

    const char * opts_watt = "10 W\n20 W\n30 W\n40 W\n50 W\n60 W\n70 W\n80 W\n90 W\n100 W";

    /*A roller on the left with left aligned text, and custom width*/
    lv_obj_t * roller_watt = lv_roller_create(panel_cerate);
    lv_roller_set_options(roller_watt, opts_watt, LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(roller_watt, 2);
    lv_obj_set_width(roller_watt, 200);
    lv_obj_add_style(roller_watt, &style_roller, LV_PART_SELECTED);
    lv_obj_set_style_text_align(roller_watt, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_align(roller_watt, LV_ALIGN_LEFT_MID, 10, 0);
    lv_obj_add_event_cb(roller_watt, roller_event_handler, LV_EVENT_ALL, NULL);
    lv_roller_set_selected(roller_watt, 5, LV_ANIM_OFF);
    lv_obj_add_style(roller_watt, &style_roller, 0);

    const char * opts_cooler = "8 Cel.\n10 Cel.\n12 Cel.\n14 Cel.\n16 Cel.\n18 Cel.\n20 Cel.\n22 Cel.\n24 Cel.\n26 Cel.";

    /*A roller on the left with left aligned text, and custom width*/
    lv_obj_t * roller_cooler = lv_roller_create(panel_cerate);
    lv_roller_set_options(roller_cooler, opts_cooler, LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(roller_cooler, 2);
    lv_obj_set_width(roller_cooler, 200);
    lv_obj_add_style(roller_cooler, &style_roller, LV_PART_SELECTED);
    lv_obj_set_style_text_align(roller_cooler, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_align(roller_cooler, LV_ALIGN_LEFT_MID, 10, 0);
    lv_obj_add_event_cb(roller_cooler, roller_event_handler, LV_EVENT_ALL, NULL);
    lv_roller_set_selected(roller_cooler, 5, LV_ANIM_OFF);
    lv_obj_add_style(roller_cooler, &style_roller, 0);

    //	add Session button
    lv_obj_t * add_session_button = lv_btn_create(panel_cerate);
    lv_obj_add_style(add_session_button, &style, 0);
    lv_obj_add_event_cb(add_session_button, btn_event_cb, LV_EVENT_ALL, NULL);

    lv_obj_t * add_session_label = lv_label_create(add_session_button);
   	lv_label_set_text(add_session_label, "Add Step");
   	lv_obj_center(add_session_label);

   	lv_obj_add_style(add_session_button, &style_sel, 0);
    lv_obj_add_event_cb(add_session_button, add_step_event_cb, LV_EVENT_PRESSED, NULL);






/////////////  Create the system panel  //////////////////////////////////
    lv_obj_t * panel_system = lv_obj_create(parent);
    lv_obj_set_height(panel_system, LV_SIZE_CONTENT);
    lv_obj_add_style(panel_system, &style, 0);


    LV_IMG_DECLARE(patient_s);
    lv_obj_t * img_system = lv_img_create(panel_system);
    lv_img_set_src(img_system, &patient_s);

    lv_obj_t * lbl_system_title = lv_label_create(panel_system);
    lv_label_set_text(lbl_system_title, "SYSTEM STATUS");
    lv_obj_add_style(lbl_system_title, &style_title, 0);


    lv_obj_t * lbl_treatment_time = lv_label_create(panel_system);
    lv_label_set_text(lbl_treatment_time, "Treatment Time");
    lv_obj_add_style(lbl_treatment_time, &style_text_muted, 0);

    obj_system_time = lv_label_create(panel_system);
    lv_label_set_text(obj_system_time, "60:00");
    lv_obj_add_style(obj_system_time, &style_timer, 0);


    //	Create an Arc -> treatment time
    static lv_style_t style1;
    lv_style_init(&style1);

    obj_system_arc = lv_arc_create(panel_system);
    lv_arc_set_rotation(obj_system_arc, 270);
    lv_arc_set_bg_angles(obj_system_arc, 0, 360);
    lv_arc_set_value(obj_system_arc, 0);
    lv_obj_remove_style(obj_system_arc, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(obj_system_arc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_size(obj_system_arc, 250, 250);
    lv_style_set_arc_color(&style1, lv_palette_main(LV_PALETTE_ORANGE));
//    lv_style_set_arc_width(&style1, 40);
    lv_style_set_arc_rounded(&style1, 0);
    lv_obj_add_style(obj_system_arc, &style1, LV_PART_INDICATOR);


    lv_obj_t * lbl_treatment_status = lv_label_create(panel_system);
    lv_label_set_text(lbl_treatment_status, "Status");
    lv_obj_add_style(lbl_treatment_status, &style_text_muted, 0);


    obj_system_status = lv_label_create(panel_system);
    lv_label_set_text(obj_system_status, "Standby");
    lv_obj_add_style(obj_system_status, &style_timer, 0);

    lv_obj_t * lbl_delivered_power = lv_label_create(panel_system);
    lv_label_set_text(lbl_delivered_power, "Currently Delivered Power");
    lv_obj_add_style(lbl_delivered_power, &style_text_muted, 0);

    obj_currently_delivered_power = lv_label_create(panel_system);
    lv_label_set_text(obj_currently_delivered_power, "0 W");
    lv_obj_add_style(obj_currently_delivered_power, &style_timer, 0);

    lv_obj_t * lbl_delivered_energy = lv_label_create(panel_system);
    lv_label_set_text(lbl_delivered_energy, "Delivered Energy");
    lv_obj_add_style(lbl_delivered_energy, &style_text_muted, 0);

    obj_delivered_energy = lv_label_create(panel_system);
    lv_label_set_text(obj_delivered_energy, "0.0 kJ");
    lv_obj_add_style(obj_delivered_energy, &style_timer, 0);




	//  panel grid
	static lv_coord_t grid_main_col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
	static lv_coord_t grid_main_row_dsc[] = {420, 420, 80, LV_GRID_TEMPLATE_LAST};
	/*Create the top panel*/

	static lv_coord_t grid_1_col_dsc[] = {20, LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
	                                     //           0     1    2               3   4        5            6  7
	static lv_coord_t grid_1_row_dsc[] = {LV_GRID_CONTENT, 120, LV_GRID_CONTENT, 40, 150, LV_GRID_CONTENT, 40, 40,
             //  8            9  10    11             12
			LV_GRID_CONTENT, 40, 40, LV_GRID_CONTENT, 40 , LV_GRID_TEMPLATE_LAST};
	static lv_coord_t grid_2_col_dsc[] = {20, LV_GRID_FR(1), 20, LV_GRID_FR(1), 20, LV_GRID_FR(1), 20, LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};

	static lv_coord_t grid_2_row_dsc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT,
			LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT,
			250, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};

	static lv_coord_t grid_3_row_dsc[] = {LV_GRID_CONTENT, 10, 30, 30, 30, 30, 30, 30, 30, 30, LV_GRID_TEMPLATE_LAST};


    lv_obj_set_grid_dsc_array(parent, grid_main_col_dsc, grid_main_row_dsc);

// patient panel
    lv_obj_set_grid_cell(panel_patient, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
    lv_obj_set_grid_dsc_array(panel_patient, grid_1_col_dsc, grid_2_row_dsc);
    lv_obj_set_grid_cell(img_patient, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);

    lv_obj_set_grid_cell(lbl_patient_title, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_START, 0, 1);

    lv_obj_set_grid_cell(user_firstname_label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 2, 1);
    lv_obj_set_grid_cell(user_firstname, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 3, 1);

    lv_obj_set_grid_cell(user_surname_label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 4, 1);
    lv_obj_set_grid_cell(user_surname, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 5, 1);

    lv_obj_set_grid_cell(user_date_of_birth_label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 6, 1);
    lv_obj_set_grid_cell(user_date_of_birth, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 7, 1);

    lv_obj_set_grid_cell(user_weight_label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 8, 1);
    lv_obj_set_grid_cell(user_weight, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 9, 1);

    lv_obj_set_grid_cell(user_height_label, LV_GRID_ALIGN_START, 2, 1, LV_GRID_ALIGN_START, 8, 1);
    lv_obj_set_grid_cell(user_height, LV_GRID_ALIGN_START, 2, 1, LV_GRID_ALIGN_START, 9, 1);

    lv_obj_set_grid_cell(user_gender_label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 10, 1);
    lv_obj_set_grid_cell(user_gender, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 11, 1);

    lv_obj_set_grid_cell(user_pmcf_label, LV_GRID_ALIGN_START, 2, 1, LV_GRID_ALIGN_START, 10, 1);
    lv_obj_set_grid_cell(user_pmcf, LV_GRID_ALIGN_START, 2, 1, LV_GRID_ALIGN_START, 11, 1);




// side effect panel
    lv_obj_set_grid_cell(panel_effect, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
    lv_obj_set_grid_dsc_array(panel_effect, grid_1_col_dsc, grid_3_row_dsc);
    lv_obj_set_grid_cell(img_effect, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);

    lv_obj_set_grid_cell(lbl_effect_title, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_START, 0, 1);

    lv_obj_set_grid_cell(effect1_label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 2, 1);

    lv_obj_set_grid_cell(effect2_label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 3, 1);

    lv_obj_set_grid_cell(effect3_label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 4, 1);

    lv_obj_set_grid_cell(effect4_label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 5, 1);

    lv_obj_set_grid_cell(effect5_label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 6, 1);

    lv_obj_set_grid_cell(effect6_label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 7, 1);

    lv_obj_set_grid_cell(effect7_label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 8, 1);

    lv_obj_set_grid_cell(effect8_label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 9, 1);



// create new session
    lv_obj_set_grid_cell(panel_cerate, LV_GRID_ALIGN_STRETCH, 1, 2, LV_GRID_ALIGN_STRETCH, 0, 2);
	lv_obj_set_grid_dsc_array(panel_cerate, grid_2_col_dsc, grid_2_row_dsc);
    lv_obj_set_grid_cell(img_create, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);

    lv_obj_set_grid_cell(lbl_create_title, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_START, 0, 1);

    lv_obj_set_grid_cell(lbl_create_sub_title, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 3, 1);
//    lv_obj_set_grid_cell(lbl_step_title, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 4, 1);
//    lv_obj_set_grid_cell(lbl_minutes_title, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 5, 1);

    lv_obj_set_grid_cell(table, LV_GRID_ALIGN_STRETCH, 0, 8, LV_GRID_ALIGN_STRETCH, 6, 1);

    lv_obj_set_grid_cell(roller_minutes, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_STRETCH, 9, 3);
    lv_obj_set_grid_cell(roller_watt, LV_GRID_ALIGN_STRETCH, 2, 2, LV_GRID_ALIGN_STRETCH, 9, 3);
    lv_obj_set_grid_cell(roller_cooler, LV_GRID_ALIGN_STRETCH, 4, 2, LV_GRID_ALIGN_STRETCH, 9, 3);
    lv_obj_set_grid_cell(add_session_button, LV_GRID_ALIGN_STRETCH, 6, 2, LV_GRID_ALIGN_STRETCH, 9, 3);










	// system status
    lv_obj_set_grid_cell(panel_system, LV_GRID_ALIGN_STRETCH, 3, 1, LV_GRID_ALIGN_STRETCH, 0, 2);
	lv_obj_set_grid_dsc_array(panel_system, grid_1_col_dsc, grid_1_row_dsc);
    lv_obj_set_grid_cell(img_system, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);

    lv_obj_set_grid_cell(lbl_system_title, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_START, 0, 1);

    lv_obj_set_grid_cell(obj_system_arc, LV_GRID_ALIGN_CENTER, 0, 3, LV_GRID_ALIGN_CENTER, 3, 1);
    lv_obj_set_grid_cell(lbl_treatment_time, LV_GRID_ALIGN_CENTER, 0, 3, LV_GRID_ALIGN_CENTER, 2, 1);
    lv_obj_set_grid_cell(obj_system_time, LV_GRID_ALIGN_CENTER, 0, 3, LV_GRID_ALIGN_CENTER, 3, 1);

    lv_obj_set_grid_cell(lbl_treatment_status, LV_GRID_ALIGN_CENTER, 0, 3, LV_GRID_ALIGN_CENTER, 5, 1);
    lv_obj_set_grid_cell(obj_system_status, LV_GRID_ALIGN_CENTER, 0, 3, LV_GRID_ALIGN_CENTER, 6, 1);

    lv_obj_set_grid_cell(lbl_delivered_power, LV_GRID_ALIGN_CENTER, 0, 3, LV_GRID_ALIGN_CENTER, 8, 1);
    lv_obj_set_grid_cell(obj_currently_delivered_power, LV_GRID_ALIGN_CENTER, 0, 3, LV_GRID_ALIGN_CENTER, 9, 1);

    lv_obj_set_grid_cell(lbl_delivered_energy, LV_GRID_ALIGN_CENTER, 0, 3, LV_GRID_ALIGN_CENTER, 11, 1);
    lv_obj_set_grid_cell(obj_delivered_energy, LV_GRID_ALIGN_CENTER, 0, 3, LV_GRID_ALIGN_CENTER, 12, 1);







    downcount = 3600;
    lv_timer_t * timer = lv_timer_create(treatment_timer, 100, 0);
}


void set_vswr_angle(int s, int32_t v)
{
    float val = 0;
   // val = (3530 - 2870)/410;
   // val = (val * v) + 2870;

    switch(s){
		case 0:
			if(init_vswr){
				val = 2870 + v * 1.8;
				lv_img_set_angle(img_needle1, val);
			}
			break;
		case 1:
			if(init_vswr){
				val = 725 - v * 3.0;
				lv_img_set_angle(img_needle2, val);
			}
			break;
    }
}

static void set_angle(void * img, int32_t v)
{
    lv_img_set_angle(img, v);
}




static void treatment_timer(lv_timer_t * timer)
{
	static int step = 0;

	switch(step){
		case 0:
			lv_label_set_text_fmt(obj_system_time, "%02d:%02d", downcount/60, downcount%60);
			break;
		case 6:
			lv_label_set_text_fmt(obj_system_time, "%02d %02d", downcount/60, downcount%60);
			break;
		case 9:
			break;
		default:
			break;
	}
	step++;

	if(step > 9){
		lv_arc_set_value(obj_system_arc, downcount/36);
		step = 0;
	    downcount--;
	}

	if(downcount < 0) downcount = 3600;
}



static void patient_create(lv_obj_t * parent)
{
    lv_obj_set_flex_flow(parent, LV_FLEX_FLOW_ROW_WRAP);
    static lv_style_t style;
    static lv_style_t style_btn;
    lv_style_init(&style);
    lv_style_set_radius(&style, 3);


    /*Create a keyboard*/
    lv_obj_t * kb = lv_keyboard_create(lv_scr_act());
    lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);


//  Create the first panel -> Patient Area
    lv_obj_t * panel1 = lv_obj_create(parent);
    lv_obj_set_height(panel1, LV_SIZE_CONTENT);
    lv_obj_add_style(panel1, &style, 0);

    LV_IMG_DECLARE(patient_s);
    lv_obj_t * img_patient = lv_img_create(panel1);
    lv_img_set_src(img_patient, &patient_s);

    lv_obj_t * panel1_title = lv_label_create(panel1);
    lv_label_set_text(panel1_title, "PATIENT DATA");
    lv_obj_add_style(panel1_title, &style_title, 0);

    lv_obj_t * user_firstname_label = lv_label_create(panel1);
    lv_label_set_text(user_firstname_label, "Firstname");
    lv_obj_add_style(user_firstname_label, &style_text_muted, 0);
    lv_obj_t * user_firstname = lv_textarea_create(panel1);
    lv_textarea_set_one_line(user_firstname, true);
    lv_textarea_set_placeholder_text(user_firstname, "Your firstname");
    lv_obj_add_event_cb(user_firstname, ta_event_cb, LV_EVENT_ALL, kb);
    lv_obj_add_style(user_firstname, &style, 0);

    lv_obj_t * user_surname_label = lv_label_create(panel1);
    lv_label_set_text(user_surname_label, "Surtname");
    lv_obj_add_style(user_surname_label, &style_text_muted, 0);
    lv_obj_t * user_surname = lv_textarea_create(panel1);
    lv_textarea_set_one_line(user_surname, true);
    lv_textarea_set_placeholder_text(user_surname, "Your surname");
    lv_obj_add_event_cb(user_surname, ta_event_cb, LV_EVENT_ALL, kb);
    lv_obj_add_style(user_surname, &style, 0);

    lv_obj_t * birthday_label = lv_label_create(panel1);
    lv_label_set_text(birthday_label, "Date of Birth");
    lv_obj_add_style(birthday_label, &style_text_muted, 0);
    lv_obj_t * birthdate = lv_textarea_create(panel1);
    lv_textarea_set_one_line(birthdate, true);
    lv_obj_add_event_cb(birthdate, birthday_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_style(birthdate, &style, 0);

    lv_obj_t * weight_label = lv_label_create(panel1);
    lv_label_set_text(weight_label, "Weight");
    lv_obj_add_style(weight_label, &style_text_muted, 0);
    lv_obj_t * weight = lv_textarea_create(panel1);
    lv_textarea_set_one_line(weight, true);
    lv_obj_add_event_cb(weight, ta_event_cb, LV_EVENT_ALL, kb);
    lv_obj_add_style(weight, &style, 0);

    lv_obj_t * height_label = lv_label_create(panel1);
    lv_label_set_text(height_label, "Height");
    lv_obj_add_style(height_label, &style_text_muted, 0);
    lv_obj_t * height = lv_textarea_create(panel1);
    lv_textarea_set_one_line(height, true);
    lv_obj_add_event_cb(height, ta_event_cb, LV_EVENT_ALL, kb);
    lv_obj_add_style(height, &style, 0);

    lv_obj_t * gender_label = lv_label_create(panel1);
    lv_label_set_text(gender_label, "Gender");
    lv_obj_add_style(gender_label, &style_text_muted, 0);
    lv_obj_t * gender = lv_dropdown_create(panel1);
    lv_dropdown_set_options_static(gender, "Male\nFemale\nOther");
    lv_obj_add_style(gender, &style, 0);

    lv_obj_t * pmcf_label = lv_label_create(panel1);
    lv_label_set_text(pmcf_label, "PMCF");
    lv_obj_add_style(pmcf_label, &style_text_muted, 0);

//    lv_obj_t * pmcf = lv_switch_create(panel1);
//    lv_obj_add_state(pmcf, LV_STATE_CHECKED);
    lv_obj_t * pmcf = lv_checkbox_create(panel1);
    lv_checkbox_set_text(pmcf, " PMCF");
    lv_obj_add_state(pmcf, LV_STATE_CHECKED);
//    lv_obj_add_style(pmcf, &style_title, 0);


//  Create the second panel
    lv_obj_t * panel2 = lv_obj_create(parent);
    lv_obj_set_height(panel2, LV_SIZE_CONTENT);
    lv_obj_add_style(panel2, &style, 0);

    LV_IMG_DECLARE(patient_s);
    lv_obj_t * img_diagnostayic = lv_img_create(panel2);
    lv_img_set_src(img_diagnostayic, &patient_s);

    lv_obj_t * panel2_title = lv_label_create(panel2);
    lv_label_set_text(panel2_title, "DIAGNOSTIC DATA");
    lv_obj_add_style(panel2_title, &style_title, 0);

    lv_obj_t * tumor_label = lv_label_create(panel2);
    lv_label_set_text(tumor_label, "Tumor");
    lv_obj_add_style(tumor_label, &style_text_muted, 0);
    lv_obj_t * tumor = lv_dropdown_create(panel2);
    lv_dropdown_set_options_static(tumor, "Lorem lpsum\nOther");
    lv_obj_add_style(tumor, &style, 0);

    lv_obj_t * node_label = lv_label_create(panel2);
    lv_label_set_text(node_label, "Node");
    lv_obj_add_style(node_label, &style_text_muted, 0);
    lv_obj_t * node = lv_dropdown_create(panel2);
    lv_dropdown_set_options_static(node, "NO - kein Lymphknotendefall\nOther");
    lv_obj_add_style(node, &style, 0);

    lv_obj_t * metastasis_label = lv_label_create(panel2);
    lv_label_set_text(metastasis_label, "Metastasis");
    lv_obj_add_style(metastasis_label, &style_text_muted, 0);
    lv_obj_t * metastasis = lv_dropdown_create(panel2);
    lv_dropdown_set_options_static(metastasis, "Lorem lpsum\nOther");
    lv_obj_add_style(metastasis, &style, 0);

    lv_obj_t * karnofsky_label = lv_label_create(panel2);
    lv_label_set_text(karnofsky_label, "Karnofsky Index");
    lv_obj_add_style(karnofsky_label, &style_text_muted, 0);
    lv_obj_t * karnofsky = lv_dropdown_create(panel2);
    lv_dropdown_set_options_static(karnofsky, "100%\n90%\n80%\n70%\n60%\n50%\n40%\n30%\n20%\n10%\n0%");
    lv_obj_add_style(karnofsky, &style, 0);

    lv_obj_t * notes_label = lv_label_create(panel2);
    lv_label_set_text(notes_label, "Notes");
    lv_obj_add_style(notes_label, &style_text_muted, 0);

    lv_obj_t * notes = lv_textarea_create(panel2);
//    lv_textarea_set_one_line(notes, true);
    lv_obj_add_event_cb(notes, ta_event_cb, LV_EVENT_ALL, kb);
    lv_obj_add_style(notes, &style, 0);







//  Create the third panel
    lv_obj_t * panel3 = lv_obj_create(parent);
    lv_obj_set_height(panel3, LV_SIZE_CONTENT);
    lv_obj_add_style(panel3, &style, 0);

    LV_IMG_DECLARE(patient_s);
    lv_obj_t * img_treatment = lv_img_create(panel3);
    lv_img_set_src(img_treatment, &patient_s);

    lv_obj_t * panel3_title = lv_label_create(panel3);
    lv_label_set_text(panel3_title, "TREATMENT DATA");
    lv_obj_add_style(panel3_title, &style_title, 0);

    lv_obj_t * start_of_therapy_label = lv_label_create(panel3);
    lv_label_set_text(start_of_therapy_label, "Start of Therapy");
    lv_obj_add_style(start_of_therapy_label, &style_text_muted, 0);
    lv_obj_t * start_of_therapy = lv_dropdown_create(panel3);
    lv_dropdown_set_options_static(start_of_therapy, "26.02.2018\nOther");
    lv_obj_add_style(start_of_therapy, &style, 0);

    lv_obj_t * additional_treatment_label = lv_label_create(panel3);
    lv_label_set_text(additional_treatment_label, "Additional Treatment");
    lv_obj_add_style(additional_treatment_label, &style_text_muted, 0);

    lv_obj_t * surgery = lv_checkbox_create(panel3);
    lv_checkbox_set_text(surgery, " Surgery");
    lv_obj_add_state(surgery, LV_STATE_CHECKED);

    lv_obj_t * compltherapy = lv_checkbox_create(panel3);
    lv_checkbox_set_text(compltherapy, " Compl. Therapy");
//    lv_obj_add_state(compltherapy, LV_STATE_CHECKED);

    lv_obj_t * radiotherapy = lv_checkbox_create(panel3);
    lv_checkbox_set_text(radiotherapy, " Ridiotherapy");
//    lv_obj_add_state(radiotherapy, LV_STATE_CHECKED);

    lv_obj_t * chemotherapy = lv_checkbox_create(panel3);
    lv_checkbox_set_text(chemotherapy, " Chemotherapy");
//    lv_obj_add_state(chemotherapy, LV_STATE_CHECKED);

    lv_obj_t * freetext = lv_textarea_create(panel3);
    lv_textarea_set_one_line(freetext, true);
    lv_obj_add_event_cb(freetext, ta_event_cb, LV_EVENT_ALL, kb);
    lv_textarea_set_placeholder_text(freetext, "Free Text Therapy");
    lv_obj_add_style(freetext, &style, 0);

    lv_obj_t * lorem_ipsum_label = lv_label_create(panel3);
    lv_label_set_text(lorem_ipsum_label, "Lorem Ipsum");
    lv_obj_add_style(lorem_ipsum_label, &style_text_muted, 0);

    lv_obj_t * primary_tumor = lv_checkbox_create(panel3);
    lv_checkbox_set_text(primary_tumor, " Primary Tumor");
    lv_obj_add_state(primary_tumor, LV_STATE_CHECKED);

    lv_obj_t * metastsasis_check = lv_checkbox_create(panel3);
    lv_checkbox_set_text(metastsasis_check, " Metastasis");

    lv_obj_t * session_history_label = lv_label_create(panel3);
    lv_label_set_text(session_history_label, "Session History");
    lv_obj_add_style(session_history_label, &style_text_muted, 0);

    lv_obj_t * table = lv_table_create(panel3);
    lv_table_set_cell_value(table, 0, 0, "Number");
    lv_table_set_cell_value(table, 0, 1, "Date");
    lv_table_set_cell_value(table, 0, 2, "Duration");
    lv_table_set_cell_value(table, 0, 3, "Total Power");
    lv_table_set_cell_value(table, 0, 4, "Peak Power");
    lv_table_set_cell_value(table, 0, 5, "Side Effects");
    lv_table_set_col_width(table, 0, 130);
    lv_table_set_col_width(table, 1, 130);
    lv_table_set_col_width(table, 2, 130);
    lv_table_set_col_width(table, 3, 150);
    lv_table_set_col_width(table, 4, 150);
    lv_table_set_col_width(table, 5, 150);

    int i;
    for(i=1;i<20;i++){

        lv_table_set_cell_value(table, i, 0, "000");
        lv_table_set_cell_value(table, i, 1, "00.00.00");
        lv_table_set_cell_value(table, i, 2, "123 min");
        lv_table_set_cell_value(table, i, 3, "123 kjoule");
        lv_table_set_cell_value(table, i, 4, "165 kjoule");
        lv_table_set_cell_value(table, i, 5, "-----");
    }

    /*Set a smaller height to the table. It'll make it scrollable*/
    lv_obj_set_height(table, 480);
    lv_obj_center(table);

    /*Add an event callback to to apply some custom drawing*/
    lv_obj_add_event_cb(table, draw_part_event_cb, LV_EVENT_DRAW_PART_BEGIN, NULL);



//  panel grid
    static lv_coord_t grid_main_col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static lv_coord_t grid_main_row_dsc[] = {420, 420, 80, LV_GRID_TEMPLATE_LAST};
    /*Create the top panel*/

    static lv_coord_t grid_1_col_dsc[] = {20, LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static lv_coord_t grid_1_row_dsc[] = {LV_GRID_CONTENT, 20, LV_GRID_CONTENT, 40, 15, LV_GRID_CONTENT, 40, 15,
    		LV_GRID_CONTENT, 40, 15, LV_GRID_CONTENT, 40, 15, LV_GRID_CONTENT, 40, 15, LV_GRID_CONTENT, 40, 15,
			LV_GRID_CONTENT, 40, LV_GRID_TEMPLATE_LAST};

    static lv_coord_t grid_2_row_dsc[] = {LV_GRID_CONTENT, 20, LV_GRID_CONTENT, 40, 15, LV_GRID_CONTENT, 40, 15,
    		LV_GRID_CONTENT, 40, 15, LV_GRID_CONTENT, 40, 15, LV_GRID_CONTENT, 40, 25, LV_GRID_TEMPLATE_LAST};

    static lv_coord_t grid_3_row_dsc[] = {LV_GRID_CONTENT, 20, LV_GRID_CONTENT, 40, 0, LV_GRID_CONTENT, 40, 15,
    		LV_GRID_CONTENT, 0, 15, LV_GRID_CONTENT, 40, 15, LV_GRID_CONTENT, 40, 25, LV_GRID_TEMPLATE_LAST};

    lv_obj_set_grid_dsc_array(parent, grid_main_col_dsc, grid_main_row_dsc);

    lv_obj_set_grid_cell(panel1, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 2);
    lv_obj_set_grid_dsc_array(panel1, grid_1_col_dsc, grid_1_row_dsc);
    lv_obj_set_grid_cell(img_patient, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);

    lv_obj_set_grid_cell(panel1_title, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_START, 0, 1);

    lv_obj_set_grid_cell(user_firstname, LV_GRID_ALIGN_STRETCH, 0, 5, LV_GRID_ALIGN_CENTER, 3, 1);
    lv_obj_set_grid_cell(user_firstname_label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 2, 1);

    lv_obj_set_grid_cell(user_surname, LV_GRID_ALIGN_STRETCH, 0, 5, LV_GRID_ALIGN_CENTER, 6, 1);
    lv_obj_set_grid_cell(user_surname_label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 5, 1);

    lv_obj_set_grid_cell(birthdate, LV_GRID_ALIGN_STRETCH, 0, 3, LV_GRID_ALIGN_CENTER, 9, 1);
    lv_obj_set_grid_cell(birthday_label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 8, 1);

    lv_obj_set_grid_cell(weight, LV_GRID_ALIGN_STRETCH, 0, 3, LV_GRID_ALIGN_CENTER, 12, 1);
    lv_obj_set_grid_cell(weight_label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 11, 1);

    lv_obj_set_grid_cell(height, LV_GRID_ALIGN_STRETCH, 0, 3, LV_GRID_ALIGN_CENTER, 15, 1);
    lv_obj_set_grid_cell(height_label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 14, 1);

    lv_obj_set_grid_cell(gender, LV_GRID_ALIGN_STRETCH, 0, 3, LV_GRID_ALIGN_CENTER, 18, 1);
    lv_obj_set_grid_cell(gender_label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 17, 1);

    lv_obj_set_grid_cell(pmcf, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_CENTER, 21, 1);
    lv_obj_set_grid_cell(pmcf_label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 20, 1);

// second
    lv_obj_set_grid_cell(panel2, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 0, 2);
    lv_obj_set_grid_dsc_array(panel2, grid_1_col_dsc, grid_2_row_dsc);
    lv_obj_set_grid_cell(img_diagnostayic, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);

    lv_obj_set_grid_cell(panel2_title, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_START, 0, 1);

    lv_obj_set_grid_cell(tumor, LV_GRID_ALIGN_STRETCH, 0, 5, LV_GRID_ALIGN_CENTER, 3, 1);
    lv_obj_set_grid_cell(tumor_label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 2, 1);

    lv_obj_set_grid_cell(node, LV_GRID_ALIGN_STRETCH, 0, 5, LV_GRID_ALIGN_CENTER, 6, 1);
    lv_obj_set_grid_cell(node_label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 5, 1);

    lv_obj_set_grid_cell(metastasis, LV_GRID_ALIGN_STRETCH, 0, 5, LV_GRID_ALIGN_CENTER, 9, 1);
    lv_obj_set_grid_cell(metastasis_label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 8, 1);

    lv_obj_set_grid_cell(karnofsky, LV_GRID_ALIGN_STRETCH, 0, 3, LV_GRID_ALIGN_CENTER, 12, 1);
    lv_obj_set_grid_cell(karnofsky_label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 11, 1);

    lv_obj_set_grid_cell(notes, LV_GRID_ALIGN_STRETCH, 0, 5, LV_GRID_ALIGN_CENTER, 16, 1);
    lv_obj_set_grid_cell(notes_label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 14, 1);




// third
    lv_obj_set_grid_cell(panel3, LV_GRID_ALIGN_STRETCH, 2, 2, LV_GRID_ALIGN_STRETCH, 0, 2);
    lv_obj_set_grid_dsc_array(panel3, grid_1_col_dsc, grid_3_row_dsc);
    lv_obj_set_grid_cell(img_treatment, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_CENTER, 0, 1);

    lv_obj_set_grid_cell(panel3_title, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_START, 0, 1);

    lv_obj_set_grid_cell(start_of_therapy, LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_CENTER, 3, 1);
    lv_obj_set_grid_cell(start_of_therapy_label, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 2, 1);

    lv_obj_set_grid_cell(additional_treatment_label, LV_GRID_ALIGN_START, 2, 1, LV_GRID_ALIGN_START, 2, 1);
    lv_obj_set_grid_cell(surgery, LV_GRID_ALIGN_STRETCH, 2, 1, LV_GRID_ALIGN_CENTER, 3, 1);
    lv_obj_set_grid_cell(compltherapy, LV_GRID_ALIGN_STRETCH, 3, 1, LV_GRID_ALIGN_CENTER, 3, 1);
    lv_obj_set_grid_cell(radiotherapy, LV_GRID_ALIGN_STRETCH, 4, 1, LV_GRID_ALIGN_CENTER, 3, 1);

    lv_obj_set_grid_cell(chemotherapy, LV_GRID_ALIGN_STRETCH, 2, 1, LV_GRID_ALIGN_CENTER, 5, 1);
    lv_obj_set_grid_cell(freetext, LV_GRID_ALIGN_STRETCH, 3, 2, LV_GRID_ALIGN_CENTER, 5, 1);

    lv_obj_set_grid_cell(lorem_ipsum_label, LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_CENTER, 6, 1);
    lv_obj_set_grid_cell(primary_tumor, LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_CENTER, 7, 1);
    lv_obj_set_grid_cell(metastsasis_check, LV_GRID_ALIGN_STRETCH, 2, 2, LV_GRID_ALIGN_CENTER, 7, 1);

    lv_obj_set_grid_cell(session_history_label, LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_START, 10, 1);
    lv_obj_set_grid_cell(table, LV_GRID_ALIGN_START, 0, 2, LV_GRID_ALIGN_START, 11, 1);



    //	Save button
    lv_obj_t * save_button = lv_btn_create(parent);
    lv_obj_add_style(save_button, &style_sel, 0);
    lv_obj_set_grid_cell(save_button, LV_GRID_ALIGN_STRETCH, 2, 1, LV_GRID_ALIGN_STRETCH, 2, 1);
    lv_obj_add_event_cb(save_button, btn_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_style(save_button, &style, 0);

    lv_obj_t * savel_label = lv_label_create(save_button);
   	lv_label_set_text(savel_label, "Save");
   	lv_obj_center(savel_label);

    //	New Session button
    lv_obj_t * new_button = lv_btn_create(parent);
    lv_obj_add_style(new_button, &style_sel, 0);
    lv_obj_set_grid_cell(new_button, LV_GRID_ALIGN_STRETCH, 3, 1, LV_GRID_ALIGN_STRETCH, 2, 1);
    lv_obj_add_event_cb(new_button, btn_event_cb, LV_EVENT_ALL, NULL);
    lv_obj_add_style(new_button, &style, 0);

    lv_obj_t * new_label = lv_label_create(new_button);
   	lv_label_set_text(new_label, "New Session");
   	lv_obj_center(new_label);

}


static void btn_event_cb(lv_event_t * e)
{
//    lv_event_code_t code = lv_event_get_code(e);
//    lv_obj_t * btn = lv_event_get_target(e);
//    if(code == LV_EVENT_CLICKED) {
//        static uint8_t cnt = 0;
//        cnt++;

        /*Get the first child of the button which is the label and change its text*/
//        lv_obj_t * label = lv_obj_get_child(btn, 0);
//        lv_label_set_text_fmt(label, "Button: %d", cnt);
//    }
}



static void ta_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    lv_obj_t * kb = lv_event_get_user_data(e);
    if(code == LV_EVENT_FOCUSED) {
        if(lv_indev_get_type(lv_indev_get_act()) != LV_INDEV_TYPE_KEYPAD) {
            lv_keyboard_set_textarea(kb, ta);
            lv_obj_set_style_max_height(kb, LV_HOR_RES * 2 / 3, 0);
            lv_obj_update_layout(tv);   /*Be sure the sizes are recalculated*/
            lv_obj_set_height(tv, LV_VER_RES - lv_obj_get_height(kb));
            lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
            lv_obj_scroll_to_view_recursive(ta, LV_ANIM_OFF);
        }
    }
    else if(code == LV_EVENT_DEFOCUSED) {
        lv_keyboard_set_textarea(kb, NULL);
        lv_obj_set_height(tv, LV_VER_RES);
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
    }
    else if(code == LV_EVENT_READY || code == LV_EVENT_CANCEL) {
        lv_obj_set_height(tv, LV_VER_RES);
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_state(ta, LV_STATE_FOCUSED);
        lv_indev_reset(NULL, ta);   /*To forget the last clicked object to make it focusable again*/
    }
}

static void calendar_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_user_data(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_VALUE_CHANGED) {
        lv_calendar_date_t d;
        lv_calendar_get_pressed_date(obj, &d);
        char buf[32];
        lv_snprintf(buf, sizeof(buf), "%02d.%02d.%d", d.day, d.month, d.year);
        lv_textarea_set_text(ta, buf);

        lv_obj_del(calendar);
        lv_obj_del(calendar_header);
        calendar = NULL;
        calendar_header = NULL;
        lv_obj_clear_flag(lv_layer_top(), LV_OBJ_FLAG_CLICKABLE);
        lv_obj_set_style_bg_opa(lv_layer_top(), LV_OPA_TRANSP, 0);
    }
}


static void birthday_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);

    if(code == LV_EVENT_FOCUSED) {
        if(lv_indev_get_type(lv_indev_get_act()) == LV_INDEV_TYPE_POINTER) {
            if(calendar == NULL) {
                lv_obj_add_flag(lv_layer_top(), LV_OBJ_FLAG_CLICKABLE);
                calendar = lv_calendar_create(lv_layer_top());
                lv_obj_set_style_bg_opa(lv_layer_top(), LV_OPA_50, 0);
                lv_obj_set_style_bg_color(lv_layer_top(), lv_palette_main(LV_PALETTE_GREY), 0);
                lv_obj_set_size(calendar, 800, 800);
                lv_calendar_set_showed_date(calendar, 1990, 01);
                lv_obj_align(calendar, LV_ALIGN_CENTER, 0, 30);
                lv_obj_add_event_cb(calendar, calendar_event_cb, LV_EVENT_ALL, ta);

                calendar_header = lv_calendar_header_dropdown_create(lv_layer_top(), calendar);
            }
        }
    }
}

static void draw_part_event_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    lv_obj_draw_part_dsc_t * dsc = lv_event_get_param(e);
    /*If the cells are drawn...*/
    if(dsc->part == LV_PART_ITEMS) {
        uint32_t row = dsc->id /  lv_table_get_col_cnt(obj);
        uint32_t col = dsc->id - row * lv_table_get_col_cnt(obj);

        /*Make the texts in the first cell center aligned*/
        if(row == 0) {
            dsc->label_dsc->align = LV_TEXT_ALIGN_CENTER;
            dsc->rect_dsc->bg_color = lv_color_mix(lv_palette_main(LV_PALETTE_BLUE), dsc->rect_dsc->bg_color, LV_OPA_20);
            dsc->rect_dsc->bg_opa = LV_OPA_COVER;
        }
        /*In the first column align the texts to the right*/
        else if(col == 0) {
            dsc->label_dsc->flag = LV_TEXT_ALIGN_RIGHT;
        }

        /*MAke every 2nd row grayish*/
        if((row != 0 && row % 2) == 0) {
            dsc->rect_dsc->bg_color = lv_color_mix(lv_palette_main(LV_PALETTE_GREY), dsc->rect_dsc->bg_color, LV_OPA_10);
            dsc->rect_dsc->bg_opa = LV_OPA_COVER;
        }
    }
}

static void passwd_btn_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * btn = lv_event_get_target(e);
    if(code == LV_EVENT_CLICKED) {
        /*Get the first child of the button which is the label and change its text*/
        lv_obj_t * label = lv_obj_get_child(btn, 0);
        lv_label_set_text(label, "COLLECT");
        lv_obj_add_flag(passwd, LV_OBJ_FLAG_HIDDEN);
    }
}


static void passwd_panel_create(lv_obj_t * parent)
{
    passwd = lv_obj_create(parent);
    lv_obj_set_size(passwd, 1920, 1080);
    lv_obj_set_align(passwd, LV_ALIGN_CENTER);

//    lv_obj_remove_style_all(panel);
//    lv_obj_set_size(kb, 400, 200);
    lv_obj_t * panel = lv_obj_create(passwd);
    lv_obj_set_size(panel, 450, 800);
//    lv_obj_set_align(panel, LV_ALIGN_CENTER);

    lv_obj_set_flex_flow(passwd, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(passwd, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_radius(&style, 3);
    lv_obj_add_style(panel, &style, 0);

    /*Create a keyboard*/
    lv_obj_t * kb = lv_keyboard_create(passwd);
    lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);


	LV_IMG_DECLARE(img_login_main_b);
	lv_obj_t * image_login_main = lv_img_create(panel);
	lv_img_set_src(image_login_main, &img_login_main_b);

	LV_IMG_DECLARE(img_account);
	lv_obj_t * image_account = lv_img_create(panel);
	lv_img_set_src(image_account, &img_account);
	lv_img_set_zoom(image_account, 110);
//	lv_obj_set_pos(image_account, 100,200);

//	lv_obj_t * lbl_patient_title = lv_label_create(panel);
//	lv_label_set_text(lbl_patient_title, "PATIENT DATA");
//	lv_obj_add_style(lbl_patient_title, &style_title, 0);

	lv_obj_t* lbl_account = lv_label_create(panel);
    lv_label_set_text(lbl_account, "Account");
    lv_obj_add_style(lbl_account, &style_text_muted, 0);

    lv_obj_t * user_name = lv_textarea_create(panel);
    lv_textarea_set_one_line(user_name, true);
    lv_textarea_set_placeholder_text(user_name, "ADMIN");
    lv_obj_add_event_cb(user_name, ta_event_cb, LV_EVENT_ALL, kb);
    lv_obj_add_style(user_name, &style, 0);

	LV_IMG_DECLARE(img_passwd);
	lv_obj_t * image_passwd = lv_img_create(panel);
	lv_img_set_src(image_passwd, &img_passwd);
	lv_img_set_zoom(image_passwd, 110);

	lv_obj_t* lbl_passwd = lv_label_create(panel);
    lv_label_set_text(lbl_passwd, "Password");
    lv_obj_add_style(lbl_passwd, &style_text_muted, 0);

    lv_obj_t * password = lv_textarea_create(panel);
    lv_textarea_set_one_line(password, true);
    lv_textarea_set_password_mode(password, true);
    lv_textarea_set_placeholder_text(password, "Min. 8 chars.");
    lv_obj_add_event_cb(password, ta_event_cb, LV_EVENT_ALL, kb);
    lv_obj_add_style(password, &style, 0);

    //	login button
    lv_obj_t * btn_login = lv_btn_create(panel);
    lv_obj_add_event_cb(btn_login, passwd_btn_event_cb, LV_EVENT_ALL, NULL);

    lv_obj_t * btn_lbl_login = lv_label_create(btn_login);
   	lv_label_set_text(btn_lbl_login, "Login");
   	lv_obj_center(btn_lbl_login);
    lv_obj_add_style(btn_login, &style, 0);
    lv_obj_add_style(btn_login, &style_sel, 0);

	lv_obj_t* lbl_forget_passwd = lv_label_create(panel);
    lv_label_set_text(lbl_forget_passwd, "Forgot Password?");
    lv_obj_add_style(lbl_forget_passwd, &style_text_muted, 0);



	static lv_coord_t grid_main_col_dsc[] = {40, 40, LV_GRID_FR(1), LV_GRID_FR(1), 40, LV_GRID_TEMPLATE_LAST};
	static lv_coord_t grid_main_row_dsc[] = {200, 60, 20, LV_GRID_CONTENT, 10, 20, LV_GRID_CONTENT, 80, 70, 40, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(panel, grid_main_col_dsc, grid_main_row_dsc);
    lv_obj_set_grid_cell(image_login_main, LV_GRID_ALIGN_CENTER, 0, 5, LV_GRID_ALIGN_END, 0, 1);
    lv_obj_set_grid_cell(image_account, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 2, 1);
    lv_obj_set_grid_cell(lbl_account, LV_GRID_ALIGN_START, 2, 1, LV_GRID_ALIGN_CENTER, 2, 1);
    lv_obj_set_grid_cell(user_name, LV_GRID_ALIGN_STRETCH, 1, 3, LV_GRID_ALIGN_STRETCH, 3, 1);

    lv_obj_set_grid_cell(image_passwd, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 5, 1);
    lv_obj_set_grid_cell(lbl_passwd, LV_GRID_ALIGN_START, 2, 1, LV_GRID_ALIGN_CENTER, 5, 1);
    lv_obj_set_grid_cell(password, LV_GRID_ALIGN_STRETCH, 1, 3, LV_GRID_ALIGN_STRETCH, 6, 1);

    lv_obj_set_grid_cell(btn_login, LV_GRID_ALIGN_STRETCH, 1, 3, LV_GRID_ALIGN_STRETCH, 8, 1);
    lv_obj_set_grid_cell(lbl_forget_passwd, LV_GRID_ALIGN_CENTER, 0, 5, LV_GRID_ALIGN_CENTER, 9, 1);

}

