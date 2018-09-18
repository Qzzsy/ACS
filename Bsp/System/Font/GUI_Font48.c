/**
 ******************************************************************************
 * @file		GUI_Font48.c
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



GUI_CONST_STORAGE GUI_CHARINFO GUI_FontHZ48_CharInfo[2] =  
{     
    {  24,  24,  3, (void *)"A48"},   /* 字符 */     
    {  48,  48,  6, (void *)"H48"},   /* 汉字 */        
};

GUI_CONST_STORAGE GUI_FONT_PROP GUI_FontHZ48_PropHZ= { 
      0x4081,  
      0xFEFE, 
      &GUI_FontHZ48_CharInfo[1], 
      (void *)0,  
}; 

GUI_CONST_STORAGE  GUI_FONT_PROP GUI_FontHZ48_PropASC= { 
      0x0000,  
      0x007F,  
      &GUI_FontHZ48_CharInfo[0], 
      (void GUI_CONST_STORAGE *)&GUI_FontHZ48_PropHZ,  
};

GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ48 =  
{ 
	  GUI_FONTTYPE_USER,
      48,  
      48,  
      1,   
      1,   
      (void GUI_CONST_STORAGE *)&GUI_FontHZ48_PropASC 
};

GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ48x2 =  
{ 
	  GUI_FONTTYPE_USER,
      48,  
      48,  
      2,   
      2,   
      (void GUI_CONST_STORAGE *)&GUI_FontHZ48_PropASC 
}; 







