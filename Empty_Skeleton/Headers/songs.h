
#define OFF		100

#define C4 		3831
#define CS4 	3610
#define D4 		3401
#define DS4 	3215
#define E4 		3030
#define F4 		2857
#define FS4 	2725
#define G4 		2551
#define GS4 	2410
#define A4 		2272
#define AS4 	2146
#define B4 		2024

#define C5 		C4/2
#define CS5 	CS4/2
#define D5 		D4/2
#define DS5 	DS4/2
#define E5 		E4/2
#define F5 		F4/2
#define FS5 	FS4/2
#define G5 		G4/2
#define GS5 	GS4/2
#define A5 		A4/2
#define AS5 	AS4/2
#define B5 		B4/2

#define C6 		C5/2
#define CS6 	CS5/2
#define D6 		D5/2
#define DS6 	DS5/2
#define E6 		E5/2
#define F6 		F5/2
#define FS6 	FS5/2
#define G6 		G5/2
#define GS6 	GS5/2
#define A6 		A5/2
#define AS6 	AS5/2
#define B6 		B5/2

#define C7 		C6/2
#define CS7 	CS6/2
#define D7 		D6/2
#define DS7 	DS6/2
#define E7 		E6/2
#define F7 		F6/2
#define FS7 	FS6/2
#define G7 		G6/2
#define GS7 	GS6/2
#define A7 		A6/2
#define AS7 	AS6/2
#define B7 		B6/2

#define C3 		C4*2
#define CS3 	CS4*2
#define D3 		D4*2
#define DS3 	DS4*2
#define E3 		E4*2
#define F3 		F4*2
#define FS3 	FS4*2
#define G3 		G4*2
#define GS3 	GS4*2
#define A3 		A4*2
#define AS3 	AS4*2
#define B3 		B4*2

#define C2 		C3*2
#define CS2 	CS3*2
#define D2 		D3*2
#define DS2 	DS3*2
#define E2 		E3*2
#define F2 		F3*2
#define FS2 	FS3*2
#define G2 		G3*2
#define GS2 	GS3*2
#define A2 		A3*2
#define AS2 	AS3*2
#define B2 		B3*2

#define C1 		C2*2
#define CS1	  CS2*2
#define D1 		D2*2
#define DS1 	DS2*2
#define E1		E2*2
#define F1 		F2*2
#define FS1 	FS2*2
#define G1 		G2*2
#define GS1 	GS2*2
#define A1 		A2*2
#define AS1 	AS2*2
#define B1 		B2*2

struct tone {
	int duration;
	int pitch;
	int volume;
};
void ring_doorbell(void);

//int song_duration[];
//struct tone song_data[];
void play_tone(unsigned int duration, int period, int vol);
void udelay(unsigned int delay_in_us);
void dawn_wakeup_alarm();
void standard_wakeup_alarm();
