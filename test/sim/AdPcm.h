/// <summary>
/// AD-PCM コーダー
/// </summary>
// Copyright(C) 2005-2015 Cores Co., Ltd.Japan
// $Id: AdPcm.h 1441 2018-03-18 14:19:49Z coas-nagasima $
#ifndef AdPcmH
#define AdPcmH

//---------------------------------------------------------------------------
class CAdPcm
{
public:
	CAdPcm(int nbits);
	virtual ~CAdPcm();
private:
	int m_a;
	int m_d;
	int m_word;
	int m_nleft;
	int m_nbits;
	static unsigned short ulaw_to_s16[];
	static unsigned short alaw_to_s16[];
	static unsigned char alaw_to_ulaw[];
	static unsigned char ulaw_to_alaw[];
	static inline void tlookup(const unsigned char *table, unsigned char *buff, unsigned long n)
	{
		while (n--) {
			*buff = table[*buff];
			buff++;
		}
	}
	static int Mx[3][8];
	static int bitmask[9];
	int get_bits(unsigned char **in, int *len);
	void put_bits(int data, int nbits, unsigned char **out, int *len);
public:
	void Init();
	static unsigned char linear2ulaw(int sample);
	void ulaw2alaw(unsigned char *buff, unsigned long len);
	void alaw2ulaw(unsigned char *buff, unsigned long len);
	int adpcm2xlaw(int fmt, unsigned char *in, unsigned char *out, int len);
	int xlaw2adpcm(int fmt, unsigned char *in, unsigned char *out, int len);
	int adpcm2linear(unsigned char *in, short *out, int len);
	int linear2adpcm(short *in, unsigned char *out, int len);
};

//---------------------------------------------------------------------------
#endif
