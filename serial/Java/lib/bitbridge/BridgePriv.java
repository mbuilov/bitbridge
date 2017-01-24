/********************************************************************************
* bitbridge - runtime library of another serialization/rpc mechanism (bitbridge)
* Copyright (C) 2008-2017 Michael M. Builov, https://github.com/mbuilov/bitbridge
* Licensed under LGPL version 3 or any later version, see COPYING.LGPL3
********************************************************************************/

/*  BridgePriv.java */

package bitbridge;

import static bitbridge.Bridge.*;

public final class BridgePriv {

	/* internal structures used while packing */

	public static class _pk_mem_info {
		public byte[] mem;    /* output buffer */
		public int at;        /* offset in mem[] where to store bytes (after bits array) */
		public _pk_mem_info(int size, int head, int tail) throws BridgeBadReserves {
			head += 0x80000000;
			size += head;
			if (size < head)
				throw new BridgeBadReserves();
			size += tail;
			if (size < tail + 0x80000000)
				throw new BridgeBadReserves();
			mem = new byte[size + 0x80000000];
		}
	}

	public static class _pk_mem_conv_info extends _pk_mem_info {
		public boolean big;
		public _pk_mem_conv_info(int size, int head, int tail) throws BridgeBadReserves {
			super(size, head, tail);
		}
	}

	public static class _pk_bits_info extends _pk_mem_conv_info {
		public int bits;      /* offset in mem[] where to store bits (before bytes array) */
		public int bit_count; /* number of filled bits */
		public _pk_bits_info(int size, int head, int tail) throws BridgeBadReserves {
			super(size, head, tail);
		}
	}

	public static class _pk_bits_mem_info extends _pk_bits_info {
		public _pk_bits_mem_info(int size, int head, int tail) throws BridgeBadReserves {
			super(size, head, tail);
		}
	}

	public static class _pk_bits_mem_conv_info extends _pk_bits_mem_info {
		public _pk_bits_mem_conv_info(int size, int head, int tail) throws BridgeBadReserves {
			super(size, head, tail);
		}
	}

	/* internal structures used while unpacking */

	public static class _upk_mem_no_lim_info {
		public byte[] mem;
		public int at;
		public _upk_mem_no_lim_info(byte[] _mem) {
			mem = _mem;
		}
	}

	public static class _upk_mem_info extends _upk_mem_no_lim_info {
		public int lim;
		public _upk_mem_info(byte[] _mem, int _lim) {
			super(_mem);
			lim = _lim;
		}
	}

	public static class _upk_mem_conv_info extends _upk_mem_info {
		public boolean big;
		public _upk_mem_conv_info(byte[] _mem, int _lim) {
			super(_mem, _lim);
		}
	}

	public static class _upk_bits_info extends _upk_mem_conv_info {
		public int bits;      /* offset in mem[] where bits are stored (before bytes array) */
		public int bit_count; /* number of processed bits */
		public _upk_bits_info(byte[] _mem, int _lim) {
			super(_mem, _lim);
		}
	}

	public static class _upk_bits_mem_info extends _upk_bits_info {
		public _upk_bits_mem_info(byte[] _mem, int _lim) {
			super(_mem, _lim);
		}
	}

	public static class _upk_bits_mem_conv_info extends _upk_bits_mem_info {
		public _upk_bits_mem_conv_info(byte[] _mem, int _lim) {
			super(_mem, _lim);
		}
	}

	/* check bit in bits array */
	public static boolean _bridge_get_bit(byte[] bits, int offset, int bit_number) {
		return 0 != (bits[offset + (bit_number >>> 3)] & (byte)(1 << (bit_number & 7)));
	}

	/* set bit in bits array */
	public static void _bridge_set_bit(byte[] bits, int offset, int bit_number) {
		bits[offset + (bit_number >>> 3)] |= (byte)(1 << (bit_number & 7));
	}

	/* bytes should never be negative (finally should be in [0..BRIDGE_MAX]) */
	public static int _br_of_add_bytes(
		int bytes/*>=0*/,
		int n/*>=0*/
	) throws BridgeBytesOverflow {
		bytes += n;
		if (bytes < 0)
			throw new BridgeBytesOverflow();
		return bytes; /*>=0*/
	}

	/* bits should never overflow (finally should be in [0..BRIDGE_BIT_COUNT_MAX]) */
	public static int _br_of_inc_bits(
		int bits/*any,xor'ed with 0x80000000*/
	) throws BridgeBitsOverflow {
		if (0x80000000 == ++bits)
			throw new BridgeBitsOverflow();
		return bits; /*any,xor'ed with 0x80000000*/
	}

	/* bits should never overflow (finally should be in [0..BRIDGE_BIT_COUNT_MAX]) */
	public static int _br_of_add_bits(
		int bits/*any,xor'ed with 0x80000000*/,
		int n/*any*/
	) throws BridgeBitsOverflow {
		n += bits;
		if (n < bits)
			throw new BridgeBitsOverflow();
		return n; /*any,xor'ed with 0x80000000*/
	}

	/* get bit array size in bytes */
	public static int bridge_bit_array_size_nz(
		int bit_count/*[1..BRIDGE_BIT_COUNT_MAX]*/
	) {
		return ((bit_count - 1) >>> 3) + 1; /* [1..BRIDGE_MAX) */
	}

	/* check that it's possible to unpack sz packed bytes */
	public static void upk_check_mem_lim(
		int mem/*(lim - mem)[0..BRIDGE_MAX]*/,
		int lim/*(lim - mem)[0..BRIDGE_MAX]*/,
		int sz/*>=0*/
	) throws BridgeBytesOverLimit {
		if (sz > lim - mem)
			throw new BridgeBytesOverLimit();
	}

	/* check that it's possible to unpack bit_count packed bits */
	public static void _upk_check_bits_lim_nz(
		int bits/*(lim - bits)[0..BRIDGE_MAX]*/,
		int lim/*(lim - bits)[0..BRIDGE_MAX]*/,
		int bit_count/*[1..BRIDGE_BIT_COUNT_MAX]*/
	) throws BridgeBitsOverLimit {
		if (bridge_bit_array_size_nz(bit_count)/*[1..BRIDGE_MAX)*/ > lim - bits)
			throw new BridgeBitsOverLimit();
	}

	/* check that it's possible to unpack b packed bits */
	public static void upk_check_bits_lim_nz(
		int bits/*(lim - bits)[0..BRIDGE_MAX]*/,
		int lim/*(lim - bits)[0..BRIDGE_MAX]*/,
		int bit_count/*[0..BRIDGE_BIT_COUNT_MAX],xor'ed with 0x80000000*/,
		int b/*[1..BRIDGE_BIT_COUNT_MAX]*/
	) throws BridgeBitsOverflow, BridgeBitsOverLimit {
		bit_count = _br_of_add_bits(bit_count, b) ^ 0x80000000;
		/* BRIDGE_BIT_COUNT_MAX == 0xFFFFFFFF, so bit_count is in [1..BRIDGE_BIT_COUNT_MAX] */
		_upk_check_bits_lim_nz(bits, lim, bit_count/*[1..BRIDGE_BIT_COUNT_MAX]*/);
	}

	/* check that it's possible to unpack b*n packed bits */
	public static void upk_check_bits_lim_n(
		int bits/*(lim - bits)[0..BRIDGE_MAX]*/,
		int lim/*(lim - bits)[0..BRIDGE_MAX]*/,
		int bit_count/*[0..BRIDGE_BIT_COUNT_MAX],xor'ed with 0x80000000*/,
		int b/*[1..BRIDGE_BIT_COUNT_MAX]*/,
		int n/*[1..BRIDGE_MAX]*/
	) throws BridgeBadCounter, BridgeBitsOverflow, BridgeBitsOverLimit {
		/* BRIDGE_BIT_COUNT_MAX == 0xFFFFFFFF, so n is in [1..BRIDGE_BIT_COUNT_MAX] */
		if (b != 1 && n > (int)(BRIDGE_BIT_COUNT_MAX/((long)b & 0xFFFFFFFFL)))
			throw new BridgeBadCounter();
		upk_check_bits_lim_nz(bits, lim, bit_count, (n*b)/*[1..BRIDGE_BIT_COUNT_MAX]*/);
	}

	/* assume array item is variable-sized with packed size >= packed,
	  check that number of items is in limits [0..BRIDGE_MAX/packed] */
	public static void _bridge_check_array_counter(
		int c/*any*/,
		int packed/*[1..BRIDGE_MAX]*/
	) throws BridgeBadCounter {
		if (c < 0 || c > BRIDGE_MAX/packed)
			throw new BridgeBadCounter();
	}

	/* assume array item is fixed-sized with packed size == packed,
	  check that number of items is in limits [0..BRIDGE_MAX] */
	public static void _bridge_check_fixed_array_counter(
		int mem/*(lim - mem)[0..BRIDGE_MAX]*/,
		int lim/*(lim - mem)[0..BRIDGE_MAX]*/,
		int c/*any*/,
		int packed/*[2..BRIDGE_MAX]*/
	) throws BridgeBadCounter, BridgeBytesOverLimit {
		_bridge_check_array_counter(c, packed);
		upk_check_mem_lim(mem, lim, (c*packed)/*[0..BRIDGE_MAX]*/);
	}

	/* check that number of bytes is in limits [0..BRIDGE_MAX] */
	public static void _bridge_check_byte_array_counter(
		int mem/*(lim - mem)[0..BRIDGE_MAX]*/,
		int lim/*(lim - mem)[0..BRIDGE_MAX]*/,
		int c/*any*/
	) throws BridgeBadCounter, BridgeBytesOverLimit {
		if (c < 0)
			throw new BridgeBadCounter();
		upk_check_mem_lim(mem, lim, c);
	}

	/* check that it's possible to unpack one packed bit, then unpack that bit */
	public static boolean _bridge_unpack_next_bit(_upk_bits_info info, int lim)
		throws BridgeBitsOverflow, BridgeBitsOverLimit
	{
		int bit_number = info.bit_count ^ 0x80000000;
		info.bit_count = _br_of_inc_bits(info.bit_count); /* != 0x80000000 */
		/* BRIDGE_BIT_COUNT_MAX == 0xFFFFFFFF, so bit_number is in [1..BRIDGE_BIT_COUNT_MAX] */
		if (bridge_bit_array_size_nz(info.bit_count ^ 0x80000000) > lim - info.bits)
			throw new BridgeBitsOverLimit();
		return _bridge_get_bit(info.mem, info.bits, bit_number);
	}

	public static long _bridge_bits_bytes_from_bits_and_bytes(
		int bit_count/*any*/, int byte_count/*>=0*/)
	{
		return ((long)bit_count << 32) | byte_count;
	}

	public static int _bridge_bits_bytes_get_bytes(long bb) {
		return (int)(bb & 0xFFFFFFFFL);
	}

	public static int _bridge_bits_bytes_get_bits(long bb) {
		return (int)(bb >>> 32);
	}

	/* determine packed integers size */

	public static int bridge_ppshort_packed_size(int n) {
		int x = n & 0xFFFF;
		return 1 + (x > 0x7F ? 1 : 0) + (x > 0x3FFF ? 1 : 0);
	}

	public static int bridge_ppint_packed_size(int n) {
		if (0 != (n & ~0x3FFF)) {
			if (0 != (n & ~0x1FFFFF)) {
				if (0 != (n & ~0xFFFFFFF))
					return 5;
				return 4;
			}
			return 3;
		}
		if (0 != (n & ~0x7F))
			return 2;
		return 1;
	}

	public static int bridge_pplong_packed_size(long n) {
		if (0 != (n & ~0x7FFFFFFFFL)) {
			if (0 != (n & ~0x1FFFFFFFFFFFFL)) {
				if (0 != (n & ~0xFFFFFFFFFFFFFFL))
					return 9;
				return 8;
			}
			if (0 != (n & ~0x3FFFFFFFFFFL))
				return 7;
			return 6;
		}
		if (0 != (n & ~0x3FFFL)) {
			if (0 != (n & ~0x1FFFFFL)) {
				if (0 != (n & ~0xFFFFFFFL))
					return 5;
				return 4;
			}
			return 3;
		}
		if (0 != (n & ~0x7FL))
			return 2;
		return 1;
	}

	public static int bridge_pshort_packed_size(int n) {
		return bridge_ppshort_packed_size((n << 1) ^ (n >> 15));
	}

	public static int bridge_pint_packed_size(int n) {
		return bridge_ppint_packed_size((n << 1) ^ (n >> 31));
	}

	public static int bridge_plong_packed_size(long n) {
		return bridge_pplong_packed_size((n << 1) ^ (n >> 63));
	}

	/* counter coded in this way:
	 * 01111111                                             0-127        (1 byte)
	 * 11111111 01111111                                  128-16383      (2 bytes)
	 * 11111111 11111111 01111111                       16384-2097151    (3 bytes)
	 * 11111111 11111111 11111111 01111111            2097151-268435455  (4 bytes)
	 * 11111111 11111111 11111111 11111111 00000111 268435456-2147483647 (5 bytes) */

	/* note: BRIDGE_MAX + BRIDGE_COUNTER_MAX_PACKED_SIZE must be positive */
	public static final int BRIDGE_COUNTER_MAX_PACKED_SIZE = 5;

	public static int bridge_counter_packed_size(int c/*>=0*/) {
		if (c > 0x3FFF) {
			if (c > 0x1FFFFF) {
				if (c > 0xFFFFFFF)
					return 5;
				return 4;
			}
			return 3;
		}
		if (c > 0x7F)
			return 2;
		return 1;
	}

	public static int bridge_counter_packed_size_minus_one(int c/*>=0*/) {
		if (c > 0x3FFF) {
			if (c > 0x1FFFFF) {
				if (c > 0xFFFFFFF)
					return 4;
				return 3;
			}
			return 2;
		}
		if (c > 0x7F)
			return 1;
		return 0;
	}

	/* add number of bytes needed to pack bit array, 1 is already counted in c */
	public static int bridge_add_bit_array_packed_size_(int c, int bit_count/*[0..BRIDGE_BIT_COUNT_MAX]*/)
		throws BridgeBytesOverflow
	{
		if (bit_count != 0) {
			int bit_bytes = bridge_bit_array_size_nz(bit_count/*!=0*/);
			return _br_of_add_bytes(c, bit_bytes + bridge_ppint_packed_size(bit_count) - 1);
		}
		return c;
	}

	public static int _bridge_pack_ppshort(byte[] dst, int at, int n) {
		int x = n & 0xFFFF;
		while (x > 0x7F) {
			dst[at++] = (byte)(0xFF & (x | 0x80));
			x >>>= 7;
		}
		dst[at++] = (byte)(0xFF & x);
		return at;
	}

	public static int _bridge_pack_ppint(byte[] dst, int at, int n) {
		while ((n & ~0x7F) != 0) {
			dst[at++] = (byte)(0xFF & (n | 0x80));
			n >>>= 7;
		}
		dst[at++] = (byte)(0xFF & n);
		return at;
	}

	public static int _bridge_pack_pplong(byte[] dst, int at, long n) {
		int i = 0;
		while ((n & ~0x7FL) != 0) {
			dst[at++] = (byte)(0xFF & (n | 0x80));
			n >>>= 7;
			if (++i == 8)
				break;
		}
		dst[at++] = (byte)(0xFF & n);
		return at;
	}

	public static int _bridge_unpack_ppshort_no_lim(_upk_mem_no_lim_info pos) {
		int at = pos.at;
		byte[] mem = pos.mem;
		int x = mem[at++];
		if (x < 0) {
			x ^= mem[at++] << 7;
			if (x >= 0)
				x ^= (mem[at++] << 14) ^ 0x3F80;
			else
				x ^= 0xFFFFFF80;
		}
		pos.at = at;
		return x; /* should be <= 0xFFFF if src is good */
	}

	public static int _bridge_unpack_ppshort(_upk_mem_info pos) throws BridgeBytesOverLimit {
		int at = pos.at;
		int lim = pos.lim;
		if (lim - at >= 3)
			return _bridge_unpack_ppshort_no_lim(pos);
		if (at == lim)
			throw new BridgeBytesOverLimit();
		{
			byte[] mem = pos.mem;
			int x = mem[at++];
			if (x < 0) {
				if (at == lim)
					throw new BridgeBytesOverLimit();
				x ^= mem[at++] << 7;
				if (x >= 0) {
					if (at == lim)
						throw new BridgeBytesOverLimit();
					x ^= (mem[at++] << 14) ^ 0x3F80;
				}
				else
					x ^= 0xFFFFFF80;
			}
			pos.at = at;
			return x; /* should be <= 0xFFFF if src is good */
		}
	}

	public static int bridge_unpack_ppint_no_lim(_upk_mem_no_lim_info pos) {
		int at = pos.at;
		byte[] mem = pos.mem;
		int x = mem[at++];
		if (x < 0) {
			x ^= mem[at++] << 7;
			if (x >= 0) {
				x ^= mem[at++] << 14;
				if (x < 0) {
					x ^= mem[at++] << 21;
					if (x >= 0)
						x ^= (mem[at++] << 28) ^ 0xFE03F80;
					else
						x ^= 0xFFE03F80;
				}
				else
					x ^= 0x3F80;
			}
			else
				x ^= 0xFFFFFF80;
		}
		pos.at = at;
		return x;
	}

	public static int bridge_unpack_ppint(_upk_mem_info pos) throws BridgeBytesOverLimit {
		int at = pos.at;
		int lim = pos.lim;
		if (lim - at >= 5)
			return bridge_unpack_ppint_no_lim(pos);
		if (at == lim)
			throw new BridgeBytesOverLimit();
		{
			byte[] mem = pos.mem;
			int x = mem[at++];
			if (x < 0) {
				if (at == lim)
					throw new BridgeBytesOverLimit();
				x ^= mem[at++] << 7;
				if (x >= 0) {
					if (at == lim)
						throw new BridgeBytesOverLimit();
					x ^= mem[at++] << 14;
					if (x < 0) {
						if (at == lim)
							throw new BridgeBytesOverLimit();
						x ^= mem[at++] << 21;
						if (x >= 0) {
							if (at == lim)
								throw new BridgeBytesOverLimit();
							x ^= (mem[at++] << 28) ^ 0xFE03F80;
						}
						else
							x ^= 0xFFE03F80;
					}
					else
						x ^= 0x3F80;
				}
				else
					x ^= 0xFFFFFF80;
			}
			pos.at = at;
			return x;
		}
	}

	public static long bridge_unpack_pplong_no_lim(_upk_mem_no_lim_info pos) {
		int at = pos.at;
		byte[] mem = pos.mem;
		long x = mem[at++];
		if (x < 0) {
			x ^= (long)mem[at++] << 7;
			if (x >= 0) {
				x ^= (long)mem[at++] << 14;
				if (x < 0) {
					x ^= (long)mem[at++] << 21;
					if (x >= 0) {
						x ^= (long)mem[at++] << 28;
						if (x < 0) {
							x ^= (long)mem[at++] << 35;
							if (x >= 0) {
								x ^= (long)mem[at++] << 42;
								if (x < 0) {
									x ^= (long)mem[at++] << 49;
									if (x >= 0)
										x ^= ((long)mem[at++] << 56) ^ 0xFE03F80FE03F80L;
									else
										x ^= 0xFFFE03F80FE03F80L;
								}
								else
									x ^= 0x3F80FE03F80L;
							}
							else
								x ^= 0xFFFFFFF80FE03F80L;
						}
						else
							x ^= 0xFE03F80L;
					}
					else
						x ^= 0xFFFFFFFFFFE03F80L;
				}
				else
					x ^= 0x3F80L;
			}
			else
				x ^= 0xFFFFFFFFFFFFFF80L;
		}
		pos.at = at;
		return x;
	}

	public static long bridge_unpack_pplong(_upk_mem_info pos) throws BridgeBytesOverLimit {
		int at = pos.at;
		int lim = pos.lim;
		if (lim - at >= 9)
			return bridge_unpack_pplong_no_lim(pos);
		if (at == lim)
			throw new BridgeBytesOverLimit();
		{
			byte[] mem = pos.mem;
			long x = mem[at++];
			if (x < 0) {
				if (at == lim)
					throw new BridgeBytesOverLimit();
				x ^= (long)mem[at++] << 7;
				if (x >= 0) {
					if (at == lim)
						throw new BridgeBytesOverLimit();
					x ^= (long)mem[at++] << 14;
					if (x < 0) {
						if (at == lim)
							throw new BridgeBytesOverLimit();
						x ^= (long)mem[at++] << 21;
						if (x >= 0) {
							if (at == lim)
								throw new BridgeBytesOverLimit();
							x ^= (long)mem[at++] << 28;
							if (x < 0) {
								if (at == lim)
									throw new BridgeBytesOverLimit();
								x ^= (long)mem[at++] << 35;
								if (x >= 0) {
									if (at == lim)
										throw new BridgeBytesOverLimit();
									x ^= (long)mem[at++] << 42;
									if (x < 0) {
										if (at == lim)
											throw new BridgeBytesOverLimit();
										x ^= (long)mem[at++] << 49;
										if (x >= 0) {
											if (at == lim)
												throw new BridgeBytesOverLimit();
											x ^= ((long)mem[at++] << 56) ^ 0xFE03F80FE03F80L;
										}
										else
											x ^= 0xFFFE03F80FE03F80L;
									}
									else
										x ^= 0x3F80FE03F80L;
								}
								else
									x ^= 0xFFFFFFF80FE03F80L;
							}
							else
								x ^= 0xFE03F80L;
						}
						else
							x ^= 0xFFFFFFFFFFE03F80L;
					}
					else
						x ^= 0x3F80L;
				}
				else
					x ^= 0xFFFFFFFFFFFFFF80L;
			}
			pos.at = at;
			return x;
		}
	}

	public static int _bridge_pack_pshort(byte[] dst, int at, int n) {
		return _bridge_pack_ppshort(dst, at, (n << 1) ^ (n >> 15));
	}

	public static int _bridge_pack_pint(byte[] dst, int at, int n) {
		return _bridge_pack_ppint(dst, at, (n << 1) ^ (n >> 31));
	}

	public static int _bridge_pack_plong(byte[] dst, int at, long n) {
		return _bridge_pack_pplong(dst, at, (n << 1) ^ (n >> 63));
	}

	public static short bridge_unpack_pshort_no_lim(_upk_mem_no_lim_info pos) {
		int x = _bridge_unpack_ppshort_no_lim(pos); /* <= 0xFFFF if src is good */
		return (short)(((x >>> 1) ^ -(x & 1)) & 0xFFFF);
	}

	public static short bridge_unpack_pshort(_upk_mem_info pos) throws BridgeBytesOverLimit {
		int x = _bridge_unpack_ppshort(pos); /* <= 0xFFFF if src is good */
		return (short)(((x >>> 1) ^ -(x & 1)) & 0xFFFF);
	}

	public static short bridge_unpack_ppshort_no_lim(_upk_mem_no_lim_info pos) {
		return (short)_bridge_unpack_ppshort_no_lim(pos); /* <= 0xFFFF if src is good */
	}

	public static short bridge_unpack_ppshort(_upk_mem_info pos) throws BridgeBytesOverLimit {
		return (short)_bridge_unpack_ppshort(pos); /* <= 0xFFFF if src is good */
	}

	public static int bridge_unpack_pint_no_lim(_upk_mem_no_lim_info pos) {
		int x = bridge_unpack_ppint_no_lim(pos);
		return (x >>> 1) ^ -(x & 1);
	}

	public static int bridge_unpack_pint(_upk_mem_info pos) throws BridgeBytesOverLimit {
		int x = bridge_unpack_ppint(pos);
		return (x >>> 1) ^ -(x & 1);
	}

	public static long bridge_unpack_plong_no_lim(_upk_mem_no_lim_info pos) {
		long x = bridge_unpack_pplong_no_lim(pos);
		return (x >>> 1) ^ -(x & 1);
	}

	public static long bridge_unpack_plong(_upk_mem_info pos) throws BridgeBytesOverLimit {
		long x = bridge_unpack_pplong(pos);
		return (x >>> 1) ^ -(x & 1);
	}

	public static int _bridge_pack_counter(byte[] dst, int at, int c/*[0..BRIDGE_MAX]*/) {
		while (c > 0x7F) {
			dst[at++] = (byte)(0xFF & (c | 0x80));
			c >>>= 7;
		}
		dst[at++] = (byte)(0xFF & c);
		return at;
	}

	public static int _bridge_unpack_counter_no_lim(_upk_mem_no_lim_info pos) {
		return bridge_unpack_ppint_no_lim(pos); /* any value */
	}

	public static int _bridge_unpack_counter(_upk_mem_info pos) throws BridgeBytesOverLimit {
		return bridge_unpack_ppint(pos); /* any value */
	}

	public static int _bridge_pack_bit_counter(byte[] dst, int at, int bit_count/*[0..BRIDGE_BIT_COUNT_MAX]*/) {
		return _bridge_pack_ppint(dst, at, bit_count);
	}

	public static int _bridge_unpack_and_check_byte_counter(_upk_mem_info pos)
		throws BridgeBadCounter, BridgeBytesOverLimit
	{
		int c = _bridge_unpack_counter(pos);
		_bridge_check_byte_array_counter(pos.at, pos.lim, c);
		return c;
	}

	public static int _get_short_big(byte[] src, int at) {
		int a = src[at++];
		int b = src[at] & 0xFF;
		return ((a << 8) | b) & 0xFFFF;
	}

	public static int _get_short_little(byte[] src, int at) {
		int a = src[at++] & 0xFF;
		int b = src[at];
		return (a | (b << 8)) & 0xFFFF;
	}

	public static int _bridge_get_short_array(	byte[] src, int at,
		short[] dst, int count/*[0..BRIDGE_MAX/2]*/, boolean big)
	{
		int i = 0;
		if (big) {
			while (i < count) {
				dst[i++] = (short)_get_short_big(src, at);
				at += 2;
			}
		}
		else {
			while (i < count) {
				dst[i++] = (short)_get_short_little(src, at);
				at += 2;
			}
		}
		return at;
	}

	public static int _put_short_big(byte[] dst, int at, int x) {
		dst[at++] = (byte)((x >>> 8) & 0xFF);
		dst[at++] = (byte)(x & 0xFF);
		return at;
	}

	public static int _put_short_little(byte[] dst, int at, int x) {
		dst[at++] = (byte)(x & 0xFF);
		dst[at++] = (byte)((x >>> 8) & 0xFF);
		return at;
	}

	public static int _bridge_put_short_array(	byte[] dst, int at,
		short[] src/*null?*/, int count/*[0..BRIDGE_MAX/2]*/, boolean big)
	{
		int i = 0;
		if (big) {
			while (i < count)
				at = _put_short_big(dst, at, src[i++]);
		}
		else {
			while (i < count)
				at = _put_short_little(dst, at, src[i++]);
		}
		return at;
	}

	public static int _get_int_big(byte[] src, int at) {
		int a = src[at++];
		int b = src[at++] & 0xFF;
		int c = src[at++] & 0xFF;
		int d = src[at] & 0xFF;
		return (a << 24) | (b << 16) | (c << 8) | d;
	}

	public static int _get_int_little(byte[] src, int at) {
		int a = src[at++] & 0xFF;
		int b = src[at++] & 0xFF;
		int c = src[at++] & 0xFF;
		int d = src[at];
		return a | (b << 8) | (c << 16) | (d << 24);
	}

	public static int _bridge_get_int_array(	byte[] src, int at,
		int[] dst, int count/*[0..BRIDGE_MAX/4]*/, boolean big)
	{
		int i = 0;
		if (big) {
			while (i < count) {
				dst[i++] = _get_int_big(src, at);
				at += 4;
			}
		}
		else {
			while (i < count) {
				dst[i++] = _get_int_little(src, at);
				at += 4;
			}
		}
		return at;
	}

	public static int _bridge_get_float_array(	byte[] src, int at,
		float[] dst, int count/*[0..BRIDGE_MAX/4]*/, boolean big)
	{
		int i = 0;
		if (big) {
			while (i < count) {
				dst[i++] = Float.intBitsToFloat(_get_int_big(src, at));
				at += 4;
			}
		}
		else {
			while (i < count) {
				dst[i++] = Float.intBitsToFloat(_get_int_little(src, at));
				at += 4;
			}
		}
		return at;
	}

	public static int _put_int_big(byte[] dst, int at, int x) {
		dst[at++] = (byte)(x >>> 24);
		dst[at++] = (byte)((x >>> 16) & 0xFF);
		dst[at++] = (byte)((x >>> 8) & 0xFF);
		dst[at++] = (byte)(x & 0xFF);
		return at;
	}

	public static int _put_int_little(byte[] dst, int at, int x) {
		dst[at++] = (byte)(x & 0xFF);
		dst[at++] = (byte)((x >>> 8) & 0xFF);
		dst[at++] = (byte)((x >>> 16) & 0xFF);
		dst[at++] = (byte)(x >>> 24);
		return at;
	}

	public static int _bridge_put_int_array(byte[] dst, int at,
		int[] src/*null?*/, int count/*[0..BRIDGE_MAX/4]*/, boolean big)
	{
		int i = 0;
		if (big) {
			while (i < count)
				at = _put_int_big(dst, at, src[i++]);
		}
		else {
			while (i < count)
				at = _put_int_little(dst, at, src[i++]);
		}
		return at;
	}

	public static int _bridge_put_float_array(byte[] dst, int at,
		float[] src/*null?*/, int count/*[0..BRIDGE_MAX/4]*/, boolean big)
	{
		int i = 0;
		if (big) {
			while (i < count)
				at = _put_int_big(dst, at, Float.floatToRawIntBits(src[i++]));
		}
		else {
			while (i < count)
				at = _put_int_little(dst, at, Float.floatToRawIntBits(src[i++]));
		}
		return at;
	}

	public static long _get_long_big(byte[] src, int at) {
		int x = _get_int_big(src, at);
		int y = _get_int_big(src, at + 4);
		return ((long)x << 32) | ((long)y & 0xFFFFFFFFL);
	}

	public static long _get_long_little(byte[] src, int at) {
		int x = _get_int_little(src, at);
		int y = _get_int_little(src, at + 4);
		return ((long)x & 0xFFFFFFFFL) | ((long)y << 32);
	}

	public static int _bridge_get_long_array(byte[] src, int at,
		long[] dst, int count/*[0..BRIDGE_MAX/8]*/, boolean big)
	{
		int i = 0;
		if (big) {
			while (i < count) {
				dst[i++] = _get_long_big(src, at);
				at += 8;
			}
		}
		else {
			while (i < count) {
				dst[i++] = _get_long_little(src, at);
				at += 8;
			}
		}
		return at;
	}

	public static int _bridge_get_double_array(byte[] src, int at,
		double[] dst, int count/*[0..BRIDGE_MAX/8]*/, boolean big)
	{
		int i = 0;
		if (big) {
			while (i < count) {
				dst[i++] = Double.longBitsToDouble(_get_long_big(src, at));
				at += 8;
			}
		}
		else {
			while (i < count) {
				dst[i++] = Double.longBitsToDouble(_get_long_little(src, at));
				at += 8;
			}
		}
		return at;
	}

	public static int _put_long_big(byte[] dst, int at, long x) {
		at = _put_int_big(dst, at, (int)(x >>> 32));
		at = _put_int_big(dst, at, (int)(x & 0xFFFFFFFFL));
		return at;
	}

	public static int _put_long_little(byte[] dst, int at, long x) {
		at = _put_int_little(dst, at, (int)(x & 0xFFFFFFFFL));
		at = _put_int_little(dst, at, (int)(x >>> 32));
		return at;
	}

	public static int _bridge_put_long_array(byte[] dst, int at,
		long[] src/*null?*/, int count/*[0..BRIDGE_MAX/8]*/, boolean big)
	{
		int i = 0;
		if (big) {
			while (i < count)
				at = _put_long_big(dst, at, src[i++]);
		}
		else {
			while (i < count)
				at = _put_long_little(dst, at, src[i++]);
		}
		return at;
	}

	public static int _bridge_put_double_array(byte[] dst, int at,
		double[] src/*null?*/, int count/*[0..BRIDGE_MAX/8]*/, boolean big)
	{
		int i = 0;
		if (big) {
			while (i < count)
				at = _put_long_big(dst, at, Double.doubleToRawLongBits(src[i++]));
		}
		else {
			while (i < count)
				at = _put_long_little(dst, at, Double.doubleToRawLongBits(src[i++]));
		}
		return at;
	}

	public static int _bridge_put_bytes(byte[] dst, int at, byte[] src, int bytes) {
		System.arraycopy(src, 0, dst, at, bytes);
		return at + bytes;
	}

	/* pack array couter, then array itsef */

	public static int bridge_pack_byte_array(byte[] dst, int at, byte[] src/*null?*/) {
		if (null != src) {
			at = _bridge_pack_counter(dst, at, src.length/*[0..BRIDGE_MAX)*/);
			return _bridge_put_bytes(dst, at, src, src.length);
		}
		return at + 1/*0*/;
	}

	public static int bridge_pack_short_array(byte[] dst, int at, short[] src/*null?*/, boolean big) {
		if (null != src) {
			at = _bridge_pack_counter(dst, at, src.length/*[0..BRIDGE_MAX)*/);
			return _bridge_put_short_array(dst, at, src, src.length, big);
		}
		return at + 1/*0*/;
	}

	public static int bridge_pack_int_array(byte[] dst, int at, int[] src/*null?*/, boolean big) {
		if (null != src) {
			at = _bridge_pack_counter(dst, at, src.length/*[0..BRIDGE_MAX)*/);
			return _bridge_put_int_array(dst, at, src, src.length, big);
		}
		return at + 1/*0*/;
	}

	public static int bridge_pack_long_array(byte[] dst, int at, long[] src/*null?*/, boolean big) {
		if (null != src) {
			at = _bridge_pack_counter(dst, at, src.length/*[0..BRIDGE_MAX)*/);
			return _bridge_put_long_array(dst, at, src, src.length, big);
		}
		return at + 1/*0*/;
	}

	public static int bridge_pack_float_array(byte[] dst, int at, float[] src/*null?*/, boolean big) {
		if (null != src) {
			at = _bridge_pack_counter(dst, at, src.length/*[0..BRIDGE_MAX)*/);
			return _bridge_put_float_array(dst, at, src, src.length, big);
		}
		return at + 1/*0*/;
	}

	public static int bridge_pack_double_array(byte[] dst, int at, double[] src/*null?*/, boolean big) {
		if (null != src) {
			at = _bridge_pack_counter(dst, at, src.length/*[0..BRIDGE_MAX)*/);
			return _bridge_put_double_array(dst, at, src, src.length, big);
		}
		return at + 1/*0*/;
	}

	/* allocate, then unpack byte, short, int, float, long and double arrays */

	public static byte[] bridge_unpack_byte_array_no_lim(
		_upk_mem_no_lim_info pos, int count/*[0..BRIDGE_MAX]*/)
	{
		int at = pos.at;
		pos.at = at + count;
		return java.util.Arrays.copyOfRange(pos.mem, at, pos.at);
	}

	public static short[] bridge_unpack_short_array_no_lim(
		_upk_mem_no_lim_info pos, int count/*[0..BRIDGE_MAX/2]*/, boolean big)
	{
		short[] arr = new short[count];
		pos.at = _bridge_get_short_array(pos.mem, pos.at, arr, count, big);
		return arr;
	}

	public static int[] bridge_unpack_int_array_no_lim(
		_upk_mem_no_lim_info pos, int count/*[0..BRIDGE_MAX/4]*/, boolean big)
	{
		int[] arr = new int[count];
		pos.at = _bridge_get_int_array(pos.mem, pos.at, arr, count, big);
		return arr;
	}

	public static float[] bridge_unpack_float_array_no_lim(
		_upk_mem_no_lim_info pos, int count/*[0..BRIDGE_MAX/4]*/, boolean big)
	{
		float[] arr = new float[count];
		pos.at = _bridge_get_float_array(pos.mem, pos.at, arr, count, big);
		return arr;
	}

	public static long[] bridge_unpack_long_array_no_lim(
		_upk_mem_no_lim_info pos, int count/*[0..BRIDGE_MAX/8]*/, boolean big)
	{
		long[] arr = new long[count];
		pos.at = _bridge_get_long_array(pos.mem, pos.at, arr, count, big);
		return arr;
	}

	public static double[] bridge_unpack_double_array_no_lim(
		_upk_mem_no_lim_info pos, int count/*[0..BRIDGE_MAX/8]*/, boolean big)
	{
		double[] arr = new double[count];
		pos.at = _bridge_get_double_array(pos.mem, pos.at, arr, count, big);
		return arr;
	}

	/* put byte,short,int,long,float,double */

	public static int _bridge_put_byte(byte[] dst, int at, int x) {
		dst[at++] = (byte)(x & 0xFF);
		return at;
	}

	public static int _bridge_put_short(byte[] dst, int at, int x, boolean big) {
		return big ? _put_short_big(dst, at, x) : _put_short_little(dst, at, x);
	}

	public static int _bridge_put_int(byte[] dst, int at, int x, boolean big) {
		return big ? _put_int_big(dst, at, x) : _put_int_little(dst, at, x);
	}

	public static int _bridge_put_long(byte[] dst, int at, long x, boolean big) {
		return big ? _put_long_big(dst, at, x) : _put_long_little(dst, at, x);
	}

	public static int _bridge_put_float(byte[] dst, int at, float x, boolean big) {
		return _bridge_put_int(dst, at, Float.floatToRawIntBits(x), big);
	}

	public static int _bridge_put_double(byte[] dst, int at, double x, boolean big) {
		return _bridge_put_long(dst, at, Double.doubleToRawLongBits(x), big);
	}

	/* get byte,short,int,long,float,double */

	public static int _bridge_get_byte_no_lim(byte[] src, int at) {
		int x = src[at];
		return x & 0xFF;
	}

	public static int _bridge_get_short_no_lim(byte[] src, int at, boolean big) {
		return big ? _get_short_big(src, at) : _get_short_little(src, at); /* <= 0xFFFF */
	}

	public static int _bridge_get_int_no_lim(byte[] src, int at, boolean big) {
		return big ? _get_int_big(src, at) : _get_int_little(src, at);
	}

	public static long _bridge_get_long_no_lim(byte[] src, int at, boolean big) {
		return big ? _get_long_big(src, at) : _get_long_little(src, at);
	}

	public static float _bridge_get_float_no_lim(byte[] src, int at, boolean big) {
		return Float.intBitsToFloat(_bridge_get_int_no_lim(src, at, big));
	}

	public static double _bridge_get_double_no_lim(byte[] src, int at, boolean big) {
		return Double.longBitsToDouble(_bridge_get_long_no_lim(src, at, big));
	}

	/* unpack byte,short,int,long,float,double */

	public static byte bridge_unpack_byte_no_lim(_upk_mem_no_lim_info pos) {
		return (byte)_bridge_get_byte_no_lim(pos.mem, pos.at++);
	}

	public static byte bridge_unpack_byte(_upk_mem_info pos) throws BridgeBytesOverLimit {
		if (pos.at == pos.lim)
			throw new BridgeBytesOverLimit();
		return bridge_unpack_byte_no_lim(pos);
	}

	public static short bridge_unpack_short_no_lim(_upk_mem_no_lim_info pos, boolean big) {
		int at = pos.at;
		pos.at += 2;
		return (short)_bridge_get_short_no_lim(pos.mem, at, big);
	}

	public static short bridge_unpack_short(_upk_mem_info pos, boolean big) throws BridgeBytesOverLimit {
		if (pos.lim - pos.at < 2)
			throw new BridgeBytesOverLimit();
		return bridge_unpack_short_no_lim(pos, big);
	}

	public static int bridge_unpack_int_no_lim(_upk_mem_no_lim_info pos, boolean big) {
		int at = pos.at;
		pos.at += 4;
		return _bridge_get_int_no_lim(pos.mem, at, big);
	}

	public static int bridge_unpack_int(_upk_mem_info pos, boolean big) throws BridgeBytesOverLimit {
		if (pos.lim - pos.at < 4)
			throw new BridgeBytesOverLimit();
		return bridge_unpack_int_no_lim(pos, big);
	}

	public static long bridge_unpack_long_no_lim(_upk_mem_no_lim_info pos, boolean big) {
		int at = pos.at;
		pos.at += 8;
		return _bridge_get_long_no_lim(pos.mem, at, big);
	}

	public static long bridge_unpack_long(_upk_mem_info pos, boolean big) throws BridgeBytesOverLimit {
		if (pos.lim - pos.at < 8)
			throw new BridgeBytesOverLimit();
		return bridge_unpack_long_no_lim(pos, big);
	}

	public static float bridge_unpack_float_no_lim(_upk_mem_no_lim_info pos, boolean big) {
		int at = pos.at;
		pos.at += 4;
		return _bridge_get_float_no_lim(pos.mem, at, big);
	}

	public static float bridge_unpack_float(_upk_mem_info pos, boolean big) throws BridgeBytesOverLimit {
		if (pos.lim - pos.at < 4)
			throw new BridgeBytesOverLimit();
		return bridge_unpack_float_no_lim(pos, big);
	}

	public static double bridge_unpack_double_no_lim(_upk_mem_no_lim_info pos, boolean big) {
		int at = pos.at;
		pos.at += 8;
		return _bridge_get_double_no_lim(pos.mem, at, big);
	}

	public static double bridge_unpack_double(_upk_mem_info pos, boolean big) throws BridgeBytesOverLimit {
		if (pos.lim - pos.at < 8)
			throw new BridgeBytesOverLimit();
		return bridge_unpack_double_no_lim(pos, big);
	}

	/* pack number of bits in array */
	/* pack array of bits */
	public static int bridge_pack_bit_array(byte[] dst, int at,
		byte[] arr/*null?*/, int bit_count/*[0..BRIDGE_BIT_COUNT_MAX]*/)
	{
		if (bit_count != 0) {
			at = _bridge_pack_bit_counter(dst, at, bit_count);
			return _bridge_put_bytes(dst, at, arr, bridge_bit_array_size_nz(bit_count/*!=0*/));
		}
		return at + 1/*0*/;
	}

	/* allocate and copy array of bits */
	public static byte[] bridge_unpack_bit_array_no_lim(_upk_mem_no_lim_info pos, int bit_bytes/*[1..BRIDGE_MAX)*/) {
		int at = pos.at;
		pos.at = at + bit_bytes;
		return java.util.Arrays.copyOfRange(pos.mem, at, pos.at);
	}

	/* allocate and copy array of bits */
	public static byte[] _bridge_unpack_bit_array(_upk_mem_info pos,
		int bit_bytes/*[1..BRIDGE_MAX)*/) throws BridgeBytesOverLimit
	{
		upk_check_mem_lim(pos.at, pos.lim, bit_bytes);
		return bridge_unpack_bit_array_no_lim(pos, bit_bytes);
	}

	/* allocate and copy array of bits, for bit_count == 0 return empty array */
	public static byte[] bridge_unpack_bit_array(
		_upk_mem_info pos, int bit_count) throws BridgeBytesOverLimit
	{
		/* BRIDGE_BIT_COUNT_MAX == 0xFFFFFFFFL, so bit_count is in [0..BRIDGE_BIT_COUNT_MAX] range */
		if (bit_count != 0) {
			int bit_bytes = bridge_bit_array_size_nz(bit_count/*!=0*/); /*[1..BRIDGE_MAX)*/
			return _bridge_unpack_bit_array(pos, bit_bytes/*[[1..BRIDGE_MAX)*/);
		}
		return new byte[0];
	}

	/* add number of bytes needed to pack an array with counter, 1 is already counted in c */

	public static int bridge_add_byte_array_packed_size_(int c, byte[] arr/*null?*/)
		throws BridgeTooManyElements, BridgeBytesOverflow
	{
		if (null != arr) {
			if (arr.length > BRIDGE_MAX - BRIDGE_COUNTER_MAX_PACKED_SIZE)
				throw new BridgeTooManyElements();
			return _br_of_add_bytes(c, arr.length + bridge_counter_packed_size_minus_one(arr.length));
		}
		return c;
	}

	public static int bridge_add_short_array_packed_size_(int c, short[] arr/*null?*/)
		throws BridgeTooManyElements, BridgeBytesOverflow
	{
		if (null != arr) {
			if (arr.length > (BRIDGE_MAX - BRIDGE_COUNTER_MAX_PACKED_SIZE)/2)
				throw new BridgeTooManyElements();
			return _br_of_add_bytes(c, arr.length*2 + bridge_counter_packed_size_minus_one(arr.length));
		}
		return c;
	}

	public static int bridge_add_int_array_packed_size_(int c, int[] arr/*null?*/)
		throws BridgeTooManyElements, BridgeBytesOverflow
	{
		if (null != arr) {
			if (arr.length > (BRIDGE_MAX - BRIDGE_COUNTER_MAX_PACKED_SIZE)/4)
				throw new BridgeTooManyElements();
			return _br_of_add_bytes(c, arr.length*4 + bridge_counter_packed_size_minus_one(arr.length));
		}
		return c;
	}

	public static int bridge_add_long_array_packed_size_(int c, long[] arr/*null?*/)
		throws BridgeTooManyElements, BridgeBytesOverflow
	{
		if (null != arr) {
			if (arr.length > (BRIDGE_MAX - BRIDGE_COUNTER_MAX_PACKED_SIZE)/8)
				throw new BridgeTooManyElements();
			return _br_of_add_bytes(c, arr.length*8 + bridge_counter_packed_size_minus_one(arr.length));
		}
		return c;
	}

	public static int bridge_add_float_array_packed_size_(int c, float[] arr/*null?*/)
		throws BridgeTooManyElements, BridgeBytesOverflow
	{
		if (null != arr) {
			if (arr.length > (BRIDGE_MAX - BRIDGE_COUNTER_MAX_PACKED_SIZE)/4)
				throw new BridgeTooManyElements();
			return _br_of_add_bytes(c, arr.length*4 + bridge_counter_packed_size_minus_one(arr.length));
		}
		return c;
	}

	public static int bridge_add_double_array_packed_size_(int c, double[] arr/*null?*/)
		throws BridgeTooManyElements, BridgeBytesOverflow
	{
		if (null != arr) {
			if (arr.length > (BRIDGE_MAX - BRIDGE_COUNTER_MAX_PACKED_SIZE)/8)
				throw new BridgeTooManyElements();
			return _br_of_add_bytes(c, arr.length*8 + bridge_counter_packed_size_minus_one(arr.length));
		}
		return c;
	}

	/* utf16 -> utf8 */
	public static int _utf8_len(String s/*!=null*/) {
		int lim = s.length();
		int i = 0;
		int len = 0;
		int len2 = 0;
		while (i < lim) {
			int c = s.charAt(i++);
			if (c >= 0x80) {
				if (c >= 0x800) {
					if (0xD800 == (c & 0xFC00)) {
						if (i == lim)
							return -1; /* bad utf16 char */
						i++; /* don't count next surrogate */
					}
					len2++;
					len++;
					continue;
				}
				len++;
			}
		}
		len += lim;
		if (len >= 0)
			len += len2;
		return len; /* <0 on signed integer overflow */
	}

	/* utf16 -> utf8 */
	public static int _utf8_pack(byte[] dst, int at, String s/*!=null*/) {
		int lim = s.length();
		int i = 0;
		while (i < lim) {
			int c = s.charAt(i++);
			if (c >= 0x80) {
				if (c >= 0x800) {
					if (0xD800 == (c & 0xFC00)) {
						c = ((c << 10) ^ s.charAt(i++) ^ 0xA0DC00) + 0x10000;
						dst[at++] = (byte)(c >> 18);
						dst[at++] = (byte)(((c >> 12) | 0x80) & 0xBF);
					}
					else
						dst[at++] = (byte)((c >> 12) | 0xE0);
					dst[at++] = (byte)(((c >> 6) | 0x80) & 0xBF);
				}
				else
					dst[at++] = (byte)((c >> 6) | 0xC0);
				dst[at++] = (byte)((c | 0x80) & 0xBF);
				continue;
			}
			dst[at++] = (byte)c;
		}
		return at;
	}

	/* utf8 -> utf16, allow nulls inside string */
	public static String _utf8_unpack(byte[] src, int at, int lim/*>=at*/) {
		char[] buf = new char[lim - at];
		int i = 0;
		while (at < lim) {
			int a = src[at++];
			if (a < 0) {
				int b;
				if (a > 0xFFFFFFDF) {
					if (a > 0xFFFFFFEF) {
						if (a > 0xFFFFFFF7)
							return null; /* max 4 utf8 bytes for char */
						if (lim - at <= 2)
							return null;
						b = src[at++];
						if (0x80 != (b & 0xC0))
							return null;
						a = (a << 6) ^ b;
						b = src[at++];
						if (0x80 != (b & 0xC0))
							return null;
						a = (a << 6) ^ b;
						b = src[at++];
						if (0x80 != (b & 0xC0))
							return null;
						a = (a << 6) ^ b ^ 0x381F80; /* 0xFFC00000 ^ 0xFFF81F80 */
						a -= 0x10000;
						buf[i++] = (char)((a >> 10) + 0xD800);
						a = (a & 0x3FF) + 0xDC00;
					}
					else {
						if (lim - at <= 1)
							return null;
						b = src[at++];
						if (0x80 != (b & 0xC0))
							return null;
						a = (a << 6) ^ b;
						b = src[at++];
						if (0x80 != (b & 0xC0))
							return null;
						a = (a << 6) ^ b ^ 0xFFFE1F80; /* 0xFFFE0000 ^ 0x1F80 */
					}
				}
				else if (a > 0xFFFFFFBF) {
					if (at == lim)
						return null;
					b = src[at++];
					if (0x80 != (b & 0xC0))
						return null;
					a = (a << 6) ^ b ^ 0xF80; /* 0xFFFFF000 ^ 0xFFFFFF80 */
				}
				else
					return null; /* not expecting 10xxxxxx */
				buf[i++] = (char)a;
				continue;
			}
			buf[i++] = (char)a;
		}
		return new String(buf, 0, i);
	}

	/* utf8 -> utf16, until first 0 */
	public static String _utf8_unpack_z(_upk_mem_info pos) {
		int at = pos.at;
		int lim = pos.lim;
		byte[] mem = pos.mem;
		char[] buf = new char[lim - at];
		int i = 0;
		while (at < lim) {
			int a = mem[at++];
			if (a < 0) {
				int b;
				if (a > 0xFFFFFFDF) {
					if (a > 0xFFFFFFEF) {
						if (a > 0xFFFFFFF7)
							return null; /* max 4 utf8 bytes for char */
						if (lim - at <= 2)
							return null;
						b = mem[at++];
						if (0x80 != (b & 0xC0))
							return null;
						a = (a << 6) ^ b;
						b = mem[at++];
						if (0x80 != (b & 0xC0))
							return null;
						a = (a << 6) ^ b;
						b = mem[at++];
						if (0x80 != (b & 0xC0))
							return null;
						a = (a << 6) ^ b ^ 0x381F80; /* 0xFFC00000 ^ 0xFFF81F80 */
						a -= 0x10000;
						buf[i++] = (char)((a >> 10) + 0xD800);
						a = (a & 0x3FF) + 0xDC00;
					}
					else {
						if (lim - at <= 1)
							return null;
						b = mem[at++];
						if (0x80 != (b & 0xC0))
							return null;
						a = (a << 6) ^ b;
						b = mem[at++];
						if (0x80 != (b & 0xC0))
							return null;
						a = (a << 6) ^ b ^ 0xFFFE1F80; /* 0xFFFE0000 ^ 0x1F80 */
					}
				}
				else if (a > 0xFFFFFFBF) {
					if (at == lim)
						return null;
					b = mem[at++];
					if (0x80 != (b & 0xC0))
						return null;
					a = (a << 6) ^ b ^ 0xF80; /* 0xFFFFF000 ^ 0xFFFFFF80 */
				}
				else
					return null; /* not expecting 10xxxxxx */
				buf[i++] = (char)a;
				continue;
			}
			if (0 == a) {
				pos.at = at;
				return new String(buf, 0, i);
			}
			buf[i++] = (char)a;
		}
		return null; /* expecting 0 before lim */
	}

	public static String _bridge_unpack_utf8(byte[] src, int at, int lim/*>=at*/) throws BridgeBadString {
		String s = _utf8_unpack(src, at, lim);
		if (null == s)
			throw new BridgeBadString();
		return s;
	}

	public static int _bridge_str_packed_size__(String str/*!=null*/) throws BridgeTooLongString {
		int len = _utf8_len(str);
		if (0 == len)
			return 0;
		/* note: need one byte more for terminating '\0' */
		if (len < 0 || len >= BRIDGE_MAX - BRIDGE_COUNTER_MAX_PACKED_SIZE)
			throw new BridgeTooLongString();
		return len + bridge_counter_packed_size(len/*[0..BRIDGE_MAX)*/); /* [0..BRIDGE_MAX) */
	}

	public static int _bridge_pstr_packed_size__(String str/*!=null*/) throws BridgeTooLongString {
		int len = _utf8_len(str);
		/* note: need one byte more for terminating '\0' */
		if (len < 0 || len >= BRIDGE_MAX)
			throw new BridgeTooLongString();
		return len; /* [0..BRIDGE_MAX) */
	}

	public static int _bridge_str_packed_size_(String str/*null?*/) throws BridgeTooLongString {
		return null != str ? _bridge_str_packed_size__(str) : 0; /* [0..BRIDGE_MAX) */
	}

	public static int _bridge_pstr_packed_size_(String str/*null?*/) throws BridgeTooLongString {
		return null != str ? _bridge_pstr_packed_size__(str) : 0; /* [0..BRIDGE_MAX) */
	}

	public static int _bridge_add_str_array_packed_size(int c, String[] arr/*!=null*/, int len)
		throws BridgeTooLongString, BridgeBytesOverflow
	{
		int i = 0;
		while (i < len)
			c = _br_of_add_bytes(c, _bridge_str_packed_size_(arr[i++]) + 1/*'\0'*/);
		return c;
	}

	public static int _bridge_add_pstr_array_packed_size(int c, String[] arr/*!=null*/, int len)
		throws BridgeTooLongString, BridgeBytesOverflow
	{
		int i = 0;
		while (i < len)
			c = _br_of_add_bytes(c, _bridge_pstr_packed_size_(arr[i++]) + 1/*'\0'*/);
		return c;
	}

	/* add number of bytes needed to pack an array with counter, 1 is already counted in c */
	public static int bridge_add_str_array_packed_size_(int c, String[] arr/*null?*/)
	 	throws BridgeTooLongString, BridgeBytesOverflow
	{
		if (null != arr) {
			c = _bridge_add_str_array_packed_size(c, arr, arr.length);
			return _br_of_add_bytes(c, bridge_counter_packed_size_minus_one(arr.length));
		}
		return c;
	}

	/* add number of bytes needed to pack an array with counter, 1 is already counted in c */
	public static int bridge_add_pstr_array_packed_size_(int c, String[] arr/*null?*/)
		throws BridgeTooLongString, BridgeBytesOverflow
	{
		if (null != arr) {
			c = _bridge_add_pstr_array_packed_size(c, arr, arr.length);
			return _br_of_add_bytes(c, bridge_counter_packed_size_minus_one(arr.length));
		}
		return c;
	}

	/* pack string length + '\0'-terminated string */
	public static int _bridge_pack_str(byte[] dst, int at, String str/*!=null*/) {
		int len = _utf8_len(str);
		at = _bridge_pack_counter(dst, at, len/*[0..BRIDGE_MAX)*/);
		return _utf8_pack(dst, at, str) + 1/*terminating 0*/;
	}

	/* pack '\0'-terminated string */
	public static int _bridge_pack_pstr(byte[] dst, int at, String str/*!=null*/) {
		return _utf8_pack(dst, at, str) + 1/*terminating 0*/;
	}

	/* pack string length + '\0'-terminated string */
	public static int bridge_pack_str(byte[] dst, int at, String str/*null?*/) {
		if (null != str) {
			int len = _utf8_len(str);
			at = _bridge_pack_counter(dst, at, len/*[0..BRIDGE_MAX)*/);
			at = _utf8_pack(dst, at, str);
		}
		return at + 1/*terminating 0*/;
	}

	/* pack '\0'-terminated string */
	public static int bridge_pack_pstr(byte[] dst, int at, String str/*null?*/) {
		if (null != str)
			at = _utf8_pack(dst, at, str);
		return at + 1/*terminating 0*/;
	}

	/* pack array of strings without elements counter */
	public static int _bridge_pack_str_array(byte[] dst, int at, String[] arr/*!=null*/, int len) {
		int i = 0;
		while (i < len)
			at = bridge_pack_str(dst, at, arr[i++]);
		return at;
	}

	/* pack array of strings without elements counter */
	public static int _bridge_pack_pstr_array(byte[] dst, int at, String[] arr/*!=null*/, int len) {
		int i = 0;
		while (i < len)
			at = bridge_pack_pstr(dst, at, arr[i++]);
		return at;
	}

	/* pack number of array elements */
	/* pack array of strings */
	public static int bridge_pack_str_array(byte[] dst, int at, String[] arr/*null?*/) {
		if (null != arr) {
			at = _bridge_pack_counter(dst, at, arr.length/*[0..BRIDGE_MAX)*/);
			return _bridge_pack_str_array(dst, at, arr, arr.length);
		}
		return at + 1/*0*/;
	}

	/* pack number of array elements */
	/* pack array of strings */
	public static int bridge_pack_pstr_array(byte[] dst, int at, String[] arr/*null?*/) {
		if (null != arr) {
			at = _bridge_pack_counter(dst, at, arr.length/*[0..BRIDGE_MAX)*/);
			return _bridge_pack_pstr_array(dst, at, arr, arr.length);
		}
		return at + 1/*0*/;
	}

	public static String _bridge_unpack_str(_upk_mem_info pos)
		throws BridgeBytesOverLimit, BridgeBadString
	{
		int _len = _bridge_unpack_counter(pos);
		if (_len != 0) {
			int at = pos.at;
			/* expecting '\0'-terminated string, but don't check for '\0' at end */
			if (_len < 0 || _len >= pos.lim - at)
				throw new BridgeBytesOverLimit();
			pos.at = at + _len + 1/*'\0'*/;
			return _bridge_unpack_utf8(pos.mem, at, at + _len/*>0*/);
		}
		return "";
	}

	public static String _bridge_unpack_pstr(_upk_mem_info pos)
		throws BridgeBytesOverLimit, BridgeBadString
	{
		int at = pos.at;
		int lim = pos.lim;
		if (lim - at <= 16384) {
			String s = _utf8_unpack_z(pos);
			if (null == s)
				throw new BridgeBadString();
			return s;
		}
		{
			byte[] mem = pos.mem;
			/* determine '\0'-terminated utf8 string length */
			while (at < lim) {
				if (0 == mem[at])
					break;
				at++;
			}
			if (at == lim)
				throw new BridgeBadString(); /* string is not '\0'-terminated */
			{
				int _at = pos.at;
				pos.at = at + 1/*'\0'*/;
				return _bridge_unpack_utf8(mem, _at, at/*==_at?*/);
			}
		}
	}

	public static String[] _bridge_unpack_str_array(_upk_mem_info pos, int count/*[0..BRIDGE_MAX]*/)
		throws BridgeBytesOverLimit, BridgeBadString
	{
		String[] arr = new String[count];
		int i = 0;
		while (i < count)
			arr[i++] = _bridge_unpack_str(pos);
		return arr;
	}

	public static String[] _bridge_unpack_pstr_array(_upk_mem_info pos, int count/*[0..BRIDGE_MAX]*/)
		throws BridgeBytesOverLimit, BridgeBadString
	{
		String[] arr = new String[count];
		int i = 0;
		while (i < count)
			arr[i++] = _bridge_unpack_pstr(pos);
		return arr;
	}

	public static void _bridge_check_simple_var_type_counter(int count/*any*/) throws BridgeBadCounter {
		if (count < 0 || count > BRIDGE_MAX - BRIDGE_COUNTER_MAX_PACKED_SIZE)
			throw new BridgeBadCounter();
	}

	public static String[] bridge_unpack_str_array(_upk_mem_info pos, int count/*any*/)
		throws BridgeBadCounter, BridgeBytesOverLimit, BridgeBadString
	{
		_bridge_check_simple_var_type_counter(count);
		return _bridge_unpack_str_array(pos, count/*[0..BRIDGE_MAX-BRIDGE_COUNTER_MAX_PACKED_SIZE*/);
	}

	public static String[] bridge_unpack_pstr_array(_upk_mem_info pos, int count/*any*/)
		throws BridgeBadCounter, BridgeBytesOverLimit, BridgeBadString
	{
		_bridge_check_simple_var_type_counter(count);
		return _bridge_unpack_pstr_array(pos, count/*[0..BRIDGE_MAX-BRIDGE_COUNTER_MAX_PACKED_SIZE*/);
	}

	/* packed array size without array elements counter */

	public static int _bridge_pshort_array_packed_size(short[] arr/*!=null*/, int len) {
		int c = 0;
		int i = 0;
		if (len >= 8) {
			int n = ((len <= 0x55555555) ? len : 0x55555555) - 8;
			do {
				c += bridge_pshort_packed_size(arr[i++]);
				c += bridge_pshort_packed_size(arr[i++]);
				c += bridge_pshort_packed_size(arr[i++]);
				c += bridge_pshort_packed_size(arr[i++]);
				c += bridge_pshort_packed_size(arr[i++]);
				c += bridge_pshort_packed_size(arr[i++]);
				c += bridge_pshort_packed_size(arr[i++]);
				c += bridge_pshort_packed_size(arr[i++]);
			} while (i <= n);
		}
		while (i < len && c >= 0)
			c += bridge_pshort_packed_size(arr[i++]);
		return c; /* <0 on overflow */
	}

	public static int _bridge_pint_array_packed_size(int[] arr/*!=null*/, int len) {
		int c = 0;
		int i = 0;
		if (len >= 4) {
			int n = ((len <= 0x33333333) ? len : 0x33333333) - 4;
			do {
				c += bridge_pint_packed_size(arr[i++]);
				c += bridge_pint_packed_size(arr[i++]);
				c += bridge_pint_packed_size(arr[i++]);
				c += bridge_pint_packed_size(arr[i++]);
			} while (i <= n);
		}
		while (i < len && c >= 0)
			c += bridge_pint_packed_size(arr[i++]);
		return c; /* <0 on overflow */
	}

	public static int _bridge_plong_array_packed_size(long[] arr/*!=null*/, int len) {
		int c = 0;
		int i = 0;
		if (len >= 4) {
			int n = ((len <= 0x1C71C71C) ? len : 0x1C71C71C) - 4;
			do {
				c += bridge_plong_packed_size(arr[i++]);
				c += bridge_plong_packed_size(arr[i++]);
				c += bridge_plong_packed_size(arr[i++]);
				c += bridge_plong_packed_size(arr[i++]);
			} while (i <= n);
		}
		while (i < len && c >= 0)
			c += bridge_plong_packed_size(arr[i++]);
		return c; /* <0 on overflow */
	}

	public static int _bridge_ppshort_array_packed_size(short[] arr/*!=null*/, int len) {
		int c = 0;
		int i = 0;
		if (len >= 4) {
			int n = ((len <= 0x55555555) ? len : 0x55555555) - 4;
			do {
				c += bridge_ppshort_packed_size(arr[i++]);
				c += bridge_ppshort_packed_size(arr[i++]);
				c += bridge_ppshort_packed_size(arr[i++]);
				c += bridge_ppshort_packed_size(arr[i++]);
			} while (i <= n);
		}
		while (i < len && c >= 0)
			c += bridge_ppshort_packed_size(arr[i++]);
		return c; /* <0 on overflow */
	}

	public static int _bridge_ppint_array_packed_size(int[] arr/*!=null*/, int len) {
		int c = 0;
		int i = 0;
		if (len >= 4) {
			int n = ((len <= 0x33333333) ? len : 0x33333333) - 4;
			do {
				c += bridge_ppint_packed_size(arr[i++]);
				c += bridge_ppint_packed_size(arr[i++]);
				c += bridge_ppint_packed_size(arr[i++]);
				c += bridge_ppint_packed_size(arr[i++]);
			} while (i <= n);
		}
		while (i < len && c >= 0)
			c += bridge_ppint_packed_size(arr[i++]);
		return c; /* <0 on overflow */
	}

	public static int _bridge_pplong_array_packed_size(long[] arr/*!=null*/, int len) {
		int c = 0;
		int i = 0;
		if (len >= 4) {
			int n = ((len <= 0x1C71C71C) ? len : 0x1C71C71C) - 4;
			do {
				c += bridge_pplong_packed_size(arr[i++]);
				c += bridge_pplong_packed_size(arr[i++]);
				c += bridge_pplong_packed_size(arr[i++]);
				c += bridge_pplong_packed_size(arr[i++]);
			} while (i <= n);
		}
		while (i < len && c >= 0)
			c += bridge_pplong_packed_size(arr[i++]);
		return c; /* <0 on overflow */
	}

	/* add number of bytes needed to pack an array */

	public static int _bridge_add_pshort_array_packed_size(int c, short[] arr/*!=null*/, int len)
		throws BridgeTooManyElements, BridgeBytesOverflow
	{
		int sz = _bridge_pshort_array_packed_size(arr, len);
		if (sz < 0)
			throw new BridgeTooManyElements();
		return _br_of_add_bytes(c, sz);
	}

	public static int _bridge_add_pint_array_packed_size(int c, int[] arr/*!=null*/, int len)
		throws BridgeTooManyElements, BridgeBytesOverflow
	{
		int sz = _bridge_pint_array_packed_size(arr, len);
		if (sz < 0)
			throw new BridgeTooManyElements();
		return _br_of_add_bytes(c, sz);
	}

	public static int _bridge_add_plong_array_packed_size(int c, long[] arr/*!=null*/, int len)
		throws BridgeTooManyElements, BridgeBytesOverflow
	{
		int sz = _bridge_plong_array_packed_size(arr, len);
		if (sz < 0)
			throw new BridgeTooManyElements();
		return _br_of_add_bytes(c, sz);
	}

	public static int _bridge_add_ppshort_array_packed_size(int c, short[] arr/*!=null*/, int len)
		throws BridgeTooManyElements, BridgeBytesOverflow
	{
		int sz = _bridge_ppshort_array_packed_size(arr, len);
		if (sz < 0)
			throw new BridgeTooManyElements();
		return _br_of_add_bytes(c, sz);
	}

	public static int _bridge_add_ppint_array_packed_size(int c, int[] arr/*!=null*/, int len)
		throws BridgeTooManyElements, BridgeBytesOverflow
	{
		int sz = _bridge_ppint_array_packed_size(arr, len);
		if (sz < 0)
			throw new BridgeTooManyElements();
		return _br_of_add_bytes(c, sz);
	}

	public static int _bridge_add_pplong_array_packed_size(int c, long[] arr/*!=null*/, int len)
		throws BridgeTooManyElements, BridgeBytesOverflow
	{
		int sz = _bridge_pplong_array_packed_size(arr, len);
		if (sz < 0)
			throw new BridgeTooManyElements();
		return _br_of_add_bytes(c, sz);
	}

	/* add number of bytes needed to pack an array with counter, 1 is already counted in c */

	public static int bridge_add_pshort_array_packed_size_(int c, short[] arr/*null?*/)
		throws BridgeTooManyElements, BridgeBytesOverflow
	{
		if (null != arr) {
			c = _bridge_add_pshort_array_packed_size(c, arr, arr.length);
			return _br_of_add_bytes(c, bridge_counter_packed_size_minus_one(arr.length));
		}
		return c;
	}

	public static int bridge_add_pint_array_packed_size_(int c, int[] arr/*null?*/)
		throws BridgeTooManyElements, BridgeBytesOverflow
	{
		if (null != arr) {
			c = _bridge_add_pint_array_packed_size(c, arr, arr.length);
			return _br_of_add_bytes(c, bridge_counter_packed_size_minus_one(arr.length));
		}
		return c;
	}

	public static int bridge_add_plong_array_packed_size_(int c, long[] arr/*null?*/)
		throws BridgeTooManyElements, BridgeBytesOverflow
	{
		if (null != arr) {
			c = _bridge_add_plong_array_packed_size(c, arr, arr.length);
			return _br_of_add_bytes(c, bridge_counter_packed_size_minus_one(arr.length));
		}
		return c;
	}

	public static int bridge_add_ppshort_array_packed_size_(int c, short[] arr/*null?*/)
		throws BridgeTooManyElements, BridgeBytesOverflow
	{
		if (null != arr) {
			c = _bridge_add_ppshort_array_packed_size(c, arr, arr.length);
			return _br_of_add_bytes(c, bridge_counter_packed_size_minus_one(arr.length));
		}
		return c;
	}

	public static int bridge_add_ppint_array_packed_size_(int c, int[] arr/*null?*/)
		throws BridgeTooManyElements, BridgeBytesOverflow
	{
		if (null != arr) {
			c = _bridge_add_ppint_array_packed_size(c, arr, arr.length);
			return _br_of_add_bytes(c, bridge_counter_packed_size_minus_one(arr.length));
		}
		return c;
	}

	public static int bridge_add_pplong_array_packed_size_(int c, long[] arr/*null?*/)
		throws BridgeTooManyElements, BridgeBytesOverflow
	{
		if (null != arr) {
			c = _bridge_add_pplong_array_packed_size(c, arr, arr.length);
			return _br_of_add_bytes(c, bridge_counter_packed_size_minus_one(arr.length));
		}
		return c;
	}

	/* pack array without elements counter */

	public static int _bridge_pack_pshort_array(byte[] dst, int at, short[] arr/*!=null*/, int len) {
		int i = 0;
		while (i < len)
			at = _bridge_pack_pshort(dst, at, arr[i++]);
		return at;
	}

	public static int _bridge_pack_pint_array(byte[] dst, int at, int[] arr/*!=null*/, int len) {
		int i = 0;
		while (i < len)
			at = _bridge_pack_pint(dst, at, arr[i++]);
		return at;
	}

	public static int _bridge_pack_plong_array(byte[] dst, int at, long[] arr/*!=null*/, int len) {
		int i = 0;
		while (i < len)
			at = _bridge_pack_plong(dst, at, arr[i++]);
		return at;
	}

	public static int _bridge_pack_ppshort_array(byte[] dst, int at, short[] arr/*!=null*/, int len) {
		int i = 0;
		while (i < len)
			at = _bridge_pack_ppshort(dst, at, arr[i++]);
		return at;
	}

	public static int _bridge_pack_ppint_array(byte[] dst, int at, int[] arr/*!=null*/, int len) {
		int i = 0;
		while (i < len)
			at = _bridge_pack_ppint(dst, at, arr[i++]);
		return at;
	}

	public static int _bridge_pack_pplong_array(byte[] dst, int at, long[] arr/*!=null*/, int len) {
		int i = 0;
		while (i < len)
			at = _bridge_pack_pplong(dst, at, arr[i++]);
		return at;
	}

	/* pack array with elements counter */

	public static int bridge_pack_pshort_array(byte[] dst, int at, short[] arr/*null?*/) {
		if (null != arr) {
			at = _bridge_pack_counter(dst, at, arr.length/*[0..BRIDGE_MAX)*/);
			return _bridge_pack_pshort_array(dst, at, arr, arr.length);
		}
		return at + 1/*0*/;
	}

	public static int bridge_pack_pint_array(byte[] dst, int at, int[] arr/*null?*/) {
		if (null != arr) {
			at = _bridge_pack_counter(dst, at, arr.length/*[0..BRIDGE_MAX)*/);
			return _bridge_pack_pint_array(dst, at, arr, arr.length);
		}
		return at + 1/*0*/;
	}

	public static int bridge_pack_plong_array(byte[] dst, int at, long[] arr/*null?*/) {
		if (null != arr) {
			at = _bridge_pack_counter(dst, at, arr.length/*[0..BRIDGE_MAX)*/);
			return _bridge_pack_plong_array(dst, at, arr, arr.length);
		}
		return at + 1/*0*/;
	}

	public static int bridge_pack_ppshort_array(byte[] dst, int at, short[] arr/*null?*/) {
		if (null != arr) {
			at = _bridge_pack_counter(dst, at, arr.length/*[0..BRIDGE_MAX)*/);
			return _bridge_pack_ppshort_array(dst, at, arr, arr.length);
		}
		return at + 1/*0*/;
	}

	public static int bridge_pack_ppint_array(byte[] dst, int at, int[] arr/*null?*/) {
		if (null != arr) {
			at = _bridge_pack_counter(dst, at, arr.length/*[0..BRIDGE_MAX)*/);
			return _bridge_pack_ppint_array(dst, at, arr, arr.length);
		}
		return at + 1/*0*/;
	}

	public static int bridge_pack_pplong_array(byte[] dst, int at, long[] arr/*null?*/) {
		if (null != arr) {
			at = _bridge_pack_counter(dst, at, arr.length/*[0..BRIDGE_MAX)*/);
			return _bridge_pack_pplong_array(dst, at, arr, arr.length);
		}
		return at + 1/*0*/;
	}

	/* unpack array items not checking limits */

	public static void _bridge_unpack_pshort_array_no_lim(
		_upk_mem_no_lim_info pos, int count/*0?*/, short[] arr/*!=null*/)
	{
		int i = 0;
		while (i < count)
			arr[i++] = bridge_unpack_pshort_no_lim(pos);
	}

	public static void _bridge_unpack_pint_array_no_lim(
		_upk_mem_no_lim_info pos, int count/*0?*/, int[] arr/*!=null*/)
	{
		int i = 0;
		while (i < count)
			arr[i++] = bridge_unpack_pint_no_lim(pos);
	}

	public static void _bridge_unpack_plong_array_no_lim(
		_upk_mem_no_lim_info pos, int count/*0?*/, long[] arr/*!=null*/)
	{
		int i = 0;
		while (i < count)
			arr[i++] = bridge_unpack_plong_no_lim(pos);
	}

	public static void _bridge_unpack_ppshort_array_no_lim(
		_upk_mem_no_lim_info pos, int count/*0?*/, short[] arr/*!=null*/)
	{
		int i = 0;
		while (i < count)
			arr[i++] = bridge_unpack_ppshort_no_lim(pos);
	}

	public static void _bridge_unpack_ppint_array_no_lim(
		_upk_mem_no_lim_info pos, int count/*0?*/, int[] arr/*!=null*/)
	{
		int i = 0;
		while (i < count)
			arr[i++] = bridge_unpack_ppint_no_lim(pos);
	}

	public static void _bridge_unpack_pplong_array_no_lim(
		_upk_mem_no_lim_info pos, int count/*0?*/, long[] arr/*!=null*/)
	{
		int i = 0;
		while (i < count)
			arr[i++] = bridge_unpack_pplong_no_lim(pos);
	}

	/* allocate array of fixed size, then unpack items */

	public static short[] bridge_unpack_pshort_array_no_lim(_upk_mem_no_lim_info pos, int count/*[1..BRIDGE_MAX/3]*/) {
		short[] arr = new short[count];
		_bridge_unpack_pshort_array_no_lim(pos, count, arr);
		return arr;
	}

	public static short[] bridge_unpack_ppshort_array_no_lim(_upk_mem_no_lim_info pos, int count/*[1..BRIDGE_MAX/3]*/) {
		short[] arr = new short[count];
		_bridge_unpack_ppshort_array_no_lim(pos, count, arr);
		return arr;
	}

	public static int[] bridge_unpack_pint_array_no_lim(_upk_mem_no_lim_info pos, int count/*[1..BRIDGE_MAX/5]*/) {
		int[] arr = new int[count];
		_bridge_unpack_pint_array_no_lim(pos, count, arr);
		return arr;
	}

	public static int[] bridge_unpack_ppint_array_no_lim(_upk_mem_no_lim_info pos, int count/*[1..BRIDGE_MAX/5]*/) {
		int[] arr = new int[count];
		_bridge_unpack_ppint_array_no_lim(pos, count, arr);
		return arr;
	}

	public static long[] bridge_unpack_plong_array_no_lim(_upk_mem_no_lim_info pos, int count/*[1..BRIDGE_MAX/9]*/) {
		long[] arr = new long[count];
		_bridge_unpack_plong_array_no_lim(pos, count, arr);
		return arr;
	}

	public static long[] bridge_unpack_pplong_array_no_lim(_upk_mem_no_lim_info pos, int count/*[1..BRIDGE_MAX/9]*/) {
		long[] arr = new long[count];
		_bridge_unpack_pplong_array_no_lim(pos, count, arr);
		return arr;
	}

	/* allocate array, then unpack array items */

	public static short[] _bridge_unpack_pshort_array(_upk_mem_info pos,
		int count/*[0..BRIDGE_MAX]*/) throws BridgeBytesOverLimit
	{
		short[] arr = new short[count];
		int i = (pos.lim - pos.at)/3;
		if (i > 0) {
			if (i > count)
				i = count;
			_bridge_unpack_pshort_array_no_lim(pos, i, arr);
		}
		while (i < count)
			arr[i++] = bridge_unpack_pshort(pos);
		return arr;
	}

	public static int[] _bridge_unpack_pint_array(_upk_mem_info pos,
		int count/*[0..BRIDGE_MAX]*/) throws BridgeBytesOverLimit
	{
		int[] arr = new int[count];
		int i = (pos.lim - pos.at)/5;
		if (i > 0) {
			if (i > count)
				i = count;
			_bridge_unpack_pint_array_no_lim(pos, i, arr);
		}
		while (i < count)
			arr[i++] = bridge_unpack_pint(pos);
		return arr;
	}

	public static long[] _bridge_unpack_plong_array(_upk_mem_info pos,
		int count/*[0..BRIDGE_MAX]*/) throws BridgeBytesOverLimit
	{
		long[] arr = new long[count];
		int i = (pos.lim - pos.at)/9;
		if (i > 0) {
			if (i > count)
				i = count;
			_bridge_unpack_plong_array_no_lim(pos, i, arr);
		}
		while (i < count)
			arr[i++] = bridge_unpack_plong(pos);
		return arr;
	}

	public static short[] _bridge_unpack_ppshort_array(_upk_mem_info pos,
		int count/*[0..BRIDGE_MAX]*/) throws BridgeBytesOverLimit
	{
		short[] arr = new short[count];
		int i = (pos.lim - pos.at)/3;
		if (i > 0) {
			if (i > count)
				i = count;
			_bridge_unpack_ppshort_array_no_lim(pos, i, arr);
		}
		while (i < count)
			arr[i++] = bridge_unpack_ppshort(pos);
		return arr;
	}

	public static int[] _bridge_unpack_ppint_array(_upk_mem_info pos,
		int count/*[0..BRIDGE_MAX]*/) throws BridgeBytesOverLimit
	{
		int[] arr = new int[count];
		int i = (pos.lim - pos.at)/5;
		if (i > 0) {
			if (i > count)
				i = count;
			_bridge_unpack_ppint_array_no_lim(pos, i, arr);
		}
		while (i < count)
			arr[i++] = bridge_unpack_ppint(pos);
		return arr;
	}

	public static long[] _bridge_unpack_pplong_array(_upk_mem_info pos,
		int count/*[0..BRIDGE_MAX]*/) throws BridgeBytesOverLimit
	{
		long[] arr = new long[count];
		int i = (pos.lim - pos.at)/9;
		if (i > 0) {
			if (i > count)
				i = count;
			_bridge_unpack_pplong_array_no_lim(pos, i, arr);
		}
		while (i < count)
			arr[i++] = bridge_unpack_pplong(pos);
		return arr;
	}

	/* check array elements counter, allocate array, then unpack items */

	public static short[] bridge_unpack_pshort_array(_upk_mem_info pos,
		int count/*any*/) throws BridgeBadCounter, BridgeBytesOverLimit
	{
		_bridge_check_simple_var_type_counter(count);
		return _bridge_unpack_pshort_array(pos, count/*[0..BRIDGE_MAX-BRIDGE_COUNTER_MAX_PACKED_SIZE]*/);
	}

	public static int[] bridge_unpack_pint_array(_upk_mem_info pos,
		int count/*any*/) throws BridgeBadCounter, BridgeBytesOverLimit
	{
		_bridge_check_simple_var_type_counter(count);
		return _bridge_unpack_pint_array(pos, count/*[0..BRIDGE_MAX-BRIDGE_COUNTER_MAX_PACKED_SIZE]*/);
	}

	public static long[] bridge_unpack_plong_array(_upk_mem_info pos,
		int count/*any*/) throws BridgeBadCounter, BridgeBytesOverLimit
	{
		_bridge_check_simple_var_type_counter(count);
		return _bridge_unpack_plong_array(pos, count/*[0..BRIDGE_MAX-BRIDGE_COUNTER_MAX_PACKED_SIZE]*/);
	}

	public static short[] bridge_unpack_ppshort_array(_upk_mem_info pos,
		int count/*any*/) throws BridgeBadCounter, BridgeBytesOverLimit
	{
		_bridge_check_simple_var_type_counter(count);
		return _bridge_unpack_ppshort_array(pos, count/*[0..BRIDGE_MAX-BRIDGE_COUNTER_MAX_PACKED_SIZE]*/);
	}

	public static int[] bridge_unpack_ppint_array(_upk_mem_info pos,
		int count/*any*/) throws BridgeBadCounter, BridgeBytesOverLimit
	{
		_bridge_check_simple_var_type_counter(count);
		return _bridge_unpack_ppint_array(pos, count/*[0..BRIDGE_MAX-BRIDGE_COUNTER_MAX_PACKED_SIZE]*/);
	}

	public static long[] bridge_unpack_pplong_array(_upk_mem_info pos,
		int count/*any*/) throws BridgeBadCounter, BridgeBytesOverLimit
	{
		_bridge_check_simple_var_type_counter(count);
		return _bridge_unpack_pplong_array(pos, count/*[0..BRIDGE_MAX-BRIDGE_COUNTER_MAX_PACKED_SIZE]*/);
	}

	/* check counter, allocate, then unpack byte, short, int, float, long and double arrays */

	public static byte[] bridge_unpack_fixed_byte_array(_upk_mem_info pos,
		int count/*>=0*/) throws BridgeBytesOverLimit
	{
		upk_check_mem_lim(pos.at, pos.lim, count);
		return bridge_unpack_byte_array_no_lim(pos, count/*[0..BRIDGE_MAX]*/);
	}

	public static byte[] bridge_unpack_byte_array(_upk_mem_info pos,
		int count/*any*/) throws BridgeBadCounter, BridgeBytesOverLimit
	{
		if (count < 0)
			throw new BridgeBadCounter();
		return bridge_unpack_fixed_byte_array(pos, count/*>=0*/);
	}

	public static short[] bridge_unpack_fixed_short_array(_upk_mem_info pos,
		int count/*[0..BRIDGE_MAX/2]*/, boolean big) throws BridgeBytesOverLimit
	{
		upk_check_mem_lim(pos.at, pos.lim, count*2);
		return bridge_unpack_short_array_no_lim(pos, count/*[0..BRIDGE_MAX/2]*/, big);
	}

	public static short[] bridge_unpack_short_array(_upk_mem_info pos,
		int count/*any*/, boolean big) throws BridgeBadCounter, BridgeBytesOverLimit
	{
		_bridge_check_array_counter(count, 2);
		return bridge_unpack_fixed_short_array(pos, count/*[0..BRIDGE_MAX/2]*/, big);
	}

	public static int[] bridge_unpack_fixed_int_array(_upk_mem_info pos,
		int count/*[0..BRIDGE_MAX/4]*/, boolean big) throws BridgeBytesOverLimit
	{
		upk_check_mem_lim(pos.at, pos.lim, count*4);
		return bridge_unpack_int_array_no_lim(pos, count/*[0..BRIDGE_MAX/4]*/, big);
	}

	public static int[] bridge_unpack_int_array(_upk_mem_info pos,
		int count/*any*/, boolean big) throws BridgeBadCounter, BridgeBytesOverLimit
	{
		_bridge_check_array_counter(count, 4);
		return bridge_unpack_fixed_int_array(pos, count/*[0..BRIDGE_MAX/4]*/, big);
	}

	public static long[] bridge_unpack_fixed_long_array(_upk_mem_info pos,
		int count/*[0..BRIDGE_MAX/8]*/, boolean big) throws BridgeBytesOverLimit
	{
		upk_check_mem_lim(pos.at, pos.lim, count*8);
		return bridge_unpack_long_array_no_lim(pos, count/*[0..BRIDGE_MAX/8]*/, big);
	}

	public static long[] bridge_unpack_long_array(_upk_mem_info pos,
		int count/*any*/, boolean big) throws BridgeBadCounter, BridgeBytesOverLimit
	{
		_bridge_check_array_counter(count, 8);
		return bridge_unpack_fixed_long_array(pos, count/*[0..BRIDGE_MAX/8]*/, big);
	}

	public static float[] bridge_unpack_fixed_float_array(_upk_mem_info pos,
		int count/*[0..BRIDGE_MAX/4]*/, boolean big) throws BridgeBytesOverLimit
	{
		upk_check_mem_lim(pos.at, pos.lim, count*4);
		return bridge_unpack_float_array_no_lim(pos, count/*[0..BRIDGE_MAX/4]*/, big);
	}

	public static float[] bridge_unpack_float_array(_upk_mem_info pos,
		int count/*any*/, boolean big) throws BridgeBadCounter, BridgeBytesOverLimit
	{
		_bridge_check_array_counter(count, 4);
		return bridge_unpack_fixed_float_array(pos, count/*[0..BRIDGE_MAX/4]*/, big);
	}

	public static double[] bridge_unpack_fixed_double_array(_upk_mem_info pos,
		int count/*[0..BRIDGE_MAX/8]*/, boolean big) throws BridgeBytesOverLimit
	{
		upk_check_mem_lim(pos.at, pos.lim, count*8);
		return bridge_unpack_double_array_no_lim(pos, count/*[0..BRIDGE_MAX/8]*/, big);
	}

	public static double[] bridge_unpack_double_array(_upk_mem_info pos,
		int count/*any*/, boolean big) throws BridgeBadCounter, BridgeBytesOverLimit
	{
		_bridge_check_array_counter(count, 8);
		return bridge_unpack_fixed_double_array(pos, count/*[0..BRIDGE_MAX/8]*/, big);
	}
}
