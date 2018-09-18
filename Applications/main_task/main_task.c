
#include "lvgl/lvgl.h"
#include "main_task.h"
#include "../lv_font.h"

extern lv_font_t WRJZY_20;
extern lv_font_t HZ_16;
char *menu_name[] = {" ”∆µ","“Ù¿÷","¿∂—¿","Œﬁœﬂ","¿÷“Ù","—¿¿∂","ƒ÷÷”","¡øµÁ","…Ë÷√"};


void main_task_creat()
{	

	static lv_style_t style_new;                         /*Styles can't be local variables*/ 
	lv_style_copy(&style_new, &lv_style_pretty);         /*Copy a built-in style as a starting point*/ 
//	style_new.body.radius = LV_RADIUS_CIRCLE;            /*Fully round corners*/ 
	style_new.body.main_color = LV_COLOR_GRAY;          /*White main color*/ 
	style_new.body.grad_color = LV_COLOR_GRAY;           /*Blue gradient color*/ 
	style_new.body.shadow.color = LV_COLOR_BLACK;       /*Light gray shadow color*/ 
//	style_new.body.shadow.width = 8;                     /*8 px shadow*/ 
//	style_new.body.border.width = 8;                     /*2 px border width*/ 
	

	
	
	lv_obj_t * scr = lv_scr_act();
	lv_obj_set_style(scr,&style_new);
    
    
    GUI_GetDataFromMemory(HZ_16,"ƒ„∫√");
    static lv_style_t label_style;
    lv_style_copy(&label_style, &lv_style_plain);
    label_style.text.font = &HZ_16; /*Set the base font whcih is concatenated with the others*/ 	 
    
    lv_obj_t * label = lv_label_create(lv_scr_act(),NULL);	

	lv_label_set_style(label, &label_style); 
    lv_obj_set_pos(label,20,20);
    lv_label_set_text (label," !");
    
//    label_style.text.font = &WRJZY_20; /*Set the base font whcih is concatenated with the others*/ 	 
//    
//    lv_obj_t * label1 = lv_label_create(lv_scr_act(),NULL);	

//	lv_label_set_style(label1, &label_style); 
//    lv_obj_set_pos(label1,60,60);
//    lv_label_set_text (label1," !");
    
}