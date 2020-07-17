#include <inttypes.h>
#include "TTS_data.h"


uint8_t sound_E        []={(  46), 0xE1,0xC3,0xE1,0xC7,0x8F,0x0F };
uint8_t sound_A        []={(  46), 0xF8,0x03,0x0F,0x07,0xC1,0xE3 };
uint8_t sound_I        []={(  46), 0xFF,0x40,0x17,0xFF,0x00,0x03 };
uint8_t sound_O        []={(  46), 0xF8,0x7C,0xC1,0xF1,0xF8,0x03 };
uint8_t sound_U        []={(  46), 0xFE,0x00,0x7F,0xFC,0x00,0x03 };

uint8_t sound_closed0  []={(   0)                                };
uint8_t sound_closed   []={(  46), 0x00,0x00,0x00,0x00,0x00,0x00 };

uint8_t sound_L        []={(  46), 0xFE,0x03,0xC0,0xFF,0x00,0x00 };
uint8_t sound_M        []={(  46), 0xFF,0xF8,0x00,0x7F,0xF8,0x03 };
uint8_t sound_N        []={(  46), 0xFF,0xF0,0x01,0xFF,0xE0,0x03 };
uint8_t sound_B        []={(  46), 0xAA,0xCA,0x5A,0xD5,0x21,0x3D };

uint8_t sound_H        []={(  46), 0xE1,0x5C,0x35,0xC5,0xAA,0x35 };

uint8_t sound_2DNT2    []={(  46), 0xEF,0x40,0x17,0xFF,0x00,0x03 };

uint8_t sound_G_K      []={(  90), 0x3E,0x8E,0x38,0x73,0xCF,0xF8,0x78,0xC3,0xDF,0x1C,0xF1,0xC7 };

uint8_t sound_opn_HMNV []={(  46), 0xFF,0xFC,0x20,0x00,0x00,0x03 };

uint8_t sound_2V2      []={(  46), 0xFF,0xFF,0x08,0x79,0x00,0x02 };
uint8_t sound_3V2      []={(  46), 0xFF,0xE1,0xC7,0x1F,0xE0,0x03 };


uint8_t sound_soft1    []={(  46), 0xFF,0xD0,0x01,0xFF,0xF0,0x03 };
uint8_t sound_soft2    []={(  46), 0x7F,0x01,0xFA,0x5F,0xC0,0x07 };
uint8_t sound_soft3    []={(  46), 0xF8,0x0F,0xC0,0xFF,0x00,0x42 };


uint8_t sound_S        []={( 254), 0xAA,0xA5,0x55,0x5A, 0xAA,0xAA,0x5A,0xA5, 0x5A,0xAA,0x55,0x55, 0xAA,0xAA,0xA5,0x55 };
uint8_t sound_S1       []={(  46), 0xAA,0xA5,0x55,0x5A, 0xAA,0xAA                      };
uint8_t sound_S_z      []={(  46),           0x55,0x5A, 0xAA,0xAA,0x5A,0xA5            };
uint8_t sound_S_z2     []={(  46),                      0xAA,0xAA,0x5A,0xA5, 0x5A,0xAA };


uint8_t sound_CH       []={( 254), 0xB7,0x66,0x6C,0xD8, 0xF9,0xB3,0x6C,0xAD, 0x37,0x37,0x66,0xFC, 0x9B,0x87,0xF6,0xC0 };
uint8_t sound_ZH0      []={(  46), 0xB7,0x66,0x6C,0xD8, 0xF9,0xB3  };
uint8_t sound_ZH1      []={(  46),           0x6C,0xD8, 0xF9,0xB3,0x6C,0xAD };
uint8_t sound_ZH2      []={(  46),                      0xF9,0xB3,0x6C,0xAD, 0x37,0x37 };

uint8_t sound_1R       []={(  46), 0xF8,0x0F,0x09,0xF1,0xFE,0x03 };
uint8_t sound_3R       []={(  46), 0xFE,0x1F,0xF8,0x00,0x00,0x1F };

uint8_t sound_D_T      []={(  46), 0xB7,0xDE,0x46,0xF6, 0x96,0xB4 };

uint8_t sound_F        []={( 254), 0x4F,0xAA,0xA9,0x55, 0xAA,0xAA,0xA5,0x69, 0x59,0x9A,0x6A,0x95,0x55,0x95, 0x55,0x6A,0xA5,0x55, 0xA9,0x4D,0x66,0x6A };

uint8_t sound_TH       []={(  94), 0x66,0x6A,0x92,0xEC,0xA5,0x55,0xD2,0x96,0x55,0xA2,0xBA,0xCD };

uint8_t sound_SH       []={( 254), 0x00,0x66,0x99,0xCC,0x67,0x31,0x8E,0x66,
                                           0x39,0xA6,0x6B,0x19,0x66,0x59,0xC6,0x71,
                                           0x09,0x67,0x19,0xCB,0x01,0x71,0xCC,0x73,
                                           0x19,0x99,0xCC,0xC6,0x67,0x19,0x9A,0xC6 };


typedef enum          {  snd_A,  snd_E,  snd_I,  snd_O,  snd_U,  snd_closed0,  snd_closed,  snd_L,  snd_M,  snd_N,  snd_B,  snd_H,  snd_2DNT2,   snd_G_K,  snd_opn_HMNV,  snd_2V2,  snd_3V2,  snd_soft1,  snd_soft2,  snd_soft3,  snd_S,  snd_S1,  snd_S_z,  snd_S_z2,  snd_CH,  snd_ZH0,  snd_ZH1,  snd_ZH2, snd_1R,  snd_3R,  snd_D_T,  snd_F,  snd_TH,  snd_SH}TSounds;
uint8_t* snd_list[] = {sound_A,sound_E,sound_I,sound_O,sound_U,sound_closed0,sound_closed,sound_L,sound_M,sound_N,sound_B,sound_H,sound_2DNT2, sound_G_K,sound_opn_HMNV,sound_2V2,sound_3V2,sound_soft1,sound_soft2,sound_soft3,sound_S,sound_S1,sound_S_z,sound_S_z2,sound_CH,sound_ZH0,sound_ZH1,sound_ZH2, sound_1R,sound_3R,sound_D_T,sound_F,sound_TH,sound_SH};


#define Stop  0x40u
#define fonem(Sample, Repeats)   (static_cast<uint16_t>(static_cast<uint16_t>(Sample) << 8u)|(Repeats##u & 0x1Fu))

uint16_t f_AH []={ fonem(snd_A,10)+Stop  };
uint16_t f_AA []={ fonem(snd_A,15)+Stop  };
uint16_t f_EH []={ fonem(snd_E,10)+Stop  };
uint16_t f_AE []={ fonem(snd_E,14)+Stop  };
uint16_t f_IH []={ fonem(snd_I, 6)+Stop  };
uint16_t f_IY []={ fonem(snd_I,14)+Stop  };
uint16_t f_AO []={ fonem(snd_O,14)+Stop  };
uint16_t f_OW []={ fonem(snd_O,14), fonem(snd_U, 6)+Stop  };
uint16_t f_OY []={ fonem(snd_O,14), fonem(snd_I, 6)+Stop  };
uint16_t f_UH []={ fonem(snd_U, 6)+Stop  };
uint16_t f_UW []={ fonem(snd_U,14)+Stop  };

uint16_t f_AY []={ fonem(snd_A, 6), fonem(snd_soft1,3),  fonem(snd_soft2,2), fonem(snd_soft3,2)+Stop};
uint16_t f_AW []={ fonem(snd_A, 6), fonem(snd_U, 6) + Stop};
uint16_t f_ER []={ fonem(snd_1R,6) + Stop };
uint16_t f_EY []={ fonem(snd_E, 6), fonem(snd_I, 6) + Stop };

uint16_t f_JH  []={ fonem(snd_closed, 4),  fonem(snd_D_T,1) + fonem(snd_ZH0,2),      fonem(snd_ZH1,2), fonem(snd_ZH2,2)+Stop };
uint16_t f_NG  []={ fonem(snd_opn_HMNV,1),  fonem(snd_N, 6), fonem(snd_G_K,1) +Stop };
uint16_t f_Y   []={ fonem(snd_soft1,3),  fonem(snd_soft2,2), fonem(snd_soft3,2) + Stop };
uint16_t f_W   []={ fonem(snd_opn_HMNV,4), fonem(snd_U, 4)+Stop  };

uint16_t f_S  []={ fonem(snd_S  ,2)+Stop };
uint16_t f_SH []={ fonem(snd_SH ,2)+Stop };

uint16_t f_D []={ fonem(snd_closed, 8),  fonem(snd_D_T,1), fonem(snd_2DNT2,2)+Stop };
uint16_t f_T []={ fonem(snd_closed0,8),  fonem(snd_D_T,1) + Stop };

uint16_t f_G []={ fonem(snd_closed,5),   fonem(snd_G_K,1) +Stop };
uint16_t f_K []={ fonem(snd_closed0, 8), fonem(snd_G_K,1) +Stop };

uint16_t f_H  []={ fonem(snd_opn_HMNV,1),  fonem(snd_H, 4)+Stop };
uint16_t f_N  []={ fonem(snd_opn_HMNV,1),  fonem(snd_N, 6)+Stop };
uint16_t f_M  []={ fonem(snd_opn_HMNV,1),  fonem(snd_M, 6)+Stop };

uint16_t f_DH []={ fonem(snd_closed0,8),  fonem(snd_TH,1)+Stop };
uint16_t f_TH []={ fonem(snd_closed0,3),  fonem(snd_TH,1)+Stop };


uint16_t f_Z  []={ fonem(snd_opn_HMNV,2),  fonem(snd_S1,2),
                          fonem(snd_S_z,2),  fonem(snd_S_z2,2)+Stop };

uint16_t f_L  []={ fonem(snd_L, 3), fonem(snd_opn_HMNV,1) + Stop };
uint16_t f_R  []={ fonem(snd_1R,2),   fonem(snd_closed0,3), fonem(snd_3R,1)+Stop };

uint16_t f_CH []={ fonem(snd_closed0,8),  fonem(snd_CH,1)+Stop };
uint16_t f_ZH []={ fonem(snd_ZH0,2),      fonem(snd_ZH1,2), fonem(snd_ZH2,2)+Stop };

uint16_t f_B  []={ fonem(snd_closed,4),   fonem(snd_B,1) +Stop };
uint16_t f_P  []={ fonem(snd_closed0, 8), fonem(snd_B,1) +Stop };
uint16_t f_F  []={ fonem(snd_F     ,2) +Stop };
uint16_t f_V  []={  fonem(snd_opn_HMNV,4),fonem(snd_2V2,2),fonem(snd_3V2,2) +Stop };

//============================================================================

/*
 *
AA, //	odd     AA D
AE, //	at	AE T
AH, //	hut	HH AH T
AO, //	ought	AO T
AW, //	cow	K AW
AY, //	hide	HH AY D
B, // 	be	B IY
CH, //	cheese	CH IY Z
D , //	dee	D IY
DH, //	thee	DH IY
EH, //	Ed	EH D
ER, //	hurt	HH ER T
EY, //	ate	EY T
F, // 	fee	F IY
G, // 	green	G R IY N
HH, //	he	HH IY
IH, //	it	IH T
IY, //	eat	IY T
JH, //	gee	JH IY
K, // 	key	K IY
L, // 	lee	L IY
M, // 	me	M IY
N, // 	knee	N IY
NG, //	ping	P IH NG
OW, //	oat	OW T
OY, //	toy	T OY
P, // 	pee	P IY
R, // 	read	R IY D
S, // 	sea	S IY
SH, //	she	SH IY
T, // 	tea	T IY
TH, //	theta	TH EY T AH
UH, //	hood	HH UH D
UW, //	two	T UW
V, // 	vee	V IY
W, // 	we	W IY
Y, // 	yield	Y IY L D
Z, // 	zee	Z IY
ZH, //	seizure	S IY ZH ER
P1, P2, P4, P8
 */
uint16_t* phonems[]  = {
		f_AA,
		f_AE,
		f_AH,
		f_AO,
		f_AW,
		f_AY,
		f_B,
		f_CH,
		f_D,
		f_DH,
		f_EH,
		f_ER,
		f_EY,
		f_F,
		f_G,
		f_H,
		f_IH,
		f_IY,
		f_JH,
		f_K,
		f_L,
		f_M,
		f_N,
		f_NG,
		f_OW,
		f_OY,
		f_P,
		f_R,
		f_S,
		f_SH,
		f_T,
		f_TH,
		f_UH,
		f_UW,
		f_V,
		f_W,
		f_Y,
		f_Z,
		f_ZH,
		};

uint16_t* get_phoneme(uint8_t ph)
{
	return phonems[ph];
}

SndBlock get_sound(uint8_t SndCode)
{
	return { snd_list[SndCode] + 1, *snd_list[SndCode]};
}

