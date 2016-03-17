// saw_bits.h - Basic bit manipulation; bit streaming library
//
// This is free and unencumbered software released into the public domain.
// 
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.
//
// In jurisdictions that recognize copyright laws, the author or authors
// of this software dedicate any and all copyright interest in the
// software to the public domain.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
// For more information, please refer to <http://unlicense.org/>

//-----------------------------------------------------------------------------------------------------------
// History
// - v1.03 - 03/17/16 - Slight clean-ups
// - v1.02 - 12/19/12 - Silenced 3 warnings
// - v1.01 - 10/03/12 - Fixed typo in BsUndoLbs
//                    - Fixed address calculation error in BsSkipLsb and BsSkipMsb
//                    - Fixed unused bits wrongly written as 0s in BsWriteFlushLsb and BsWriteFlushMsb
//                    - Fixed issue if more than 7 bits need flushed (ie due to BsUndoLsb/Msb)
// - v1.00 - 10/01/12 - Use int for 32 bit types instead of long (long is 64 bits in linux 64-bit)
//                    - Added full bit streaming functionality. I/O on MSB-LSB or LSB-MSB
//                    - Added BitReverseBuf8
// - v0.99 - 09/28/12 - Initial release by Scott Williams

//-----------------------------------------------------------------------------------------------------------
// Notes
// - Mainly interesting for the BitStream library (Bs* functions)
// - Much of the bit twiddling here is directly from http://graphics.stanford.edu/~seander/bithacks.html
// - The Bit*Buf and Byte*Buf functions don't need inlined, but for the simplicity of keeping
//   them all in this header they are inlined.
// - BIT ORDERING
//   For example of a 5 bit number.  Notice the numbers themselves are still MSB-LSB,
//   regardless of the packing method in which they are stored.
//
//   MSB (to LSB) order of bit packing
//   Most significant bit of the stored number goes to the most significant available
//   bit in storage.
//   54321543 21543215 43215432 15432154 32154321
//   -------- -------- -------- -------- --------
//   11111222 22333334 44445555 56666677 77788888
//
//   LSB (to MSB) order of bit packing
//   Least significant bit of the stored number goes to the least significant available
//   bit in storage.
//   32154321 15432154 43215432 21543215 54321543
//   -------- -------- -------- -------- --------
//   22211111 43333322 55554444 77666665 88888777
//
//   Bit positions are ALWAYS left to right. 
//   Ex: A byte's bits are ordered 76543210

//-----------------------------------------------------------------------------------------------------------
// Todo

#ifndef _SAW_BITS_H_INCLUDED
#define _SAW_BITS_H_INCLUDED

namespace saw {

// The basic n-bit types
typedef unsigned long long u64;
typedef long long i64;
typedef unsigned int u32;
typedef int i32;
typedef unsigned short u16;
typedef short i16;
typedef unsigned char u8;
typedef char i8;

// Bit stream (use the concrete types)
template <class T> struct BitStreamT {
	T *pBase;
	T *pPos;
	u32 hasBits;
	u32 currBits;
	u32 bytesLeft;
};
typedef BitStreamT<u8> BitStreamOut;
typedef BitStreamT<const u8> BitStreamIn;

//-------------------------------------------------------------------------------------------------------
// DECLARATIONS
//-------------------------------------------------------------------------------------------------------
// Bit and byte manipulation
inline u32 BitSet(u32 bits, u32 mask);
inline u32 BitReset(u32 bits, u32 mask);
inline u32 BitFlip(u32 bits, u32 mask);
inline u32 BitToggle(u32 bits, u32 mask, bool toggle);
inline u8 BitReverse8(u8 value);
inline u16 BitReverse16(u16 value);
inline u32 BitReverse32(u32 value);
inline int BitGetCount(u32 value);
inline int BitGetMsb(u32 value);
inline int BitGetLsb(u32 value);
inline void BitInvertBuf(void *pData, u32 numBytes);
inline void BitReverseBuf8(void *pData, u32 numBytes);
inline void ByteSwap16(u16 &value);
inline void ByteSwap32(u32 &value);
inline void ByteSwap64(u64 &value);
inline void ByteSwapBuf16(void *pData, u32 count); // count in words, not bytes
inline void ByteSwapBuf32(void *pData, u32 count); // count in double words, not bytes

// Bit streams
inline void BsInit(BitStreamIn *pBits, const void *pBase, u32 byteSize);
inline void BsInit(BitStreamOut *pBits, void *pBase, u32 byteSize);
inline bool BsReadLsb(BitStreamIn *pBits, u32 *pOut, u8 numBits);
inline bool BsReadMsb(BitStreamIn *pBits, u32 *pOut, u8 numBits);
inline void BsReadFlush(BitStreamIn *pBits);  // Flush partial byte if there is one
inline bool BsWriteLsb(BitStreamOut *pBits, u32 in, u8 numBits);
inline bool BsWriteMsb(BitStreamOut *pBits, u32 in, u8 numBits);
inline void BsWriteFlushLsb(BitStreamOut *pBits);  // Flush partial byte if there is one
inline void BsWriteFlushMsb(BitStreamOut *pBits);  // Flush partial byte if there is one
template <class BitStream> inline void BsSetPosLsb(BitStream *pBits, u32 bitPos);
template <class BitStream> inline void BsSetPosMsb(BitStream *pBits, u32 bitPos);
template <class BitStream> inline void BsSkipLsb(BitStream *pBits, i32 numBits);
template <class BitStream> inline void BsSkipMsb(BitStream *pBits, i32 numBits);
template <class BitStream> inline void BsUndoLsb(BitStream *pBits, u32 in, u8 numBits);
template <class BitStream> inline void BsUndoMsb(BitStream *pBits, u32 in, u8 numBits);

//-------------------------------------------------------------------------------------------------------
// DEFINITIONS
//-------------------------------------------------------------------------------------------------------
inline u32 BitSet(u32 bits, u32 mask) {	return bits | mask; }

//-------------------------------------------------------------------------------------------------------
inline u32 BitReset(u32 bits, u32 mask) { return bits & ~mask; }

//-------------------------------------------------------------------------------------------------------
inline u32 BitFlip(u32 bits, u32 mask) { return bits ^ mask; }

//-------------------------------------------------------------------------------------------------------
inline u32 BitToggle(u32 bits, u32 mask, bool toggle) {
	return toggle ? BitSet(bits, mask) : BitReset(bits, mask);
}

//-------------------------------------------------------------------------------------------------------
inline u8 BitReverse8(u8 value) {
	return static_cast<u8>(((value * 0x0802ul & 0x22110ul) | (value * 0x8020ul & 0x88440ul)) * 0x10101ul >> 16);
}

//-------------------------------------------------------------------------------------------------------
inline u16 BitReverse16(u16 value) {
	value = ((value & 0xaaaa) >> 1) | ((value & 0x5555) << 1);
	value = ((value & 0xcccc) >> 2) | ((value & 0x3333) << 2);
	value = ((value & 0xf0f0) >> 4) | ((value & 0x0f0f) << 4);
	return (value >> 8) | (value << 8);
}

//-------------------------------------------------------------------------------------------------------
inline u32 BitReverse32(u32 value) {
	value = ((value & 0xaaaaaaaa) >> 1) | ((value & 0x55555555) << 1);
	value = ((value & 0xcccccccc) >> 2) | ((value & 0x33333333) << 2);
	value = ((value & 0xf0f0f0f0) >> 4) | ((value & 0x0f0f0f0f) << 4);
	value = ((value & 0xff00ff00) >> 8) | ((value & 0x00ff00ff) << 8);
	return (value >> 16) | (value << 16);
}

//-------------------------------------------------------------------------------------------------------
inline int BitGetCount(u32 value) {
   value = value - ((value >> 1) & 0x55555555);
   value = (value & 0x33333333) + ((value >>  2) & 0x33333333);
   return ((value + (value >> 4) & 0xf0f0f0f0) * 0x01010101) >> 24;
}

//-------------------------------------------------------------------------------------------------------
inline int BitGetMsb(u32 value) {
	static const int MultiplyDeBruijnBitPosition[32] = { 0, 9, 1, 10, 13, 21, 2, 29, 
		11, 14, 16, 18, 22, 25, 3, 30, 8, 12, 20, 28, 15, 17, 24, 7, 19, 27, 23, 6, 26, 5, 4, 31 };
	value |= value >> 1;
	value |= value >> 2;
	value |= value >> 4;
	value |= value >> 8;
	value |= value >> 16;
	return MultiplyDeBruijnBitPosition[(value * 0x07c4acddu) >> 27];
}

//-------------------------------------------------------------------------------------------------------
inline int BitGetLsb(u32 value) {
	static const int MultiplyDeBruijnBitPosition[32] = { 0, 1, 28, 2, 29, 14, 24, 3, 
		30, 22, 20, 15, 25, 17, 4, 8, 31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9 };
	return MultiplyDeBruijnBitPosition[((value & -static_cast<i32>(value)) * 0x077cb531u) >> 27];
}

//-----------------------------------------------------------------------------------------------------------
inline void BitInvertBuf(void *pData, u32 numBytes) {
	u32 *pData32 = reinterpret_cast<u32 *>(pData);
	for (u32 i = numBytes >> 2; i--; pData32++)
		*pData32 = ~(*pData32);
	u8 *pData8 = reinterpret_cast<u8 *>(pData32);
	for (u32 i = numBytes & 3; i--; pData8++)
		*pData8 = ~(*pData8);
}

//-----------------------------------------------------------------------------------------------------------
inline void BitReverseBuf8(void *pData, u32 numBytes) {
	u32 *pData32 = reinterpret_cast<u32 *>(pData);
	for (u32 i = numBytes >> 2; i--; pData32++) {  // Reverse 4 bytes at a time in parallel
		*pData32 = ((*pData32 & 0xaaaaaaaa) >> 1) | ((*pData32 & 0x55555555) << 1);
		*pData32 = ((*pData32 & 0xcccccccc) >> 2) | ((*pData32 & 0x33333333) << 2);
		*pData32 = ((*pData32 & 0xf0f0f0f0) >> 4) | ((*pData32 & 0x0f0f0f0f) << 4);
	}
	u8 *pData8 = reinterpret_cast<u8 *>(pData32);
	for (u32 i = numBytes & 3; i--; pData8++)
		*pData8 = static_cast<u8>(((*pData8 * 0x0802ul & 0x22110ul) | (*pData8 * 0x8020ul & 0x88440ul)) * 0x10101ul >> 16);
}

//-------------------------------------------------------------------------------------------------------
inline void ByteSwap16(u16 &value) { value = (value >> 8) | (value << 8); }

//-------------------------------------------------------------------------------------------------------
inline void ByteSwap32(u32 &value) {
	value = ((value & 0xff00ff00) >> 8) | ((value & 0x00ff00ff) << 8);
	value = (value >> 16) | (value << 16);
}

//-------------------------------------------------------------------------------------------------------
inline void ByteSwap64(u64 &value) {
	value = ((value & 0xff00ff00ff00ff00ull) >> 8) | ((value & 0x00ff00ff00ff00ffull) << 8);
	value = ((value & 0xffff0000ffff0000ull) >> 16) | ((value & 0x0000ffff0000ffffull) << 16);
	value = (value >> 32) | (value << 32);
}

//-----------------------------------------------------------------------------------------------------------
inline void ByteSwapBuf16(void *pData, u32 count) {
	u16 *pData16 = reinterpret_cast<u16 *>(pData);
	for (u32 i = 0; i < count; i++)
		ByteSwap16(pData16[i]);
}

//-----------------------------------------------------------------------------------------------------------
inline void ByteSwapBuf32(void *pData, u32 count) {
	u32 *pData32 = reinterpret_cast<u32 *>(pData);
	for (u32 i = 0; i < count; i++)
		ByteSwap32(pData32[i]);
}

//-------------------------------------------------------------------------------------------------------
inline void BsInit(BitStreamIn *pBits, const void *pBase, u32 byteSize) {
	pBits->pPos = pBits->pBase = reinterpret_cast<const u8 *>(pBase);
	pBits->bytesLeft = byteSize;
	pBits->hasBits = pBits->currBits = 0;
}

//-----------------------------------------------------------------------------------------------------------
inline void BsInit(BitStreamOut *pBits, void *pBase, u32 byteSize) {
	pBits->pPos = pBits->pBase = reinterpret_cast<u8 *>(pBase);
	pBits->bytesLeft = byteSize;
	pBits->hasBits = pBits->currBits = 0;
}

//-----------------------------------------------------------------------------------------------------------
inline bool BsReadLsb(BitStreamIn *pBits, u32 *pOut, u8 numBits) {
	while (pBits->hasBits < numBits) {
		if (pBits->bytesLeft == 0)
			return false;
		pBits->currBits |= *pBits->pPos++ << pBits->hasBits;
		pBits->bytesLeft--;
		pBits->hasBits += 8;
	}
	*pOut = pBits->currBits & ((1 << numBits) - 1);
	pBits->currBits >>= numBits;
	pBits->hasBits -= numBits;
	return true;
}

//-----------------------------------------------------------------------------------------------------------
inline bool BsReadMsb(BitStreamIn *pBits, u32 *pOut, u8 numBits) {
	while (pBits->hasBits < numBits) {
		if (pBits->bytesLeft == 0)
			return false;
		pBits->currBits |= *pBits->pPos++ << (24 - pBits->hasBits);
		pBits->bytesLeft--;
		pBits->hasBits += 8;
	}
	*pOut = pBits->currBits >> (32 - numBits);
	pBits->currBits <<= numBits;
	pBits->hasBits -= numBits;
	return true;
}

//-----------------------------------------------------------------------------------------------------------
inline void BsReadFlush(BitStreamIn *pBits) {
	if (pBits->hasBits) {
		pBits->pPos++;
		pBits->bytesLeft--;
		pBits->currBits = 0;
		pBits->hasBits = 0;
	}
}

//-----------------------------------------------------------------------------------------------------------
inline bool BsWriteLsb(BitStreamOut *pBits, u32 in, u8 numBits) {
	pBits->currBits |= in << pBits->hasBits;
	pBits->hasBits += numBits;
	while (pBits->hasBits >= 8) {
		*pBits->pPos++ = pBits->currBits & 0xff;
		pBits->bytesLeft--;
        pBits->currBits >>= 8;
        pBits->hasBits -= 8;
	}
	return true;
}

//-----------------------------------------------------------------------------------------------------------
inline bool BsWriteMsb(BitStreamOut *pBits, u32 in, u8 numBits) {
	pBits->currBits |= in << (32 - numBits - pBits->hasBits);
	pBits->hasBits += numBits;
	while (pBits->hasBits >= 8) {
		*pBits->pPos++ = pBits->currBits >> 24;
		pBits->bytesLeft--;
        pBits->currBits <<= 8;
        pBits->hasBits -= 8;
	}
	return true;
}

//-----------------------------------------------------------------------------------------------------------
inline void BsWriteFlushLsb(BitStreamOut *pBits) {
	BsWriteLsb(pBits, 0, 0);
    if (pBits->hasBits) {
		u8 mask = (1 << pBits->hasBits) - 1;
		*pBits->pPos = *pBits->pPos & ~mask;
		*pBits->pPos++ |= pBits->currBits & mask;
		pBits->bytesLeft--;
        pBits->hasBits = 0;
        pBits->currBits = 0;
    }
}

//-----------------------------------------------------------------------------------------------------------
inline void BsWriteFlushMsb(BitStreamOut *pBits) {
	BsWriteMsb(pBits, 0, 0);
    if (pBits->hasBits) {
		u8 invmask = (1 << (8 - pBits->hasBits)) - 1;
		*pBits->pPos = *pBits->pPos & invmask;
		*pBits->pPos++ |= (pBits->currBits >> 24) & ~invmask;
		pBits->bytesLeft--;
        pBits->hasBits = 0;
        pBits->currBits = 0;
    }
}

//-----------------------------------------------------------------------------------------------------------
template <class BitStream> inline void BsSetPosLsb(BitStream *pBits, u32 bitPos) {
	pBits->bytesLeft = pBits->bytesLeft + (pBits->pPos - pBits->pBase) - ((bitPos + 7) >> 3);
	pBits->pPos = pBits->pBase + (bitPos >> 3);
	pBits->hasBits = (8 - (bitPos & 7)) & 7;
	pBits->currBits = 0;
	if (pBits->hasBits)
		pBits->currBits = *pBits->pPos++ >> (bitPos & 7);
}

//-----------------------------------------------------------------------------------------------------------
template <class BitStream> inline void BsSetPosMsb(BitStream *pBits, u32 bitPos) {
	pBits->bytesLeft = pBits->bytesLeft + (pBits->pPos - pBits->pBase) - ((bitPos + 7) >> 3);
	pBits->pPos = pBits->pBase + (bitPos >> 3);
	pBits->hasBits = (8 - (bitPos & 7)) & 7;
	pBits->currBits = 0;
	if (pBits->hasBits)
		pBits->currBits = *pBits->pPos++ << (24 + (bitPos & 7));
}

//-----------------------------------------------------------------------------------------------------------
template <class BitStream> inline void BsSkipLsb(BitStream *pBits, i32 numBits) {
	i32 curr = static_cast<i32>(((pBits->pPos - pBits->pBase) << 3) + pBits->hasBits);
	BsSetPosLsb(pBits, static_cast<u32>(curr + numBits));
}

//-----------------------------------------------------------------------------------------------------------
template <class BitStream> inline void BsSkipMsb(BitStream *pBits, i32 numBits) {
	i32 curr = static_cast<i32>(((pBits->pPos - pBits->pBase) << 3) + pBits->hasBits);
	BsSetPosMsb(pBits, static_cast<u32>(curr + numBits));
}

//-----------------------------------------------------------------------------------------------------------
template <class BitStream> inline void BsUndoLsb(BitStream *pBits, u32 in, u8 numBits) {
	pBits->currBits <<= numBits;
	pBits->currBits |= in & ((1 << numBits) - 1);
	pBits->hasBits += numBits;
}

//-----------------------------------------------------------------------------------------------------------
template <class BitStream> inline void BsUndoMsb(BitStream *pBits, u32 in, u8 numBits) {
	pBits->currBits >>= numBits;
	pBits->currBits |= in << (32 - numBits);
	pBits->hasBits += numBits;
}

}  // namespace

#endif  // _SAW_BITS_H_INCLUDED
