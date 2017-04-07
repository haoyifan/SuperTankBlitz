/*
 * rfid.c
 *
 *  Created on: Mar 22, 2016
 *      Author: haoyifan
 */

#include "rfid.h"
//#include <stdio.h>

#define MAX_LEN 16
byte str[MAX_LEN];

void AddicoreRFID_Init(void)
{
	// this gpio port is used to reset the rfid
	MSS_GPIO_init();
	MSS_GPIO_config( MSS_GPIO_0, MSS_GPIO_OUTPUT_MODE);

	MSS_SPI_init( &g_mss_spi1 );
	MSS_SPI_configure_master_mode
	(
			&g_mss_spi1,
			MSS_SPI_SLAVE_0,
			MSS_SPI_MODE0,
			MSS_SPI_PCLK_DIV_8,
			16
	);

	// reset code
	MSS_SPI_set_slave_select( &g_mss_spi1, MSS_SPI_SLAVE_0 );
	MSS_GPIO_set_output
	(
	    MSS_GPIO_0,
	    0x01
	);
	delay();
	MSS_SPI_clear_slave_select( &g_mss_spi1, MSS_SPI_SLAVE_0 );

	// set the mode, timer and frequency
    Write_AddicoreRFID(TModeReg, 0x8F);		//Tauto=1; f(Timer) = 6.78MHz/TPreScaler
    Write_AddicoreRFID(TPrescalerReg, 0xFF);	//TModeReg[3..0] + TPrescalerReg
    Write_AddicoreRFID(TReloadRegL, 0x00);
    Write_AddicoreRFID(TReloadRegH, 0x8F);

	Write_AddicoreRFID(TxAutoReg, 0x40);		//100%ASK
	Write_AddicoreRFID(ModeReg, 0x3D);		//CRC Initial value 0x6363	???

	// turn on the antenna
	AntennaOn();		//Open the antenna
}



void Write_AddicoreRFID(byte addr, byte val)
{
	uint16_t tx_frame = (((((addr<<1)&0x7E)))<<8) + val;

	MSS_SPI_set_slave_select( &g_mss_spi1, MSS_SPI_SLAVE_0 );
	MSS_SPI_transfer_frame( &g_mss_spi1, tx_frame );
	MSS_SPI_clear_slave_select( &g_mss_spi1, MSS_SPI_SLAVE_0 );
}


byte Read_AddicoreRFID(byte addr)
{
	uint8_t val;
	uint16_t tx_frame = ((((addr<<1)&0x7E) | 0x80)<<8);

	MSS_SPI_set_slave_select( &g_mss_spi1, MSS_SPI_SLAVE_0 );
	val = MSS_SPI_transfer_frame( &g_mss_spi1, tx_frame );
	MSS_SPI_clear_slave_select( &g_mss_spi1, MSS_SPI_SLAVE_0 );
	return val;
}


void SetBitMask(byte reg, byte mask)
{
    byte tmp;
    tmp = Read_AddicoreRFID(reg);
    Write_AddicoreRFID(reg, tmp | mask);  // set bit mask
}


void ClearBitMask(byte reg, byte mask)
{
    byte tmp;
    tmp = Read_AddicoreRFID(reg);
    Write_AddicoreRFID(reg, tmp & (~mask));  // clear bit mask
}


void AntennaOn(void)
{
	byte temp;

	temp = Read_AddicoreRFID(TxControlReg);
	if (!(temp & 0x03))
	{
		SetBitMask(TxControlReg, 0x03);
	}
}

void AddicoreRFID_Reset(void)
{
    Write_AddicoreRFID(CommandReg, PCD_SOFTRESET);
}


byte AddicoreRFID_Request(byte reqMode, byte *TagType)
{
	byte status = 0;

	Write_AddicoreRFID(BitFramingReg, 0x07);

	TagType[0] = reqMode;
	status = AddicoreRFID_ToCard(PCD_TRANSCEIVE, TagType, 1, TagType, &_RxBits);

	if ((status != MI_OK) || (_RxBits != 0x10))
	{
		status = MI_ERR;
	}

	return status;
}


byte AddicoreRFID_ToCard(byte command, byte *sendData, byte sendLen, byte *backData, uint32_t *backLen)
{
    byte status = MI_ERR;
    byte irqEn = 0x00;
    byte waitIRq = 0x00;
    byte lastBits;
    byte n;
    uint32_t i;

    switch (command)
    {
        case PCD_MFAUTHENT:		//Certification cards close
		{
			irqEn = 0x12;
			waitIRq = 0x10;
			break;
		}
		case PCD_TRANSCEIVE:	//Transmit FIFO data
		{
			irqEn = 0x77;
			waitIRq = 0x30;
			break;
		}
		default:
			break;
    }

    Write_AddicoreRFID(ComIrqReg, irqEn|0x80);	//Interrupt request
    ClearBitMask(ComIrqReg, 0x80);			//Clear all interrupt request bit
    SetBitMask(FIFOLevelReg, 0x80);			//FlushBuffer=1, FIFO Initialization

	Write_AddicoreRFID(CommandReg, PCD_IDLE);	//NO action; Cancel the current command???

	//Writing data to the FIFO
    for (i=0; i<sendLen; i++)
    {
		Write_AddicoreRFID(FIFODataReg, sendData[i]);
	}

	//Execute the command
	Write_AddicoreRFID(CommandReg, command);
    if (command == PCD_TRANSCEIVE)
    {
		SetBitMask(BitFramingReg, 0x80);		//StartSend=1,transmission of data starts
	}

	//Waiting to receive data to complete
	i = 10000;	//i according to the clock frequency adjustment, the operator M1 card maximum waiting time 25ms???
    do
    {
		//ComIrqReg[7..0]
		//Set1 TxIRq RxIRq IdleIRq HiAlerIRq LoAlertIRq ErrIRq TimerIRq
    	//n = Read_AddicoreRFID(CommandReg);
        n = Read_AddicoreRFID(ComIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x01) && !(n&waitIRq));

    ClearBitMask(BitFramingReg, 0x80);			//StartSend=0

    if (i != 0)
    {
        if(!(Read_AddicoreRFID(ErrorReg) & 0x1B))	//BufferOvfl Collerr CRCErr ProtecolErr
        {
            status = MI_OK;
            if (n & irqEn & 0x01)
            {
				status = MI_NO_TAG_ERR;			//??
			}

            if (command == PCD_TRANSCEIVE)
            {
               	n = Read_AddicoreRFID(FIFOLevelReg);
              	lastBits = Read_AddicoreRFID(ControlReg) & 0x07;
                if (lastBits)
                {
					*backLen = (n-1)*8 + lastBits;
				}
                else
                {
					*backLen = n*8;
				}

                if (n == 0)
                {
					n = 1;
				}
                if (n > MAX_LEN)
                {
					n = MAX_LEN;
				}

				//Reading the received data in FIFO
                for (i=0; i<n; i++)
                {
					backData[i] = Read_AddicoreRFID(FIFODataReg);
				}
            }
        }
        else
        {
			status = MI_ERR;
		}

    }

    SetBitMask(ControlReg,0x80);           //timer stops
    Write_AddicoreRFID(CommandReg, PCD_IDLE);

    return status;
}


byte AddicoreRFID_Anticoll(byte *serNum)
{
    byte status;
    byte i;
	byte serNumCheck=0;
    //uint unLen;

	Write_AddicoreRFID(BitFramingReg, 0x00);		//TxLastBists = BitFramingReg[2..0]

    serNum[0] = PICC_ANTICOLL;
    serNum[1] = 0x20;
    status = AddicoreRFID_ToCard(PCD_TRANSCEIVE, serNum, 2, serNum, &_RxBits);

    if (status == MI_OK)
	{
		//Check card serial number
		for (i=0; i<4; i++)
		{
		 	serNumCheck ^= serNum[i];
		}
		if (serNumCheck != serNum[i])
		{
			status = MI_ERR;
		}
    }

    return status;
}

void CalulateCRC(byte *pIndata, byte len, byte *pOutData)
{
    byte i, n;

    ClearBitMask(DivIrqReg, 0x04);			//CRCIrq = 0
    SetBitMask(FIFOLevelReg, 0x80);			//Clear the FIFO pointer
    Write_AddicoreRFID(CommandReg, PCD_IDLE);

	//Writing data to the FIFO
    for (i=0; i<len; i++)
    {
		Write_AddicoreRFID(FIFODataReg, *(pIndata+i));
	}
    Write_AddicoreRFID(CommandReg, PCD_CALCCRC);

	//Wait CRC calculation is complete
    i = 0xFF;
    do
    {
        n = Read_AddicoreRFID(DivIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x04));			//CRCIrq = 1

	//Read CRC calculation result
    pOutData[0] = Read_AddicoreRFID(CRCResultRegL);
    pOutData[1] = Read_AddicoreRFID(CRCResultRegM);
}

void delay()
{
	int x = 0;
	for(; x < 1000000; x++)
	{}
}

uint8_t reachRFID()
{
	uint32_t checksum1;
	uint32_t status;
	uint8_t str[MAX_LEN];


	str[1] = 0;
	//Find tags, return tag type
	status = AddicoreRFID_Request(PICC_REQIDL, str);
	//if (status == MI_OK)
	//{
		//printf("RFID tag detected\n\r");
		//printf("%d, %d\n\r", str[0], str[1]);
	//}

	//Anti-collision, return tag serial number 4 bytes
	status = AddicoreRFID_Anticoll(str);
	if (status == MI_OK)
	{
		checksum1 = str[0] ^ str[1] ^ str[2] ^ str[3];
		return str[0];
	}
	return 0;
}
