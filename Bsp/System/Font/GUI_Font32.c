/**
 ******************************************************************************
 * @file		GUI_Font32.c
 * @author		�Ž�����С��
 * @version		V1.0.0
 * @date		2018-01-20
 * @brief       ���ֺ�����ʾ��ʽ�ʺ�ʹ�õȿ����壬�ǵȿ��΢���źڵ�������ʾЧ���ϲ
 * @note		
 * @History
 * Date           	Author    	version    				Notes
 * 2018-01-20    	ZSY       	V1.0.0				first version.
 */
	
/* Includes ------------------------------------------------------------------*/
#include "GUI.h"
#include "GUI_Type.h"


GUI_CONST_STORAGE GUI_CHARINFO GUI_FontHZ32_CharInfo[2] =  
{     
      {  16,  16,  2, (void *)"A32"},    
      {  32,  32,  4, (void *)"H32"},        
}; 

GUI_CONST_STORAGE GUI_FONT_PROP GUI_FontHZ32_PropHZ= { 
      0x4081,  
      0xFEFE, 
      &GUI_FontHZ32_CharInfo[1], 
      (void *)0,  
};

GUI_CONST_STORAGE  GUI_FONT_PROP GUI_FontHZ32_PropASC= { 
      0x0000,
      0x007F,  
      &GUI_FontHZ32_CharInfo[0], 
      (void GUI_CONST_STORAGE *)&GUI_FontHZ32_PropHZ,  
}; 

GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ32 =  
{ 
	GUI_FONTTYPE_USER,
	32,  
	32,  
	1,   
	1,   
	(void GUI_CONST_STORAGE *)&GUI_FontHZ32_PropASC
}; 

GUI_CONST_STORAGE  GUI_FONT GUI_FontHZ32x2 =  
{ 
	GUI_FONTTYPE_USER,
	32,  
	32,  
	2,   
	2,   
	(void GUI_CONST_STORAGE *)&GUI_FontHZ32_PropASC 
};






