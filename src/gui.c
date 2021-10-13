#include "gui.h"

#include <math.h>

static const lv_coord_t ecg_sample[] = {
    -2, 2, 0, -15, -39, -63, -71, -68, -67, -69, -84, -95, -104, -107, -108, -107, -107, -107, -107, -114, -118, -117,
    -112, -100, -89, -83, -71, -64, -58, -58, -62, -62, -58, -51, -46, -39, -27, -10, 4, 7, 1, -3, 0, 14, 24, 30, 25, 19,
    13, 7, 12, 15, 18, 21, 13, 6, 9, 8, 17, 19, 13, 11, 11, 11, 23, 30, 37, 34, 25, 14, 15, 19, 28, 31, 26, 23, 25, 31,
    39, 37, 37, 34, 30, 32, 22, 29, 31, 33, 37, 23, 13, 7, 2, 4, -2, 2, 11, 22, 33, 19, -1, -27, -55, -67, -72, -71, -63,
    -49, -18, 35, 113, 230, 369, 525, 651, 722, 730, 667, 563, 454, 357, 305, 288, 274, 255, 212, 173, 143, 117, 82, 39,
    -13, -53, -78, -91, -101, -113, -124, -131, -131, -131, -129, -128, -129, -125, -123, -123, -129, -139, -148, -153,
    -159, -166, -183, -205, -227, -243, -248, -246, -254, -280, -327, -381, -429, -473, -517, -556, -592, -612, -620,
    -620, -614, -604, -591, -574, -540, -497, -441, -389, -358, -336, -313, -284, -222, -167, -114, -70, -47, -28, -4, 12,
    38, 52, 58, 56, 56, 57, 68, 77, 86, 86, 80, 69, 67, 70, 82, 85, 89, 90, 89, 89, 88, 91, 96, 97, 91, 83, 78, 82, 88, 95,
    96, 105, 106, 110, 102, 100, 96, 98, 97, 101, 98, 99, 100, 107, 113, 119, 115, 110, 96, 85, 73, 64, 69, 76, 79,
    78, 75, 85, 100, 114, 113, 105, 96, 84, 74, 66, 60, 75, 85, 89, 83, 67, 61, 67, 73, 79, 74, 63, 57, 56, 58, 61, 55,
    48, 45, 46, 55, 62, 55, 49, 43, 50, 59, 63, 57, 40, 31, 23, 25, 27, 31, 35, 34, 30, 36, 34, 42, 38, 36, 40, 46, 50,
    47, 32, 30, 32, 52, 67, 73, 71, 63, 54, 53, 45, 41, 28, 13, 3, 1, 4, 4, -8, -23, -32, -31, -19, -5, 3, 9, 13, 19,
    24, 27, 29, 25, 22, 26, 32, 42, 51, 56, 60, 57, 55, 53, 53, 54, 59, 54, 49, 26, -3, -11, -20, -47, -100, -194, -236,
    -212, -123, 8, 103, 142, 147, 120, 105, 98, 93, 81, 61, 40, 26, 28, 30, 30, 27, 19, 17, 21, 20, 19, 19, 22, 36, 40,
    35, 20, 7, 1, 10, 18, 27, 22, 6, -4, -2, 3, 6, -2, -13, -14, -10, -2, 3, 2, -1, -5, -10, -19, -32, -42, -55, -60,
    -68, -77, -86, -101, -110, -117, -115, -104, -92, -84, -85, -84, -73, -65, -52, -50, -45, -35, -20, -3, 12, 20, 25,
    26, 28, 28, 30, 28, 25, 28, 33, 42, 42, 36, 23, 9, 0, 1, -4, 1, -4, -4, 1, 5, 9, 9, -3, -1, -18, -50, -108, -190,
    -272, -340, -408, -446, -537, -643, -777, -894, -920, -853, -697, -461, -251, -60, 58, 103, 129, 139, 155, 170, 173,
    178, 185, 190, 193, 200, 208, 215, 225, 224, 232, 234, 240, 240, 236, 229, 226, 224, 232, 233, 232, 224, 219, 219,
    223, 231, 226, 223, 219, 218, 223, 223, 223, 233, 245, 268, 286, 296, 295, 283, 271, 263, 252, 243, 226, 210, 197,
    186, 171, 152, 133, 117, 114, 110, 107, 96, 80, 63, 48, 40, 38, 34, 28, 15, 2, -7, -11, -14, -18, -29, -37, -44, -50,
    -58, -63, -61, -52, -50, -48, -61, -59, -58, -54, -47, -52, -62, -61, -64, -54, -52, -59, -69, -76, -76, -69, -67,
    -74, -78, -81, -80, -73, -65, -57, -53, -51, -47, -35, -27, -22, -22, -24, -21, -17, -13, -10, -11, -13, -20, -20,
    -12, -2, 7, -1, -12, -16, -13, -2, 2, -4, -5, -2, 9, 19, 19, 14, 11, 13, 19, 21, 20, 18, 19, 19, 19, 16, 15, 13, 14,
    9, 3, -5, -9, -5, -3, -2, -3, -3, 2, 8, 9, 9, 5, 6, 8, 8, 7, 4, 3, 4, 5, 3, 5, 5, 13, 13, 12, 10, 10, 15, 22, 17,
    14, 7, 10, 15, 16, 11, 12, 10, 13, 9, -2, -4, -2, 7, 16, 16, 17, 16, 7, -1, -16, -18, -16, -9, -4, -5, -10, -9, -8,
    -3, -4, -10, -19, -20, -16, -9, -9, -23, -40, -48, -43, -33, -19, -21, -26, -31, -33, -19, 0, 17, 24, 9, -17, -47,
    -63, -67, -59, -52, -51, -50, -49, -42, -26, -21, -15, -20, -23, -22, -19, -12, -8, 5, 18, 27, 32, 26, 25, 26, 22,
    23, 17, 14, 17, 21, 25, 2, -45, -121, -196, -226, -200, -118, -9, 73, 126, 131, 114, 87, 60, 42, 29, 26, 34, 35, 34,
    25, 12, 9, 7, 3, 2, -8, -11, 2, 23, 38, 41, 23, 9, 10, 13, 16, 8, -8, -17, -23, -26, -25, -21, -15, -10, -13, -13,
    -19, -22, -29, -40, -48, -48, -54, -55, -66, -82, -85, -90, -92, -98, -114, -119, -124, -129, -132, -146, -146, -138,
    -124, -99, -85, -72, -65, -65, -65, -66, -63, -64, -64, -58, -46, -26, -9, 2, 2, 4, 0, 1, 4, 3, 10, 11, 10, 2, -4,
    0, 10, 18, 20, 6, 2, -9, -7, -3, -3, -2, -7, -12, -5, 5, 24, 36, 31, 25, 6, 3, 7, 12, 17, 11, 0, -6, -9, -8, -7, -5,
    -6, -2, -2, -6, -2, 2, 14, 24, 22, 15, 8, 4, 6, 7, 12, 16, 25, 20, 7, -16, -41, -60, -67, -65, -54, -35, -11, 30,
    84, 175, 302, 455, 603, 707, 743, 714, 625, 519, 414, 337, 300, 281, 263, 239, 197, 163, 136, 109, 77, 34, -18, -50,
    -66, -74, -79, -92, -107, -117, -127, -129, -135, -139, -141, -155, -159, -167, -171, -169, -174, -175, -178, -191,
    -202, -223, -235, -243, -237, -240, -256, -298, -345, -393, -432, -475, -518, -565, -596, -619, -623, -623, -614,
    -599, -583, -559, -524, -477, -425, -383, -357, -331, -301, -252, -198, -143, -96, -57, -29, -8, 10, 31, 45, 60, 65,
    70, 74, 76, 79, 82, 79, 75, 62,
};

static lv_obj_t * chart;
lv_chart_series_t * ser1;
lv_chart_series_t * ser2;
uint32_t pcnt = sizeof(ecg_sample) / sizeof(ecg_sample[0]);
uint32_t cnt = 0;


#include "lvgl/lvgl.h"

#define RPM_VAL_RANGE        4000
#define RPM_VAL_WARNNING     3000
#define RPM_ARC_RANGE        240
#define RPM_ARC_OFFSET       150
#define RPM_ARC_WARNNING     180
#define RPM_ARC_RADIUS       212
#define RPM_MIDDLE_X         260
#define RPM_MIDDLE_Y         258

#define OIL_VAL_RANGE        100
#define OIL_VAL_WARNNING     10
#define OIL_ARC_RANGE        90
#define OIL_ARC_OFFSET       180
#define OIL_ARC_RADIUS       196
#define OIL_MIDDLE_X         716
#define OIL_MIDDLE_Y         240

#define COOLANT_VAL_RANGE    120
#define COOLANT_VAL_WARNNING 100
#define COOLANT_ARC_RANGE    90
#define COOLANT_ARC_OFFSET   180
#define COOLANT_ARC_WARNNING 75
#define COOLANT_ARC_RADIUS   196
#define COOLANT_MIDDLE_X     969
#define COOLANT_MIDDLE_Y     240


lv_style_t arc_red_style;
lv_style_t arc_blue_style;
lv_style_t arc_mark_style;
lv_style_t arc_opa0_style;

lv_obj_t *label_rpm;
lv_obj_t *arc_rpm;
lv_obj_t *mark_rpm;

lv_obj_t *label_oil;
lv_obj_t *arc_oil;
lv_obj_t *mark_oil;

lv_obj_t *label_coolant;
lv_obj_t *arc_coolant;
lv_obj_t *mark_coolant;

static void gauge_init_style(void)
{
    lv_style_init(&arc_mark_style);
    lv_style_set_arc_opa(&arc_mark_style, LV_OPA_100);
    lv_style_set_arc_rounded(&arc_mark_style, 0);
    lv_style_set_arc_color(&arc_mark_style, lv_color_make(95, 70, 70));
    lv_style_set_arc_width(&arc_mark_style, 100);

    lv_style_init(&arc_blue_style);
    lv_style_set_arc_opa(&arc_blue_style, LV_OPA_50);
    lv_style_set_arc_rounded(&arc_blue_style, 0);
    lv_style_set_arc_color(&arc_blue_style, lv_color_make(200, 200, 250));
    lv_style_set_arc_width(&arc_blue_style, 80);

    lv_style_init(&arc_red_style);
    lv_style_set_arc_opa(&arc_red_style, LV_OPA_50);
    lv_style_set_arc_rounded(&arc_red_style, 0);
    lv_style_set_arc_color(&arc_red_style, lv_color_make(250, 20, 20));
    lv_style_set_arc_width(&arc_red_style, 80);

    lv_style_init(&arc_opa0_style);
    lv_style_set_arc_opa(&arc_opa0_style, LV_OPA_0);
    lv_style_set_arc_rounded(&arc_opa0_style, 0);
}

static void gauge_draw(lv_obj_t **label, lv_obj_t **arc, lv_obj_t **mark, uint16_t x, uint16_t y, uint16_t radius, uint16_t arc_offset)
{
    /* Label */
    *label = lv_label_create(lv_scr_act());
    lv_obj_set_pos(*label, x, y);
//    lv_label_set_text(*label, "0");

    /* Gauge bar */
    *arc = lv_arc_create(lv_scr_act());
    lv_arc_set_angles(*arc, 0, 0);
    lv_arc_set_bg_angles(*arc, 0, 0);
    lv_arc_set_rotation(*arc, arc_offset);

    lv_obj_set_pos(*arc, x-radius, y-radius);
    lv_obj_set_size(*arc, radius*2, radius*2);
    lv_obj_add_style(*arc, &arc_red_style, LV_PART_MAIN);
    lv_obj_add_style(*arc, &arc_blue_style, LV_PART_INDICATOR);

    lv_obj_remove_style(*arc, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(*arc, LV_OBJ_FLAG_CLICKABLE);

    /* Gauge arrow */
    *mark = lv_arc_create(lv_scr_act());
    lv_arc_set_angles(*mark, 0, 0);
    lv_arc_set_bg_angles(*mark, 0, 0);
    lv_arc_set_rotation(*mark, arc_offset);

    lv_obj_set_pos(*mark, x-radius, y-radius);
    lv_obj_set_size(*mark, radius*2, radius*2);
    lv_obj_add_style(*mark, &arc_opa0_style, LV_PART_MAIN);
    lv_obj_add_style(*mark, &arc_mark_style, LV_PART_INDICATOR);

    lv_obj_remove_style(*mark, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(*mark, LV_OBJ_FLAG_CLICKABLE);
}

static void gauge_update(uint8_t mode, uint16_t value)
{
    char val_string[8] = {0, };

    uint8_t  val_check = 0;
    uint16_t val_range, val_warn;
    uint16_t arc_range, arc_offset, arc_warn, arc_value;

    lv_obj_t *label, *arc, *mark;

    switch(mode)
    {
        case 0:
            val_range  = RPM_VAL_RANGE;
            val_warn   = RPM_VAL_WARNNING;
            arc_range  = RPM_ARC_RANGE;
            arc_offset = RPM_ARC_OFFSET;
            arc_warn   = RPM_ARC_WARNNING;

            label = label_rpm;
            arc   = arc_rpm;
            mark  = mark_rpm;
            break;

        case 1:
            val_range  = OIL_VAL_RANGE;
            val_warn   = OIL_VAL_WARNNING;
            arc_range  = OIL_ARC_RANGE;
            arc_offset = OIL_ARC_OFFSET;
            arc_warn   = 0;

            label = label_oil;
            arc   = arc_oil;
            mark  = mark_oil;
            break;

        case 2:
            val_range  = COOLANT_VAL_RANGE;
            val_warn   = COOLANT_VAL_WARNNING;
            arc_range  = COOLANT_ARC_RANGE;
            arc_offset = COOLANT_ARC_OFFSET;
            arc_warn   = COOLANT_ARC_WARNNING;

            label = label_coolant;
            arc   = arc_coolant;
            mark  = mark_coolant;
            break;

        default:
            printf("ERROR : invalid mode = %d\r\n", mode);
            return;
    }

//    sprintf(val_string, "%d", value);
//    lv_label_set_text(label, val_string);

    arc_value = (value * arc_range) / val_range;
    if(mode == 1) val_check = value <= val_warn;
    else          val_check = value > val_warn;

    if(val_check)
    {
        lv_arc_set_angles(arc, 0, arc_warn);
        lv_arc_set_bg_angles(arc, arc_warn, arc_value);
    }
    else
    {
        lv_arc_set_angles(arc, 0, arc_value);
        lv_arc_set_bg_angles(arc, 0, 0);
    }
    lv_arc_set_angles(mark, arc_value, arc_value+1);
}

void exam_gague_draw(void)
{
    gauge_init_style();
    gauge_draw(&label_rpm, &arc_rpm, &mark_rpm, RPM_MIDDLE_X, RPM_MIDDLE_Y, RPM_ARC_RADIUS, RPM_ARC_OFFSET);
//    gauge_draw(&label_oil, &arc_oil, &mark_oil, OIL_MIDDLE_X, OIL_MIDDLE_Y, OIL_ARC_RADIUS, OIL_ARC_OFFSET);
//    gauge_draw(&label_coolant, &arc_coolant, &mark_coolant, COOLANT_MIDDLE_X, COOLANT_MIDDLE_Y, COOLANT_ARC_RADIUS, COOLANT_ARC_OFFSET);
}

uint8_t val1 = 0;
uint8_t val2 = 0;
void exam_gauge_update(void)
{
    if(val1 <= 100)
    {
        gauge_update(0, val1*4000/100);
//        gauge_update(1, val1);
//        gauge_update(2, val1*120/100);
        //usleep(50 * 1000);
        val1++;
        if(val1 == 100) val2 = 100;
    }
    else
    {
        if(val2 > 0)
        {
            val2--;
            if(val2 == 0) val1 = 0;
            gauge_update(0, val2*4000/100);
//            gauge_update(1, val2);
//            gauge_update(2, val2*120/100);
            //usleep(50 * 1000);
        }
    }

}

void add_data(void)
{
    float res = 0;    
    if(++cnt >= pcnt) cnt = 0;
//    res = HighPassFilter_Update(&filter, ecg_sample[cnt]);
    lv_chart_set_next_value(chart, ser1, ecg_sample[cnt]);
    lv_chart_set_next_value(chart, ser2, res + 100);
    //printf("->%d, %f\n", ecg_sample[cnt], res);

    exam_gauge_update();

}



void add_colorwheel(void)
{
    lv_obj_t * cw;

    cw = lv_colorwheel_create(lv_scr_act(), true);
    lv_obj_set_size(cw, 200, 200);
    lv_obj_center(cw);
}

void test(void)
{
    lv_obj_t * cw;

    /*Create a chart*/
    //lv_obj_t * chart;
    chart = lv_chart_create(lv_scr_act());
    lv_obj_set_size(chart, 800, 500);
    lv_obj_center(chart);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);   /*Show lines and points too*/

    /*Add two data series*/
    ser1 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);
    ser2 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_GREEN), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, -1000, 1000);

    //lv_chart_set_zoom_x(chart, 100);
    //lv_chart_set_point_count(chart, 500);
    /*Do not display points on the data*/
    lv_obj_set_style_size(chart, 0, LV_PART_INDICATOR);


    lv_chart_set_update_mode(chart, LV_CHART_UPDATE_MODE_SHIFT);


    cw = lv_colorwheel_create(lv_scr_act(), true);
    lv_obj_set_size(cw, 100, 100);
    lv_obj_set_pos(cw, 500,50);

    lv_obj_t * arc = lv_arc_create(lv_scr_act());
    lv_arc_set_bg_start_angle(arc, 0);
    lv_arc_set_bg_end_angle(arc, 360);
    lv_arc_set_start_angle(arc, 0);
    lv_arc_set_end_angle(arc, 0);


    exam_gague_draw();


}

void test1(void)
{


    exam_gague_draw();


}
