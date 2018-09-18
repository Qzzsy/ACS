/**
 ******************************************************************************
 * @file		GUI_Font16.c
 * @author		门禁开发小组
 * @version		V1.0.0
 * @date		2018-01-20
 * @brief       这种汉字显示方式适合使用等宽字体，非等宽的微软雅黑等字体显示效果较差。
 * @note		
 * @History
 * Date           	Author    	version    				Notes
 * 2018-01-20    	ZSY       	V1.0.0				first version.
 */
	
/* Includes ------------------------------------------------------------------*/

#include "GUI.h"
#include "GUI_Type.h"


GUI_CONST_STORAGE GUI_CHARINFO GUI_FontHZ16_CharInfo[2] =  
{     
    {  8,    8,  1, (void *)"A16"},      
    {  16,  16,  2, (void *)"H16"},           
}; 

GUI_CONST_STORAGE GUI_FONT_PROP GUI_FontHZ16_PropHZ= { 
      0x4081,  
      0xFEFE, 
      &GUI_FontHZ16_CharInfo[1], 
      (void *)0,  
}; 

GUI_CONST_STORAGE  GUI_FONT_PROP GUI_FontHZ16_PropASC= { 
      0x0000,  
      0x007F,  
      &GUI_FontHZ16_CharInfo[0], 
      (void GUI_CONST_STORAGE *)&GUI_FontHZ16_PropHZ,  
}; 

GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ16 =  
{ 
	  GUI_FONTTYPE_USER,
      16,  
      16,  
      1,   
      1,   
      (void GUI_CONST_STORAGE *)&GUI_FontHZ16_PropASC,
}; 
GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ16x2 =  
{ 
	  GUI_FONTTYPE_USER,
      16,  
      16,  
      2,   
      2,   
      (void GUI_CONST_STORAGE *)&GUI_FontHZ16_PropASC 
};







