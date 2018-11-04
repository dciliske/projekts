/*--------------------------------------------------------------------------
N76E003.H

Header file for Nuvoton N76E003
--------------------------------------------------------------------------*/

#ifdef __KEIL
sfr P0					= 0x80;
sfr SP					= 0x81;
sfr DPL					= 0x82;
sfr DPH					= 0x83;
sfr RCTRIM0			= 0x84;
sfr RCTRIM1			= 0x85;
sfr RWK         = 0x86;
sfr PCON        = 0x87;

sfr TCON        = 0x88;
sfr TMOD        = 0x89;
sfr TL0         = 0x8A;
sfr TL1         = 0x8B;
sfr TH0         = 0x8C;
sfr TH1         = 0x8D;
sfr CKCON       = 0x8E;
sfr WKCON       = 0x8F;

sfr P1          = 0x90;
sfr SFRS        = 0x91; //TA Protection
sfr CAPCON0     = 0x92;
sfr CAPCON1     = 0x93;
sfr CAPCON2     = 0x94;
sfr CKDIV       = 0x95;
sfr CKSWT       = 0x96; //TA Protection
sfr CKEN        = 0x97; //TA Protection

sfr SCON        = 0x98;
sfr SBUF        = 0x99;
sfr SBUF_1      = 0x9A;
sfr EIE         = 0x9B;
sfr EIE1        = 0x9C;
sfr CHPCON      = 0x9F; //TA Protection

sfr P2          = 0xA0;
sfr AUXR1       = 0xA2;
sfr BODCON0     = 0xA3; //TA Protection
sfr IAPTRG      = 0xA4; //TA Protection
sfr IAPUEN      = 0xA5;	//TA Protection
sfr IAPAL       = 0xA6;
sfr IAPAH       = 0xA7;

sfr IE          = 0xA8;
sfr SADDR       = 0xA9;
sfr WDCON       = 0xAA; //TA Protection
sfr BODCON1     = 0xAB; //TA Protection
sfr P3M1        = 0xAC;
sfr P3S         = 0xAC; //Page1
sfr P3M2        = 0xAD;
sfr P3SR        = 0xAD; //Page1
sfr IAPFD       = 0xAE;
sfr IAPCN       = 0xAF;

sfr P3          = 0xB0;
sfr P0M1        = 0xB1;
sfr P0S         = 0xB1; //Page1
sfr P0M2        = 0xB2;
sfr P0SR        = 0xB2; //Page1
sfr P1M1        = 0xB3;
sfr P1S         = 0xB3; //Page1
sfr P1M2        = 0xB4;
sfr P1SR        = 0xB4; //Page1
sfr P2S         = 0xB5;
sfr IPH         = 0xB7;
sfr PWMINTC			= 0xB7;	//Page1

sfr IP          = 0xB8;
sfr SADEN       = 0xB9;
sfr SADEN_1     = 0xBA;
sfr SADDR_1     = 0xBB;
sfr I2DAT       = 0xBC;
sfr I2STAT      = 0xBD;
sfr I2CLK       = 0xBE;
sfr I2TOC       = 0xBF;

sfr I2CON       = 0xC0;
sfr I2ADDR      = 0xC1;
sfr ADCRL       = 0xC2;
sfr ADCRH       = 0xC3;
sfr T3CON       = 0xC4;
sfr PWM4H       = 0xC4; //Page1
sfr RL3         = 0xC5;
sfr PWM5H       = 0xC5;	//Page1
sfr RH3         = 0xC6;
sfr PIOCON1     = 0xC6; //Page1
sfr TA          = 0xC7;

sfr T2CON       = 0xC8;
sfr T2MOD       = 0xC9;
sfr RCMP2L      = 0xCA;
sfr RCMP2H      = 0xCB;
sfr TL2         = 0xCC;
sfr PWM4L       = 0xCC; //Page1
sfr TH2         = 0xCD;
sfr PWM5L       = 0xCD; //Page1
sfr ADCMPL      = 0xCE;
sfr ADCMPH      = 0xCF;

sfr PSW         = 0xD0;
sfr PWMPH       = 0xD1;
sfr PWM0H				= 0xD2;
sfr PWM1H				= 0xD3;
sfr PWM2H				= 0xD4;
sfr PWM3H				= 0xD5;
sfr PNP					= 0xD6;
sfr FBD					= 0xD7;

sfr PWMCON0			= 0xD8;
sfr PWMPL       = 0xD9;
sfr PWM0L				= 0xDA;
sfr PWM1L				= 0xDB;
sfr PWM2L				= 0xDC;
sfr PWM3L				= 0xDD;
sfr PIOCON0			= 0xDE;
sfr PWMCON1     = 0xDF;

sfr ACC         = 0xE0;
sfr ADCCON1     = 0xE1;
sfr ADCCON2     = 0xE2;
sfr ADCDLY      = 0xE3;
sfr C0L         = 0xE4;
sfr C0H         = 0xE5;
sfr C1L         = 0xE6;
sfr C1H         = 0xE7;

sfr ADCCON0     = 0xE8;
sfr PICON       = 0xE9;
sfr PINEN       = 0xEA;
sfr PIPEN       = 0xEB;
sfr PIF         = 0xEC;
sfr C2L         = 0xED;
sfr C2H         = 0xEE;
sfr EIP         = 0xEF;

sfr B           = 0xF0;
sfr CAPCON3			= 0xF1;
sfr CAPCON4			= 0xF2;
sfr SPCR        = 0xF3;
sfr SPCR2				= 0xF3; //Page1
sfr SPSR        = 0xF4;
sfr SPDR        = 0xF5;
sfr AINDIDS			= 0xF6;
sfr EIPH        = 0xF7;

sfr SCON_1      = 0xF8;
sfr PDTEN       = 0xF9; //TA Protection
sfr PDTCNT      = 0xFA; //TA Protection
sfr PMEN        = 0xFB;
sfr PMD         = 0xFC;
sfr EIP1        = 0xFE;
sfr EIPH1       = 0xFF;

/*  BIT Registers  */
/*  SCON_1  */
sbit SM0_1      = SCON_1^7;
sbit FE_1       = SCON_1^7;
sbit SM1_1      = SCON_1^6;
sbit SM2_1      = SCON_1^5;
sbit REN_1      = SCON_1^4;
sbit TB8_1      = SCON_1^3;
sbit RB8_1      = SCON_1^2;
sbit TI_1       = SCON_1^1;
sbit RI_1       = SCON_1^0;

/*  ADCCON0  */
sbit ADCF       = ADCCON0^7;
sbit ADCS       = ADCCON0^6;
sbit ETGSEL1    = ADCCON0^5;
sbit ETGSEL0    = ADCCON0^4;
sbit ADCHS3     = ADCCON0^3;
sbit ADCHS2     = ADCCON0^2;
sbit ADCHS1     = ADCCON0^1;
sbit ADCHS0     = ADCCON0^0;

/*  PWMCON0  */
sbit PWMRUN     = PWMCON0^7;
sbit LOAD       = PWMCON0^6;
sbit PWMF       = PWMCON0^5;
sbit CLRPWM     = PWMCON0^4;


/*  PSW */
sbit CY         = PSW^7;
sbit AC         = PSW^6;
sbit F0         = PSW^5;
sbit RS1        = PSW^4;
sbit RS0        = PSW^3;
sbit OV         = PSW^2;
sbit P          = PSW^0;

/*  T2CON  */
sbit TF2        = T2CON^7;
sbit TR2        = T2CON^2;
sbit CM_RL2     = T2CON^0;

/*  I2CON  */
sbit I2CEN      = I2CON^6;
sbit STA        = I2CON^5;
sbit STO        = I2CON^4;
sbit SI         = I2CON^3;
sbit AA         = I2CON^2;
sbit I2CPX	= I2CON^0;

/*  IP  */
sbit PADC       = IP^6;
sbit PBOD       = IP^5;
sbit PS         = IP^4;
sbit PT1        = IP^3;
sbit PX1        = IP^2;
sbit PT0        = IP^1;
sbit PX0        = IP^0;

/*  P3  */
sbit P30		= P3^0;


/*  IE  */
sbit EA         = IE^7;
sbit EADC       = IE^6;
sbit EBOD       = IE^5;
sbit ES         = IE^4;
sbit ET1        = IE^3;
sbit EX1        = IE^2;
sbit ET0        = IE^1;
sbit EX0        = IE^0;

/*  P2  */
sbit P20        = P2^0;

/*  SCON  */
sbit SM0        = SCON^7;
sbit FE         = SCON^7;
sbit SM1        = SCON^6;
sbit SM2        = SCON^5;
sbit REN        = SCON^4;
sbit TB8        = SCON^3;
sbit RB8        = SCON^2;
sbit TI         = SCON^1;
sbit RI         = SCON^0;

/*  P1  */
sbit P17	= P1^7;
sbit P16	= P1^6;
sbit TXD_1	= P1^6;
sbit P15	= P1^5;
sbit P14	= P1^4;
sbit SDA	= P1^4;
sbit P13	= P1^3;
sbit SCL	= P1^3;
sbit P12        = P1^2;
sbit P11        = P1^1;
sbit P10        = P1^0;

/*  TCON  */
sbit TF1				= TCON^7;
sbit TR1				= TCON^6;
sbit TF0				= TCON^5;
sbit TR0				= TCON^4;
sbit IE1				= TCON^3;
sbit IT1				= TCON^2;
sbit IE0				= TCON^1;
sbit IT0				= TCON^0;

/*  P0  */

sbit P07				= P0^7;
sbit RXD				= P0^7;
sbit P06				= P0^6;
sbit TXD				= P0^6;
sbit P05				= P0^5;
sbit P04				= P0^4;
sbit STADC				= P0^4;
sbit P03				= P0^3;
sbit P02				= P0^2;
sbit RXD_1				= P0^2;
sbit P01				= P0^1;
sbit MISO				= P0^1;
sbit P00				= P0^0;
sbit MOSI				= P0^0;

#elif defined __SDCC
__sfr __at(0x80) P0;
__sfr __at(0x81) SP;
__sfr __at(0x82) DPL;
__sfr __at(0x83) DPH;
__sfr __at(0x84) RCTRIM0;
__sfr __at(0x85) RCTRIM1;
__sfr __at(0x86) RWK;
__sfr __at(0x87) PCON;

__sfr __at(0x88) TCON;
__sfr __at(0x89) TMOD;
__sfr __at(0x8A) TL0;
__sfr __at(0x8B) TL1;
__sfr __at(0x8C) TH0;
__sfr __at(0x8D) TH1;
__sfr __at(0x8E) CKCON;
__sfr __at(0x8F) WKCON;

__sfr __at(0x90) P1;
__sfr __at(0x91) SFRS; //TA Protection
__sfr __at(0x92) CAPCON0;
__sfr __at(0x93) CAPCON1;
__sfr __at(0x94) CAPCON2;
__sfr __at(0x95) CKDIV;
__sfr __at(0x96) CKSWT; //TA Protection
__sfr __at(0x97) CKEN; //TA Protection

__sfr __at(0x98) SCON;
__sfr __at(0x99) SBUF;
__sfr __at(0x9A) SBUF_1;
__sfr __at(0x9B) EIE;
__sfr __at(0x9C) EIE1;
__sfr __at(0x9F) CHPCON; //TA Protection

__sfr __at(0xA0) P2;
__sfr __at(0xA2) AUXR1;
__sfr __at(0xA3) BODCON0; //TA Protection
__sfr __at(0xA4) IAPTRG; //TA Protection
__sfr __at(0xA5) IAPUEN;	//TA Protection
__sfr __at(0xA6) IAPAL;
__sfr __at(0xA7) IAPAH;

__sfr __at(0xA8) IE;
__sfr __at(0xA9) SADDR;
__sfr __at(0xAA) WDCON; //TA Protection
__sfr __at(0xAB) BODCON1; //TA Protection
__sfr __at(0xAC) P3M1;
__sfr __at(0xAC) P3S; //Page1
__sfr __at(0xAD) P3M2;
__sfr __at(0xAD) P3SR; //Page1
__sfr __at(0xAE) IAPFD;
__sfr __at(0xAF) IAPCN;

__sfr __at(0xB0) P3;
__sfr __at(0xB1) P0M1;
__sfr __at(0xB1) P0S; //Page1
__sfr __at(0xB2) P0M2;
__sfr __at(0xB2) P0SR; //Page1
__sfr __at(0xB3) P1M1;
__sfr __at(0xB3) P1S; //Page1
__sfr __at(0xB4) P1M2;
__sfr __at(0xB4) P1SR; //Page1
__sfr __at(0xB5) P2S;
__sfr __at(0xB7) IPH;
__sfr __at(0xB7) PWMINTC;	//Page1

__sfr __at(0xB8) IP;
__sfr __at(0xB9) SADEN;
__sfr __at(0xBA) SADEN_1;
__sfr __at(0xBB) SADDR_1;
__sfr __at(0xBC) I2DAT;
__sfr __at(0xBD) I2STAT;
__sfr __at(0xBE) I2CLK;
__sfr __at(0xBF) I2TOC;

__sfr __at(0xC0) I2CON;
__sfr __at(0xC1) I2ADDR;
__sfr __at(0xC2) ADCRL;
__sfr __at(0xC3) ADCRH;
__sfr __at(0xC4) T3CON;
__sfr __at(0xC4) PWM4H; //Page1
__sfr __at(0xC5) RL3;
__sfr __at(0xC5) PWM5H;	//Page1
__sfr __at(0xC6) RH3;
__sfr __at(0xC6) PIOCON1; //Page1
__sfr __at(0xC7) TA;

__sfr __at(0xC8) T2CON;
__sfr __at(0xC9) T2MOD;
__sfr __at(0xCA) RCMP2L;
__sfr __at(0xCB) RCMP2H;
__sfr __at(0xCC) TL2;
__sfr __at(0xCC) PWM4L; //Page1
__sfr __at(0xCD) TH2;
__sfr __at(0xCD) PWM5L; //Page1
__sfr __at(0xCE) ADCMPL;
__sfr __at(0xCF) ADCMPH;

__sfr __at(0xD0) PSW;
__sfr __at(0xD1) PWMPH;
__sfr __at(0xD2) PWM0H;
__sfr __at(0xD3) PWM1H;
__sfr __at(0xD4) PWM2H;
__sfr __at(0xD5) PWM3H;
__sfr __at(0xD6) PNP;
__sfr __at(0xD7) FBD;

__sfr __at(0xD8) PWMCON0;
__sfr __at(0xD9) PWMPL;
__sfr __at(0xDA) PWM0L;
__sfr __at(0xDB) PWM1L;
__sfr __at(0xDC) PWM2L;
__sfr __at(0xDD) PWM3L;
__sfr __at(0xDE) PIOCON0;
__sfr __at(0xDF) PWMCON1;

__sfr __at(0xE0) ACC;
__sfr __at(0xE1) ADCCON1;
__sfr __at(0xE2) ADCCON2;
__sfr __at(0xE3) ADCDLY;
__sfr __at(0xE4) C0L;
__sfr __at(0xE5) C0H;
__sfr __at(0xE6) C1L;
__sfr __at(0xE7) C1H;

__sfr __at(0xE8) ADCCON0;
__sfr __at(0xE9) PICON;
__sfr __at(0xEA) PINEN;
__sfr __at(0xEB) PIPEN;
__sfr __at(0xEC) PIF;
__sfr __at(0xED) C2L;
__sfr __at(0xEE) C2H;
__sfr __at(0xEF) EIP;

__sfr __at(0xF0) B;
__sfr __at(0xF1) CAPCON3;
__sfr __at(0xF2) CAPCON4;
__sfr __at(0xF3) SPCR;
__sfr __at(0xF3) SPCR2; //Page1
__sfr __at(0xF4) SPSR;
__sfr __at(0xF5) SPDR;
__sfr __at(0xF6) AINDIDS;
__sfr __at(0xF7) EIPH;

__sfr __at(0xF8) SCON_1;
__sfr __at(0xF9) PDTEN; //TA Protection
__sfr __at(0xFA) PDTCNT; //TA Protection
__sfr __at(0xFB) PMEN;
__sfr __at(0xFC) PMD;
__sfr __at(0xFE) EIP1;
__sfr __at(0xFF) EIPH1;


/*  BIT Registers  */
/*  SCON_1  */
__sbit __at(0xFF) SM0_1;
__sbit __at(0xFF) FE_1;
__sbit __at(0xFE) SM1_1;
__sbit __at(0xFD) SM2_1;
__sbit __at(0xFC) REN_1;
__sbit __at(0xFB) TB8_1;
__sbit __at(0xFA) RB8_1;
__sbit __at(0xF9) TI_1;
__sbit __at(0xF8) RI_1;

/*  ADCCON0  */
__sbit __at(0xEF) ADCF;
__sbit __at(0xEE) ADCS;
__sbit __at(0xED) ETGSEL1;
__sbit __at(0xEC) ETGSEL0;
__sbit __at(0xEB) ADCHS3;
__sbit __at(0xEA) ADCHS2;
__sbit __at(0xE9) ADCHS1;
__sbit __at(0xE0) ADCHS0;

/*  PWMCON0  */
__sbit __at(0xDF) PWMRUN;
__sbit __at(0xDE) LOAD;
__sbit __at(0xDD) PWMF;
__sbit __at(0xDC) CLRPWM;


/*  PSW */
__sbit __at(0xD7) CY;
__sbit __at(0xD6) AC;
__sbit __at(0xD5) F0;
__sbit __at(0xD4) RS1;
__sbit __at(0xD3) RS0;
__sbit __at(0xD2) OV;
__sbit __at(0xD1) P;

/*  T2CON  */
__sbit __at(0xCF) TF2;
__sbit __at(0xCA) TR2;
__sbit __at(0xC8) CM_RL2;

/*  I2CON  */
__sbit __at(0xC6) I2CEN;
__sbit __at(0xC5) STA;
__sbit __at(0xC4) STO;
__sbit __at(0xC3) SI;
__sbit __at(0xC2) AA;
__sbit __at(0xC0) I2CPX;

/*  IP  */
__sbit __at(0xBE) PADC;
__sbit __at(0xBD) PBOD;
__sbit __at(0xBC) PS;
__sbit __at(0xBB) PT1;
__sbit __at(0xBA) PX1;
__sbit __at(0xB9) PT0;
__sbit __at(0xB8) PX0;

/*  P3  */
__sbit __at(0xB0) P30;


/*  IE  */
__sbit __at(0xAF) EA;
__sbit __at(0xAE) EADC;
__sbit __at(0xAD) EBOD;
__sbit __at(0xAC) ES;
__sbit __at(0xAB) ET1;
__sbit __at(0xAA) EX1;
__sbit __at(0xA9) ET0;
__sbit __at(0xA8) EX0;

/*  P2  */
__sbit __at(0xA0) P20;

/*  SCON  */
__sbit __at(0x9F) SM0;
__sbit __at(0x9F) FE;
__sbit __at(0x9E) SM1;
__sbit __at(0x9D) SM2;
__sbit __at(0x9C) REN;
__sbit __at(0x9B) TB8;
__sbit __at(0x9A) RB8;
__sbit __at(0x99) TI;
__sbit __at(0x98) RI;

/*  P1  */
__sbit __at(0x97) P17;
__sbit __at(0x96) P16;
__sbit __at(0x96) TXD_1;
__sbit __at(0x95) P15;
__sbit __at(0x94) P14;
__sbit __at(0x94) SDA;
__sbit __at(0x93) P13;
__sbit __at(0x93) SCL;
__sbit __at(0x92) P12;
__sbit __at(0x91) P11;
__sbit __at(0x90) P10;

/*  TCON  */
__sbit __at(0x8F) TF1;
__sbit __at(0x8E) TR1;
__sbit __at(0x8D) TF0;
__sbit __at(0x8C) TR0;
__sbit __at(0x8B) IE1;
__sbit __at(0x8A) IT1;
__sbit __at(0x89) IE0;
__sbit __at(0x88) IT0;

/*  P0  */

__sbit __at(0x87) P07;
__sbit __at(0x87) RXD;
__sbit __at(0x86) P06;
__sbit __at(0x86) TXD;
__sbit __at(0x85) P05;
__sbit __at(0x84) P04;
__sbit __at(0x84) STADC;
__sbit __at(0x83) P03;
__sbit __at(0x82) P02;
__sbit __at(0x82) RXD_1;
__sbit __at(0x81) P01;
__sbit __at(0x81) MISO;
__sbit __at(0x80) P00;
__sbit __at(0x80) MOSI;



#endif

















