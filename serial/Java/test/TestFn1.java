/********************************************************************************
* bitbridge - another serialization/rpc mechanism
* Copyright (C) 2008-2017 Michael M. Builov, https://github.com/mbuilov/bitbridge
* Licensed under GPL version 3 or any later version, see COPYING
********************************************************************************/

/* TestFn1.java */

import static bitbridge.Bridge.*;
import bridge_test_lib.gen_test.*;
import bridge_test_lib.gen_test;

public class TestFn1 {

	private int n;

	private <T> T repack(int type_idx, T si, int packed, boolean big) throws Exception {
		byte[] mem = gen_test.pack(si, type_idx, 0, 0, big);
		System.out.print(" " + ++n);
		if (packed == mem.length) {
			System.out.print(" " + ++n);
			{
				BridgeIterator it = new BridgeIterator(0);
				@SuppressWarnings("unchecked")
				T so = (T)gen_test.unpack(type_idx, it, mem, mem.length, big);
				System.out.print(" " + ++n);
				if (it.at == mem.length) {
					System.out.print(" " + ++n);
					return so;
				}
			}
		}
		throw new Exception("wrong packed: " + packed + " != " + mem.length);
	}

	private void _check(boolean b) throws Exception {
		if (!b)
			throw new Exception("bad check");
		System.out.print(" " + ++n);
	}

	private void _start() {
		System.out.print(++n + " ->");
	}

	private void _end() {
		System.out.println();
	}

	public void test_fn1(boolean big) throws Exception {

		_start();
		{
			s1 si = new s1();
			{
				si.set_b(false);
				{
					s1 so = repack(si._type_idx_, si, 1, big);
					_check(si.get_b() == so.get_b());
				}
			}
			{
				si.set_b(true);
				{
					s1 so = repack(si._type_idx_, si, 1, big);
					_check(si.get_b() == so.get_b());
				}
			}
		}
		_end();

		_start();
		{
			s2 si = new s2();
			_check(!si.has_ob());
			{
				si.set_ob(true);
				_check(si.has_ob());
				{
					s2 so = repack(si._type_idx_, si, 1, big);
					_check(so.has_ob() && so.get_ob() == si.get_ob());
				}
			}
			{
				si.set_ob(false);
				_check(si.has_ob());
				{
					s2 so = repack(si._type_idx_, si, 1, big);
					_check(so.has_ob() && so.get_ob() == si.get_ob());
				}
			}
			{
				si.unset_ob();
				_check(!si.has_ob());
				{
					s2 so = repack(si._type_idx_, si, 1, big);
					_check(!so.has_ob());
				}
			}
		}
		_end();

		_start();
		{
			s3 si = new s3();
			_check(null == si.ab && 0 == si.ab_bit_count);
			{
				s3 so = repack(si._type_idx_, si, 1, big);
				_check(null != so.ab && 0 == so.ab.length && 0 == so.ab_bit_count);
			}
			{
				si.new_ab(5);
				si.
					ab_set_bit(0).
					ab_set_bit(2).
					ab_set_bit(4);
				{
					s3 so = repack(si._type_idx_, si, 2, big);
					_check(si.ab_bit_count == so.ab_bit_count);
					_check(
						 so.ab_get_bit(0) &&
						!so.ab_get_bit(1) &&
						 so.ab_get_bit(2) &&
						!so.ab_get_bit(3) &&
						 so.ab_get_bit(4)
					);
				}
			}
			{
				int bit_counter = 0;
				si.new_ab(9);
				si.
					ab_set_bit(bit_counter++, true).
					ab_set_bit(bit_counter++, false).
					ab_set_bit(bit_counter++, false).
					ab_set_bit(bit_counter++, true).
					ab_set_bit(bit_counter++, true).
					ab_set_bit(bit_counter++, false).
					ab_set_bit(bit_counter++, true).
					ab_set_bit(bit_counter++, false).
					ab_set_bit(bit_counter++, true);
			}
			{
				s3 so = repack(si._type_idx_, si, 3, big);
				_check(si.ab_bit_count == so.ab_bit_count);
				{
					int bit_counter = 0;
					_check(
						 so.ab_get_bit(bit_counter++) &&
						!so.ab_get_bit(bit_counter++) &&
						!so.ab_get_bit(bit_counter++) &&
						 so.ab_get_bit(bit_counter++) &&
						 so.ab_get_bit(bit_counter++) &&
						!so.ab_get_bit(bit_counter++) &&
						 so.ab_get_bit(bit_counter++) &&
						!so.ab_get_bit(bit_counter++) &&
						 so.ab_get_bit(bit_counter++)
					);
				}
			}
		}
		_end();

		_start();
		{
			s4 si = new s4();
			{
				si.b = 123;
				{
					s4 so = repack(si._type_idx_, si, 1, big);
					_check(si.b == so.b);
				}
			}
		}
		_end();

		_start();
		{
			s5 si = new s5();
			_check(!si.has_ob());
			{
				si.set_ob((byte)112);
				_check(si.has_ob());
				{
					s5 so = repack(si._type_idx_, si, 2, big);
					_check(si.has_ob() == so.has_ob());
					_check(si.ob == so.ob);
				}
			}
		}
		_end();

		_start();
		{
			s6 si = new s6();
			_check(null == si.ab);
			{
				s6 so = repack(si._type_idx_, si, 1, big);
				_check(null != so.ab && 0 == so.ab.length);
			}
			{
				si.set_ab(new byte[] {
					(byte)91,
					(byte)112,
					(byte)122
				});
				{
					s6 so = repack(si._type_idx_, si, 4, big);
					_check(so.ab != null && si.ab.length == so.ab.length);
					_check(
						so.ab[0] == (byte)91 &&
						so.ab[1] == (byte)112 &&
						so.ab[2] == (byte)122
					);
				}
			}
		}
		_end();

		_start();
		{
			s7 si = new s7();
			{
				si.s = 25443;
				{
					s7 so = repack(si._type_idx_, si, 2, big);
					_check(so.s == si.s);
				}
			}
		}
		_end();

		_start();
		{
			s8 si = new s8();
			_check(!si.has_os());
			{
				si.os = 25443;
				{
					s8 so = repack(si._type_idx_, si, 1, big);
					_check(!so.has_os());
				}
			}
			{
				si.set_os(si.os);
				_check(si.os == 25443);
				{
					s8 so = repack(si._type_idx_, si, 3, big);
					_check(so.has_os());
					_check(so.os == si.os);
				}
			}
		}
		_end();

		_start();
		{
			s9 si = new s9();
			_check(null == si.as);
			{
				s9 so = repack(si._type_idx_, si, 1, big);
				_check(null != so.as && 0 == so.as.length);
			}
			{
				short[] as = si.new_as(5);
				as[0] = (short)12345;
				as[1] = (short)21212;
				as[2] = (short)32321;
				as[3] = (short)11223;
				as[4] = (short)15556;
			}
			{
				s9 so = repack(si._type_idx_, si, 11, big);
				_check(so.as != null && so.as.length == si.as.length);
				_check(
					so.as[0] == (short)12345 &&
					so.as[1] == (short)21212 &&
					so.as[2] == (short)32321 &&
					so.as[3] == (short)11223 &&
					so.as[4] == (short)15556
				);
			}
		}
		_end();

		_start();
		{
			s10 si = new s10();
			{
				si.i = 1122334455;
				{
					s10 so = repack(si._type_idx_, si, 4, big);
					_check(so.i == si.i);
				}
			}
		}
		_end();

		_start();
		{
			s11 si = new s11();
			_check(!si.has_oi());
			{
				si.oi = 0x55442544;
				{
					s11 so = repack(si._type_idx_, si, 1, big);
					_check(!so.has_oi());
				}
			}
			{
				si.set_oi(si.oi);
				{
					s11 so = repack(si._type_idx_, si, 5, big);
					_check(so.has_oi());
					_check(so.oi == si.oi);
				}
			}
		}
		_end();

		_start();
		{
			s12 si = new s12();
			_check(null == si.ai);
			{
				s12 so = repack(si._type_idx_, si, 1, big);
				_check(null != so.ai && so.ai.length == 0);
			}
			{
				int q = 126;
				for (;;) {
					si.ai = new int[q];
					{
						for (int i = 0; i < q; i++)
							si.ai[i] = (1 << (i & (32 - 1)))^i;
					}
					{
						s12 so = repack(si._type_idx_, si, si.ai.length*4 + (
							q ==   126 ? 1 :
							q ==   127 ? 1 :
							q ==   128 ? 2 :
							q ==   129 ? 2 :
							q == 16383 ? 2 :
							q == 16384 ? 3 : 0), big);
						_check(null != so.ai && so.ai.length == si.ai.length);
						{
							int i = 0;
							for (; i < q; i++) {
								if (so.ai[i] != si.ai[i])
									break;
							}
							_check(i == q);
						}
					}
					if (q == 126)
						q = 127;
					else if (q == 127)
						q = 128;
					else if (q == 128)
						q = 129;
					else if (q == 129)
						q = 16383;
					else if (q == 16383)
						q = 16384;
					else if (q == 16384)
						break;
				}
			}
		}
		_end();

		_start();
		{
			s13 si = new s13();
			{
				si.l = -1122334455453334442l;
				{
					s13 so = repack(si._type_idx_, si, 8, big);
					_check(so.l == si.l);
				}
			}
		}
		_end();

		_start();
		{
			s14 si = new s14();
			_check(!si.has_ol());
			{
				si.ol = 24234234554425443l;
				{
					s14 so = repack(si._type_idx_, si, 1, big);
					_check(!so.has_ol());
				}
			}
			{
				si.set_ol(si.ol);
				{
					s14 so = repack(si._type_idx_, si, 9, big);
					_check(so.has_ol());
					_check(so.ol == si.ol);
					_check(so.ol == 24234234554425443l);
				}
			}
		}
		_end();

		_start();
		{
			s15 si = new s15();
			_check(null == si.al);
			{
				s15 so = repack(si._type_idx_, si, 1, big);
				_check(null != so.al && 0 == so.al.length);
			}
			{
				long[] al = si.new_al(66);
				for (int i = 0; i < 66; i++)
					al[i] = (1l << (i & (64 - 1)))^i;
			}
			{
				s15 so = repack(si._type_idx_, si, 1 + si.al.length*8, big);
				_check(null != so.al && so.al.length == si.al.length);
				{
					int i = 0;
					for (; i < 66; i++) {
						if (so.al[i] != si.al[i])
							break;
					}
					_check(i == 66);
				}
			}
		}
		_end();

		_start();
		{
			s16 si = new s16();
			si.f = -1122.334f;
			{
				s16 so = repack(si._type_idx_, si, 4, big);
				_check(so.f == si.f);
			}
		}
		_end();

		_start();
		{
			s17 si = new s17();
			_check(!si.has_of());
			{
				si.of = 2423.42345f;
				{
					s17 so = repack(si._type_idx_, si, 1, big);
					_check(!so.has_of());
				}
			}
			{
				si.set_of(si.of);
				{
					s17 so = repack(si._type_idx_, si, 5, big);
					_check(so.has_of());
					_check(so.of == si.of);
				}
			}
		}
		_end();

		_start();
		{
			s18 si = new s18();
			_check(null == si.af);
			{
				s18 so = repack(si._type_idx_, si, 1, big);
				_check(null != so.af && 0 == so.af.length);
			}
			{
				si.af = new float[16383];
				{
					for (int i = 0; i < 16383; i++)
						si.af[i] = (float)((1 << (i & (32 - 1)))^i)/((float)i + 1.0f);
				}
				{
					s18 so = repack(si._type_idx_, si, 2 + si.af.length*4, big);
					_check(null != so.af && so.af.length == si.af.length);
					{
						int i = 0;
						for (; i < 16383; i++) {
							if (so.af[i] != si.af[i])
								break;
						}
						_check(i == 16383);
					}
				}
			}
		}
		_end();

		_start();
		{
			s19 si = new s19();
			{
				si.d = -1122345543.3343453455d;
				{
					s19 so = repack(si._type_idx_, si, 8, big);
					_check(so.d == si.d);
				}
			}
		}
		_end();

		_start();
		{
			s20 si = new s20();
			_check(!si.has_od());
			{
				si.od = 2423234234112.79768956042345d;
				{
					s20 so = repack(si._type_idx_, si, 1, big);
					_check(!so.has_od());
				}
			}
			{
				si.set_od(si.od);
				{
					s20 so = repack(si._type_idx_, si, 9, big);
					_check(so.has_od());
					_check(so.od == si.od);
				}
			}
		}
		_end();

		_start();
		{
			s21 si = new s21();
			_check(null == si.ad);
			{
				s21 so = repack(si._type_idx_, si, 1, big);
				_check(null != so.ad && 0 == so.ad.length);
			}
			{
				si.ad = new double[16384];
				{
					for (int i = 0; i < 16384; i++)
						si.ad[i] = (double)((1l << (i & (64 - 1)))^i)/(i + 1.0);
				}
				{
					s21 so = repack(si._type_idx_, si, 3 + si.ad.length*8, big);
					_check(null != so.ad && so.ad.length == si.ad.length);
					{
						int i = 0;
						for (; i < 16384; i++) {
							if (so.ad[i] != si.ad[i])
								break;
						}
						_check(i == 16384);
					}
				}
			}
		}
		_end();

		_start();
		{
			String test_str= "some Test_22$ string.";
			s22 si = new s22();
			_check(null == si.s);
			{
				s22 so = repack(si._type_idx_, si, 1, big);
				_check(null != so.s && so.s.isEmpty());
			}
			{
				si.s = test_str;
				{
					s22 so = repack(si._type_idx_, si, 1 + test_str.length() + 1, big);
					_check(null != so.s && so.s.equals(si.s));
				}
			}
			{
				si.s = test_str.substring(1, test_str.length() - 1);
				{
					s22 so = repack(si._type_idx_, si, 1 + test_str.length() + 1 - 2, big);
					_check(null != so.s && so.s.equals(si.s));
				}
			}
		}
		_end();

		_start();
		{
			String test_str = "";
			s22 si = new s22();
			_check(null == si.s);
			{
				s22 so = repack(si._type_idx_, si, 1, big);
				_check(null != so.s && so.s.isEmpty());
			}
			{
				si.s = test_str;
				{
					s22 so = repack(si._type_idx_, si, 1, big);
					_check(null != so.s && so.s.equals(si.s));
				}
			}
		}
		_end();

		_start();
		{
			String test_str = "some Test_23$ string.";
			s23 si = new s23();
			_check(null == si.os);
			{
				s23 so = repack(si._type_idx_, si, 1, big);
				_check(null == so.os);
			}
			{
				si.os = test_str;
				{
					s23 so = repack(si._type_idx_, si, 2 + test_str.length() + 1, big);
					_check(null != so.os && so.os.equals(si.os));
				}
			}
			{
				si.os = test_str.substring(1, test_str.length() - 1);
				{
					s23 so = repack(si._type_idx_, si, 2 + test_str.length() + 1 - 2, big);
					_check(null != so.os && so.os.equals(si.os));
				}
			}
		}
		_end();

		_start();
		{
			String test_str = "";
			s23 si = new s23();
			_check(null == si.os);
			{
				si.os = test_str;
				{
					s23 so = repack(si._type_idx_, si, 2, big);
					_check(null != so.os && so.os.equals(si.os));
				}
			}
		}
		_end();

		_start();
		{
			String test_str= "some Test_24$ string.(  )";
			s24 si = new s24();
			_check(null == si.as);
			{
				s24 so = repack(si._type_idx_, si, 1, big);
				_check(null != so.as && 0 == so.as.length);
			}
			{
				si.as = new String[16];
				System.out.println();
				{
					int i = 0;
					for (; i < 14; i++) {
						String s = test_str.substring(0, test_str.length() - ((i < 10) ? 2 : 3)) +
							i + test_str.substring(test_str.length() - 1, test_str.length());
						_check(null == si.as[i]);
						si.as[i] = s;
						System.out.println(" " + si.as[i]);
					}
					si.as[i] = "";
				}
				{
					s24 so = repack(si._type_idx_, si, 1 + 14*(1 + test_str.length() + 1) + 1/*""*/ + 1/*null*/, big);
					_check(null != so.as && 16 == so.as.length);
					_check(so.as.length == si.as.length);
					{
						int i = 0;
						for (; i < 14; i++) {
							if (!si.as[i].equals(so.as[i]))
								break;
						}
						_check(i == 14);
						_check(null != so.as[14] && 0 == so.as[14].length());
						_check(null != so.as[15] && 0 == so.as[15].length());
					}
				}
			}
		}
		_end();

		_start();
		{
			x1 si = new x1();
			_check(null == si.rb);
			{
				si.new_rb();
				_check(!si.rb_get_bit(0));
				_check(!si.rb_get_bit(1));
				{
					x1 so = repack(si._type_idx_, si, 1, big);
					_check(null != so.rb);
					_check(so.rb_get_bit(0) == si.rb_get_bit(0));
					_check(so.rb_get_bit(1) == si.rb_get_bit(1));
				}
			}
			{
				si.rb_set_bit(0);
				si.rb_set_bit(1, true);
				_check(si.rb_get_bit(0));
				_check(si.rb_get_bit(1));
				{
					x1 so = repack(si._type_idx_, si, 1, big);
					_check(null != so.rb);
					_check(so.rb_get_bit(0) == si.rb_get_bit(0));
					_check(so.rb_get_bit(1) == si.rb_get_bit(1));
				}
			}
			{
				si.rb_clear_bit(1);
				_check(!si.rb_get_bit(1));
				{
					x1 so = repack(si._type_idx_, si, 1, big);
					_check(null != so.rb);
					_check(so.rb_get_bit(0) == si.rb_get_bit(0));
					_check(so.rb_get_bit(1) == si.rb_get_bit(1));
				}
			}
		}
		_end();

		_start();
		{
			x2 si = new x2();
			_check(null == si.rb);
			{
				si.new_rb();
				_check(null != si.rb && si.rb_length_ == si.rb.length);
				{
					int i = 0;
					for (; i < si.rb_bit_count_; i++) {
						if (si.rb_get_bit(i))
							break;
					}
					_check(i == si.rb_bit_count_);
				}
				{
					x2 so = repack(si._type_idx_, si, 25, big);
					_check(null != so.rb && so.rb.length == si.rb.length);
					{
						int i = 0;
						for (; i < si.rb_bit_count_; i++) {
							if (si.rb_get_bit(i) != so.rb_get_bit(i))
								break;
						}
						_check(i == si.rb_bit_count_);
					}
				}
			}
			{
				for (int i = 0; i < si.rb_bit_count_; i++)
					si.rb_set_bit(i, 0 != (i%5));
			}
			{
				x2 so = repack(si._type_idx_, si, 25, big);
				_check(null != so.rb && so.rb.length == si.rb.length);
				{
					int i = 0;
					for (; i < si.rb_bit_count_; i++) {
						if (si.rb_get_bit(i) != so.rb_get_bit(i))
							break;
					}
					_check(i == si.rb_bit_count_);
				}
			}
		}
		_end();

		_start();
		{
			x3 si = new x3();
			_check(null == si.rt);
			{
				si.new_rt()[0] = (byte)77;
				{
					x3 so = repack(si._type_idx_, si, 1, big);
					_check(null != so.rt && si.rt.length == so.rt.length);
					_check(si.rt[0] == so.rt[0]);
				}
			}
		}
		_end();

		_start();
		{
			x4 si = new x4();
			_check(null == si.rt);
			{
				si.rt = new byte[]{77, 78};
				{
					x4 so = repack(si._type_idx_, si, 2, big);
					_check(null != so.rt);
					_check(si.rt[0] == so.rt_get(0));
					_check(si.rt[1] == so.rt_get(1));
				}
			}
		}
		_end();

		_start();
		{
			x5 si = new x5();
			_check(null == si.rt);
			{
				si.new_rt()[0] = (short)32767;
				{
					x5 so = repack(si._type_idx_, si, 2, big);
					_check(null != so.rt);
					_check(si.rt[0] == so.rt[0]);
				}
			}
			{
				si.new_rt()[0] = (short)-32768;
				{
					x5 so = repack(si._type_idx_, si, 2, big);
					_check(null != so.rt);
					_check(si.rt[0] == so.rt[0]);
				}
			}
		}
		_end();

		_start();
		{
			x6 si = new x6();
			_check(null == si.rt);
			{
				short[] rt = si.new_rt();
				rt[0] = (short)1;
				rt[1] = (short)-1;
				{
					x6 so = repack(si._type_idx_, si, 4, big);
					_check(null != so.rt);
					_check(so.rt[0] == (short)1);
					_check(so.rt[1] == (short)-1);
				}
			}
		}
		_end();

		_start();
		{
			x7 si = new x7();
			_check(null == si.rt);
			{
				si.new_rt()[0] = -234;
				{
					x7 so = repack(si._type_idx_, si, 4, big);
					_check(null != so.rt);
					_check(si.rt[0] == so.rt[0]);
				}
			}
			{
				si.new_rt()[0] = -233232768;
				{
					x7 so = repack(si._type_idx_, si, 4, big);
					_check(null != so.rt);
					_check(si.rt[0] == so.rt[0]);
				}
			}
		}
		_end();

		_start();
		{
			x8 si = new x8();
			_check(null == si.rt);
			{
				si.rt = new int[] {1234783245, -7};
				{
					x8 so = repack(si._type_idx_, si, 8, big);
					_check(null != so.rt);
					_check(si.rt[0] == so.rt[0]);
					_check(si.rt[1] == so.rt[1]);
				}
			}
		}
		_end();

		_start();
		{
			x9 si = new x9();
			_check(null == si.rt);
			{
				si.rt = new long[] {0xffffccddeeffffl};
				{
					x9 so = repack(si._type_idx_, si, 8, big);
					_check(null != so.rt);
					_check(si.rt[0] == so.rt[0]);
				}
			}
			{
				si.new_rt()[0] = -23247237433232768l;
				{
					x9 so = repack(si._type_idx_, si, 8, big);
					_check(null != so.rt);
					_check(si.rt[0] == so.rt[0]);
				}
			}
		}
		_end();

		_start();
		{
			x10 si = new x10();
			_check(null == si.rt);
			{
				si.rt = new long[] {123871231234783245l, -7328742384234l};
				{
					x10 so = repack(si._type_idx_, si, 16, big);
					_check(null != so.rt);
					_check(si.rt[0] == so.rt[0]);
					_check(si.rt[1] == so.rt[1]);
				}
			}
		}
		_end();

		_start();
		{
			x11 si = new x11();
			_check(null == si.rt);
			{
				si.set_rt(new float[] {2311934.34345f});
				{
					x11 so = repack(si._type_idx_, si, 4, big);
					_check(null != so.rt);
					_check(si.rt[0] == so.rt[0]);
				}
			}
			{
				si.new_rt()[0] = -0.345112f;
				{
					x11 so = repack(si._type_idx_, si, 4, big);
					_check(null != so.rt);
					_check(si.rt[0] == so.rt[0]);
				}
			}
		}
		_end();

		_start();
		{
			x12 si = new x12();
			_check(null == si.rt);
			{
				si.new_rt();
				si.
					rt_set(0, 23545.1312f).
					rt_set(1, -7328.44234f);
				{
					x12 so = repack(si._type_idx_, si, 8, big);
					_check(null != so.rt);
					_check(si.rt[0] == so.rt[0]);
					_check(si.rt[1] == so.rt[1]);
				}
			}
		}
		_end();

		_start();
		{
			x13 si = new x13();
			_check(null == si.rt);
			{
				si.set_rt(23284823311934.343234224345);
				{
					x13 so = repack(si._type_idx_, si, 8, big);
					_check(null != so.rt);
					_check(si.rt[0] == so.rt[0]);
				}
			}
			{
				si.rt = new double[] {-0.345324235123112, -1, -1, -1};
				{
					x13 so = repack(si._type_idx_, si, 8, big);
					_check(null != so.rt);
					_check(si.rt[0] == so.rt[0]);
				}
			}
		}
		_end();

		_start();
		{
			x14 si = new x14();
			_check(null == si.rt);
			{
				si.rt = new double[] {
					2300040545.131234123102,
					-7323406620668.0000044234
				};
				{
					x14 so = repack(si._type_idx_, si, 16, big);
					_check(null != so.rt);
					_check(si.rt[0] == so.rt[0]);
					_check(si.rt[1] == so.rt[1]);
				}
			}
		}
		_end();

		_start();
		{
			x15 si = new x15();
			_check(null == si.rt);
			{
				si.new_rt();
				_check(null == si.rt[0]);
				{
					x15 so = repack(si._type_idx_, si, 1, big);
					_check(null != so.rt && 1 == so.rt.length);
					_check(null != so.rt[0] && so.rt[0].isEmpty());
				}
			}
			{
				si.set_rt("");
				{
					x15 so = repack(si._type_idx_, si, 1, big);
					_check(null != so.rt && 1 == so.rt.length);
					_check(null != so.rt[0] && so.rt[0].isEmpty());
				}
			}
			{
				si.rt_set("aaaabb");
				{
					x15 so = repack(si._type_idx_, si, 1 + 7, big);
					_check(null != so.rt && 1 == so.rt.length);
					_check(null != so.rt[0] && so.rt[0].equals(si.rt[0]));
				}
			}
		}
		_end();

		_start();
		{
			x16 si = new x16();
			_check(null == si.rt);
			{
				si.new_rt();
				_check(null == si.rt_get(0));
				_check(null == si.rt_get(1));
				si.rt_set(1, "");
				_check(si.rt[1].isEmpty());
				{
					x16 so = repack(si._type_idx_, si, 2, big);
					_check(null != so.rt && 2 == so.rt.length);
					_check(null != so.rt[0] && null != so.rt[1]);
					_check(so.rt[0].isEmpty() && so.rt[1].isEmpty());
				}
			}
			{
				String teststr = "xxyynn";
				si.new_rt();
				_check(null == si.rt_get(0));
				_check(null == si.rt_get(1));
				si.rt[0] = teststr;
				si.rt_set(1, "xxy");
				{
					x16 so = repack(si._type_idx_, si,
						1 + teststr.length() + 1/*'\0'*/ + 1 + "xxy".length() + 1/*'\0'*/, big);
					_check(null != so.rt && 2 == so.rt.length);
					_check(null != so.rt[0] && null != so.rt[1]);
					_check(so.rt[0].equals(si.rt[0]));
					_check(so.rt[1].equals(si.rt[1]));
				}
			}
		}
		_end();

		_start();
		{
			ps7 si = new ps7();
			{
				short arr[] = {0,1,2,62,63,64,65,126,127,128,129,8190,8191,8192,8193,8194,32766,32767,-32768};
				for (int i = 0; i < arr.length; i++) {
					si.s = arr[i];
					if (si.s < 0)
						si.s = 0;
					{
						int pz = si.s < 64 ? 1 : si.s < 8192 ? 2 : 3;
						ps7 so = repack(si._type_idx_, si, pz, big);
						_check(si.s == so.s);
					}
					si.s = arr[i];
					if (si.s > 0)
						si.s = (short)-si.s;
					{
						int pz = si.s >= -64 ? 1 : si.s >= -8192 ? 2 : 3;
						ps7 so = repack(si._type_idx_, si, pz, big);
						_check(si.s == so.s);
					}
				}
			}
		}
		_end();

		_start();
		{
			ps8 si = new ps8();
			_check(!si.has_os());
			{
				ps8 so = repack(si._type_idx_, si, 1, big);
				_check(!so.has_os());
			}
			{
				si.set_os((short)-32755);
				{
					ps8 so = repack(si._type_idx_, si, 4, big);
					_check(so.has_os());
					_check(si.os == so.os);
				}
			}
		}
		_end();

		_start();
		{
			ps9 si = new ps9();
			_check(null == si.as);
			{
				ps9 so = repack(si._type_idx_, si, 1, big);
				_check(null != so.as && 0 == so.as.length);
			}
			{
				si.new_as(10000);
				{
					for (int i = 0; i < 10000; i++)
						si.as[i] = (short)((1 << (i & (16 - 1)))^i);
				}
				{
					ps9 so = repack(si._type_idx_, si, 23177, big);
					_check(null != so.as && so.as.length == si.as.length);
					{
						int i = 0;
						for (; i < 10000; i++) {
							if (si.as[i] != so.as[i])
								break;
						}
						_check(i == 10000);
					}
				}
			}
			{
				si.new_as(10000);
				{
					for (int i = 0; i < 10000; i++) {
						si.as[i] = (short)((1 << (i & (16 - 1)))^i);
						if (si.as[i] >= 0)
							si.as[i] = (short)(si.as[i] & 8191);
						else if ((short)(1 << (16 - 1)) == si.as[i])
							si.as[i] = 0;
						else
							si.as[i] = (short)-(-si.as[i] & 8191);
					}
				}
				{
					ps9 so = repack(si._type_idx_, si, 19886, big);
					_check(null != so.as && so.as.length == si.as.length);
					{
						int i = 0;
						for (; i < 10000; i++) {
							if (si.as[i] != so.as[i])
								break;
						}
						_check(i == 10000);
					}
				}
			}
		}
		_end();

		_start();
		{
			ps10 si = new ps10();
			{
				int arr[] = {0,1,2,62,63,64,65,126,127,128,129,8190,8191,8192,8193,8194,32766,32767,32768,
					1048573,1048574,1048575,1048576,1048577,1048578,134217726,134217727,134217728,134217729,134217730,2147483647,-2147483647-1};
				for (int i = 0; i < arr.length; i++) {
					si.i = arr[i];
					if (si.i < 0)
						si.i = 0;
					{
						int pz = si.i < 64 ? 1 : si.i < 8192 ? 2 : si.i < 1048576 ? 3 : si.i < 134217728 ? 4 : 5;
						ps10 so = repack(si._type_idx_, si, pz, big);
						_check(si.i == so.i);
					}
					si.i = arr[i];
					if (si.i > 0)
						si.i = -si.i;
					{
						int pz = si.i >= -64 ? 1 : si.i >= -8192 ? 2 : si.i >= -1048576 ? 3 : si.i >= -134217728 ? 4 : 5;
						ps10 so = repack(si._type_idx_, si, pz, big);
						_check(si.i == so.i);
					}
				}
			}
		}
		_end();

		_start();
		{
			ps11 si = new ps11();
			_check(!si.has_oi());
			{
				ps11 so = repack(si._type_idx_, si, 1, big);
				_check(!so.has_oi());
			}
			{
				si.set_oi(-32234755);
				{
					ps11 so = repack(si._type_idx_, si, 5, big);
					_check(so.has_oi());
					_check(si.oi == so.oi);
				}
			}
		}
		_end();

		_start();
		{
			ps12 si = new ps12();
			_check(null == si.ai);
			{
				ps12 so = repack(si._type_idx_, si, 1, big);
				_check(null != so.ai && 0 == so.ai.length);
			}
			{
				si.new_ai(10000);
				{
					for (int i = 0; i < 10000; i++)
						si.ai[i] = (1 << (i & (32 - 1)))^i;
				}
				{
					ps12 so = repack(si._type_idx_, si, 31893, big);
					_check(null != so.ai && so.ai.length == si.ai.length);
					{
						int i = 0;
						for (; i < 10000; i++) {
							if (si.ai[i] != so.ai[i])
								break;
						}
						_check(i == 10000);
					}
				}
			}
			{
				si.new_ai(10000);
				{
					for (int i = 0; i < 10000; i++) {
						si.ai[i] = (1 << (i & (32 - 1)))^i;
						if (si.ai[i] >= 0)
							si.ai[i] = si.ai[i] & 0x7FFFFFF;
						else if ((1 << (32 - 1)) == si.ai[i])
							si.ai[i] = 0;
						else
							si.ai[i] = -(-si.ai[i] & 0x7FFFFFF);
					}
				}
				{
					ps12 so = repack(si._type_idx_, si, 28053, big);
					_check(null != so.ai && so.ai.length == si.ai.length);
					{
						int i = 0;
						for (; i < 10000; i++) {
							if (si.ai[i] != so.ai[i])
								break;
						}
						_check(i == 10000);
					}
				}
			}
		}
		_end();

		_start();
		{
			ps13 si = new ps13();
			{
				long arr[] = {0,1,2,62,63,64,65,126,127,128,129,8190,8191,8192,8193,8194,32766,32767,32768,
					1048573,1048574,1048575,1048576,1048577,1048578,134217726,134217727,134217728,134217729,134217730,2147483647,2147483648l,
					17179869182l,17179869183l,17179869184l,17179869185l,17179869186l,2199023255550l,2199023255551l,2199023255552l,
					2199023255553l,2199023255554l,281474976710654l,281474976710655l,281474976710656l,281474976710657l,
					281474976710658l,36028797018963966l,36028797018963967l,36028797018963968l,36028797018963969l,
					36028797018963970l,4611686018427387902l,4611686018427387903l,4611686018427387904l,4611686018427387905l,
					9223372036854775806l,9223372036854775807l,-9223372036854775807l-1};
				for (int i = 0; i < arr.length; i++) {
					si.l = arr[i];
					if (si.l < 0)
						si.l = 0;
					{
						int pz = si.l < 64 ? 1 : si.l < 8192 ? 2 : si.l < 1048576 ? 3 : si.l < 134217728 ? 4 :
							si.l < 17179869184l ? 5 : si.l < 2199023255552l ? 6 : si.l < 281474976710656l ? 7 :
							si.l < 36028797018963968l ? 8 : si.l < 4611686018427387904l ? 9 : 9;
						ps13 so = repack(si._type_idx_, si, pz, big);
						_check(si.l == so.l);
					}
					si.l = arr[i];
					if (si.l > 0)
						si.l = -si.l;
					{
						int pz = si.l >= -64 ? 1 : si.l >= -8192 ? 2 : si.l >= -1048576 ? 3 : si.l >= -134217728 ? 4 :
							si.l >= -17179869184l ? 5 : si.l >= -2199023255552l ? 6 : si.l >= -281474976710656l ? 7 :
							si.l >= -36028797018963968l ? 8 : si.l >= -4611686018427387904l ? 9 : 9;
						ps13 so = repack(si._type_idx_, si, pz, big);
						_check(si.l == so.l);
					}
				}
			}
		}
		_end();

		_start();
		{
			ps14 si = new ps14();
			_check(!si.has_ol());
			{
				ps14 so = repack(si._type_idx_, si, 1, big);
				_check(!so.has_ol());
			}
			{
				si.set_ol(-7358435340032234755l);
				ps14 so = repack(si._type_idx_, si, 10, big);
				_check(so.has_ol());
				_check(si.ol == so.ol);
			}
		}
		_end();

		_start();
		{
			ps15 si = new ps15();
			_check(null == si.al);
			{
				ps15 so = repack(si._type_idx_, si, 1, big);
				_check(null != so.al && 0 == so.al.length);
			}
			{
				si.new_al(10000);
				{
					for (int i = 0; i < 10000; i++)
						si.al[i] = (1l << (i & (64 - 1)))^i;
				}
				{
					ps15 so = repack(si._type_idx_, si, 53099, big);
					_check(null != so.al && so.al.length == si.al.length);
					{
						int i = 0;
						for (; i < 10000; i++) {
							if (si.al[i] != so.al[i])
								break;
						}
						_check(i == 10000);
					}
				}
			}
			{
				si.new_al(10000);
				{
					for (int i = 0; i < 10000; i++) {
						si.al[i] = (1l << (i & (64 - 1)))^i;
						if (si.al[i] >= 0)
							si.al[i] = si.al[i] & 0x3FFFFFFFFFFFFFl;
						else if ((1l << (64 - 1)) == si.al[i])
							si.al[i] = 0;
						else
							si.al[i] = -(-si.al[i] & 0x3FFFFFFFFFFFFFl);
					}
				}
				{
					ps15 so = repack(si._type_idx_, si, 43514, big);
					_check(null != so.al && so.al.length == si.al.length);
					{
						int i = 0;
						for (; i < 10000; i++) {
							if (si.al[i] != so.al[i])
								break;
						}
						_check(i == 10000);
					}
				}
			}
		}
		_end();

		_start();
		{
			String test_str = "some Test_p22$ string.";
			ps22 si = new ps22();
			_check(null == si.s);
			{
				ps22 so = repack(si._type_idx_, si, 1, big);
				_check(null != so.s && so.s.isEmpty());
			}
			{
				si.s = test_str;
				{
					ps22 so = repack(si._type_idx_, si, test_str.length() + 1/*'\0'*/, big);
					_check(null != so.s && so.s.equals(si.s));
				}
			}
			{
				si.s = "";
				{
					ps22 so = repack(si._type_idx_, si, 1, big);
					_check(null != so.s && so.s.isEmpty());
				}
			}
			{
				si.s = "ღმერთსი შემვედრე, ნუთუ კვლა დამხსნას სოფლისა შრომასა, ცეცხლს, წყალსა და მიწასა, ჰაერთა თანა მრომასა";
				{
					ps22 so = repack(si._type_idx_, si, 263 + 1/*'\0'*/, big);
					_check(null != so.s && so.s.equals(si.s));
				}
			}
		}
		_end();

		_start();
		{
			String test_str = "some Test_p23$ string.";
			ps23 si = new ps23();
			_check(null == si.os);
			{
				ps23 so = repack(si._type_idx_, si, 1, big);
				_check(null == so.os);
			}
			{
				si.os = test_str;
				{
					ps23 so = repack(si._type_idx_, si, 1 + test_str.length() + 1/*'\0'*/, big);
					_check(null != so.os && so.os.equals(si.os));
				}
			}
			{
				si.os = "";
				{
					ps23 so = repack(si._type_idx_, si, 1 + 1/*'\0'*/, big);
					_check(null != so.os && so.os.equals(si.os));
				}
			}
		}
		_end();

		_start();
		{
			String test_str = "some Test_p24$ string.(  )";
			ps24 si = new ps24();
			_check(null == si.as);
			{
				ps24 so = repack(si._type_idx_, si, 1, big);
				_check(null != so.as && 0 == so.as.length);
			}
			{
				si.new_as(16);
				System.out.println();
				{
					int i = 0;
					for (; i < 14; i++) {
						String s = test_str.substring(0, test_str.length() - ((i < 10) ? 2 : 3)) +
							i + test_str.substring(test_str.length() - 1, test_str.length());
						_check(null == si.as[i]);
						si.as[i] = s;
						System.out.println(" " + si.as[i]);
					}
					si.as[i] = "";
				}
				{
					ps24 so = repack(si._type_idx_, si, 1 + 14*(test_str.length() + 1) + 1/*""*/ + 1/*null*/, big);
					_check(null != so.as && 16 == so.as.length);
					_check(so.as.length == si.as.length);
					{
						int i = 0;
						for (; i < 14; i++) {
							if (!si.as[i].equals(so.as[i]))
								break;
						}
						_check(i == 14);
						_check(null != so.as[14] && 0 == so.as[14].length());
						_check(null != so.as[15] && 0 == so.as[15].length());
					}
				}
			}
		}
		_end();

		_start();
		{
			px5 si = new px5();
			_check(null == si.rt);
			{
				si.set_rt((short)127);
				{
					px5 so = repack(si._type_idx_, si, 2, big);
					_check(null != so.get_rt() && 1 == so.rt.length);
					_check(si.rt[0] == so.rt_get());
				}
			}
			{
				si.rt = new short[] {(short)-32};
				{
					px5 so = repack(si._type_idx_, si, 1, big);
					_check(null != so.rt && 1 == so.rt.length);
					_check(si.rt[0] == so.rt_get());
				}
			}
		}
		_end();

		_start();
		{
			px6 si = new px6();
			_check(null == si.get_rt());
			{
				si.set_rt(
					(short)2000,
					(short)-64
				);
				{
					px6 so = repack(si._type_idx_, si, 3, big);
					_check(null != so.rt && 2 == so.rt.length);
					_check(si.rt[0] == so.rt_get(0));
					_check(si.rt[1] == so.rt_get(1));
				}
			}
		}
		_end();

		_start();
		{
			px7 si = new px7();
			_check(null == si.get_rt());
			{
				si.rt = new int[] {-837584};
				{
					px7 so = repack(si._type_idx_, si, 3, big);
					_check(null != so.rt && 1 == so.rt.length);
					_check(si.rt[0] == so.rt_get());
				}
			}
			{
				si.rt = new int[] {-23232768};
				{
					px7 so = repack(si._type_idx_, si, 4, big);
					_check(null != so.rt && 1 == so.rt.length);
					_check(si.rt[0] == so.rt_get());
				}
			}
		}
		_end();

		_start();
		{
			px8 si = new px8();
			_check(null == si.get_rt());
			{
				si.set_rt(1234783245, -7);
				{
					px8 so = repack(si._type_idx_, si, 6, big);
					_check(null != so.rt && 2 == so.rt.length);
					_check(si.rt[0] == so.rt_get(0));
					_check(si.rt[1] == so.rt_get(1));
				}
			}
		}
		_end();

		_start();
		{
			px9 si = new px9();
			_check(null == si.get_rt());
			{
				si.set_rt(new long[] {0xffffceffffl});
				{
					px9 so = repack(si._type_idx_, si, 6, big);
					_check(null != so.rt && 1 == so.rt.length);
					_check(si.rt[0] == so.rt_get());
				}
			}
			{
				si.set_rt(-23247237433232768l);
				{
					px9 so = repack(si._type_idx_, si, 8, big);
					_check(null != so.rt && 1 == so.rt.length);
					_check(si.rt[0] == so.rt_get());
				}
			}
		}
		_end();

		_start();
		{
			px10 si = new px10();
			_check(null == si.get_rt());
			{
				si.rt = new long[] {123871231234783245l, -7328742384234l};
				{
					px10 so = repack(si._type_idx_, si, 16, big);
					_check(null != so.rt && 2 == so.rt.length);
					_check(si.rt[0] == so.rt_get(0));
					_check(si.rt[1] == so.rt_get(1));
				}
			}
		}
		_end();

		_start();
		{
			px15 si = new px15();
			_check(null == si.get_rt());
			{
				si.new_rt();
				_check(null == si.rt_get());
				{
					px15 so = repack(si._type_idx_, si, 1, big);
					_check(null != so.rt && 1 == so.get_rt().length);
					_check(null != so.rt[0] && so.rt[0].isEmpty());
				}
			}
			{
				si.rt = new String[] {""};
				{
					px15 so = repack(si._type_idx_, si, 1, big);
					_check(null != so.rt && 1 == so.get_rt().length);
					_check(null != so.rt[0] && so.rt[0].isEmpty());
				}
			}
			{
				si.rt_set("aaaabb");
				{
					px15 so = repack(si._type_idx_, si, si.rt[0].length() + 1/*'\0'*/, big);
					_check(null != so.rt && 1 == so.get_rt().length);
					_check(null != so.rt[0] && so.rt[0].equals(si.rt[0]));
				}
			}
		}
		_end();

		_start();
		{
			px16 si = new px16();
			_check(null == si.get_rt());
			{
				si.new_rt();
				_check(null == si.rt[0]);
				_check(null == si.rt[1]);
				si.rt[1] = "";
				_check(si.rt[1].isEmpty());
				{
					px16 so = repack(si._type_idx_, si, 2, big);
					_check(null != so.rt && 2 == so.rt.length);
					_check(null != so.rt[0] && so.rt[0].isEmpty());
					_check(null != so.rt[1] && so.rt[1].isEmpty());
				}
			}
			{
				String teststr = "xxyynn";
				si.set_rt(teststr, "xxy");
				{
					px16 so = repack(si._type_idx_, si,
						si.rt[0].length() + 1/*'\0'*/ + si.rt[1].length() + 1/*'\0'*/, big);
					_check(null != so.rt && 2 == so.rt.length);
					_check(null != so.rt[0] && so.rt[0].equals(si.rt[0]));
					_check(null != so.rt[1] && so.rt[1].equals(si.rt[1]));
				}
			}
		}
		_end();

		_start();
		{
			pps7 si = new pps7();
			{
				short arr[] = {0,1,2,62,63,64,65,126,127,128,129,8190,8191,8192,8193,8194,16383,16484,16385,32766,32767,-32768};
				for (int i = 0; i < arr.length; i++) {
					si.s = arr[i];
					if (si.s < 0)
						si.s = 0;
					{
						int pz = si.s < 128 ? 1 : si.s < 16384 ? 2 : 3;
						pps7 so = repack(si._type_idx_, si, pz, big);
						_check(si.s == so.s);
					}
					si.s = arr[i];
					if (si.s > 0)
						si.s = (short)-si.s;
					{
						int pz = (si.s != 0) ? 3 : 1;
						pps7 so = repack(si._type_idx_, si, pz, big);
						_check(si.s == so.s);
					}
				}
			}
		}
		_end();

		_start();
		{
			pps8 si = new pps8();
			_check(!si.has_os());
			{
				pps8 so = repack(si._type_idx_, si, 1, big);
				_check(!so.has_os());
			}
			{
				si.set_os((short)-3);
				{
					pps8 so = repack(si._type_idx_, si, 4, big);
					_check(so.has_os());
					_check(si.os == so.get_os());
				}
			}
		}
		_end();

		_start();
		{
			pps9 si = new pps9();
			_check(null == si.get_as());
			{
				pps9 so = repack(si._type_idx_, si, 1, big);
				_check(null != so.get_as() && 0 == so.as.length);
			}
			{
				si.new_as(10000);
				{
					for (int i = 0; i < 10000; i++)
						si.as[i] = (short)((1 << (i & (16 - 1)))^i);
				}
				{
					pps9 so = repack(si._type_idx_, si, 21140, big);
					_check(null != so.as && 10000 == so.as.length);
					{
						int i = 0;
						for (; i < 10000; i++) {
							if (si.as[i] != so.as[i])
								break;
						}
						_check(i == 10000);
					}
				}
			}
			{
				si.as = new short[10000];
				{
					for (int i = 0; i < 10000; i++) {
						si.as[i] = (short)((1 << (i & (16 - 1)))^i);
						if (si.as[i] >= 0)
							si.as[i] = (short)(si.as[i] & 8191);
						else if ((short)(1 << (16 - 1)) == si.as[i])
							si.as[i] = 0;
						else
							si.as[i] = (short)-(-si.as[i] & 8191);
					}
				}
				{
					pps9 so = repack(si._type_idx_, si, 20403, big);
					_check(null != so.as && 10000 == so.as.length);
					{
						int i = 0;
						for (; i < 10000; i++) {
							if (si.as[i] != so.as[i])
								break;
						}
						_check(i == 10000);
					}
				}
			}
		}
		_end();

		_start();
		{
			pps10 si = new pps10();
			{
				int arr[] = {0,1,2,62,63,64,65,126,127,128,129,8190,8191,8192,8193,8194,16383,16484,16385,32766,32767,32768,
					1048573,1048574,1048575,1048576,1048577,1048578,2097150,2097151,2097152,2097153,2097154,
					134217726,134217727,134217728,134217729,134217730,268435454,268435455,268435456,268435457,268435458,
					2147483647,-2147483647-1};
				for (int i = 0; i < arr.length; i++) {
					si.i = arr[i];
					if (si.i < 0)
						si.i = 0;
					{
						int pz = si.i < 128 ? 1 : si.i < 16384 ? 2 : si.i < 2097152 ? 3 : si.i < 268435456 ? 4 : 5;
						pps10 so = repack(si._type_idx_, si, pz, big);
						_check(si.i == so.i);
					}
					si.i = arr[i];
					if (si.i > 0)
						si.i = -si.i;
					{
						int pz = (si.i != 0) ? 5 : 1;
						pps10 so = repack(si._type_idx_, si, pz, big);
						_check(si.i == so.i);
					}
				}
			}
		}
		_end();

		_start();
		{
			pps11 si = new pps11();
			_check(!si.has_oi());
			{
				pps11 so = repack(si._type_idx_, si, 1, big);
				_check(!so.has_oi());
			}
			{
				si.set_oi(234755);
				{
					pps11 so = repack(si._type_idx_, si, 4, big);
					_check(so.has_oi());
					_check(si.oi == so.oi);
				}
			}
		}
		_end();

		_start();
		{
			pps12 si = new pps12();
			_check(null == si.ai);
			{
				pps12 so = repack(si._type_idx_, si, 1, big);
				_check(null != so.ai && 0 == so.ai.length);
			}
			{
				si.new_ai(10000);
				{
					for (int i = 0; i < 10000; i++)
						si.ai[i] = (1 << (i & (32 - 1)))^i;
				}
				{
					pps12 so = repack(si._type_idx_, si, 30244, big);
					_check(null != so.ai && 10000 == so.ai.length);
					{
						int i = 0;
						for (; i < 10000; i++) {
							if (si.ai[i] != so.ai_get(i))
								break;
						}
						_check(i == 10000);
					}
				}
			}
			{
				si.new_ai(10000);
				{
					for (int i = 0; i < 10000; i++) {
						si.ai[i] = (1 << (i & (32 - 1)))^i;
						if (si.ai[i] >= 0)
							si.ai[i] = si.ai[i] & 0x7FFFFFF;
						else if ((1 << (32 - 1)) == si.ai[i])
							si.ai[i] = 0;
						else
							si.ai[i] = -(-si.ai[i] & 0x7FFFFFF);
					}
				}
				{
					pps12 so = repack(si._type_idx_, si, 26796, big);
					_check(null != so.ai && 10000 == so.ai.length);
					{
						int i = 0;
						for (; i < 10000; i++) {
							if (si.ai[i] != so.ai[i])
								break;
						}
						_check(i == 10000);
					}
				}
			}
		}
		_end();

		_start();
		{
			pps13 si = new pps13();
			{
				long arr[] = {0,1,2,62,63,64,65,126,127,128,129,8190,8191,8192,8193,8194,16383,16484,16385,32766,32767,32768,
					1048573,1048574,1048575,1048576,1048577,1048578,2097150,2097151,2097152,2097153,2097154,
					134217726,134217727,134217728,134217729,134217730,268435454,268435455,268435456,268435457,268435458,
					2147483647,2147483648l,17179869182l,17179869183l,17179869184l,17179869185l,17179869186l,
					34359738366l,34359738367l,34359738368l,34359738369l,34359738370l,2199023255550l,2199023255551l,2199023255552l,
					2199023255553l,2199023255554l,4398046511103l,4398046511104l,4398046511105l,281474976710654l,281474976710655l,
					281474976710656l,281474976710657l,281474976710658l,562949953421310l,562949953421311l,562949953421312l,
					562949953421313l,562949953421314l,281474976710658l,36028797018963966l,36028797018963967l,36028797018963968l,
					36028797018963969l,36028797018963970l,72057594037927934l,72057594037927935l,72057594037927936l,72057594037927937l,
					72057594037927938l,4611686018427387902l,4611686018427387903l,4611686018427387904l,4611686018427387905l,
					9223372036854775806l,9223372036854775807l,-9223372036854775807l-1};
				for (int i = 0; i < arr.length; i++) {
					si.l = arr[i];
					if (si.l < 0)
						si.l = 0;
					{
						int pz = si.l < 128 ? 1 : si.l < 16384 ? 2 : si.l < 2097152 ? 3 : si.l < 268435456 ? 4 :
							si.l < 34359738368l ? 5 : si.l < 4398046511104l ? 6 : si.l < 562949953421312l ? 7 :
							si.l < 72057594037927936l ? 8 : si.l <= 9223372036854775807l ? 9 : 9;
						pps13 so = repack(si._type_idx_, si, pz, big);
						_check(si.l == so.l);
					}
					si.l = arr[i];
					if (si.l > 0)
						si.l = -si.l;
					{
						int pz = (si.l != 0) ? 9 : 1;
						pps13 so = repack(si._type_idx_, si, pz, big);
						_check(si.l == so.l);
					}
				}
			}
		}
		_end();

		_start();
		{
			pps14 si = new pps14();
			_check(!si.has_ol());
			{
				pps14 so = repack(si._type_idx_, si, 1, big);
				_check(!so.has_ol());
			}
			{
				si.set_ol(63584350032234755l);
				pps14 so = repack(si._type_idx_, si, 9, big);
				_check(so.has_ol());
				_check(si.ol == so.ol);
			}
		}
		_end();

		_start();
		{
			pps15 si = new pps15();
			_check(null == si.al);
			{
				pps15 so = repack(si._type_idx_, si, 1, big);
				_check(null != so.al && 0 == so.al.length);
			}
			{
				si.new_al(10000);
				{
					for (int i = 0; i < 10000; i++)
						si.al[i] = (1l << (i & (64 - 1)))^i;
				}
				{
					pps15 so = repack(si._type_idx_, si, 51644, big);
					_check(null != si.get_al() && 10000 == so.get_al().length);
					{
						int i = 0;
						for (; i < 10000; i++) {
							if (si.al[i] != so.al[i])
								break;
						}
						_check(i == 10000);
					}
				}
			}
			{
				si.al = new long[10000];
				{
					for (int i = 0; i < 10000; i++) {
						si.al[i] = (1l << (i & (64 - 1)))^i;
						if (si.al[i] >= 0)
							si.al[i] = si.al[i] & 0x3FFFFFFFFFFFFFl;
						else if ((1l << (64 - 1)) == si.al[i])
							si.al[i] = 0;
						else
							si.al[i] = -(-si.al[i] & 0x3FFFFFFFFFFFFFl);
					}
				}
				{
					pps15 so = repack(si._type_idx_, si, 42110, big);
					_check(null != si.get_al() && 10000 == so.get_al().length);
					{
						int i = 0;
						for (; i < 10000; i++) {
							if (si.al[i] != so.al[i])
								break;
						}
						_check(i == 10000);
					}
				}
			}
		}
		_end();

		_start();
		{
			ppx5 si = new ppx5();
			_check(null == si.get_rt());
			{
				si.new_rt();
				_check(null != si.get_rt() && 1 == si.rt.length);
				si.rt[0] = 127;
				{
					ppx5 so = repack(si._type_idx_, si, 1, big);
					_check(null != so.rt && 1 == so.rt.length);
					_check(si.rt[0] == so.rt[0]);
				}
			}
			{
				si.set_rt((short)-32);
				{
					ppx5 so = repack(si._type_idx_, si, 3, big);
					_check(null != so.rt && 1 == so.rt.length);
					_check(si.rt[0] == so.rt[0]);
				}
			}
		}
		_end();

		_start();
		{
			ppx6 si = new ppx6();
			_check(null == si.rt);
			{
				si.rt = new short[] {2000, -64};
				{
					ppx6 so = repack(si._type_idx_, si, 5, big);
					_check(null != so.rt && 2 == so.get_rt().length);
					_check(si.rt[0] == so.get_rt()[0]);
					_check(si.rt[1] == so.get_rt()[1]);
				}
			}
		}
		_end();

		_start();
		{
			ppx7 si = new ppx7();
			_check(null == si.rt);
			{
				si.set_rt(837584);
				{
					ppx7 so = repack(si._type_idx_, si, 3, big);
					_check(null != so.rt && 1 == so.rt.length);
					_check(si.rt[0] == so.rt[0]);
				}
			}
			{
				si.rt = new int[] {-23232768};
				{
					ppx7 so = repack(si._type_idx_, si, 5, big);
					_check(null != so.rt && 1 == so.rt.length);
					_check(si.rt[0] == so.rt[0]);
				}
			}
		}
		_end();

		_start();
		{
			ppx8 si = new ppx8();
			_check(null == si.rt);
			{
				si.set_rt(1234783245, -7);
				{
					ppx8 so = repack(si._type_idx_, si, 10, big);
					_check(null != so.rt);
					_check(si.rt[0] == so.rt[0]);
					_check(si.rt[1] == so.rt[1]);
				}
			}
		}
		_end();

		_start();
		{
			ppx9 si = new ppx9();
			_check(null == si.rt);
			{
				si.set_rt(0xffffceffffl);
				{
					ppx9 so = repack(si._type_idx_, si, 6, big);
					_check(null != so.rt && 1 == so.rt.length);
					_check(si.rt[0] == so.rt[0]);
				}
			}
			{
				si.set_rt(-23247237433232768l);
				{
					ppx9 so = repack(si._type_idx_, si, 9, big);
					_check(null != so.rt && 1 == so.rt.length);
					_check(si.rt[0] == so.rt[0]);
				}
			}
		}
		_end();

		_start();
		{
			ppx10 si = new ppx10();
			_check(null == si.rt);
			{
				si.rt = new long[] {123871231234783245l, -7328742384234l};
				{
					ppx10 so = repack(si._type_idx_, si, 18, big);
					_check(null != so.rt && so.rt_length_ == so.rt.length);
					_check(si.rt_get(0) == so.rt_get(0));
					_check(si.rt_get(1) == so.rt_get(1));
				}
			}
		}
		_end();

		_start();
		{
			s25 si = new s25();
			_check(!si.has_oi());
			_check(null == si.s);
			{
				s25 so = repack(si._type_idx_, si, 2, big);
				_check(!so.has_oi());
				_check(null != so.s && 0 == so.get_s().length());
			}
			{
				si.s = "a";
				si.set_b(true);
				{
					s25 so = repack(si._type_idx_, si, 1 + 1 + si.s.length() + 1/*'\0'*/, big);
					_check(!so.has_oi());
					_check(si.get_b() == so.get_b());
					_check(null != so.get_s() && si.s.equals(so.s));
				}
			}
			{
				si.set_oi(-213123245);
				{
					s25 so = repack(si._type_idx_, si, 8, big);
					_check(si.get_b() == so.get_b());
					_check(so.has_oi());
					_check(si.oi == so.oi);
					_check(null != so.get_s() && si.s.equals(so.s));
				}
			}
			{
				si.unset_oi();
				{
					s25 so = repack(si._type_idx_, si, 4, big);
					_check(si.get_b() == so.get_b());
					_check(!so.has_oi());
					_check(null != so.get_s() && si.s.equals(so.s));
				}
			}
		}
		_end();

		_start();
		{
			s26 si = new s26();
			_check(!si.has_od());
			_check(null == si.aa);
			{
				si.b = 121;
				{
					s26 so = repack(si._type_idx_, si, 4, big);
					_check(!so.has_od());
					_check(si.b == so.b);
					_check(null != so.aa && 0 == so.aa.length);
				}
			}
			{
				si.set_od(1223601.321);
				{
					s26 so = repack(si._type_idx_, si, 12, big);
					_check(so.has_od());
					_check(si.b == so.b);
					_check(si.od == so.od);
					_check(null != so.aa && 0 == so.aa.length);
				}
			}
			{
				si.new_aa(1);
				_check(null == si.aa[0]);
				si.aa[0] = new s27().
					set_ab(null, 0).
					set_as4(null);
				_check(null == si.aa[0].ab);
				_check(0 == si.aa[0].ab_bit_count);
				_check(null == si.aa[0].s);
				si.aa[0].set_s(new s25());
				_check(null == si.aa[0].s.s);
				_check(null == si.aa[0].s2);
				si.aa[0].set_s2(new s26());
				_check(null == si.aa[0].s2.aa);
				_check(null == si.aa[0].os3);
				_check(null == si.aa[0].as4);
				{
					s26 so = repack(si._type_idx_, si, 17, big);
					_check(so.has_od());
					_check(si.b == so.b);
					_check(si.od == so.od);
					_check(null != so.aa && 1 == so.aa.length);
					_check(null != so.aa[0]);
					_check(null != so.aa[0].ab && 0 == so.aa[0].ab.length);
					_check(0 == so.aa[0].ab_bit_count);
					_check(null != so.aa[0].s.s && so.aa[0].s.s.isEmpty());
					_check(null != so.aa[0].s2.aa && 0 == so.aa[0].s2.aa.length);
					_check(null == so.aa[0].os3);
					_check(null != so.aa[0].as4 && 0 == so.aa[0].as4.length);
				}
			}
			{
				si.aa[0].new_ab(6);
				_check(!si.aa[0].ab_get_bit(0));
				_check(!bridge_get_bit(si.aa[0].ab, 1));
				_check(!si.aa[0].ab_get_bit(2));
				_check(!bridge_get_bit(si.aa[0].ab, 3));
				_check(!si.aa[0].ab_get_bit(4));
				_check(!bridge_get_bit(si.aa[0].ab, 5));
				bridge_set_bit(si.aa[0].ab, 2, true);
				si.aa[0].ab_set_bit(3, true);
				bridge_set_bit(si.aa[0].ab, 5, true);
				{
					s26 so = repack(si._type_idx_, si, 18, big);
					_check(so.has_od());
					_check(si.b == so.b);
					_check(si.od == so.od);
					_check(null != so.aa && 1 == so.aa.length);
					_check(null != so.aa[0].ab);
					_check(so.aa[0].ab_bit_count == 6);
					_check(null != so.aa[0].s.s && so.aa[0].s.s.isEmpty());
					_check(null != so.aa[0].s2.aa && 0 == so.aa[0].s2.aa.length);
					_check(null == so.aa[0].os3);
					_check(null != so.aa[0].as4 && 0 == so.aa[0].as4.length);
					_check(!bridge_get_bit(so.aa[0].ab, 0));
					_check(!so.aa[0].ab_get_bit(1));
					_check(bridge_get_bit(so.aa[0].ab, 2));
					_check(so.aa[0].ab_get_bit(3));
					_check(!bridge_get_bit(so.aa[0].ab, 4));
					_check(so.aa[0].ab_get_bit(5));
				}
			}
		}
		_end();

		_start();
		{
			s30 si = new s30();
			{
				si.i = 243454;
				si.l = -1022010155451104042l;
				{
					s30 so = repack(si._type_idx_, si, 12, big);
					_check(so.i == si.i);
					_check(so.l == si.l);
				}
			}
		}
		_end();

		_start();
		{
			s31 si = new s31();
			_check(null == si.aa);
			{
				si.b = 24;
				si.new_rc().
					set_i(243454).
					set_l(-1022010155451104042l);
				si.set_rd(
					new s30().
						set_i(243455).
						set_l(-1022010155451104043l),
					new s30().
						set_i(243456).
						set_l(-1022010155451104044l)
				);
				{
					s31 so = repack(si._type_idx_, si, 38, big);
					_check(null != so.aa && 0 == so.aa.length);
					_check(so.b == si.b);
					_check(null != so.rc);
					_check(so.rc.i == si.rc.i);
					_check(so.rc.l == si.rc.l);
					_check(null != so.rd && so.rd_length_ == so.rd.length);
					_check(so.rd_get(0).i == si.rd_get(0).i);
					_check(so.rd_get(0).l == si.rd_get(0).l);
					_check(so.rd_get(1).i == si.rd_get(1).i);
					_check(so.rd_get(1).l == si.rd_get(1).l);
				}
			}
			{
				si.set_b((byte)22);
				si.set_aa(new s30[] {
					new s30().
						set_i(112233500).
						set_l(1234567890451104042l),
					new s30().
						set_i(445566778).
						set_l(1234567847365987432l)
				});
				{
					s31 so = repack(si._type_idx_, si, 62, big);
					_check(null != so.aa && so.aa.length == si.aa.length);
					_check(so.aa[0].i == si.aa[0].i);
					_check(so.aa[0].l == si.aa[0].l);
					_check(so.aa[1].i == si.aa[1].i);
					_check(so.aa[1].l == si.aa[1].l);
					_check(so.b == si.b);
					_check(null != so.rc);
					_check(so.rc.i == si.rc.i);
					_check(so.rc.l == si.rc.l);
					_check(null != so.rd && 2 == so.rd.length);
					_check(so.rd[0].i == si.rd[0].i);
					_check(so.rd[0].l == si.rd[0].l);
					_check(so.rd[1].i == si.rd[1].i);
					_check(so.rd[1].l == si.rd[1].l);
				}
			}
		}
		_end();

		_start();
		{
			s35 si = new s35();
			_check(null == si.aa1);
			_check(null == si.aa2);
			_check(null == si.aa3);
			{
				si.f = -545.334f;
				{
					s35 so = repack(si._type_idx_, si, 7, big);
					_check(so.f == si.f);
					_check(null != so.aa1 && 0 == so.aa1.length);
					_check(null != so.aa2 && 0 == so.aa2.length);
					_check(null != so.aa3 && 0 == so.aa3.length);
				}
			}
			{
				si.f = 22.3442f;
				si.
					set_aa1(new s32[] {
						new s32().
							set_c((byte)45),
						new s32().
							set_c((byte)53)
					}).
					set_aa2(new s33[] {
						new s33().
							set_u(4545452)
					}).
					set_aa3(new s34[] {
						new s34().
							set_u1(4657432).
							set_u2(-876843).
							set_ss1(new s33().
								set_u(-87684)).
							set_u3(-8768),
						new s34().
							set_u1(67894352).
							set_u2(5465452).
							set_ss1(new s33().
								set_u(-8764)).
							set_u3(-878),
						new s34().
							set_u1(-68584352).
							set_u2(3635622).
							set_ss1(new s33().
								set_u(-864)).
							set_u3(-88),
					});
				{
					s35 so = repack(si._type_idx_, si, 61, big);
					_check(so.f == si.f);
					_check(null != so.aa1 && so.aa1.length == si.aa1.length);
					_check(null != so.aa2 && so.aa2.length == si.aa2.length);
					_check(null != so.aa3 && so.aa3.length == si.aa3.length);
					_check(2 == so.aa1.length);
					_check(null != so.aa1[0]);
					_check(null != so.aa1[1]);
					_check(1 == so.aa2.length);
					_check(null != so.aa2[0]);
					_check(3 == so.aa3.length);
					_check(null != so.aa3[0]);
					_check(null != so.aa3[1]);
					_check(null != so.aa3[2]);
					_check(so.aa1[0].c == si.aa1[0].c);
					_check(so.aa1[1].c == si.aa1[1].c);
					_check(so.aa2[0].u == si.aa2[0].u);
					_check(so.aa3[0].u1 == si.aa3[0].u1);
					_check(so.aa3[0].u2 == si.aa3[0].u2);
					_check(so.aa3[1].u1 == si.aa3[1].u1);
					_check(so.aa3[1].u2 == si.aa3[1].u2);
					_check(so.aa3[2].u1 == si.aa3[2].u1);
					_check(so.aa3[2].u2 == si.aa3[2].u2);
				}
			}
		}
		_end();

		_start();
		{
			string_list si = new string_list();
			_check(null == si.onext);
			{
				si.s = "aa";
				si.onext =
					new string_list().
						set_s("bb").
						set_onext(new string_list().
							set_s("cc"));
				{
					string_list so = repack(si._type_idx_, si, 14, big);
					_check(null != so.s && so.s.equals("aa"));
					_check(null != so.onext && so.onext.s.equals("bb"));
					_check(null != so.onext.onext && so.onext.onext.s.equals("cc"));
					_check(null == so.onext.onext.onext);
				}
			}
			{
				String str_buf = "aabbcc0d";
				string_list tail = si.onext.onext;
				for (int i = 0; i < 100; i++) {
					String s = str_buf.substring(0, str_buf.length() - ((i < 10) ? 1 : 2)) + i;
					tail.onext = new string_list().
						set_s(s);
					tail = tail.onext;
				}
			}
			{
				string_list so = repack(si._type_idx_, si, 1026, big);
				{
					int c = 0;
					string_list i = si;
					string_list o = so;
					for (; null != i; c++) {
						if (null == o || null == o.s)
							break;
						if (!i.s.equals(o.s))
							break;
						o = o.onext;
						i = i.onext;
					}
					_check((null == i) == (null == o));
					_check(c == 3 + 100);
				}
			}
		}
		_end();

		_start();
		{
			sTree si = new sTree();
			_check(null == si.oleft);
			_check(null == si.oright);
			{
				si.name = "node1";
				si.value = 1;
				{
					sTree so = repack(si._type_idx_, si, 13, big);
					_check(null == so.oleft);
					_check(null == so.oright);
					_check(null != so.name && so.name.equals(si.name));
					_check(so.value == si.value);
				}
			}
		}
		_end();

		_start();
		{
			sTree si = new sTree();
			_check(null == si.oleft);
			_check(null == si.oright);
			{
				si.name = "root20";
				si.value = 20;
			}
			{
				si.
					set_oleft(new sTree().
						set_name("left10").
						set_value(10)).
					set_oright(new sTree().
						set_name("right30").
						set_value(30));
			}
			{
				sTree so = repack(si._type_idx_, si, 39, big);
				{
					_check(null != so.oleft);
					_check(null != so.oright);
					_check(null != so.name && so.name.equals(si.name));
					_check(so.value == si.value);
					_check(so.value == 20);
				}
				{
					_check(null == so.oleft.oleft);
					_check(null == so.oleft.oright);
					_check(null != so.oleft.name && so.oleft.name.equals(si.oleft.name));
					_check(so.oleft.value == si.oleft.value);
					_check(so.oleft.value == 10);
				}
				{
					_check(null == so.oright.oleft);
					_check(null == so.oright.oright);
					_check(null != so.oright.name && so.oright.name.equals(si.oright.name));
					_check(so.oright.value == si.oright.value);
					_check(so.oright.value == 30);
				}
			}
			{
				si.oleft.
					set_oleft(new sTree().
						set_name("leftleft5").
						set_value(5)).
					set_oright(new sTree().
						set_name("leftright15").
						set_value(15));
			}
			{
				si.oright.
					set_oleft(new sTree().
						set_name("rightleft25").
						set_value(25)).
					set_oright(new sTree().
						set_name("rightright35").
						set_value(35));
			}
			{
				sTree so = repack(si._type_idx_, si, 107, big);
				{
					_check(null != so.oleft);
					_check(null != so.oright);
					_check(null != so.name && so.name.equals(si.name));
					_check(so.value == si.value);
					_check(so.value == 20);
				}
				{
					_check(null != so.oleft.oleft);
					_check(null != so.oleft.oright);
					_check(null != so.oleft.name && so.oleft.name.equals(si.oleft.name));
					_check(so.oleft.value == si.oleft.value);
					_check(so.oleft.value == 10);
				}
				{
					_check(null != so.oright.oleft);
					_check(null != so.oright.oright);
					_check(null != so.oright.name && so.oright.name.equals(si.oright.name));
					_check(so.oright.value == si.oright.value);
					_check(so.oright.value == 30);
				}
				{
					_check(null == so.oleft.oleft.oleft);
					_check(null == so.oleft.oleft.oright);
					_check(null != so.oleft.oleft.name && so.oleft.oleft.name.equals(si.oleft.oleft.name));
					_check(so.oleft.oleft.value == si.oleft.oleft.value);
					_check(so.oleft.oleft.value == 5);
				}
				{
					_check(null == so.oleft.oright.oleft);
					_check(null == so.oleft.oright.oright);
					_check(null != so.oleft.oright.name && so.oleft.oright.name.equals(si.oleft.oright.name));
					_check(so.oleft.oright.value == si.oleft.oright.value);
					_check(so.oleft.oright.value == 15);
				}
				{
					_check(null == so.oright.oleft.oleft);
					_check(null == so.oright.oleft.oright);
					_check(null != so.oright.oleft.name && so.oright.oleft.name.equals(si.oright.oleft.name));
					_check(so.oright.oleft.value == si.oright.oleft.value);
					_check(so.oright.oleft.value == 25);
				}
				{
					_check(null == so.oright.oright.oleft);
					_check(null == so.oright.oright.oright);
					_check(null != so.oright.oright.name && so.oright.oright.name.equals(si.oright.oright.name));
					_check(so.oright.oright.value == si.oright.oright.value);
					_check(so.oright.oright.value == 35);
				}
			}
		}
		_end();

		_start();
		{
			sA2 si = new sA2();
			{
				_check(null == si.ob2);
				si.m = 13;
			}
			{
				sA2 so = repack(si._type_idx_, si, 6, big);
				{
					_check(so.m == si.m && so.m == 13);
					_check(null == so.ob2);
				}
			}
			{
				si.ob2 = new sB2();
				_check(null == si.ob2.n);
				_check(null == si.ob2.oa2);
				{
					sA2 so = repack(si._type_idx_, si, 7, big);
					{
						_check(null != so.ob2);
						_check(null != so.ob2.n);
						_check(so.ob2.n.isEmpty());
						_check(null == so.ob2.oa2);
					}
				}
			}
			{
				si.ob2.n = "sss";
				{
					sA2 so = repack(si._type_idx_, si, 11, big);
					{
						_check(null != so.ob2);
						_check(null != so.ob2.n);
						_check(so.ob2.n.equals("sss"));
						_check(null == so.ob2.oa2);
					}
				}
			}
			{
				si.ob2.oa2 = new sA2();
				_check(null == si.ob2.oa2.ob2);
				si.ob2.oa2.m = 3;
				{
					sA2 so = repack(si._type_idx_, si, 15, big);
					{
						_check(null != so.ob2);
						_check(null != so.ob2.n);
						_check(so.ob2.n.equals("sss"));
						_check(null != so.ob2.oa2);
						_check(so.ob2.oa2.m == si.ob2.oa2.m && so.ob2.oa2.m == 3);
						_check(null == so.ob2.oa2.ob2);
					}
				}
			}
			{
				si.ob2.oa2.ob2 = new sB2();
				_check(null == si.ob2.oa2.ob2.n);
				_check(null == si.ob2.oa2.ob2.oa2);
				{
					sA2 so = repack(si._type_idx_, si, 16, big);
					{
						_check(null != so.ob2);
						_check(null != so.ob2.n);
						_check(so.ob2.n.equals("sss"));
						_check(null != so.ob2.oa2);
						_check(so.ob2.oa2.m == si.ob2.oa2.m && so.ob2.oa2.m == 3);
						_check(null != so.ob2.oa2.ob2);
						_check(null != so.ob2.oa2.ob2.n);
						_check(so.ob2.oa2.ob2.n.isEmpty());
						_check(null == so.ob2.oa2.ob2.oa2);
					}
				}
			}
			{
				si.ob2.oa2.ob2.n = "ddd";
				{
					sA2 so = repack(si._type_idx_, si, 20, big);
					{
						_check(null != so.ob2);
						_check(null != so.ob2.n);
						_check(so.ob2.n.equals("sss"));
						_check(null != so.ob2.oa2);
						_check(so.ob2.oa2.m == si.ob2.oa2.m && so.ob2.oa2.m == 3);
						_check(null != so.ob2.oa2.ob2);
						_check(null != so.ob2.oa2.ob2.n);
						_check(so.ob2.oa2.ob2.n.equals("ddd"));
						_check(null == so.ob2.oa2.ob2.oa2);
					}
				}
			}
			{
				si.ob2.oa2.ob2.oa2 = new sA2();
				_check(null == si.ob2.oa2.ob2.oa2.ob2);
				si.ob2.oa2.ob2.oa2.m = 7;
				{
					sA2 so = repack(si._type_idx_, si, 24, big);
					{
						_check(null != so.ob2);
						_check(null != so.ob2.n);
						_check(so.ob2.n.equals("sss"));
						_check(null != so.ob2.oa2);
						_check(so.ob2.oa2.m == si.ob2.oa2.m && so.ob2.oa2.m == 3);
						_check(null != so.ob2.oa2.ob2);
						_check(null != so.ob2.oa2.ob2.n);
						_check(so.ob2.oa2.ob2.n.equals("ddd"));
						_check(null != so.ob2.oa2.ob2.oa2);
						_check(so.ob2.oa2.ob2.oa2.m == si.ob2.oa2.ob2.oa2.m && so.ob2.oa2.ob2.oa2.m == 7);
						_check(null == so.ob2.oa2.ob2.oa2.ob2);
					}
				}
			}
		}
		_end();

		_start();
		{
			sB2 si = new sB2();
			{
				_check(null == si.n);
				_check(null == si.oa2);
				{
					sB2 so = repack(si._type_idx_, si, 3, big);
					{
						_check(null != so.n);
						_check(so.n.isEmpty());
						_check(null == so.oa2);
					}
				}
			}
			{
				si.n = "sss";
				{
					sB2 so = repack(si._type_idx_, si, 7, big);
					{
						_check(null != so.n);
						_check(so.n.equals("sss"));
						_check(null == so.oa2);
					}
				}
			}
			{
				si.oa2 = new sA2();
				_check(null == si.oa2.ob2);
				si.oa2.m = 3;
				{
					sB2 so = repack(si._type_idx_, si, 11, big);
					{
						_check(null != so.n);
						_check(so.n.equals("sss"));
						_check(null != so.oa2);
						_check(so.oa2.m == si.oa2.m && so.oa2.m == 3);
						_check(null == so.oa2.ob2);
					}
				}
			}
			{
				si.oa2.ob2 = new sB2();
				_check(null == si.oa2.ob2.n);
				_check(null == si.oa2.ob2.oa2);
				{
					sB2 so = repack(si._type_idx_, si, 12, big);
					{
						_check(null != so.n);
						_check(so.n.equals("sss"));
						_check(null != so.oa2);
						_check(so.oa2.m == si.oa2.m && so.oa2.m == 3);
						_check(null != so.oa2.ob2);
						_check(null != so.oa2.ob2.n);
						_check(so.oa2.ob2.n.isEmpty());
						_check(null == so.oa2.ob2.oa2);
					}
				}
			}
			{
				si.oa2.ob2.n = "ddd";
				{
					sB2 so = repack(si._type_idx_, si, 16, big);
					{
						_check(null != so.n);
						_check(so.n.equals("sss"));
						_check(null != so.oa2);
						_check(so.oa2.m == si.oa2.m && so.oa2.m == 3);
						_check(null != so.oa2.ob2);
						_check(null != so.oa2.ob2.n);
						_check(so.oa2.ob2.n.equals("ddd"));
						_check(null == so.oa2.ob2.oa2);
					}
				}
			}
			{
				si.oa2.ob2.oa2 = new sA2();
				_check(null == si.oa2.ob2.oa2.ob2);
				si.oa2.ob2.oa2.m = 7;
				{
					sB2 so = repack(si._type_idx_, si, 20, big);
					{
						_check(null != so.n);
						_check(so.n.equals("sss"));
						_check(null != so.oa2);
						_check(so.oa2.m == si.oa2.m && so.oa2.m == 3);
						_check(null != so.oa2.ob2);
						_check(null != so.oa2.ob2.n);
						_check(so.oa2.ob2.n.equals("ddd"));
						_check(null != so.oa2.ob2.oa2);
						_check(so.oa2.ob2.oa2.m == si.oa2.ob2.oa2.m && so.oa2.ob2.oa2.m == 7);
						_check(null == so.oa2.ob2.oa2.ob2);
					}
				}
			}
		}
		_end();

	}
}
