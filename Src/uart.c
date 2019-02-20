/*
 * uart.c
 *
 *  Created on: 08 Mar 2018
 *      Author: 19837739
 */


/*
 * user.c
 *
 *  Created on: 28 Feb 2018
 *      Author: lvisagie
 */
#include "uart.h"


char rxchar;         // temporary storage - if a character arrives from the UART, it will be placed in this variable
char cmdbuffer[CMDBUFLEN];  // buffer in which to store commands that are received from the UART
uint16_t cmdbufpos;  // this is the position in the cmdbuffer where we are currently writing to

char* TXMSG_STUDENTNUM = "$A,19837739\r\n";  // this is the string that will be returned if a $A command is received
char txbuffer[MAXTXLEN];    // buffer for replies that are to be sent out on UART

RTC_TimeTypeDef setTime; 	// buffer to hold set time value
uint8_t decrement;
uint8_t window, HH1, MM1, SS1, HH2, MM2, SS2;
timeStruct heatingSchedule[6];



void uartInit(void)
{
	// tell the UART we want to be interrupted after one character was received
	HAL_UART_Receive_IT(&huart1, (uint8_t*)&rxchar, 1);
}

// HAL_UART_RxCpltCallback - callback function that will be called from the UART interrupt handler. This function will execute whenever a character is received from the UART
void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
	// the interrupt handler will automatically put the received character in the rxchar variable (no need to write any code for that).
	// if it is a $ character, clear the command buffer first
	if (rxchar == '$')
		cmdbufpos = 0;

	// add character to command buffer, but only if there is more space in the command buffer
	if (cmdbufpos < CMDBUFLEN)
		cmdbuffer[cmdbufpos++] = rxchar;

	// check if there is a complete command in the cmdbuffer
	if (cmdbuffer[cmdbufpos-1] == '\n')
	{
		// call a helper function to decode the command
		DecodeCommand();

		// clear buffer
		cmdbufpos = 0;
	}


	// tell the UART we want to be interrupted after another character was received
	HAL_UART_Receive_IT(&huart1, (uint8_t*)&rxchar, 1);
}


// DecodeCommand - decode and act on commands received from the UART
void DecodeCommand()
{
	uint8_t numcharswritten;
	uint8_t totalCharsWritten = 0;


	if(cmdbuffer[0] != '$')
	{
		HAL_UART_Transmit(&huart1, (uint8_t*)"Invalid command: Missing $\r\n", 28, 1000);
	}
	else
	{
		switch (cmdbuffer[1])
		{
		case 'A' :
			// student number request
			// return string %A,<student num><CR><LF>  (length will always be 13 bytes)
			HAL_UART_Transmit(&huart1, (uint8_t*)TXMSG_STUDENTNUM, 13, 1000);
			break;

		case 'B' :
			valveState = (uint16_t)cmdbuffer[3] - 48;
			valveControl();

			txbuffer[0] = '$'; txbuffer[1] = 'B'; txbuffer[2] = '\r'; txbuffer[3] = '\n';
			HAL_UART_Transmit(&huart1, (uint8_t*)txbuffer, 4, 1000);
			break;

		case 'C' :
			heatingScheduleState = (uint16_t)cmdbuffer[3] - 48;

			txbuffer[0] = '$'; txbuffer[1] = 'C'; txbuffer[2] = '\r'; txbuffer[3] = '\n';
			HAL_UART_Transmit(&huart1, (uint8_t*)txbuffer, 4, 1000);

			break;

		case 'D' :
			heaterState = (uint16_t)cmdbuffer[3] - 48;

			txbuffer[0] = '$'; txbuffer[1] = 'D'; txbuffer[2] = '\r'; txbuffer[3] = '\n';
			HAL_UART_Transmit(&huart1, (uint8_t*)txbuffer, 4, 1000);
			break;

		case 'E' :
			// Enable/disable   logging  to  flash  memory
			loggingState = (uint16_t)cmdbuffer[3] - 48;

			txbuffer[0] = '$'; txbuffer[1] = 'E'; txbuffer[2] = '\r'; txbuffer[3] = '\n';
			HAL_UART_Transmit(&huart1, (uint8_t*)txbuffer, 4, 1000);
			break;

		case 'F':
			// set temperature set-point
			// parse temperature value
			String2Int(cmdbuffer+3, &temp_setpoint);

			// return string %F<CR><LF>  (length will always be 4 bytes)
			txbuffer[0] = '$'; txbuffer[1] = 'F'; txbuffer[2] = '\r'; txbuffer[3] = '\n';
			HAL_UART_Transmit(&huart1, (uint8_t*)txbuffer, 4, 1000);
			break;

		case 'G':
			// get temperature set-point
			// return string with the format $G,<temp><CR><LF>
			txbuffer[0] = '$'; txbuffer[1] = 'G'; txbuffer[2] = ',';
			numcharswritten = Int2String(txbuffer+3, temp_setpoint, 4);
			txbuffer[3+numcharswritten] = '\r'; txbuffer[4+numcharswritten] = '\n';
			HAL_UART_Transmit(&huart1, (uint8_t*)txbuffer, numcharswritten+5, 1000);
			break;

		case 'H' :
			// set time cmd, return empty
			decrement = 0;

			String2IntU8(cmdbuffer+3, &setTime.Hours); //HH
			if(setTime.Hours < 10) decrement++;
			String2IntU8(cmdbuffer+(6-decrement), &setTime.Minutes); //mm
			if(setTime.Minutes < 10) decrement++;
			String2IntU8(cmdbuffer+(9-decrement), &setTime.Seconds); //ss

			// write new value to rtc
			__HAL_RTC_WRITEPROTECTION_DISABLE(&hrtc);
			RTC_EnterInitMode(&hrtc);
			HAL_RTC_SetTime(&hrtc, &setTime, RTC_FORMAT_BIN);
			//HAL_RTC_SetDate(&hrtc, &rtcDate, RTC_FORMAT_BIN);
			__HAL_RTC_WRITEPROTECTION_ENABLE(&hrtc);



			txbuffer[0] = '$'; txbuffer[1] = 'H'; txbuffer[2] = '\r'; txbuffer[3] = '\n';
			HAL_UART_Transmit(&huart1, (uint8_t*)txbuffer, 4, 1000);
			break;

		case 'I' :
			// request current time
			// preamble
			txbuffer[0] = '$'; txbuffer[1] = 'I'; txbuffer[2] = ',';
			totalCharsWritten = 3;
			// arg 1: HH
			numcharswritten = Int2String(&txbuffer[totalCharsWritten], rtcTime.Hours, 2);
			totalCharsWritten += numcharswritten;
			txbuffer[totalCharsWritten++] = ',';
			// arg 2: mm
			numcharswritten = Int2String(&txbuffer[totalCharsWritten], rtcTime.Minutes, 2);
			totalCharsWritten += numcharswritten;
			txbuffer[totalCharsWritten++] = ',';
			// arg 3: ss
			numcharswritten = Int2String(&txbuffer[totalCharsWritten], rtcTime.Seconds, 2);
			totalCharsWritten += numcharswritten;
			// \r\n
			txbuffer[totalCharsWritten++] = '\r'; txbuffer[totalCharsWritten++] = '\n';

			HAL_UART_Transmit(&huart1, (uint8_t*)txbuffer, totalCharsWritten, 1000);
			break;

		case 'J' :
			// set heating schedule window, return empty
			decrement = 0;

			String2IntU8(cmdbuffer+3, &window); //window
			String2IntU8(cmdbuffer+5, &HH1); //HH
			if(HH1 < 10) decrement++;
			String2IntU8(cmdbuffer+(8-decrement), &MM1); //mm
			if(MM1 < 10) decrement++;
			String2IntU8(cmdbuffer+(11-decrement), &SS1); //ss
			if(SS1 < 10) decrement++;
			String2IntU8(cmdbuffer+(14-decrement), &HH2); //HH
			if(HH2 < 10) decrement++;
			String2IntU8(cmdbuffer+(17-decrement), &MM2); //MM
			if(MM2 < 10) decrement++;
			String2IntU8(cmdbuffer+(20-decrement), &SS2); //ss

			switch (window)
			{
			case 1:
				heatingSchedule[0].Hours = HH1;
				heatingSchedule[0].Minutes = MM1;
				heatingSchedule[0].Seconds = SS1;
				heatingSchedule[1].Hours = HH2;
				heatingSchedule[1].Minutes = MM2;
				heatingSchedule[1].Seconds = SS2;
				heatingSchedule[0].Total = (HH1*60*60) + (MM1*60) + (SS1);
				heatingSchedule[1].Total = (HH2*60*60) + (MM2*60) + (SS2);

				break;

			case 2:
				heatingSchedule[2].Hours = HH1;
				heatingSchedule[2].Minutes = MM1;
				heatingSchedule[2].Seconds = SS1;
				heatingSchedule[3].Hours = HH2;
				heatingSchedule[3].Minutes = MM2;
				heatingSchedule[3].Seconds = SS2;
				heatingSchedule[2].Total = (HH1*60*60) + (MM1*60) + (SS1);
				heatingSchedule[3].Total = (HH2*60*60) + (MM2*60) + (SS2);

				break;
			case 3:
				heatingSchedule[4].Hours = HH1;
				heatingSchedule[4].Minutes = MM1;
				heatingSchedule[4].Seconds = SS1;
				heatingSchedule[5].Hours = HH2;
				heatingSchedule[5].Minutes = MM2;
				heatingSchedule[5].Seconds = SS2;
				heatingSchedule[4].Total = (HH1*60*60) + (MM1*60) + (SS1);
				heatingSchedule[5].Total = (HH2*60*60) + (MM2*60) + (SS2);

				break;
			}




			txbuffer[0] = '$'; txbuffer[1] = 'J'; txbuffer[2] = '\r'; txbuffer[3] = '\n';
			HAL_UART_Transmit(&huart1, (uint8_t*)txbuffer, 4, 1000);
			break;

			case 'K' :
				// preamble
				txbuffer[0] = '$'; txbuffer[1] = 'K'; txbuffer[2] = ',';
				totalCharsWritten = 3;
				// arg 1: current RMS
				numcharswritten = Int2String(&txbuffer[totalCharsWritten], currentRMS, 5);
				totalCharsWritten += numcharswritten;
				txbuffer[totalCharsWritten++] = ',';
				// arg 2: voltage RMS
				numcharswritten = Int2String(&txbuffer[totalCharsWritten], voltageRMS, 6);
				totalCharsWritten += numcharswritten;
				txbuffer[totalCharsWritten++] = ',';
				// arg 3: ambient temp
				numcharswritten = Int2String(&txbuffer[totalCharsWritten], ambientTemp, 3);
				totalCharsWritten += numcharswritten;
				txbuffer[totalCharsWritten++] = ',';
				// arg 4: water temp
				numcharswritten = Int2String(&txbuffer[totalCharsWritten], heaterTemp, 3);
				totalCharsWritten += numcharswritten;
				txbuffer[totalCharsWritten++] = ',';
				// arg 5: accumulated water flow
				numcharswritten = Int2String(&txbuffer[totalCharsWritten], flowCount, 9);
				totalCharsWritten += numcharswritten;
				txbuffer[totalCharsWritten++] = ',';
				// arg 6: element status
				if(heaterState == 0)
				{
					txbuffer[totalCharsWritten++] = 'O'; txbuffer[totalCharsWritten++] = 'F'; txbuffer[totalCharsWritten++] = 'F'; txbuffer[totalCharsWritten++] = ',';
				}
				else
				{
					txbuffer[totalCharsWritten++] = 'O'; txbuffer[totalCharsWritten++] = 'N'; txbuffer[totalCharsWritten++] = ',';
				}
				// arg 7: valve status
				if(valveState == 0)
				{
					txbuffer[totalCharsWritten++] = 'C'; txbuffer[totalCharsWritten++] = 'L'; txbuffer[totalCharsWritten++] = 'O'; txbuffer[totalCharsWritten++] = 'S'; txbuffer[totalCharsWritten++] = 'E'; txbuffer[totalCharsWritten++] = 'D';
				}
				else
				{
					txbuffer[totalCharsWritten++] = 'O'; txbuffer[totalCharsWritten++] = 'P'; txbuffer[totalCharsWritten++] = 'E'; txbuffer[totalCharsWritten++] = 'N';
				}
				// \r\n
				txbuffer[totalCharsWritten++] = '\r'; txbuffer[totalCharsWritten++] = '\n';

				HAL_UART_Transmit(&huart1, (uint8_t*)txbuffer, totalCharsWritten, 1000);
				break;
			case 'L' :
				// request log entry

				break;
		}
	}
}


// String2Int - convert ASCII string to integer variable
uint8_t String2Int(char* input_string, int16_t* output_integer)
{
	int retval = 0;

	if (*input_string == '\0')
		return 0;

	int sign = 1;
	if (*input_string == '-')
	{
		sign = -1;
		input_string++;
	}

	while ((*input_string >= '0') && (*input_string <= '9'))
	{
		retval *= 10;
		retval += (*input_string - 48);

		if (((sign == 1) && (retval >= 32768)) ||
				((sign == -1) && (retval >= 32769)))
			return 0;

		input_string++;
	}
	*output_integer = (int16_t)(sign * retval);
	return 1;
}
// String2IntU8 - convert ASCII string to U8 variable
uint8_t String2IntU8(char* input_string, uint8_t* output_integer)
{
	int retval = 0;

	if (*input_string == '\0')
		return 0;

	if (*input_string == '-')
	{
		return 0;
	}

	while ((*input_string >= '0') && (*input_string <= '9'))
	{
		retval *= 10;
		retval += (*input_string - 48);

		if (retval >= 256)
			return 0;

		input_string++;
	}
	*output_integer = (uint8_t)retval;
	return 1;
}

// convert integer var to ASCII string
uint8_t Int2String(char* output_string, int32_t val, uint8_t maxlen)
{
	if (maxlen == 0)
		return 0;

	int numwritten = 0;

	if (val < 0)
	{
		output_string[0] = '-';
		output_string++;
		maxlen--;
		val = -val;
		numwritten = 1;
	}

	uint8_t digits = 0;
	if (val < 10)
		digits = 1;
	else if (val < 100)
		digits = 2;
	else if (val < 1000)
		digits = 3;
	else if (val < 10000)
		digits = 4;
	else if (val < 100000)
		digits = 5;
	else if (val < 1000000)
		digits = 6;
	else if (val < 10000000)
		digits = 7;
	else if (val < 100000000)
		digits = 8;
	else
		digits = 9;

	if (digits > maxlen)
		return 0; // error - not enough space in output string!

	int writepos = digits;
	while (writepos > 0)
	{
		output_string[writepos-1] = (char) ((val % 10) + 48);
		val /= 10;
		writepos--;
		numwritten++;
	}

	return numwritten;
}


//////////////////////////////////////////////////////////////////////
// to implement printf()
//////////////////////////////////////////////////////////////////////
int __io_putchar(int ch)
{
	uint8_t c[1];
	c[0] = ch & 0x00FF;
	HAL_UART_Transmit(&huart1, &c[0], 1, 10);
	return ch;
}
int _write(int file,char *ptr, int len)
{
	int DataIdx;
	for(DataIdx= 0; DataIdx< len; DataIdx++)
	{
		__io_putchar(*ptr++);
	}
	return len;
}
