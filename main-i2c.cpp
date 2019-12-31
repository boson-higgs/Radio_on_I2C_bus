// **************************************************************************
//
//
// File:         Main programm for I2C bus
//
// **************************************************************************

#include <mbed.h>
// minicom -D /dev/ttyACM0
#include "i2c-lib.h"
#include "si4735-lib.h"
#include <string>

//************************************************************************

// Direction of I2C communication
#define R	0b00000001
#define W	0b00000000

Serial pc( USBTX, USBRX );

#pragma GCC diagnostic ignored "-Wunused-but-set-variable"

DigitalIn button9(PTC9);
DigitalIn button10(PTC10);
DigitalIn volUp(PTC11);
DigitalIn volDown(PTC12);

class PCFExpander{
public:
	uint8_t address;
	PCFExpander(uint8_t address){this->address = address;};
	void SetLEDs(int size)
	{
		uint8_t ack = 0;
		i2c_start();
		ack |= i2c_output( address | W );
		ack |= i2c_output(size);
	}
	int LEDSelector(int number)
	{
		switch(number){
		case 0:
			return 0b00000000;
		case 1:
			return 0b00000001;
		case 2:
			return 0b00000011;
		case 3:
			return 0b00000111;
		case 4:
			return 0b00001111;
		case 5:
			return 0b00011111;
		case 6:
			return 0b00111111;
		case 7:
			return 0b01111111;
		case 8:
			return 0b11111111;
		}
		return 0;
	}



	uint8_t GetLeds();
};



class Radio{
public:
	Radio(){};
	void SetVolume(uint8_t volume)
	{
		uint8_t ack = 0;
		pc.printf( "\nVOLUME %d\r\n", volume);
		i2c_start();
		ack |= i2c_output(SI4735_ADDRESS | W);
		ack |= i2c_output(0x12);
		ack |= i2c_output(0x00);
		ack |= i2c_output(0x40);
		ack |= i2c_output(0x00);
		ack |= i2c_output(0x00);
		ack |= i2c_output(volume);
		i2c_stop();
	}
	void SetFrequency(int frequency)
	{
		pc.printf( "\nTunig of radio station...\r\n" );
		uint8_t ack = 0;
		i2c_start();
		ack |= i2c_output( SI4735_ADDRESS | W );
		ack |= i2c_output( 0x20 );
		ack |= i2c_output( 0x00 );
		ack |= i2c_output( frequency >> 8 );
		ack |= i2c_output( frequency & 0xff );
		ack |= i2c_output( 0x00 );
		i2c_stop();
		wait_ms( 100 );
		pc.printf( "... station tuned.\r\n\n" );
	}
	void Seek(int direction)
	{
		uint8_t ack = 0;
		i2c_start();
		ack |= i2c_output( SI4735_ADDRESS | W );
		ack |= i2c_output( 0x21 );
		ack |= i2c_output( 0 | direction<<3 );
		i2c_stop();
	}




	void RDS(){
		uint8_t RDS0, RDS1, Sync, RDS3, B1HI, B1LO, B2HI, B2LO, B3HI, B3LO, B4HI, B4LO, Status;
		uint8_t ack = 0;
		i2c_start();
		ack |= i2c_output( SI4735_ADDRESS | W );
		ack |= i2c_output( 0x24 );
		ack |= i2c_output( 1 );
		// repeated start
		i2c_start();
		ack |= i2c_output( SI4735_ADDRESS | R );
		RDS0 = i2c_input();
		i2c_ack();
		RDS1 = i2c_input();
		i2c_ack();
		Sync = i2c_input() & 1;
		i2c_ack();
		if(Sync == 0) //sync = 0 -> spatne data
		{
			pc.printf("RDS informace nejsou synchronizovany");
			return;
		}
		RDS3 = i2c_input();
		i2c_ack();
		B1HI = i2c_input();
		i2c_ack();
		B1LO = i2c_input();
		i2c_ack();
		B2HI = i2c_input();
		i2c_ack();
		B2LO = i2c_input();
		i2c_ack();
		B3HI = i2c_input();
		i2c_ack();
		B3LO = i2c_input();
		i2c_ack();
		B4HI = i2c_input();
		i2c_ack();
		B4LO = i2c_input();
		i2c_ack();
		Status = i2c_input();
		i2c_nack();
		i2c_stop();

		int blok1 = B1HI<<8 | B1LO;
		int blok2 = B2HI<<8 | B2LO;
		int blok3 = B3HI<<8 | B3LO;
		int blok4 = B4HI<<8 | B4LO;

		Status & 0b00111111;

		int skupinaCislo = (blok2 & 0xF000) >>12;
		char skupinaZnak;
		if((blok2 & 0x800)>>11 == 1){
			skupinaZnak = 'B';
		}
		else skupinaZnak = 'A';
		pc.printf("Identifikace skupiny: %d%c\r\n", skupinaCislo, skupinaZnak);
	}



	void PrintRT()
	{
	    char text[64];
	    for(int i = 0; i < 64; i++){
	        text[i] = char(0);
	    }
	    //bool isComplete;
	    while(true)
	    {

	            uint8_t RDS0, RDS1, Sync, RDS3, B1HI, B1LO, B2HI, B2LO, B3HI, B3LO, B4HI, B4LO, Status;
	            uint8_t ack = 0;
	            while ((B2HI >> 3) != 0b00100)
	            {
	                i2c_start();
	                ack =  ack | i2c_output( SI4735_ADDRESS | W );
	                ack =  ack | i2c_output( 0x24 );
	                ack =  ack | i2c_output( 1 );
	                i2c_start();
	                ack =  ack | i2c_output( SI4735_ADDRESS | R );
	                RDS0 = i2c_input();
	                i2c_ack();
	                RDS1 = i2c_input();
	                i2c_ack();
	                Sync = i2c_input() & 1;

	                if(Sync == 0) //sync = 0 -> spatne data
	                {
	                	pc.printf("RDS informace nejsou synchronizovany");
	                	i2c_nack();
	                	i2c_stop();
	                	break;
	                }
	                i2c_ack();
	                RDS3 = i2c_input();
	                i2c_ack();
	                B1HI = i2c_input();
	                i2c_ack();
	                B1LO = i2c_input();
	                i2c_ack();
	                B2HI = i2c_input();
	                i2c_ack();
	                B2LO = i2c_input();
	                i2c_ack();
	                B3HI = i2c_input();
	                i2c_ack();
	                B3LO = i2c_input();
	                i2c_ack();
	                B4HI = i2c_input();
	                i2c_ack();
	                B4LO = i2c_input();
	                i2c_ack();
	                Status = i2c_input();
	                i2c_nack();
	                i2c_stop();
	            }

	            int blok2 = B2HI<<8 | B2LO;
	            int blok3 = B3HI<<8 | B3LO;
	            int blok4 = B4HI<<8 | B4LO;

	            int pozice = blok2 & 0xF;
	            text[pozice*4] = (char) B3HI;
	            text[pozice*4 + 1] = (char) B3LO;
	            text[pozice*4 + 2] = (char) B4HI;
	            text[pozice*4 + 3] = (char) B4LO;
	            for (int i = 0; i < 64; i++)
	            {
	                pc.printf("%c", text[i]);
	            }



	      }


	}



	void ReadFrequency(){
		uint8_t S1, S2, RSSI, SNR, MULT, CAP, freq;		//do tychto premennych si nacitame odpoved z radia, vid strana 35 status of receiver
		uint8_t ack = 0;
		i2c_start();								//zaciatok komunikacie
		ack |= i2c_output( SI4735_ADDRESS | W );	//zaadresovanie radia
		ack |= i2c_output( 0x22 );			// FM_TUNE_STATUS - poziadavka na odoslanie aktualne naladenej frekvencie
		ack |= i2c_output( 0x00 );			// ARG1
		// repeated start
		i2c_start();					//zopakovany start, zmenime smer komunikacie zo zapisu na citanie
		ack |= i2c_output( SI4735_ADDRESS | R );	// change direction of communication - read data
		S1 = i2c_input();
		i2c_ack();
		S2 = i2c_input();
		i2c_ack();
		freq = (int) i2c_input() << 8;
		i2c_ack();
		freq |= i2c_input();
		i2c_ack();
		RSSI = i2c_input();
		i2c_ack();
		SNR = i2c_input();
		i2c_ack();
		MULT = i2c_input();
		i2c_ack();
		CAP = i2c_input();
		i2c_nack();					//koniec citania, negovany ACK
		i2c_stop();					//koniec komunikacie
		if ( ack != 0 )
			printf( "Communication error!\r\n" );
		else
			printf( "Current tuned frequency: %d.%dMHz\r\n", freq / 100, freq % 100 );
	}



};

int main(void)
{
	// Serial line initialization
	pc.baud(115200);
	pc.printf("K64F-KIT ready...\r\n");

	uint8_t l_S1, l_S2, l_RSSI, l_SNR, l_MULT, l_CAP;
	uint8_t l_ack = 0;
	int wrap = 0;
	int freq = 10140; // Radiozurnal
	int vol = 0;

	Radio r;
	PCFExpander pcf(0b0100 << 4 | 0b000 << 1);
	uint8_t ack = 0;

	i2c_init();
	si4735_init();

	r.SetFrequency(freq);
	r.ReadFrequency();
	while(1)
	{
		if(button10 == 0)
		{
			r.PrintRT();
		}
	}



	while(1)
	{
		if(volUp==0)
		{
			if(vol<63)
			{
				vol+=8;
				r.SetVolume(vol);
			}


		}
			if(volDown==0)
			{
				if(vol>0)
				{
				vol-=8;
				r.SetVolume(vol);
				}

			}








			/*if(SeekUp==0)
			{
				r.Seek(0);
			}

			if(SeekDown == 0)
			{
				r.Seek(1);
			}*/

	}
	return 0;

}
