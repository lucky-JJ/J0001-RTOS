#coding=utf-8

import xlrd
import os
import sys
import re

#路径前加 r，读取的文件路径
file_path = os.getcwd() + os.sep + 'GPIO.xlsx'

with open("./Pin/GPIO.h", 'w') as fp:
	fp.write('#ifndef GPIO_H\n')
	fp.write('#define GPIO_H\n\n')
	fp.write('#include ' +  '"'  + 'GpioCfg.h' +  '"' + '\n\n')
	
	fp.write('u8 GPIO_Read(Io_Type SignalType);\n')
	fp.write('void GPIO_Write(Io_Type SignalType, GPIO_PinState PinState);\n')
	fp.write('void Gpio_Init(const GPIO_InitTypeDef *config);\n')

	fp.write('\n#endif\n')
	fp.close()

with open("./Pin/GPIO.c", 'w') as fp:
	fp.write('#include ' +  '"'  + 'GPIO.h' +  '"' + '\n\n')
	fp.write('u8 GPIO_Read(Io_Type SignalType)\n')
	fp.write('{\n')
	fp.write('	return (u8)HAL_GPIO_ReadPin((GPIO_TypeDef *)IoTable[SignalType].base , (u16)IoTable[SignalType].pinPortIdx);\n')
	fp.write('}\n\n')

	fp.write('void GPIO_Write(Io_Type SignalType, GPIO_PinState PinState)\n')
	fp.write('{\n')
	fp.write('	if(SignalType < MAX_IO_NUM)\n')
	fp.write('	{\n')
	fp.write('		HAL_GPIO_WritePin(IoTable[SignalType].base, (u16)IoTable[SignalType].pinPortIdx,PinState);	\n')
	fp.write('	}\n')
	fp.write('}\n\n')

	fp.write('void Gpio_Init(const GPIO_InitTypeDef *config)\n')
	fp.write('{\n')
	fp.write('	u32 i;\n')
	fp.write('	GPIO_InitTypeDef GPIO_Initure;\n\n')
	fp.write('	/* STM32F4xx  GPIO_InitTypeDef need be add (GPIO_TypeDef * base) to adapt the script*/\n\n')
	fp.write('	/* The clock must be enabled first */\n\n')
	fp.write('    for (i = 0 ; i < NUM_OF_CONFIGURED_PINS; i++)\n')
	fp.write('    {\n')

	fp.write('    	GPIO_Initure.base 		= config[i].base;\n')
	fp.write('		GPIO_Initure.Pin 		= config[i].Pin;\n')
	fp.write('		GPIO_Initure.Mode 		= config[i].Mode;\n')
	fp.write('		GPIO_Initure.Pull 		= config[i].Pull;\n')
	fp.write('		GPIO_Initure.Speed 		= config[i].Speed;\n')
	fp.write('		GPIO_Initure.Alternate 	= config[i].Alternate;\n\n')
	fp.write('		HAL_GPIO_Init(GPIO_Initure.base,&GPIO_Initure);\n')
	fp.write('    }\n')
	fp.write('}\n\n')

	fp.close()

with open("./Pin/GpioCfg.h", 'w') as fp:
	fp.write('#ifndef GPIO_CFG_H\n')
	fp.write('#define GPIO_CFG_H\n\n')
	fp.write('#include ' +  '"'  + 'sys.h' +  '"' + '\n\n')
	with xlrd.open_workbook(file_path) as data:# 打开xls文件
		table = data.sheets()[0] # 打开第一张表
		nrows = table.nrows    # 获取表的行数
		ncols = table.ncols    # 获取表的列数
		validcnt = 0
		for i in range(nrows): # 循环逐行打印
			if i == 0 or i == 1 : # 跳过第一行
				continue
			pinusedstatecol = 3
			if table.row_values(i)[pinusedstatecol] == 'Yes':
				validcnt += 1
	fp.write('#define  NUM_OF_CONFIGURED_PINS      ' + '(' + str(validcnt) + ')\n\n')
	fp.write('typedef struct\n')
	fp.write('{\n')
	fp.write('    GPIO_TypeDef *			base;              /*!< Port base pointer.     */\n')
	fp.write('    u32						pinPortIdx;        /*!< Port pin number.       */\n')
	fp.write('} PinNameSt;\n\n')

	with xlrd.open_workbook(file_path) as data:# 打开xls文件
		table = data.sheets()[0] # 打开第一张表
		nrows = table.nrows    # 获取表的行数
		ncols = table.ncols    # 获取表的列数
		fp.write('typedef enum {\n')
		for i in range(nrows): # 循环逐行打印
			if i == 0 or i == 1: # 跳过第一行
				continue
			pinusedstatecol = 3
			if table.row_values(i)[pinusedstatecol] == 'Yes':
				alaiscol = 2
				if len(table.row_values(i)[alaiscol]) > 3:
					Str = table.row_values(i)[alaiscol].lower()
				if Str[0:2] == 'io':
					fp.write('    ' + table.row_values(i)[alaiscol] + ',\n')
		fp.write('    MAX_IO_NUM,\n')
		fp.write('}Io_Type;\n\n')

	fp.write('extern const GPIO_InitTypeDef Gpio_InitConfigArr[NUM_OF_CONFIGURED_PINS];\n')
	fp.write('extern const GPIO_InitTypeDef Gpio_DeInitConfigArr[NUM_OF_CONFIGURED_PINS];\n')
	fp.write('extern const PinNameSt IoTable[];\n')
	fp.write('\n\n#endif\n')
	fp.close()

with open("./Pin/GpioCfg.c", 'w') as fp:
	fp.write('#include ' +  '"'  + 'GpioCfg.h' +  '"' + '\n\n')
	fp.write('/*------------ Initializes the GPIO sequence table --------------*/\n\n')
	fp.write('const GPIO_InitTypeDef Gpio_InitConfigArr[NUM_OF_CONFIGURED_PINS] =\n')
	fp.write('{\n')
	with xlrd.open_workbook(file_path) as data:# 打开xls文件
		table = data.sheets()[0] # 打开第一张表
		nrows = table.nrows    # 获取表的行数
		ncols = table.ncols    # 获取表的列数

		for i in range(nrows): # 循环逐行打印
			if i == 0 or i == 1 : # 跳过第一行
				continue
			pinusedstatecol = 3
			if table.row_values(i)[pinusedstatecol].replace(' ', '') == 'Yes':
				fp.write('    {\n')
				alaiscol = 2
				commentcol = 9
				fp.write('         /*' + table.row_values(i)[alaiscol] + table.row_values(i)[commentcol] + ' */' + '\n')
				pinnamecol = 1
				Str = table.row_values(i)[pinnamecol].replace(' ', '').lower()
				fp.write('        .base = GPIO' + Str[1].upper() + ',\n')
				fp.write('        .Pin = ' + Str[2:] + ',\n')
				pullconfigcol = 4
				if table.row_values(i)[pullconfigcol].replace(' ','') == 'PullDown':
					fp.write('        .Pull = ' + 'GPIO_PULLDOWN' + ',\n')
				elif table.row_values(i)[pullconfigcol].replace(' ', '') == 'PullUp':
					fp.write('        .Pull = ' + 'GPIO_PULLUP' + ',\n')
				else:
					fp.write('        .Pull = ' + 'GPIO_NOPULL' + ',\n')
				pullconfigcol = 5
				if table.row_values(i)[pullconfigcol].replace(' ','') == 'GPIO_MODE_INPUT':
					fp.write('        .Mode = ' + 'GPIO_MODE_INPUT' + ',\n')
				elif table.row_values(i)[pullconfigcol].replace(' ', '') == 'GPIO_MODE_OUTPUT_PP':
					fp.write('        .Mode = ' + 'GPIO_MODE_OUTPUT_PP' + ',\n')
				elif table.row_values(i)[pullconfigcol].replace(' ', '') == 'GPIO_MODE_OUTPUT_OD':
					fp.write('        .Mode = ' + 'GPIO_MODE_OUTPUT_OD' + ',\n')
				elif table.row_values(i)[pullconfigcol].replace(' ', '') == 'GPIO_MODE_AF_PP':
					fp.write('        .Mode = ' + 'GPIO_MODE_AF_PP' + ',\n')
				else:
					fp.write('        .Mode = ' + 'GPIO_MODE_AF_OD' + ',\n')
				pullconfigcol = 6
				if table.row_values(i)[pullconfigcol].replace(' ','') == 'GPIO_SPEED_LOW':
					fp.write('        .Speed = ' + 'GPIO_SPEED_LOW' + ',\n')
				elif table.row_values(i)[pullconfigcol].replace(' ', '') == 'GPIO_SPEED_MEDIUM':
					fp.write('        .Speed = ' + 'GPIO_SPEED_MEDIUM' + ',\n')
				elif table.row_values(i)[pullconfigcol].replace(' ', '') == 'GPIO_SPEED_FAST':
					fp.write('        .Speed = ' + 'GPIO_SPEED_FAST' + ',\n')	
				else:
					fp.write('        .Speed = ' + 'GPIO_SPEED_HIGH' + ',\n')
				pullconfigcol = 7
				if table.row_values(i)[pullconfigcol].replace(' ','') == 'GPIO_AF0':
					fp.write('        .Alternate = ' + '0' + ',\n')
				elif table.row_values(i)[pullconfigcol].replace(' ', '') == 'GPIO_AF1':
					fp.write('        .Alternate = ' + '1' + ',\n')
				elif table.row_values(i)[pullconfigcol].replace(' ', '') == 'GPIO_AF2':
					fp.write('        .Alternate = ' + '2' + ',\n')
				elif table.row_values(i)[pullconfigcol].replace(' ', '') == 'GPIO_AF3':
					fp.write('        .Alternate = ' + '3' + ',\n')
				elif table.row_values(i)[pullconfigcol].replace(' ', '') == 'GPIO_AF4':
					fp.write('        .Alternate = ' + '4' + ',\n')
				elif table.row_values(i)[pullconfigcol].replace(' ', '') == 'GPIO_AF5':
					fp.write('        .Alternate = ' + '5' + ',\n')
				elif table.row_values(i)[pullconfigcol].replace(' ', '') == 'GPIO_AF6':
					fp.write('        .Alternate = ' + '6' + ',\n')
				elif table.row_values(i)[pullconfigcol].replace(' ', '') == 'GPIO_AF7':
					fp.write('        .Alternate = ' + '7' + ',\n')
				elif table.row_values(i)[pullconfigcol].replace(' ', '') == 'GPIO_AF8':
					fp.write('        .Alternate = ' + '8' + ',\n')
				elif table.row_values(i)[pullconfigcol].replace(' ', '') == 'GPIO_AF9':
					fp.write('        .Alternate = ' + '9' + ',\n')
				elif table.row_values(i)[pullconfigcol].replace(' ', '') == 'GPIO_AF10':
					fp.write('        .Alternate = ' + '10' + ',\n')
				elif table.row_values(i)[pullconfigcol].replace(' ', '') == 'GPIO_AF11':
					fp.write('        .Alternate = ' + '11' + ',\n')
				elif table.row_values(i)[pullconfigcol].replace(' ', '') == 'GPIO_AF12':
					fp.write('        .Alternate = ' + '12' + ',\n')
				elif table.row_values(i)[pullconfigcol].replace(' ', '') == 'GPIO_AF13':
					fp.write('        .Alternate = ' + '13' + ',\n')
				elif table.row_values(i)[pullconfigcol].replace(' ', '') == 'GPIO_AF14':
					fp.write('        .Alternate = ' + '14' + ',\n')
				else:
					fp.write('        .Alternate = ' + '15' + ',\n')
				fp.write('    },\n')
		fp.write('};\n')

		#fp.write('\nconst GPIO_InitTypeDef Gpio_DeInitConfigArr[NUM_OF_CONFIGURED_PINS] =\n')
		#fp.write('{\n\n')
		#fp.write('};\n')

		fp.write('\nconst PinNameSt IoTable[]  = {\n')
		i = 0
		while i < nrows:  # 循环逐行打印
			if i == 0 or i == 1:  # 跳过第一行
				i += 1
				continue
			pinusedstatecol = 3
			if table.row_values(i)[pinusedstatecol] == 'Yes':
				alaiscol = 2
				if len(table.row_values(i)[alaiscol]) > 3:
					Str = table.row_values(i)[alaiscol].lower()
					if Str[0:2] == 'io':
						Str = table.row_values(i)[1].replace(' ', '').lower()
						fp.write('    {GPIO' + Str[1].upper() + ', ' + Str[2:] + '},		/* ' + table.row_values(i)[alaiscol] + ' */\n')
			i += 1
		fp.write('};\n\n')
	fp.close()



