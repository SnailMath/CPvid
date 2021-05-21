#include <stdint.h>

namespace SCIF{
	static const unsigned int baseAddress = 0xA4410000;
	enum Registers16 {
		SCSMR  = 0x0000,
		SCSCR  = 0x0008,
		SCFSR  = 0x0010,       
		SCFCR  = 0x0018,
		SCFDR  = 0x001C,
		SCLSR  = 0x0024
	};
	enum Registers8 {
		SCBRR  = 0x0004,
		SCFTDR = 0x000C,
		SCFRDR = 0x0014
	};

	inline volatile uint8_t  *regAddress (Registers8 reg) {
		return reinterpret_cast<volatile uint8_t* >(baseAddress +  reg);
	}
	inline volatile uint16_t *regAddress (Registers16 reg) {
		return reinterpret_cast<volatile uint16_t*>(baseAddress +  reg);
	}

	inline uint8_t  regRead(Registers8  reg){
		return *regAddress(reg);
	}
	inline uint16_t regRead(Registers16 reg){
		return *regAddress(reg);
	}

	inline void regWrite(Registers8  reg, uint8_t  value){
		*regAddress(reg) = value;
	}
	inline void regWrite(Registers16 reg, uint16_t value){
		*regAddress(reg) = value;
	}

	enum SCFSR_BITS{
		ER	= 0b0000000010000000,
		TEND	= 0b0000000001000000,
		TDFE	= 0b0000000000100000,
		BRK	= 0b0000000000010000,
		FER	= 0b0000000000001000,
		PER	= 0b0000000000000100,
		RDF	= 0b0000000000000010,
		DR	= 0b0000000000000001
	};
	enum SCSCR_BITS{
		TIE	= 0b0000000010000000,
		RIE 	= 0b0000000001000000,
		TE	= 0b0000000000100000,
		RE	= 0b0000000000010000,
		REIE	= 0b0000000000001000,
		CKE1	= 0b0000000000000010,
		CKE0	= 0b0000000000000001
	};
	enum SCSMR_BITS{
		CA	= 0b0000000010000000,
		CHR	= 0b0000000001000000,
		PE	= 0b0000000000100000,
		OE	= 0b0000000000010000,
		STOP	= 0b0000000000001000,
		CKS1	= 0b0000000000000010,
		CKS0	= 0b0000000000000001
	};

	inline bool regTestBit(	SCFSR_BITS bit){
		return regRead(	SCFSR) & bit;}
	inline void regSetBit(	SCFSR_BITS bit){
		regWrite(	SCFSR,regRead(
				SCFSR)|bit);}
	inline void regClearBit(SCFSR_BITS bit){
		regWrite(	SCFSR,regRead(
				SCFSR)&(~bit));}
	
	inline bool regTestBit(	SCSCR_BITS bit){
		return regRead(	SCSCR) & bit;}
	inline void regSetBit(	SCSCR_BITS bit){
		regWrite(	SCSCR,regRead(
				SCSCR)|bit);}
	inline void regClearBit(SCSCR_BITS bit){
		regWrite(	SCSCR,regRead(
				SCSCR)&(~bit));}

	inline bool regTestBit(	SCSMR_BITS bit){
		return regRead(	SCSMR) & bit;}
	inline void regSetBit(	SCSMR_BITS bit){
		regWrite(	SCSMR,regRead(
				SCSMR)|bit);}
	inline void regClearBit(SCSMR_BITS bit){
		regWrite(	SCSMR,regRead(
				SCSMR)&(~bit));}
	
};
