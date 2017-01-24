/********************************************************************************
* bitbridge - runtime library of another serialization/rpc mechanism (bitbridge)
* Copyright (C) 2008-2017 Michael M. Builov, https://github.com/mbuilov/bitbridge
* Licensed under LGPL version 3 or any later version, see COPYING.LGPL3
********************************************************************************/

/*  Bridge.java */

package bitbridge;

public final class Bridge {

	/** maximum number of elements in any bridge array,
	  also this is maximum packed size of bridge structure */
	public static final int BRIDGE_MAX = 0x7FFFFFFF - 8;

	/** maximum number of bits in packed bridge structure */
	public static final long BRIDGE_BIT_COUNT_MAX = 0xFFFFFFFFL;

	/* helpers to work with bit arrays */

	/** get bit array size in bytes
	 * @param bit_count number of bits in array
	 * @return array size in bytes, &gt;= 0 */
	public static int bridge_bit_array_size(int bit_count/*may be negative*/) {
		long bc = (long)bit_count & 0xFFFFFFFFL;
		return (int)((bc + 7) >>> 3);
	}

	/** check bit in bits array
	 * @param bits bits array
	 * @param bit_number bit to check in bits array
	 * @return true if bit is set, false - otherwise */
	public static boolean bridge_get_bit(byte[] bits, int bit_number) {
		return 0 != (bits[bit_number >>> 3] & (byte)(1 << (bit_number & 7)));
	}

	/** set bit in bits array
	 * @param bits bits array
	 * @param bit_number bit to set in bits array */
	public static void bridge_set_bit(byte[] bits, int bit_number) {
		bits[bit_number >>> 3] |= (byte)(1 << (bit_number & 7));
	}

	/** clear bit in bits array
	 * @param bits bits array
	 * @param bit_number bit to clear in bits array */
	public static void bridge_clear_bit(byte[] bits, int bit_number) {
		bits[bit_number >>> 3] &= (byte)(0xFF & ~(1 << (bit_number & 7)));
	}

	/** change bit in bits array
	 * @param bits bits array
	 * @param bit_number bit to change in bits array
	 * @param set true to set bit, false - to clear */
	public static void bridge_set_bit(byte[] bits, int bit_number, boolean set) {
		int byte_number = bit_number >>> 3;
		int bit_mask = 1 << (bit_number & 7);
		if (set)
			bits[byte_number] = (byte)(0xFF & (bits[byte_number] | bit_mask));
		else
			bits[byte_number] = (byte)(0xFF & (bits[byte_number] & ~bit_mask));
	}

	/** base of bridge exceptions */
	public static class BridgeException extends Exception {
		private static final long serialVersionUID = -1143215676573465134L;
		private BridgeException(String message) {
			super(message);
		}
		private BridgeException(String message, Throwable cause) {
			super(message, cause);
		}
	}

	/** cannot pack: too many bytes to pack */
	public static class BridgeBytesOverflow extends BridgeException {
		private static final long serialVersionUID = -1198309234673465134L;
		public BridgeBytesOverflow() {
			super("bytes count overflow");
		}
	}

	/** cannot pack: too many bits to pack */
	public static class BridgeBitsOverflow extends BridgeException {
		private static final long serialVersionUID = -1143376377673465134L;
		public BridgeBitsOverflow() {
			super("bits count overflow");
		}
	}

	/** cannot unpack: not enough source bytes */
	public static class BridgeBytesOverLimit extends BridgeException {
		private static final long serialVersionUID = -1143215612213265134L;
		public BridgeBytesOverLimit() {
			super("bytes over limit");
		}
	}

	/** cannot unpack: not enough source bits */
	public static class BridgeBitsOverLimit extends BridgeException {
		private static final long serialVersionUID = -1132195346573465134L;
		public BridgeBitsOverLimit() {
			super("bits over limit");
		}
	}

	/** cannot pack: too many elements in array */
	public static class BridgeTooManyElements extends BridgeException {
		private static final long serialVersionUID = -1143224572342345134L;
		public BridgeTooManyElements() {
			super("too many array elements");
		}
	}

	/** cannot pack: too big packed size */
	public static class BridgeTooBig extends BridgeException {
		private static final long serialVersionUID = -1644714567657365134L;
		public BridgeTooBig() {
			super("too big packed size");
		}
	}

	/** cannot pack: too long string */
	public static class BridgeTooLongString extends BridgeException {
		private static final long serialVersionUID = -1203566567657346513L;
		public BridgeTooLongString() {
			super("too long string");
		}
	}

	/** failed to unpack: bad utf8 string */
	public static class BridgeBadString extends BridgeException {
		private static final long serialVersionUID = -1124254567657312514L;
		public BridgeBadString() {
			super("failed to unpack string");
		}
	}

	/** failed to unpack: bad counter */
	public static class BridgeBadCounter extends BridgeException {
		private static final long serialVersionUID = -2632854576573565134L;
		public BridgeBadCounter() {
			super("bad counter in input stream");
		}
	}

	/** failed to pack: cannot allocate buffer of too big size */
	public static class BridgeBadReserves extends BridgeException {
		private static final long serialVersionUID = -1458215696973435134L;
		public BridgeBadReserves() {
			super("bad packing buffer reserves");
		}
	}

	/** failed to pack: required field is null or null in array of objects */
	public static class BridgeNullPointer extends BridgeException {
		private static final long serialVersionUID = -1522145635573725135L;
		public BridgeNullPointer(NullPointerException cause) {
			super("required pointer is null", cause);
		}
	}

	/** failed to pack: too small array */
	public static class BridgeSmallArray extends BridgeException {
		private static final long serialVersionUID = -1143554564937345233L;
		public BridgeSmallArray(ArrayIndexOutOfBoundsException cause) {
			super("too small array", cause);
		}
	}

	/** used for unpacking */
	public static class BridgeIterator {
		/** position in source array of bytes */
		public int at;
		/** @param at_ position in source array of bytes */
		public BridgeIterator(int at_) {
			at = at_;
		}
	}
}
