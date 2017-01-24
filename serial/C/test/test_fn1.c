/********************************************************************************
* bitbridge - another serialization/rpc mechanism
* Copyright (C) 2008-2017 Michael M. Builov, https://github.com/mbuilov/bitbridge
* Licensed under GPL version 3 or any later version, see COPYING
********************************************************************************/

/* test_fn1.c */

#include <stdlib.h>
#include <stdio.h>

#include "test_fn1.h"
#include "memstack.h"
#include "gen_test.h"

#ifdef WIN32
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif
#endif

#ifdef BITBRIDGE_DEBUG
#define P1(s)      printf(s)
#define P2(s,a)    printf(s,a)
#define __START__  printf("%8.d ->", __LINE__); {
#define __CHK__(e) do {if (!(e)) goto fail; else printf(" %d", __LINE__);} while ((void)0,0)
#define __END__    } printf("\n");
#else /* !BITBRIDGE_DEBUG */
#define P1(s)
#define P2(s,a)
#define __START__  {
#define __CHK__(e) do {if (!(e)) goto fail;} while ((void)0,0)
#define __END__    }
#endif /* !BITBRIDGE_DEBUG */

#if defined __GNUC__ && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6) || defined __clang__)
#pragma GCC diagnostic ignored "-Wfloat-equal"
#ifndef __clang__
#ifndef __cplusplus
#pragma GCC diagnostic ignored "-Wunsuffixed-float-constants"
#endif
#endif
#endif

#define bridge_array_size(a) ((size_t)((const char*)a##_end - (const char*)a))

int test_fn1(
	struct bridge_allocator *struct_allocator,
	struct bridge_allocator *pack_allocator,
	struct bridge_allocator *unpack_allocator,
	struct memstack *sost,
	int convert)
{
	union bridge_pack_conv_info pc;
	union bridge_unpack_conv_info uc;

	pc.pi.ac = pack_allocator;
	pc.pi.convert = convert;

	uc.ui.ac = unpack_allocator;
	uc.ui.convert = convert;

#define BRIDGE_DEFAULT_ALLOCATOR struct_allocator

/* allocates new so */
#define REPACK(type, sz) do { \
	void *upk_mem = type##_pack(si, &pc.p); \
	__CHK__(upk_mem); __CHK__(pc.pi.size == sz); \
	bridge_unpack_src_init(&uc.ui.us, upk_mem, pc.pi.size); \
	so = type##_unpack(&uc.u); \
	__CHK__(so); __CHK__((char*)upk_mem + pc.pi.size == uc.ui.us.from); \
	bridge_ac_free(upk_mem, pc.pi.ac); \
} while ((void)0,0)

/* same as REPACK, but support converting byte-order while packing/unpacking */
#define CREPACK(type, sz) do { \
	void *upk_mem = type##_pack(si, &pc); \
	__CHK__(upk_mem); __CHK__(pc.pi.size == sz); \
	bridge_unpack_src_init(&uc.ui.us, upk_mem, pc.pi.size); \
	so = type##_unpack(&uc); \
	__CHK__(so); __CHK__((char*)upk_mem + pc.pi.size == uc.ui.us.from); \
	bridge_ac_free(upk_mem, pc.pi.ac); \
} while ((void)0,0)

/* reuses old so */
#define REPACK_S(type, sz) do { \
	void *upk_mem = type##_pack(si, &pc.p); \
	__CHK__(upk_mem); __CHK__(pc.pi.size == sz); \
	bridge_unpack_src_init(&uc.ui.us, upk_mem, pc.pi.size); \
	so = type##_unpack_to(so, &uc.u); \
	__CHK__(so); __CHK__((char*)upk_mem + pc.pi.size == uc.ui.us.from); \
	bridge_ac_free(upk_mem, pc.pi.ac); \
} while ((void)0,0)

/* same as REPACK_S, but support converting byte-order while packing/unpacking */
#define CREPACK_S(type, sz) do { \
	void *upk_mem = type##_pack(si, &pc); \
	__CHK__(upk_mem); __CHK__(pc.pi.size == sz); \
	bridge_unpack_src_init(&uc.ui.us, upk_mem, pc.pi.size); \
	so = type##_unpack_to(so, &uc); \
	__CHK__(so); __CHK__((char*)upk_mem + pc.pi.size == uc.ui.us.from); \
	bridge_ac_free(upk_mem, pc.pi.ac); \
} while ((void)0,0)

/* create, delete, then create again */
#define RENEW(type) \
	struct type *so, *si = type##_new(); \
	__CHK__(si); \
	type##_delete(si); \
	si = type##_new(); \
	__CHK__(si)

	__START__
	{
		RENEW(s1);
		{
			si->b = 0;
			REPACK(s1, 1);
			__CHK__(so->b == si->b);
			s1_ac_delete(so, uc.ui.ac);
			memstack_cleanup(sost);
		}
		{
			si->b = 1;
			REPACK(s1, 1);
			__CHK__(so->b == si->b);
			s1_ac_delete(so, uc.ui.ac);
			memstack_cleanup(sost);
		}
		s1_delete(si);
	}
	__END__

	__START__
	{
		RENEW(s2);
		{
			__CHK__(!si->has_ob);
			si->has_ob = 1;
			si->ob = 1;
			REPACK(s2, 1);
			__CHK__(so->has_ob && so->ob == si->ob);
			s2_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
		}
		{
			si->ob = 0;
			REPACK_S(s2, 1);
			__CHK__(so->has_ob && so->ob == si->ob);
			s2_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
		}
		{
			si->has_ob = 0;
			REPACK_S(s2, 1);
			__CHK__(!so->has_ob);
			s2_ac_delete(so, uc.ui.ac);
			memstack_cleanup(sost);
		}
		s2_delete(si);
	}
	__END__

	__START__
	{
		RENEW(s3);
		{
			__CHK__(!si->ab && !si->ab_bit_count);
			__CHK__(s3_new_ab(si, 5));
			bridge_set_bit(si->ab, 0, 1);
			bridge_set_bit(si->ab, 2, 1);
			bridge_set_bit(si->ab, 4, 1);
			REPACK(s3, 2);
			__CHK__(si->ab_bit_count == so->ab_bit_count);
			__CHK__(
				 bridge_get_bit(so->ab, 0) &&
				!bridge_get_bit(so->ab, 1) &&
				 bridge_get_bit(so->ab, 2) &&
				!bridge_get_bit(so->ab, 3) &&
				 bridge_get_bit(so->ab, 4)
			);
			s3_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
			bridge_free(si->ab);
		}
		{
			__CHK__(s3_new_ab(si, 9));
			{
				unsigned bit_counter = 0;
				bridge_set_bit(si->ab, bit_counter++, 1);
				bridge_set_bit(si->ab, bit_counter++, 0);
				bridge_set_bit(si->ab, bit_counter++, 0);
				bridge_set_bit(si->ab, bit_counter++, 1);
				bridge_set_bit(si->ab, bit_counter++, 1);
				bridge_set_bit(si->ab, bit_counter++, 0);
				bridge_set_bit(si->ab, bit_counter++, 1);
				bridge_set_bit(si->ab, bit_counter++, 0);
				bridge_set_bit(si->ab, bit_counter++, 1);
			}
			REPACK_S(s3, 3);
			__CHK__(si->ab_bit_count == so->ab_bit_count);
			{
				unsigned bit_counter = 0;
				__CHK__(
					 bridge_get_bit(so->ab, bit_counter++) &&
					!bridge_get_bit(so->ab, bit_counter++) &&
					!bridge_get_bit(so->ab, bit_counter++) &&
					 bridge_get_bit(so->ab, bit_counter++) &&
					 bridge_get_bit(so->ab, bit_counter++) &&
					!bridge_get_bit(so->ab, bit_counter++) &&
					 bridge_get_bit(so->ab, bit_counter++) &&
					!bridge_get_bit(so->ab, bit_counter++) &&
					 bridge_get_bit(so->ab, bit_counter++)
				);
			}
			s3_ac_delete(so, uc.ui.ac);
		}
		s3_delete(si);
	}
	__END__

	__START__
	{
		RENEW(s4);
		{
			si->b = 123;
			REPACK(s4, 1);
			__CHK__(so->b == si->b);
			s4_ac_delete(so, uc.ui.ac);
		}
		s4_delete(si);
	}
	__END__

	__START__
	{
		RENEW(s5);
		{
			__CHK__(!si->has_ob);
			si->ob = 112;
			si->has_ob = 1;
			REPACK(s5, 2);
			__CHK__(so->has_ob == si->has_ob);
			__CHK__(so->ob == si->ob);
			s5_ac_delete(so, uc.ui.ac);
		}
		s5_delete(si);
	}
	__END__

	__START__
	{
		RENEW(s6);
		{
			__CHK__(!si->ab && !si->ab_end);
			__CHK__(s6_new_ab(si, 3));
			si->ab[0] = 91;
			si->ab[1] = 112;
			si->ab[2] = 122;
			REPACK(s6, 4);
			__CHK__(bridge_array_count(so->ab) == bridge_array_count(si->ab));
			__CHK__(
				so->ab[0] == si->ab[0] &&
				so->ab[1] == si->ab[1] &&
				so->ab[2] == si->ab[2]
			);
			s6_ac_delete(so, uc.ui.ac);
		}
		s6_delete(si);
	}
	__END__

	__START__
	{
		RENEW(s7);
		{
			si->s = 25443;
			CREPACK(s7, 2);
			__CHK__(so->s == si->s);
			s7_ac_delete(so, uc.ui.ac);
		}
		s7_delete(si);
	}
	__END__

	__START__
	{
		RENEW(s8);
		{
			__CHK__(!si->has_os);
			si->os = 25443;
			CREPACK(s8, 1);
			__CHK__(!so->has_os);
			s8_ac_delete(so, uc.ui.ac);
		}
		{
			si->has_os = 1;
			__CHK__(si->os == 25443);
			CREPACK(s8, 3);
			__CHK__(so->has_os);
			__CHK__(so->os == si->os);
			s8_ac_delete(so, uc.ui.ac);
		}
		s8_delete(si);
	}
	__END__

	__START__
	{
		RENEW(s9);
		{
			__CHK__(!si->as && !si->as_end);
			CREPACK(s9, 1);
			__CHK__(!so->as && !so->as_end);
			s9_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
		}
		{
			__CHK__(s9_new_as(si, 5));
			si->as[0] = 12345;
			si->as[1] = 21212;
			si->as[2] = 32321;
			si->as[3] = 11223;
			si->as[4] = 15556;
			CREPACK_S(s9, 11);
			__CHK__(bridge_array_count(so->as) == bridge_array_count(si->as));
			__CHK__(
				so->as[0] == si->as[0] &&
				so->as[1] == si->as[1] &&
				so->as[2] == si->as[2] &&
				so->as[3] == si->as[3] &&
				so->as[4] == si->as[4]
			);
			s9_ac_delete(so, uc.ui.ac);
		}
		s9_delete(si);
	}
	__END__

	__START__
	{
		RENEW(s10);
		{
			si->i = 1122334455;
			CREPACK(s10, 4);
			__CHK__(so->i == si->i);
			s10_ac_delete(so, uc.ui.ac);
		}
		s10_delete(si);
	}
	__END__

	__START__
	{
		RENEW(s11);
		{
			__CHK__(!si->has_oi);
			si->oi = 0x55442544;
			CREPACK(s11, 1);
			__CHK__(!so->has_oi);
			s11_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
		}
		{
			si->has_oi = 1;
			CREPACK_S(s11, 5);
			__CHK__(so->has_oi);
			__CHK__(so->oi == si->oi);
			s11_ac_delete(so, uc.ui.ac);
		}
		s11_delete(si);
	}
	__END__

	__START__
	{
		RENEW(s12);
		{
			__CHK__(!si->ai && !si->ai_end);
			CREPACK(s12, 1);
			__CHK__(!so->ai && !so->ai_end);
			s12_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
		}
		{
			unsigned q = 126;
			for (;;) {
				bridge_free(si->ai);
				__CHK__(s12_new_ai(si, q));
				{
					unsigned i = 0;
					for (; i < q; i++)
						si->ai[i] = (int)((1u << (i & (sizeof(int)*8 - 1u)))^i);
				}
				CREPACK_S(s12, bridge_array_size(si->ai) + (
					q ==   126 ? 1u :
					q ==   127 ? 1u :
					q ==   128 ? 2u :
					q ==   129 ? 2u :
					q == 16383 ? 2u :
					q == 16384 ? 3u : 0
				));
				__CHK__(bridge_array_count(so->ai) == bridge_array_count(si->ai));
				{
					unsigned i = 0;
					for (; i < q; i++) {
						if (so->ai[i] != si->ai[i])
							break;
					}
					__CHK__(i == q);
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
				s12_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
			}
			s12_ac_delete(so, uc.ui.ac);
		}
		s12_delete(si);
	}
	__END__

	__START__
	{
		RENEW(s13);
		{
			si->l = -1122334455453334442LL;
			CREPACK(s13, 8);
			__CHK__(so->l == si->l);
			s13_ac_delete(so, uc.ui.ac);
		}
		s13_delete(si);
	}
	__END__

	__START__
	{
		RENEW(s14);
		{
			__CHK__(!si->has_ol);
			si->ol = 24234234554425443LL;
			CREPACK(s14, 1);
			__CHK__(!so->has_ol);
			s14_ac_delete(so, uc.ui.ac);
		}
		{
			si->has_ol = 1;
			CREPACK(s14, 9);
			__CHK__(so->has_ol);
			__CHK__(so->ol == si->ol);
			__CHK__(so->ol == 24234234554425443LL);
			s14_ac_delete(so, uc.ui.ac);
		}
		s14_delete(si);
	}
	__END__

	__START__
	{
		RENEW(s15);
		{
			__CHK__(!si->al && !si->al_end);
			CREPACK(s15, 1);
			__CHK__(!so->al && !so->al_end);
			s15_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
		}
		{
			__CHK__(s15_new_al(si, 66));
			{
				unsigned i = 0;
				for (; i < 66; i++)
					si->al[i] = (INT64_TYPE)((1llu << (i & (sizeof(unsigned INT64_TYPE)*8 - 1u)))^i);
			}
			CREPACK_S(s15, 1 + bridge_array_size(si->al));
			__CHK__(bridge_array_count(so->al) == bridge_array_count(si->al));
			{
				unsigned i = 0;
				for (; i < 66; i++) {
					if (so->al[i] != si->al[i])
						break;
				}
				__CHK__(i == 66);
			}
			s15_ac_delete(so, uc.ui.ac);
		}
		s15_delete(si);
	}
	__END__

	__START__
	{
		RENEW(s16);
		{
			si->f = -1122.334f;
			CREPACK(s16, 4);
			__CHK__(so->f == si->f);
			s16_ac_delete(so, uc.ui.ac);
		}
		s16_delete(si);
	}
	__END__

	__START__
	{
		RENEW(s17);
		{
			__CHK__(!si->has_of);
			si->of = 2423.42345f;
			CREPACK(s17, 1);
			__CHK__(!so->has_of);
			s17_ac_delete(so, uc.ui.ac);
		}
		{
			si->has_of = 1;
			CREPACK(s17, 5);
			__CHK__(so->has_of);
			__CHK__(so->of == si->of);
			s17_ac_delete(so, uc.ui.ac);
		}
		s17_delete(si);
	}
	__END__

	__START__
	{
		RENEW(s18);
		{
			__CHK__(!si->af && !si->af_end);
			CREPACK(s18, 1);
			__CHK__(!so->af && !so->af_end);
			s18_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
		}
		{
			__CHK__(s18_new_af(si, 16383));
			{
				unsigned i = 0;
				for (; i < 16383; i++)
					si->af[i] = (float)((1u << (i & (sizeof(float)*8 - 1)))^i)/((float)i + 1.0f);
			}
			CREPACK_S(s18, 2 + bridge_array_size(si->af));
			__CHK__(bridge_array_count(so->af) == bridge_array_count(si->af));
			{
				unsigned i = 0;
				for (; i < 16383; i++) {
					if (so->af[i] != si->af[i])
						break;
				}
				__CHK__(i == 16383);
			}
			s18_ac_delete(so, uc.ui.ac);
		}
		s18_delete(si);
	}
	__END__

	__START__
	{
		RENEW(s19);
		{
			si->d = -1122345543.3343453455;
			CREPACK(s19, 8);
			__CHK__(so->d == si->d);
			s19_ac_delete(so, uc.ui.ac);
		}
		s19_delete(si);
	}
	__END__

	__START__
	{
		RENEW(s20);
		{
			__CHK__(!si->has_od);
			si->od = 2423234234112.79768956042345;
			CREPACK(s20, 1);
			__CHK__(!so->has_od);
			s20_ac_delete(so, uc.ui.ac);
		}
		{
			si->has_od = 1;
			CREPACK(s20, 9);
			__CHK__(so->has_od);
			__CHK__(so->od == si->od);
			s20_ac_delete(so, uc.ui.ac);
		}
		s20_delete(si);
	}
	__END__

	__START__
	{
		RENEW(s21);
		{
			__CHK__(!si->ad && !si->ad_end);
			CREPACK(s21, 1);
			__CHK__(!so->ad && !so->ad_end);
			s21_ac_delete(so, uc.ui.ac);
		}
		{
			__CHK__(s21_new_ad(si, 16384));
			{
				unsigned i = 0;
				for (; i < 16384; i++)
					si->ad[i] = (double)((1ull << (i & (sizeof(double)*8 - 1)))^i)/(i + 1.0);
			}
			CREPACK(s21, 3 + bridge_array_size(si->ad));
			__CHK__(bridge_array_count(so->ad) == bridge_array_count(si->ad));
			{
				unsigned i = 0;
				for (; i < 16384; i++) {
					if (so->ad[i] != si->ad[i])
						break;
				}
				__CHK__(i == 16384);
			}
			s21_ac_delete(so, uc.ui.ac);
		}
		s21_delete(si);
	}
	__END__

	__START__
	{
		const char test_str[] = "some Test_22$ string.";
		RENEW(s22);
		{
			__CHK__(!si->s);
			REPACK(s22, 1);
			__CHK__(so->s && !*so->s);
			s22_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
		}
		{
			si->s = bridge_ref_str_buf(test_str);
			__CHK__(si->s);
			REPACK_S(s22, 1 + sizeof(test_str));
			__CHK__(so->s && !strcmp(so->s, test_str) && !strcmp(so->s, si->s));
			s22_ac_delete(so, uc.ui.ac);
			bridge_free(si->s);
		}
		{
			si->s = bridge_ref_str_size(test_str, sizeof(test_str));
			__CHK__(si->s);
			REPACK(s22, 1 + sizeof(test_str));
			__CHK__(so->s && !strcmp(so->s, test_str) && !strcmp(so->s, si->s));
			s22_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
			bridge_free(si->s);
		}
		{
			si->s = bridge_ref_str_size(test_str, 0/*not computed*/);
			__CHK__(si->s);
			REPACK_S(s22, 1 + sizeof(test_str));
			__CHK__(so->s && !strcmp(so->s, test_str) && !strcmp(so->s, si->s));
			s22_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
			bridge_free(si->s);
		}
		{
			si->s = bridge_ref_str(test_str);
			__CHK__(si->s);
			REPACK_S(s22, 1 + sizeof(test_str));
			__CHK__(so->s && !strcmp(so->s, test_str) && !strcmp(so->s, si->s));
			s22_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
			bridge_free(si->s);
		}
		{
			si->s = bridge_copy_str_buf(test_str);
			__CHK__(si->s);
			REPACK_S(s22, 1 + sizeof(test_str));
			__CHK__(so->s && !strcmp(so->s, test_str) && !strcmp(so->s, si->s));
			s22_ac_delete(so, uc.ui.ac);
			bridge_free(si->s);
		}
		{
			si->s = bridge_copy_str_size(test_str, sizeof(test_str));
			__CHK__(si->s);
			REPACK(s22, 1 + sizeof(test_str));
			__CHK__(so->s && !strcmp(so->s, test_str) && !strcmp(so->s, si->s));
			s22_ac_delete(so, uc.ui.ac);
			bridge_free(si->s);
		}
		{
			si->s = bridge_copy_str_size(test_str, 0/*not computed*/);
			__CHK__(si->s);
			REPACK(s22, 1 + sizeof(test_str));
			__CHK__(so->s && !strcmp(so->s, test_str) && !strcmp(so->s, si->s));
			s22_ac_delete(so, uc.ui.ac);
			bridge_free(si->s);
		}
		{
			si->s = bridge_copy_str(test_str);
			__CHK__(si->s);
			REPACK(s22, 1 + sizeof(test_str));
			__CHK__(so->s && !strcmp(so->s, test_str) && !strcmp(so->s, si->s));
			s22_ac_delete(so, uc.ui.ac);
			bridge_free(si->s);
		}
		{
			si->s = bridge_copy_sub_str(test_str + 1, str_buf_length(test_str) - 2);
			__CHK__(si->s);
			REPACK(s22, 1 + sizeof(test_str) - 2);
			__CHK__(so->s && strlen(so->s) == str_buf_length(test_str) - 2);
			__CHK__(!memcmp(so->s, test_str + 1, str_buf_length(test_str) - 2));
			__CHK__(!strcmp(so->s, si->s));
			s22_ac_delete(so, uc.ui.ac);
		}
		s22_delete(si);
	}
	__END__

	__START__
	{
		const char test_str[] = "";
		RENEW(s22);
		{
			__CHK__(!si->s);
			REPACK(s22, 1);
			__CHK__(so->s && !*so->s);
			s22_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
		}
		{
			si->s = bridge_ref_str_buf(test_str);
			__CHK__(si->s);
			REPACK_S(s22, 1);
			__CHK__(so->s && !strcmp(so->s, test_str) && !strcmp(so->s, si->s));
			s22_ac_delete(so, uc.ui.ac);
			bridge_free(si->s);
		}
		{
			si->s = bridge_ref_str_size(test_str, sizeof(test_str));
			__CHK__(si->s);
			REPACK(s22, 1);
			__CHK__(so->s && !strcmp(so->s, test_str) && !strcmp(so->s, si->s));
			s22_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
			bridge_free(si->s);
		}
		{
			si->s = bridge_ref_str_size(test_str, 0/*not computed*/);
			__CHK__(si->s);
			REPACK_S(s22, 1);
			__CHK__(so->s && !strcmp(so->s, test_str) && !strcmp(so->s, si->s));
			s22_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
			bridge_free(si->s);
		}
		{
			si->s = bridge_ref_str(test_str);
			__CHK__(si->s);
			REPACK_S(s22, 1);
			__CHK__(so->s && !strcmp(so->s, test_str) && !strcmp(so->s, si->s));
			s22_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
			bridge_free(si->s);
		}
		{
			si->s = bridge_copy_str_buf(test_str);
			__CHK__(si->s);
			REPACK_S(s22, 1);
			__CHK__(so->s && !strcmp(so->s, test_str) && !strcmp(so->s, si->s));
			s22_ac_delete(so, uc.ui.ac);
			bridge_free(si->s);
		}
		{
			si->s = bridge_copy_str_size(test_str, sizeof(test_str));
			__CHK__(si->s);
			REPACK(s22, 1);
			__CHK__(so->s && !strcmp(so->s, test_str) && !strcmp(so->s, si->s));
			s22_ac_delete(so, uc.ui.ac);
			bridge_free(si->s);
		}
		{
			si->s = bridge_copy_str_size(test_str, 0/*not computed*/);
			__CHK__(si->s);
			REPACK(s22, 1);
			__CHK__(so->s && !strcmp(so->s, test_str) && !strcmp(so->s, si->s));
			s22_ac_delete(so, uc.ui.ac);
			bridge_free(si->s);
		}
		{
			si->s = bridge_copy_str(test_str);
			__CHK__(si->s);
			REPACK(s22, 1);
			__CHK__(so->s && !strcmp(so->s, test_str) && !strcmp(so->s, si->s));
			s22_ac_delete(so, uc.ui.ac);
			bridge_free(si->s);
		}
		{
			si->s = bridge_copy_sub_str(test_str + 1, 0);
			__CHK__(si->s);
			REPACK(s22, 1);
			__CHK__(so->s && strlen(so->s) == 0);
			__CHK__(!strcmp(so->s, si->s));
			s22_ac_delete(so, uc.ui.ac);
		}
		s22_delete(si);
	}
	__END__

	__START__
	{
		const char test_str[] = "some Test_23$ string.";
		RENEW(s23);
		{
			__CHK__(!si->os);
			REPACK(s23, 1);
			__CHK__(!so->os);
			s23_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
		}
		{
			si->os = bridge_ref_str_buf(test_str);
			__CHK__(si->os);
			REPACK_S(s23, 2 + sizeof(test_str));
			__CHK__(so->os && !strcmp(so->os, test_str) && !strcmp(so->os, si->os));
			s23_ac_delete(so, uc.ui.ac);
			bridge_free(si->os);
		}
		{
			si->os = bridge_ref_str_size(test_str, sizeof(test_str));
			__CHK__(si->os);
			REPACK(s23, 2 + sizeof(test_str));
			__CHK__(so->os && !strcmp(so->os, test_str) && !strcmp(so->os, si->os));
			s23_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
			bridge_free(si->os);
		}
		{
			si->os = bridge_ref_str_size(test_str, 0/*not computed*/);
			__CHK__(si->os);
			REPACK_S(s23, 2 + sizeof(test_str));
			__CHK__(so->os && !strcmp(so->os, test_str) && !strcmp(so->os, si->os));
			s23_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
			bridge_free(si->os);
		}
		{
			si->os = bridge_ref_str(test_str);
			__CHK__(si->os);
			REPACK_S(s23, 2 + sizeof(test_str));
			__CHK__(so->os && !strcmp(so->os, test_str) && !strcmp(so->os, si->os));
			s23_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
			bridge_free(si->os);
		}
		{
			si->os = bridge_copy_str_buf(test_str);
			__CHK__(si->os);
			REPACK_S(s23, 2 + sizeof(test_str));
			__CHK__(so->os && !strcmp(so->os, test_str) && !strcmp(so->os, si->os));
			s23_ac_delete(so, uc.ui.ac);
			bridge_free(si->os);
		}
		{
			si->os = bridge_copy_str_size(test_str, sizeof(test_str));
			__CHK__(si->os);
			REPACK(s23, 2 + sizeof(test_str));
			__CHK__(so->os && !strcmp(so->os, test_str) && !strcmp(so->os, si->os));
			s23_ac_delete(so, uc.ui.ac);
			bridge_free(si->os);
		}
		{
			si->os = bridge_copy_str_size(test_str, 0/*not computed*/);
			__CHK__(si->os);
			REPACK(s23, 2 + sizeof(test_str));
			__CHK__(so->os && !strcmp(so->os, test_str) && !strcmp(so->os, si->os));
			s23_ac_delete(so, uc.ui.ac);
			bridge_free(si->os);
		}
		{
			si->os = bridge_copy_str(test_str);
			__CHK__(si->os);
			REPACK(s23, 2 + sizeof(test_str));
			__CHK__(so->os && !strcmp(so->os, test_str) && !strcmp(so->os, si->os));
			s23_ac_delete(so, uc.ui.ac);
			bridge_free(si->os);
		}
		{
			si->os = bridge_copy_sub_str(test_str + 1, str_buf_length(test_str) - 2);
			__CHK__(si->os);
			REPACK(s23, 2 + sizeof(test_str) - 2);
			__CHK__(so->os && strlen(so->os) == str_buf_length(test_str) - 2);
			__CHK__(!memcmp(so->os, test_str + 1, str_buf_length(test_str) - 2));
			__CHK__(!strcmp(so->os, si->os));
			s23_ac_delete(so, uc.ui.ac);
		}
		s23_delete(si);
	}
	__END__

	__START__
	{
		const char test_str[] = "";
		RENEW(s23);
		{
			__CHK__(!si->os);
			REPACK(s23, 1);
			__CHK__(!so->os);
			s23_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
		}
		{
			si->os = bridge_ref_str_buf(test_str);
			__CHK__(si->os);
			REPACK_S(s23, 2);
			__CHK__(so->os && !strcmp(so->os, test_str) && !strcmp(so->os, si->os));
			s23_ac_delete(so, uc.ui.ac);
			bridge_free(si->os);
		}
		{
			si->os = bridge_ref_str_size(test_str, sizeof(test_str));
			__CHK__(si->os);
			REPACK(s23, 2);
			__CHK__(so->os && !strcmp(so->os, test_str) && !strcmp(so->os, si->os));
			s23_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
			bridge_free(si->os);
		}
		{
			si->os = bridge_ref_str_size(test_str, 0/*not computed*/);
			__CHK__(si->os);
			REPACK_S(s23, 2);
			__CHK__(so->os && !strcmp(so->os, test_str) && !strcmp(so->os, si->os));
			s23_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
			bridge_free(si->os);
		}
		{
			si->os = bridge_ref_str(test_str);
			__CHK__(si->os);
			REPACK_S(s23, 2);
			__CHK__(so->os && !strcmp(so->os, test_str) && !strcmp(so->os, si->os));
			s23_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
			bridge_free(si->os);
		}
		{
			si->os = bridge_copy_str_buf(test_str);
			__CHK__(si->os);
			REPACK_S(s23, 2);
			__CHK__(so->os && !strcmp(so->os, test_str) && !strcmp(so->os, si->os));
			s23_ac_delete(so, uc.ui.ac);
			bridge_free(si->os);
		}
		{
			si->os = bridge_copy_str_size(test_str, sizeof(test_str));
			__CHK__(si->os);
			REPACK(s23, 2);
			__CHK__(so->os && !strcmp(so->os, test_str) && !strcmp(so->os, si->os));
			s23_ac_delete(so, uc.ui.ac);
			bridge_free(si->os);
		}
		{
			si->os = bridge_copy_str_size(test_str, 0/*not computed*/);
			__CHK__(si->os);
			REPACK(s23, 2);
			__CHK__(so->os && !strcmp(so->os, test_str) && !strcmp(so->os, si->os));
			s23_ac_delete(so, uc.ui.ac);
			bridge_free(si->os);
		}
		{
			si->os = bridge_copy_str(test_str);
			__CHK__(si->os);
			REPACK(s23, 2);
			__CHK__(so->os && !strcmp(so->os, test_str) && !strcmp(so->os, si->os));
			s23_ac_delete(so, uc.ui.ac);
			bridge_free(si->os);
		}
		{
			si->os = bridge_copy_sub_str(test_str + 1, 0);
			__CHK__(si->os);
			REPACK(s23, 2);
			__CHK__(so->os && strlen(so->os) == 0);
			__CHK__(!strcmp(so->os, si->os));
			s23_ac_delete(so, uc.ui.ac);
		}
		s23_delete(si);
	}
	__END__

	__START__
	{
		char test_str[] = "some Test_24$ string.(  )";
		RENEW(s24);
		{
			__CHK__(!si->as && !si->as_end);
			REPACK(s24, 1);
			__CHK__(!so->as && !so->as_end);
			s24_ac_delete(so, uc.ui.ac);
		}
		{
			__CHK__(s24_new_as(si, 16));
			{
				unsigned i = 0;
				P1("\n");
				for (; i < 14; i++) {
					test_str[sizeof(test_str) - 3] = (char)('0' + i%10);
					if (i/10)
						test_str[sizeof(test_str) - 4] = (char)('0' + i/10);
					__CHK__(!si->as[i]);
					switch (i%5) {
						case 0: si->as[i] = bridge_copy_str_buf(test_str); break;
						case 1: si->as[i] = bridge_copy_str_size(test_str, sizeof(test_str)); break;
						case 2: si->as[i] = bridge_copy_str_size(test_str, 0/*not computed*/); break;
						case 3: si->as[i] = bridge_copy_str(test_str); break;
						case 4: si->as[i] = bridge_copy_sub_str(test_str, str_buf_length(test_str)); break;
					}
					__CHK__(si->as[i]);
					P2(" %s\n", si->as[i]);
				}
				si->as[i] = bridge_ref_str_buf("");
				__CHK__(si->as[i]);
			}
			REPACK(s24, 1 + 14*(1 + sizeof(test_str)) + 1/*""*/ + 1/*NULL*/);
			__CHK__(so->as && so->as_end);
			__CHK__(bridge_array_count(si->as) == bridge_array_count(so->as));
			{
				unsigned i = 0;
				for (; i < 14; i++) {
					if (strcmp(si->as[i], so->as[i]))
						break;
				}
				__CHK__(i == 14);
				__CHK__(so->as[14] && !*so->as[14]);
				__CHK__(so->as[15] && !*so->as[15]);
			}
			s24_ac_delete(so, uc.ui.ac);
		}
		s24_delete(si);
	}
	__END__

	__START__
	{
		RENEW(x1);
		{
			__CHK__(!si->rb);
			__CHK__(x1_new_rb(si));
			__CHK__(!bridge_get_bit(*si->rb, 0));
			__CHK__(!bridge_get_bit(*si->rb, 1));
			REPACK(x1, 1);
			__CHK__(so->rb);
			__CHK__(bridge_get_bit(*si->rb, 0) == bridge_get_bit(*so->rb, 0));
			__CHK__(bridge_get_bit(*si->rb, 1) == bridge_get_bit(*so->rb, 1));
			x1_ac_delete(so, uc.ui.ac);
		}
		{
			bridge_set_bit(*si->rb, 0, 1);
			bridge_set_bit(*si->rb, 1, 1);
			REPACK(x1, 1);
			__CHK__(so->rb);
			__CHK__(bridge_get_bit(*si->rb, 0) == bridge_get_bit(*so->rb, 0));
			__CHK__(bridge_get_bit(*si->rb, 1) == bridge_get_bit(*so->rb, 1));
			x1_ac_delete(so, uc.ui.ac);
		}
		x1_delete(si);
	}
	__END__

	__START__
	{
		RENEW(x2);
		{
			__CHK__(!si->rb);
			__CHK__(x2_new_rb(si));
			{
				unsigned i = 0;
				for (; i < x2_rb_bit_count; i++) {
					if (bridge_get_bit(*si->rb, i))
						break;
				}
				__CHK__(i == x2_rb_bit_count);
			}
			REPACK(x2, 25);
			__CHK__(so->rb);
			{
				unsigned i = 0;
				for (; i < x2_rb_bit_count; i++) {
					if (bridge_get_bit(*si->rb, i) != bridge_get_bit(*so->rb, i))
						break;
				}
				__CHK__(i == x2_rb_bit_count);
			}
			x2_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
		}
		{
			{
				unsigned i = 0;
				for (; i < x2_rb_bit_count; i++)
					bridge_set_bit(*si->rb, i, !!(i%5));
			}
			REPACK_S(x2, 25);
			__CHK__(so->rb);
			{
				unsigned i = 0;
				for (; i < x2_rb_bit_count; i++) {
					if (bridge_get_bit(*si->rb, i) != bridge_get_bit(*so->rb, i))
						break;
				}
				__CHK__(i == x2_rb_bit_count);
			}
			x2_ac_delete(so, uc.ui.ac);
		}
		x2_delete(si);
	}
	__END__

	__START__
	{
		RENEW(x3);
		{
			__CHK__(!si->rt);
			__CHK__(x3_new_rt(si));
			si->rt[0] = 77;
			REPACK(x3, 1);
			__CHK__(so->rt);
			__CHK__(si->rt[0] == so->rt[0]);
			x3_ac_delete(so, uc.ui.ac);
		}
		x3_delete(si);
	}
	__END__

	__START__
	{
		RENEW(x4);
		{
			__CHK__(!si->rt);
			__CHK__(x4_new_rt(si));
			(*si->rt)[0] = 77;
			(*si->rt)[1] = 78;
			REPACK(x4, 2);
			__CHK__(so->rt);
			__CHK__((*si->rt)[0] == (*so->rt)[0]);
			__CHK__((*si->rt)[1] == (*so->rt)[1]);
			x4_ac_delete(so, uc.ui.ac);
		}
		x4_delete(si);
	}
	__END__

	__START__
	{
		RENEW(x5);
		{
			__CHK__(!si->rt);
			__CHK__(x5_new_rt(si));
			si->rt[0] = 32767;
			CREPACK(x5, 2);
			__CHK__(so->rt);
			__CHK__(si->rt[0] == so->rt[0]);
			x5_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
		}
		{
			bridge_free(si->rt);
			__CHK__(x5_new_rt(si));
			si->rt[0] = -32768;
			CREPACK_S(x5, 2);
			__CHK__(so->rt);
			__CHK__(si->rt[0] == so->rt[0]);
			x5_ac_delete(so, uc.ui.ac);
		}
		x5_delete(si);
	}
	__END__

	__START__
	{
		RENEW(x6);
		{
			__CHK__(!si->rt);
			__CHK__(x6_new_rt(si));
			(*si->rt)[0] = 1;
			(*si->rt)[1] = -1;
			CREPACK(x6, 4);
			__CHK__(so->rt);
			__CHK__((*si->rt)[0] == (*so->rt)[0]);
			__CHK__((*si->rt)[1] == (*so->rt)[1]);
			x6_ac_delete(so, uc.ui.ac);
		}
		x6_delete(si);
	}
	__END__

	__START__
	{
		RENEW(x7);
		{
			__CHK__(!si->rt);
			__CHK__(x7_new_rt(si));
			*si->rt = -234;
			CREPACK(x7, 4);
			__CHK__(so->rt);
			__CHK__(*si->rt == *so->rt);
			x7_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
		}
		{
			bridge_free(si->rt);
			__CHK__(x7_new_rt(si));
			*si->rt = -233232768;
			CREPACK_S(x7, 4);
			__CHK__(so->rt);
			__CHK__(*si->rt == *so->rt);
			x7_ac_delete(so, uc.ui.ac);
		}
		x7_delete(si);
	}
	__END__

	__START__
	{
		RENEW(x8);
		{
			__CHK__(!si->rt);
			__CHK__(x8_new_rt(si));
			(*si->rt)[0] = 1234783245;
			(*si->rt)[1] = -7;
			CREPACK(x8, 8);
			__CHK__(so->rt);
			__CHK__((*si->rt)[0] == (*so->rt)[0]);
			__CHK__((*si->rt)[1] == (*so->rt)[1]);
			x8_ac_delete(so, uc.ui.ac);
		}
		x8_delete(si);
	}
	__END__

	__START__
	{
		RENEW(x9);
		{
			__CHK__(!si->rt);
			__CHK__(x9_new_rt(si));
			*si->rt = 0xffffccddeeffffll;
			CREPACK(x9, 8);
			__CHK__(so->rt);
			__CHK__(*si->rt == *so->rt);
			x9_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
		}
		{
			bridge_free(si->rt);
			__CHK__(x9_new_rt(si));
			*si->rt = -23247237433232768ll;
			CREPACK_S(x9, 8);
			__CHK__(so->rt);
			__CHK__(*si->rt == *so->rt);
			x9_ac_delete(so, uc.ui.ac);
		}
		x9_delete(si);
	}
	__END__

	__START__
	{
		RENEW(x10);
		{
			__CHK__(!si->rt);
			__CHK__(x10_new_rt(si));
			(*si->rt)[0] = 123871231234783245ll;
			(*si->rt)[1] = -7328742384234ll;
			CREPACK(x10, 16);
			__CHK__(so->rt);
			__CHK__((*si->rt)[0] == (*so->rt)[0]);
			__CHK__((*si->rt)[1] == (*so->rt)[1]);
			x10_ac_delete(so, uc.ui.ac);
		}
		x10_delete(si);
	}
	__END__

	__START__
	{
		RENEW(x11);
		{
			__CHK__(!si->rt);
			__CHK__(x11_new_rt(si));
			*si->rt = 2311934.34345f;
			CREPACK(x11, 4);
			__CHK__(so->rt);
			__CHK__(*si->rt == *so->rt);
			x11_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
		}
		{
			bridge_free(si->rt);
			__CHK__(x11_new_rt(si));
			*si->rt = -0.345112f;
			CREPACK_S(x11, 4);
			__CHK__(so->rt);
			__CHK__(*si->rt == *so->rt);
			x11_ac_delete(so, uc.ui.ac);
		}
		x11_delete(si);
	}
	__END__

	__START__
	{
		RENEW(x12);
		{
			__CHK__(!si->rt);
			__CHK__(x12_new_rt(si));
			(*si->rt)[0] = 23545.1312f;
			(*si->rt)[1] = -7328.44234f;
			CREPACK(x12, 8);
			__CHK__(so->rt);
			__CHK__((*si->rt)[0] == (*so->rt)[0]);
			__CHK__((*si->rt)[1] == (*so->rt)[1]);
			x12_ac_delete(so, uc.ui.ac);
		}
		x12_delete(si);
	}
	__END__

	__START__
	{
		RENEW(x13);
		{
			__CHK__(!si->rt);
			__CHK__(x13_new_rt(si));
			*si->rt = 23284823311934.343234224345;
			CREPACK(x13, 8);
			__CHK__(so->rt);
			__CHK__(*si->rt == *so->rt);
			x13_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
		}
		{
			bridge_free(si->rt);
			__CHK__(x13_new_rt(si));
			*si->rt = -0.345324235123112;
			CREPACK_S(x13, 8);
			__CHK__(so->rt);
			__CHK__(*si->rt == *so->rt);
			x13_ac_delete(so, uc.ui.ac);
		}
		x13_delete(si);
	}
	__END__

	__START__
	{
		RENEW(x14);
		{
			__CHK__(!si->rt);
			__CHK__(x14_new_rt(si));
			(*si->rt)[0] = 2300040545.131234123102;
			(*si->rt)[1] = -7323406620668.0000044234;
			CREPACK(x14, 16);
			__CHK__(so->rt);
			__CHK__((*si->rt)[0] == (*so->rt)[0]);
			__CHK__((*si->rt)[1] == (*so->rt)[1]);
			x14_ac_delete(so, uc.ui.ac);
		}
		x14_delete(si);
	}
	__END__

	__START__
	{
		RENEW(x15);
		{
			__CHK__(!si->rt);
			__CHK__(x15_new_rt(si));
			__CHK__(!*si->rt);
			REPACK(x15, 1);
			__CHK__(so->rt);
			__CHK__(*so->rt);
			__CHK__(!**so->rt);
			x15_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
		}
		{
			bridge_free(si->rt);
			__CHK__(x15_new_rt(si));
			*si->rt = bridge_ref_str_buf("");
			REPACK_S(x15, 1);
			__CHK__(so->rt);
			__CHK__(*so->rt);
			__CHK__(!**so->rt);
			x15_ac_delete(so, uc.ui.ac);
		}
		{
			bridge_free(*si->rt);
			*si->rt = bridge_ref_str_buf("aaaabb");
			REPACK(x15, 1 + sizeof("aaaabb"));
			__CHK__(so->rt);
			__CHK__(*so->rt);
			__CHK__(!strcmp(*so->rt, *si->rt));
			x15_ac_delete(so, uc.ui.ac);
		}
		x15_delete(si);
	}
	__END__

	__START__
	{
		RENEW(x16);
		{
			__CHK__(!si->rt);
			__CHK__(x16_new_rt(si));
			__CHK__(!(*si->rt)[0]);
			__CHK__(!(*si->rt)[1]);
			(*si->rt)[1] = bridge_ref_str_buf("");
			__CHK__((*si->rt)[1]);
			__CHK__(!*(*si->rt)[1]);
			REPACK(x16, 2);
			__CHK__(so->rt);
			__CHK__((*so->rt)[0]);
			__CHK__((*so->rt)[1]);
			__CHK__(!*(*so->rt)[0]);
			__CHK__(!*(*so->rt)[1]);
			x16_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
		}
		{
			const char teststr[] = "xxyynn";
			bridge_free(si->rt);
			__CHK__(x16_new_rt(si));
			__CHK__(!(*si->rt)[0]);
			__CHK__(!(*si->rt)[1]);
			(*si->rt)[0] = bridge_ref_str_buf(teststr);
			(*si->rt)[1] = bridge_ref_str_buf("xxy");
			__CHK__((*si->rt)[0]);
			__CHK__((*si->rt)[1]);
			REPACK_S(x16, 1 + sizeof(teststr) + 1 + sizeof("xxy"));
			__CHK__((*so->rt)[0]);
			__CHK__((*so->rt)[1]);
			__CHK__(!strcmp((*so->rt)[0], (*si->rt)[0]));
			__CHK__(!strcmp((*so->rt)[1], (*si->rt)[1]));
			x16_ac_delete(so, uc.ui.ac);
		}
		x16_delete(si);
	}
	__END__

	__START__
	{
		RENEW(ps7);
		{
			const INT16_TYPE arr[] = {0,1,2,62,63,64,65,126,127,128,129,8190,8191,8192,8193,8194,32766,32767,-32768};
			unsigned i = 0;
			for (; i < sizeof(arr)/sizeof(arr[0]); i++) {
				si->s = arr[i];
				if (si->s < 0)
					si->s = 0;
				{
					size_t pz = si->s < 64 ? 1u : si->s < 8192 ? 2u : 3u;
					REPACK(ps7, pz);
					__CHK__(si->s == so->s);
					ps7_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
				}
				si->s = arr[i];
				if (si->s > 0)
					si->s = (INT16_TYPE)-si->s;
				{
					size_t pz = si->s >= -64 ? 1u : si->s >= -8192 ? 2u : 3u;
					REPACK_S(ps7, pz);
					__CHK__(si->s == so->s);
					ps7_ac_delete(so, uc.ui.ac);
				}
			}
		}
		ps7_delete(si);
	}
	__END__

	__START__
	{
		RENEW(ps8);
		{
			__CHK__(!si->has_os);
			REPACK(ps8, 1);
			__CHK__(!so->has_os);
			ps8_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
		}
		{
			si->os = -32755;
			si->has_os = 1;
			REPACK_S(ps8, 4);
			__CHK__(so->has_os);
			__CHK__(si->os == so->os);
			ps8_ac_delete(so, uc.ui.ac);
		}
		ps8_delete(si);
	}
	__END__

	__START__
	{
		RENEW(ps9);
		{
			__CHK__(!si->as && !si->as_end);
			REPACK(ps9, 1);
			__CHK__(!so->as && !so->as_end);
			ps9_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
		}
		{
			__CHK__(ps9_new_as(si, 10000));
			{
				unsigned i = 0;
				for (; i < 10000; i++)
					si->as[i] = (INT16_TYPE)((1u << (i & (sizeof(INT16_TYPE)*8 - 1u)))^i);
			}
			REPACK_S(ps9, 23177);
			__CHK__(so->as && so->as_end);
			{
				unsigned i = 0;
				for (; i < 10000; i++) {
					if (si->as[i] != so->as[i])
						break;
				}
				__CHK__(i == 10000);
			}
			ps9_ac_delete(so, uc.ui.ac);
			bridge_free(si->as);
		}
		{
			__CHK__(ps9_new_as(si, 10000));
			{
				unsigned i = 0;
				for (; i < 10000; i++) {
					si->as[i] = (INT16_TYPE)((1u << (i & (sizeof(INT16_TYPE)*8 - 1u)))^i);
					if (si->as[i] >= 0)
						si->as[i] = (INT16_TYPE)(si->as[i] & 8191);
					else if ((INT16_TYPE)(1u << (sizeof(INT16_TYPE)*8 - 1u)) == si->as[i])
						si->as[i] = 0;
					else
						si->as[i] = (INT16_TYPE)-(-si->as[i] & 8191);
				}
			}
			REPACK(ps9, 19886);
			__CHK__(so->as && so->as_end);
			{
				unsigned i = 0;
				for (; i < 10000; i++) {
					if (si->as[i] != so->as[i])
						break;
				}
				__CHK__(i == 10000);
			}
			ps9_ac_delete(so, uc.ui.ac);
		}
		ps9_delete(si);
	}
	__END__

	__START__
	{
		RENEW(ps10);
		{
			const INT32_TYPE arr[] = {0,1,2,62,63,64,65,126,127,128,129,8190,8191,8192,8193,8194,32766,32767,32768,
				1048573,1048574,1048575,1048576,1048577,1048578,134217726,134217727,134217728,134217729,134217730,2147483647,-2147483647-1};
			unsigned i = 0;
			for (; i < sizeof(arr)/sizeof(arr[0]); i++) {
				si->i = arr[i];
				if (si->i < 0)
					si->i = 0;
				{
					size_t pz = si->i < 64 ? 1u : si->i < 8192 ? 2u : si->i < 1048576 ? 3u : si->i < 134217728 ? 4u : 5u;
					REPACK(ps10, pz);
					__CHK__(si->i == so->i);
					ps10_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
				}
				si->i = arr[i];
				if (si->i > 0)
					si->i = (INT32_TYPE)-si->i;
				{
					size_t pz = si->i >= -64 ? 1u : si->i >= -8192 ? 2u : si->i >= -1048576 ? 3u : si->i >= -134217728 ? 4u : 5u;
					REPACK_S(ps10, pz);
					__CHK__(si->i == so->i);
					ps10_ac_delete(so, uc.ui.ac);
				}
			}
		}
		ps10_delete(si);
	}
	__END__

	__START__
	{
		RENEW(ps11);
		{
			__CHK__(!si->has_oi);
			REPACK(ps11, 1);
			__CHK__(!so->has_oi);
			ps11_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
		}
		{
			si->oi = -32234755;
			si->has_oi = 1;
			REPACK_S(ps11, 5);
			__CHK__(so->has_oi);
			__CHK__(si->oi == so->oi);
			ps11_ac_delete(so, uc.ui.ac);
		}
		ps11_delete(si);
	}
	__END__

	__START__
	{
		RENEW(ps12);
		{
			__CHK__(!si->ai && !si->ai_end);
			REPACK(ps12, 1);
			__CHK__(!so->ai && !so->ai_end);
			ps12_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
		}
		{
			__CHK__(ps12_new_ai(si, 10000));
			{
				unsigned i = 0;
				for (; i < 10000; i++)
					si->ai[i] = (INT32_TYPE)((1u << (i & (sizeof(INT32_TYPE)*8 - 1u)))^i);
			}
			REPACK_S(ps12, 31893);
			__CHK__(so->ai && so->ai_end);
			{
				unsigned i = 0;
				for (; i < 10000; i++) {
					if (si->ai[i] != so->ai[i])
						break;
				}
				__CHK__(i == 10000);
			}
			ps12_ac_delete(so, uc.ui.ac);
			bridge_free(si->ai);
		}
		{
			__CHK__(ps12_new_ai(si, 10000));
			{
				unsigned i = 0;
				for (; i < 10000; i++) {
					si->ai[i] = (INT32_TYPE)((1ull << (i & (sizeof(INT32_TYPE)*8 - 1u)))^i);
					if (si->ai[i] >= 0)
						si->ai[i] = (INT32_TYPE)(si->ai[i] & 0x7FFFFFF);
					else if ((INT32_TYPE)(1ull << (sizeof(INT32_TYPE)*8 - 1u)) == si->ai[i])
						si->ai[i] = 0;
					else
						si->ai[i] = (INT32_TYPE)-(-si->ai[i] & 0x7FFFFFF);
				}
			}
			REPACK(ps12, 28053);
			__CHK__(so->ai && so->ai_end);
			{
				unsigned i = 0;
				for (; i < 10000; i++) {
					if (si->ai[i] != so->ai[i])
						break;
				}
				__CHK__(i == 10000);
			}
			ps12_ac_delete(so, uc.ui.ac);
		}
		ps12_delete(si);
	}
	__END__

	__START__
	{
		RENEW(ps13);
		{
			const INT64_TYPE arr[] = {0,1,2,62,63,64,65,126,127,128,129,8190,8191,8192,8193,8194,32766,32767,32768,
				1048573,1048574,1048575,1048576,1048577,1048578,134217726,134217727,134217728,134217729,134217730,2147483647,2147483648,
				17179869182ll,17179869183ll,17179869184ll,17179869185ll,17179869186ll,2199023255550ll,2199023255551ll,2199023255552ll,
				2199023255553ll,2199023255554ll,281474976710654ll,281474976710655ll,281474976710656ll,281474976710657ll,
				281474976710658ll,36028797018963966ll,36028797018963967ll,36028797018963968ll,36028797018963969ll,
				36028797018963970ll,4611686018427387902ll,4611686018427387903ll,4611686018427387904ll,4611686018427387905ll,
				9223372036854775806ll,9223372036854775807ll,-9223372036854775807ll-1};
			unsigned i = 0;
			for (; i < sizeof(arr)/sizeof(arr[0]); i++) {
				si->l = arr[i];
				if (si->l < 0)
					si->l = 0;
				{
					size_t pz = si->l < 64 ? 1u : si->l < 8192 ? 2u : si->l < 1048576 ? 3u : si->l < 134217728 ? 4u :
						si->l < 17179869184ll ? 5u : si->l < 2199023255552ll ? 6u : si->l < 281474976710656ll ? 7u :
						si->l < 36028797018963968ll ? 8u : si->l < 4611686018427387904ll ? 9u : 9u;
					REPACK(ps13, pz);
					__CHK__(si->l == so->l);
					ps13_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
				}
				si->l = arr[i];
				if (si->l > 0)
					si->l = (INT64_TYPE)-si->l;
				{
					size_t pz = si->l >= -64 ? 1u : si->l >= -8192 ? 2u : si->l >= -1048576 ? 3u : si->l >= -134217728 ? 4u :
						si->l >= -17179869184ll ? 5u : si->l >= -2199023255552ll ? 6u : si->l >= -281474976710656ll ? 7u :
						si->l >= -36028797018963968ll ? 8u : si->l >= -4611686018427387904ll ? 9u : 9u;
					REPACK_S(ps13, pz);
					__CHK__(si->l == so->l);
					ps13_ac_delete(so, uc.ui.ac);
				}
			}
		}
		ps13_delete(si);
	}
	__END__

	__START__
	{
		RENEW(ps14);
		{
			__CHK__(!si->has_ol);
			REPACK(ps14, 1);
			__CHK__(!so->has_ol);
			ps14_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
		}
		{
			si->ol = -7358435340032234755ll;
			si->has_ol = 1;
			REPACK_S(ps14, 10);
			__CHK__(so->has_ol);
			__CHK__(si->ol == so->ol);
			ps14_ac_delete(so, uc.ui.ac);
		}
		ps14_delete(si);
	}
	__END__

	__START__
	{
		RENEW(ps15);
		{
			__CHK__(!si->al && !si->al_end);
			REPACK(ps15, 1);
			__CHK__(!so->al && !so->al_end);
			ps15_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
		}
		{
			__CHK__(ps15_new_al(si, 10000));
			{
				unsigned i = 0;
				for (; i < 10000; i++)
					si->al[i] = (INT64_TYPE)((1ull << (i & (sizeof(INT64_TYPE)*8 - 1u)))^i);
			}
			REPACK_S(ps15, 53099);
			__CHK__(so->al && so->al_end);
			{
				unsigned i = 0;
				for (; i < 10000; i++) {
					if (si->al[i] != so->al[i])
						break;
				}
				__CHK__(i == 10000);
			}
			ps15_ac_delete(so, uc.ui.ac);
			bridge_free(si->al);
		}
		{
			__CHK__(ps15_new_al(si, 10000));
			{
				unsigned i = 0;
				for (; i < 10000; i++) {
					si->al[i] = (INT64_TYPE)((1ull << (i & (sizeof(INT64_TYPE)*8 - 1u)))^i);
					if (si->al[i] >= 0)
						si->al[i] = (INT64_TYPE)(si->al[i] & 0x3FFFFFFFFFFFFFll);
					else if ((INT64_TYPE)(1ull << (sizeof(INT64_TYPE)*8 - 1u)) == si->al[i])
						si->al[i] = 0;
					else
						si->al[i] = (INT64_TYPE)-(-si->al[i] & 0x3FFFFFFFFFFFFFll);
				}
			}
			REPACK(ps15, 43514);
			__CHK__(so->al && so->al_end);
			{
				unsigned i = 0;
				for (; i < 10000; i++) {
					if (si->al[i] != so->al[i])
						break;
				}
				__CHK__(i == 10000);
			}
			ps15_ac_delete(so, uc.ui.ac);
		}
		ps15_delete(si);
	}
	__END__

	__START__
	{
		const char test_str[] = "some Test_p22$ string.";
		RENEW(ps22);
		{
			__CHK__(!si->s);
			REPACK(ps22, 1);
			__CHK__(so->s && !*so->s);
			ps22_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
		}
		{
			si->s = bridge_ref_str_buf(test_str);
			__CHK__(si->s);
			REPACK_S(ps22, sizeof(test_str));
			__CHK__(so->s && !strcmp(so->s, test_str) && !strcmp(so->s, si->s));
			ps22_ac_delete(so, uc.ui.ac);
			bridge_free(si->s);
		}
		{
			si->s = bridge_ref_str_buf("");
			__CHK__(si->s);
			REPACK(ps22, 1);
			__CHK__(so->s && !*so->s);
			ps22_ac_delete(so, uc.ui.ac);
		}
		ps22_delete(si);
	}
	__END__

	__START__
	{
		const char test_str[] = "some Test_p23$ string.";
		RENEW(ps23);
		{
			__CHK__(!si->os);
			REPACK(ps23, 1);
			__CHK__(!so->os);
			ps23_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
		}
		{
			si->os = bridge_copy_str_buf(test_str);
			__CHK__(si->os);
			REPACK_S(ps23, 1 + sizeof(test_str));
			__CHK__(so->os && !strcmp(so->os, test_str) && !strcmp(so->os, si->os));
			ps23_ac_delete(so, uc.ui.ac);
			bridge_free(si->os);
		}
		{
			si->os = bridge_copy_str("");
			__CHK__(si->os);
			REPACK(ps23, 2);
			__CHK__(so->os && !*so->os);
			ps23_ac_delete(so, uc.ui.ac);
		}
		ps23_delete(si);
	}
	__END__

	__START__
	{
		char test_str[] = "some Test_p24$ string.(  )";
		RENEW(ps24);
		{
			__CHK__(!si->as && !si->as_end);
			REPACK(ps24, 1);
			__CHK__(!so->as && !so->as_end);
			ps24_ac_delete(so, uc.ui.ac);
		}
		{
			__CHK__(ps24_new_as(si, 16));
			{
				unsigned i = 0;
				P1("\n");
				for (; i < 14; i++) {
					test_str[sizeof(test_str) - 3] = (char)('0' + i%10);
					if (i/10)
						test_str[sizeof(test_str) - 4] = (char)('0' + i/10);
					__CHK__(!si->as[i]);
					switch (i%5) {
						case 0: si->as[i] = bridge_copy_str_buf(test_str); break;
						case 1: si->as[i] = bridge_copy_str_size(test_str, sizeof(test_str)); break;
						case 2: si->as[i] = bridge_copy_str_size(test_str, 0/*not computed*/); break;
						case 3: si->as[i] = bridge_copy_str(test_str); break;
						case 4: si->as[i] = bridge_copy_sub_str(test_str, str_buf_length(test_str)); break;
					}
					__CHK__(si->as[i]);
					P2(" %s\n", si->as[i]);
				}
				si->as[i] = bridge_ref_str_buf("");
				__CHK__(si->as[i]);
			}
			REPACK(ps24, 1 + 14*sizeof(test_str) + 1/*""*/ + 1/*NULL*/);
			__CHK__(so->as && so->as_end);
			__CHK__(bridge_array_count(si->as) == bridge_array_count(so->as));
			{
				unsigned i = 0;
				for (; i < 14; i++) {
					if (strcmp(si->as[i], so->as[i]))
						break;
				}
				__CHK__(i == 14);
				__CHK__(so->as[14] && !*so->as[14]);
				__CHK__(so->as[15] && !*so->as[15]);
			}
			ps24_ac_delete(so, uc.ui.ac);
		}
		ps24_delete(si);
	}
	__END__

	__START__
	{
		RENEW(px5);
		{
			__CHK__(!si->rt);
			__CHK__(px5_new_rt(si));
			si->rt[0] = 127;
			REPACK(px5, 2);
			__CHK__(so->rt);
			__CHK__(si->rt[0] == so->rt[0]);
			px5_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
		}
		{
			bridge_free(si->rt);
			__CHK__(px5_new_rt(si));
			si->rt[0] = -32;
			REPACK_S(px5, 1);
			__CHK__(so->rt);
			__CHK__(si->rt[0] == so->rt[0]);
			px5_ac_delete(so, uc.ui.ac);
		}
		px5_delete(si);
	}
	__END__

	__START__
	{
		RENEW(px6);
		{
			__CHK__(!si->rt);
			__CHK__(px6_new_rt(si));
			(*si->rt)[0] = 2000;
			(*si->rt)[1] = -64;
			REPACK(px6, 3);
			__CHK__(so->rt);
			__CHK__((*si->rt)[0] == (*so->rt)[0]);
			__CHK__((*si->rt)[1] == (*so->rt)[1]);
			px6_ac_delete(so, uc.ui.ac);
		}
		px6_delete(si);
	}
	__END__

	__START__
	{
		RENEW(px7);
		{
			__CHK__(!si->rt);
			__CHK__(px7_new_rt(si));
			*si->rt = -837584;
			REPACK(px7, 3);
			__CHK__(so->rt);
			__CHK__(*si->rt == *so->rt);
			px7_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
		}
		{
			bridge_free(si->rt);
			__CHK__(px7_new_rt(si));
			*si->rt = -23232768;
			REPACK_S(px7, 4);
			__CHK__(so->rt);
			__CHK__(*si->rt == *so->rt);
			px7_ac_delete(so, uc.ui.ac);
		}
		px7_delete(si);
	}
	__END__

	__START__
	{
		RENEW(px8);
		{
			__CHK__(!si->rt);
			__CHK__(px8_new_rt(si));
			(*si->rt)[0] = 1234783245;
			(*si->rt)[1] = -7;
			REPACK(px8, 6);
			__CHK__(so->rt);
			__CHK__((*si->rt)[0] == (*so->rt)[0]);
			__CHK__((*si->rt)[1] == (*so->rt)[1]);
			px8_ac_delete(so, uc.ui.ac);
		}
		px8_delete(si);
	}
	__END__

	__START__
	{
		RENEW(px9);
		{
			__CHK__(!si->rt);
			__CHK__(px9_new_rt(si));
			*si->rt = 0xffffceffffll;
			REPACK(px9, 6);
			__CHK__(so->rt);
			__CHK__(*si->rt == *so->rt);
			px9_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
		}
		{
			bridge_free(si->rt);
			__CHK__(px9_new_rt(si));
			*si->rt = -23247237433232768ll;
			REPACK_S(px9, 8);
			__CHK__(so->rt);
			__CHK__(*si->rt == *so->rt);
			px9_ac_delete(so, uc.ui.ac);
		}
		px9_delete(si);
	}
	__END__

	__START__
	{
		RENEW(px10);
		{
			__CHK__(!si->rt);
			__CHK__(px10_new_rt(si));
			(*si->rt)[0] = 123871231234783245ll;
			(*si->rt)[1] = -7328742384234ll;
			REPACK(px10, 16);
			__CHK__(so->rt);
			__CHK__((*si->rt)[0] == (*so->rt)[0]);
			__CHK__((*si->rt)[1] == (*so->rt)[1]);
			px10_ac_delete(so, uc.ui.ac);
		}
		px10_delete(si);
	}
	__END__

	__START__
	{
		RENEW(px15);
		{
			__CHK__(!si->rt);
			__CHK__(px15_new_rt(si));
			__CHK__(!*si->rt);
			REPACK(px15, 1);
			__CHK__(so->rt);
			__CHK__(*so->rt);
			__CHK__(!**so->rt);
			px15_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
		}
		{
			bridge_free(si->rt);
			__CHK__(px15_new_rt(si));
			*si->rt = bridge_ref_str_buf("");
			REPACK_S(px15, 1);
			__CHK__(so->rt);
			__CHK__(*so->rt);
			__CHK__(!**so->rt);
			px15_ac_delete(so, uc.ui.ac);
		}
		{
			bridge_free(*si->rt);
			*si->rt = bridge_ref_str("aaaabb");
			REPACK(px15, sizeof("aaaabb"));
			__CHK__(so->rt);
			__CHK__(*so->rt);
			__CHK__(!strcmp(*so->rt, *si->rt));
			px15_ac_delete(so, uc.ui.ac);
		}
		px15_delete(si);
	}
	__END__

	__START__
	{
		RENEW(px16);
		{
			__CHK__(!si->rt);
			__CHK__(px16_new_rt(si));
			__CHK__(!(*si->rt)[0]);
			__CHK__(!(*si->rt)[1]);
			(*si->rt)[1] = bridge_ref_str_buf("");
			__CHK__((*si->rt)[1]);
			__CHK__(!*(*si->rt)[1]);
			REPACK(px16, 2);
			__CHK__(so->rt);
			__CHK__((*so->rt)[0]);
			__CHK__((*so->rt)[1]);
			__CHK__(!*(*so->rt)[0]);
			__CHK__(!*(*so->rt)[1]);
			px16_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
		}
		{
			const char teststr[] = "xxyynn";
			bridge_free(si->rt);
			__CHK__(px16_new_rt(si));
			__CHK__(!(*si->rt)[0]);
			__CHK__(!(*si->rt)[1]);
			(*si->rt)[0] = bridge_ref_str_buf(teststr);
			(*si->rt)[1] = bridge_ref_str_buf("xxy");
			__CHK__((*si->rt)[0]);
			__CHK__((*si->rt)[1]);
			REPACK_S(px16, sizeof(teststr) + sizeof("xxy"));
			__CHK__((*so->rt)[0]);
			__CHK__((*so->rt)[1]);
			__CHK__(!strcmp((*so->rt)[0], (*si->rt)[0]));
			__CHK__(!strcmp((*so->rt)[1], (*si->rt)[1]));
			px16_ac_delete(so, uc.ui.ac);
		}
		px16_delete(si);
	}
	__END__

	__START__
	{
		RENEW(pps7);
		{
			const INT16_TYPE arr[] = {0,1,2,62,63,64,65,126,127,128,129,8190,8191,8192,8193,8194,16383,16484,16385,32766,32767,-32768};
			unsigned i = 0;
			for (; i < sizeof(arr)/sizeof(arr[0]); i++) {
				si->s = arr[i];
				if (si->s < 0)
					si->s = 0;
				{
					size_t pz = si->s < 128 ? 1u : si->s < 16384 ? 2u : 3u;
					REPACK(pps7, pz);
					__CHK__(si->s == so->s);
					pps7_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
				}
				si->s = arr[i];
				if (si->s > 0)
					si->s = (INT16_TYPE)-si->s;
				{
					size_t pz = si->s ? 3u : 1u;
					REPACK_S(pps7, pz);
					__CHK__(si->s == so->s);
					pps7_ac_delete(so, uc.ui.ac);
				}
			}
		}
		pps7_delete(si);
	}
	__END__

	__START__
	{
		RENEW(pps8);
		{
			__CHK__(!si->has_os);
			REPACK(pps8, 1);
			__CHK__(!so->has_os);
			pps8_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
		}
		{
			si->os = -3;
			si->has_os = 1;
			REPACK_S(pps8, 4);
			__CHK__(so->has_os);
			__CHK__(si->os == so->os);
			pps8_ac_delete(so, uc.ui.ac);
		}
		pps8_delete(si);
	}
	__END__

	__START__
	{
		RENEW(pps9);
		{
			__CHK__(!si->as && !si->as_end);
			REPACK(pps9, 1);
			__CHK__(!so->as && !so->as_end);
			pps9_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
		}
		{
			__CHK__(pps9_new_as(si, 10000));
			{
				unsigned i = 0;
				for (; i < 10000; i++)
					si->as[i] = (INT16_TYPE)((1u << (i & (sizeof(INT16_TYPE)*8 - 1u)))^i);
			}
			REPACK_S(pps9, 21140);
			__CHK__(so->as && so->as_end);
			{
				unsigned i = 0;
				for (; i < 10000; i++) {
					if (si->as[i] != so->as[i])
						break;
				}
				__CHK__(i == 10000);
			}
			pps9_ac_delete(so, uc.ui.ac);
			bridge_free(si->as);
		}
		{
			__CHK__(pps9_new_as(si, 10000));
			{
				unsigned i = 0;
				for (; i < 10000; i++) {
					si->as[i] = (INT16_TYPE)((1u << (i & (sizeof(INT16_TYPE)*8 - 1u)))^i);
					if (si->as[i] >= 0)
						si->as[i] = (INT16_TYPE)(si->as[i] & 8191);
					else if ((INT16_TYPE)(1u << (sizeof(INT16_TYPE)*8 - 1u)) == si->as[i])
						si->as[i] = 0;
					else
						si->as[i] = (INT16_TYPE)-(-si->as[i] & 8191);
				}
			}
			REPACK(pps9, 20403);
			__CHK__(so->as && so->as_end);
			{
				unsigned i = 0;
				for (; i < 10000; i++) {
					if (si->as[i] != so->as[i])
						break;
				}
				__CHK__(i == 10000);
			}
			pps9_ac_delete(so, uc.ui.ac);
		}
		pps9_delete(si);
	}
	__END__

	__START__
	{
		RENEW(pps10);
		{
			const INT32_TYPE arr[] = {0,1,2,62,63,64,65,126,127,128,129,8190,8191,8192,8193,8194,16383,16484,16385,32766,32767,32768,
				1048573,1048574,1048575,1048576,1048577,1048578,2097150,2097151,2097152,2097153,2097154,
				134217726,134217727,134217728,134217729,134217730,268435454,268435455,268435456,268435457,268435458,
				2147483647,-2147483647-1};
			unsigned i = 0;
			for (; i < sizeof(arr)/sizeof(arr[0]); i++) {
				si->i = arr[i];
				if (si->i < 0)
					si->i = 0;
				{
					size_t pz = si->i < 128 ? 1u : si->i < 16384 ? 2u : si->i < 2097152 ? 3u : si->i < 268435456 ? 4u : 5u;
					REPACK(pps10, pz);
					__CHK__(si->i == so->i);
					pps10_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
				}
				si->i = arr[i];
				if (si->i > 0)
					si->i = (INT32_TYPE)-si->i;
				{
					size_t pz = si->i ? 5u : 1u;
					REPACK_S(pps10, pz);
					__CHK__(si->i == so->i);
					pps10_ac_delete(so, uc.ui.ac);
				}
			}
		}
		pps10_delete(si);
	}
	__END__

	__START__
	{
		RENEW(pps11);
		{
			__CHK__(!si->has_oi);
			REPACK(pps11, 1);
			__CHK__(!so->has_oi);
			pps11_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
		}
		{
			si->oi = 234755;
			si->has_oi = 1;
			REPACK_S(pps11, 4);
			__CHK__(so->has_oi);
			__CHK__(si->oi == so->oi);
			pps11_ac_delete(so, uc.ui.ac);
		}
		pps11_delete(si);
	}
	__END__

	__START__
	{
		RENEW(pps12);
		{
			__CHK__(!si->ai && !si->ai_end);
			REPACK(pps12, 1);
			__CHK__(!so->ai && !so->ai_end);
			pps12_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
		}
		{
			__CHK__(pps12_new_ai(si, 10000));
			{
				unsigned i = 0;
				for (; i < 10000; i++)
					si->ai[i] = (INT32_TYPE)((1u << (i & (sizeof(INT32_TYPE)*8 - 1u)))^i);
			}
			REPACK_S(pps12, 30244);
			__CHK__(so->ai && so->ai_end);
			{
				unsigned i = 0;
				for (; i < 10000; i++) {
					if (si->ai[i] != so->ai[i])
						break;
				}
				__CHK__(i == 10000);
			}
			pps12_ac_delete(so, uc.ui.ac);
			bridge_free(si->ai);
		}
		{
			__CHK__(pps12_new_ai(si, 10000));
			{
				unsigned i = 0;
				for (; i < 10000; i++) {
					si->ai[i] = (INT32_TYPE)((1ull << (i & (sizeof(INT32_TYPE)*8 - 1u)))^i);
					if (si->ai[i] >= 0)
						si->ai[i] = (INT32_TYPE)(si->ai[i] & 0x7FFFFFF);
					else if ((INT32_TYPE)(1ull << (sizeof(INT32_TYPE)*8 - 1u)) == si->ai[i])
						si->ai[i] = 0;
					else
						si->ai[i] = (INT32_TYPE)-(-si->ai[i] & 0x7FFFFFF);
				}
			}
			REPACK(pps12, 26796);
			__CHK__(so->ai && so->ai_end);
			{
				unsigned i = 0;
				for (; i < 10000; i++) {
					if (si->ai[i] != so->ai[i])
						break;
				}
				__CHK__(i == 10000);
			}
			pps12_ac_delete(so, uc.ui.ac);
		}
		pps12_delete(si);
	}
	__END__

	__START__
	{
		RENEW(pps13);
		{
			const INT64_TYPE arr[] = {0,1,2,62,63,64,65,126,127,128,129,8190,8191,8192,8193,8194,16383,16484,16385,32766,32767,32768,
				1048573,1048574,1048575,1048576,1048577,1048578,2097150,2097151,2097152,2097153,2097154,
				134217726,134217727,134217728,134217729,134217730,268435454,268435455,268435456,268435457,268435458,
				2147483647,2147483648,17179869182ll,17179869183ll,17179869184ll,17179869185ll,17179869186ll,
				34359738366ll,34359738367ll,34359738368ll,34359738369ll,34359738370ll,2199023255550ll,2199023255551ll,2199023255552ll,
				2199023255553ll,2199023255554ll,4398046511103ll,4398046511104ll,4398046511105ll,281474976710654ll,281474976710655ll,
				281474976710656ll,281474976710657ll,281474976710658ll,562949953421310ll,562949953421311ll,562949953421312ll,
				562949953421313ll,562949953421314ll,281474976710658ll,36028797018963966ll,36028797018963967ll,36028797018963968ll,
				36028797018963969ll,36028797018963970ll,72057594037927934ll,72057594037927935ll,72057594037927936ll,72057594037927937ll,
				72057594037927938ll,4611686018427387902ll,4611686018427387903ll,4611686018427387904ll,4611686018427387905ll,
				9223372036854775806ll,9223372036854775807ll,-9223372036854775807ll-1};
			unsigned i = 0;
			for (; i < sizeof(arr)/sizeof(arr[0]); i++) {
				si->l = arr[i];
				if (si->l < 0)
					si->l = 0;
				{
					size_t pz = si->l < 128 ? 1u : si->l < 16384 ? 2u : si->l < 2097152 ? 3u : si->l < 268435456 ? 4u :
						si->l < 34359738368ll ? 5u : si->l < 4398046511104ll ? 6u : si->l < 562949953421312ll ? 7u :
						si->l < 72057594037927936ll ? 8u : si->l <= 9223372036854775807ll ? 9u : 9u;
					REPACK(pps13, pz);
					__CHK__(si->l == so->l);
					pps13_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
				}
				si->l = arr[i];
				if (si->l > 0)
					si->l = (INT64_TYPE)-si->l;
				{
					size_t pz = si->l ? 9u : 1u;
					REPACK_S(pps13, pz);
					__CHK__(si->l == so->l);
					pps13_ac_delete(so, uc.ui.ac);
				}
			}
		}
		pps13_delete(si);
	}
	__END__

	__START__
	{
		RENEW(pps14);
		{
			__CHK__(!si->has_ol);
			REPACK(pps14, 1);
			__CHK__(!so->has_ol);
			pps14_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
		}
		{
			si->ol = 63584350032234755ll;
			si->has_ol = 1;
			REPACK_S(pps14, 9);
			__CHK__(so->has_ol);
			__CHK__(si->ol == so->ol);
			pps14_ac_delete(so, uc.ui.ac);
		}
		pps14_delete(si);
	}
	__END__

	__START__
	{
		RENEW(pps15);
		{
			__CHK__(!si->al && !si->al_end);
			REPACK(pps15, 1);
			__CHK__(!so->al && !so->al_end);
			pps15_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
		}
		{
			__CHK__(pps15_new_al(si, 10000));
			{
				unsigned i = 0;
				for (; i < 10000; i++)
					si->al[i] = (INT64_TYPE)((1ull << (i & (sizeof(INT64_TYPE)*8 - 1u)))^i);
			}
			REPACK_S(pps15, 51644);
			__CHK__(so->al && so->al_end);
			{
				unsigned i = 0;
				for (; i < 10000; i++) {
					if (si->al[i] != so->al[i])
						break;
				}
				__CHK__(i == 10000);
			}
			pps15_ac_delete(so, uc.ui.ac);
			bridge_free(si->al);
		}
		{
			__CHK__(pps15_new_al(si, 10000));
			{
				unsigned i = 0;
				for (; i < 10000; i++) {
					si->al[i] = (INT64_TYPE)((1ull << (i & (sizeof(INT64_TYPE)*8 - 1u)))^i);
					if (si->al[i] >= 0)
						si->al[i] = (INT64_TYPE)(si->al[i] & 0x3FFFFFFFFFFFFFll);
					else if ((INT64_TYPE)(1ull << (sizeof(INT64_TYPE)*8 - 1u)) == si->al[i])
						si->al[i] = 0;
					else
						si->al[i] = (INT64_TYPE)-(-si->al[i] & 0x3FFFFFFFFFFFFFll);
				}
			}
			REPACK(pps15, 42110);
			__CHK__(so->al && so->al_end);
			{
				unsigned i = 0;
				for (; i < 10000; i++) {
					if (si->al[i] != so->al[i])
						break;
				}
				__CHK__(i == 10000);
			}
			pps15_ac_delete(so, uc.ui.ac);
		}
		pps15_delete(si);
	}
	__END__

	__START__
	{
		RENEW(ppx5);
		{
			__CHK__(!si->rt);
			__CHK__(ppx5_new_rt(si));
			si->rt[0] = 127;
			REPACK(ppx5, 1);
			__CHK__(so->rt);
			__CHK__(si->rt[0] == so->rt[0]);
			ppx5_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
		}
		{
			bridge_free(si->rt);
			__CHK__(ppx5_new_rt(si));
			si->rt[0] = -32;
			REPACK_S(ppx5, 3);
			__CHK__(so->rt);
			__CHK__(si->rt[0] == so->rt[0]);
			ppx5_ac_delete(so, uc.ui.ac);
		}
		ppx5_delete(si);
	}
	__END__

	__START__
	{
		RENEW(ppx6);
		{
			__CHK__(!si->rt);
			__CHK__(ppx6_new_rt(si));
			(*si->rt)[0] = 2000;
			(*si->rt)[1] = -64;
			REPACK(ppx6, 5);
			__CHK__(so->rt);
			__CHK__((*si->rt)[0] == (*so->rt)[0]);
			__CHK__((*si->rt)[1] == (*so->rt)[1]);
			ppx6_ac_delete(so, uc.ui.ac);
		}
		ppx6_delete(si);
	}
	__END__

	__START__
	{
		RENEW(ppx7);
		{
			__CHK__(!si->rt);
			__CHK__(ppx7_new_rt(si));
			*si->rt = 837584;
			REPACK(ppx7, 3);
			__CHK__(so->rt);
			__CHK__(*si->rt == *so->rt);
			ppx7_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
		}
		{
			bridge_free(si->rt);
			__CHK__(ppx7_new_rt(si));
			*si->rt = -23232768;
			REPACK_S(ppx7, 5);
			__CHK__(so->rt);
			__CHK__(*si->rt == *so->rt);
			ppx7_ac_delete(so, uc.ui.ac);
		}
		ppx7_delete(si);
	}
	__END__

	__START__
	{
		RENEW(ppx8);
		{
			__CHK__(!si->rt);
			__CHK__(ppx8_new_rt(si));
			(*si->rt)[0] = 1234783245;
			(*si->rt)[1] = -7;
			REPACK(ppx8, 10);
			__CHK__(so->rt);
			__CHK__((*si->rt)[0] == (*so->rt)[0]);
			__CHK__((*si->rt)[1] == (*so->rt)[1]);
			ppx8_ac_delete(so, uc.ui.ac);
		}
		ppx8_delete(si);
	}
	__END__

	__START__
	{
		RENEW(ppx9);
		{
			__CHK__(!si->rt);
			__CHK__(ppx9_new_rt(si));
			*si->rt = 0xffffceffffll;
			REPACK(ppx9, 6);
			__CHK__(so->rt);
			__CHK__(*si->rt == *so->rt);
			ppx9_ac_destroy(so, uc.ui.ac); /* don't free so - will reuse it below */
		}
		{
			bridge_free(si->rt);
			__CHK__(ppx9_new_rt(si));
			*si->rt = -23247237433232768ll;
			REPACK_S(ppx9, 9);
			__CHK__(so->rt);
			__CHK__(*si->rt == *so->rt);
			ppx9_ac_delete(so, uc.ui.ac);
		}
		ppx9_delete(si);
	}
	__END__

	__START__
	{
		RENEW(ppx10);
		{
			__CHK__(!si->rt);
			__CHK__(ppx10_new_rt(si));
			(*si->rt)[0] = 123871231234783245ll;
			(*si->rt)[1] = -7328742384234ll;
			REPACK(ppx10, 18);
			__CHK__(so->rt);
			__CHK__((*si->rt)[0] == (*so->rt)[0]);
			__CHK__((*si->rt)[1] == (*so->rt)[1]);
			ppx10_ac_delete(so, uc.ui.ac);
		}
		ppx10_delete(si);
	}
	__END__

	__START__
	{
		RENEW(s25);
		{
			__CHK__(!si->has_oi);
			__CHK__(!si->s);
			CREPACK(s25, 2);
			__CHK__(!so->has_oi);
			__CHK__(so->s && !*so->s);
			s25_ac_delete(so, uc.ui.ac);
			memstack_cleanup(sost);
		}
		{
			si->s = bridge_ref_str("a");
			si->b = 1;
			__CHK__(si->s);
			CREPACK(s25, 1 + 1 + sizeof("a"));
			__CHK__(!so->has_oi);
			__CHK__(si->b == so->b);
			__CHK__(so->s && !strcmp(si->s, so->s));
			s25_ac_delete(so, uc.ui.ac);
			memstack_cleanup(sost);
		}
		{
			si->has_oi = 1;
			si->oi = -213123245;
			CREPACK(s25, 8);
			__CHK__(si->b == so->b);
			__CHK__(so->has_oi);
			__CHK__(si->oi == so->oi);
			__CHK__(so->s && !strcmp(si->s, so->s));
			s25_ac_delete(so, uc.ui.ac);
			memstack_cleanup(sost);
		}
		{
			si->has_oi = 0;
			CREPACK(s25, 4);
			__CHK__(si->b == so->b);
			__CHK__(!so->has_oi);
			__CHK__(so->s && !strcmp(si->s, so->s));
			s25_ac_delete(so, uc.ui.ac);
			memstack_cleanup(sost);
		}
		s25_delete(si);
	}
	__END__

	__START__
	{
		RENEW(s26);
		{
			__CHK__(!si->has_od);
			__CHK__(!si->aa && !si->aa_end);
			si->b = 121;
			CREPACK(s26, 4);
			__CHK__(!so->has_od);
			__CHK__(si->b == so->b);
			__CHK__(!so->aa && !so->aa_end);
			s26_ac_delete(so, uc.ui.ac);
			memstack_cleanup(sost);
		}
		{
			si->od = 1223601.321;
			si->has_od = 1;
			CREPACK(s26, 12);
			__CHK__(so->has_od);
			__CHK__(si->b == so->b);
			__CHK__(si->od == so->od);
			__CHK__(!so->aa && !so->aa_end);
			s26_ac_delete(so, uc.ui.ac);
			memstack_cleanup(sost);
		}
		{
			__CHK__(s26_new_aa(si, 1));
			__CHK__(!si->aa[0].ab);
			__CHK__(!si->aa[0].ab_bit_count);
			__CHK__(!si->aa[0].s.s);
			__CHK__(!si->aa[0].s2.aa);
			__CHK__(!si->aa[0].os3);
			__CHK__(!si->aa[0].as4 && !si->aa[0].as4_end);
			CREPACK(s26, 17);
			__CHK__(so->has_od);
			__CHK__(si->b == so->b);
			__CHK__(si->od == so->od);
			__CHK__(so->aa && so->aa_end);
			__CHK__(bridge_array_count(so->aa) == 1);
			__CHK__(!so->aa[0].ab);
			__CHK__(!so->aa[0].ab_bit_count);
			__CHK__(so->aa[0].s.s && !*so->aa[0].s.s);
			__CHK__(!so->aa[0].s2.aa && !so->aa[0].s2.aa_end);
			__CHK__(!so->aa[0].os3);
			__CHK__(!so->aa[0].as4 && !so->aa[0].as4_end);
			s26_ac_delete(so, uc.ui.ac);
			memstack_cleanup(sost);
		}
		{
			__CHK__(s27_new_ab(&si->aa[0], 6));
			__CHK__(!bridge_get_bit(si->aa[0].ab, 0));
			__CHK__(!bridge_get_bit(si->aa[0].ab, 1));
			__CHK__(!bridge_get_bit(si->aa[0].ab, 2));
			__CHK__(!bridge_get_bit(si->aa[0].ab, 3));
			__CHK__(!bridge_get_bit(si->aa[0].ab, 4));
			__CHK__(!bridge_get_bit(si->aa[0].ab, 5));
			bridge_set_bit(si->aa[0].ab, 2, 1);
			bridge_set_bit(si->aa[0].ab, 3, 1);
			bridge_set_bit(si->aa[0].ab, 5, 1);
			CREPACK(s26, 18);
			__CHK__(so->has_od);
			__CHK__(si->b == so->b);
			__CHK__(si->od == so->od);
			__CHK__(so->aa && so->aa_end);
			__CHK__(bridge_array_count(so->aa) == 1);
			__CHK__(so->aa[0].ab);
			__CHK__(so->aa[0].ab_bit_count == 6);
			__CHK__(so->aa[0].s.s && !*so->aa[0].s.s);
			__CHK__(!so->aa[0].s2.aa && !so->aa[0].s2.aa_end);
			__CHK__(!so->aa[0].os3);
			__CHK__(!so->aa[0].as4 && !so->aa[0].as4_end);
			__CHK__(!bridge_get_bit(so->aa[0].ab, 0));
			__CHK__(!bridge_get_bit(so->aa[0].ab, 1));
			__CHK__(bridge_get_bit(so->aa[0].ab, 2));
			__CHK__(bridge_get_bit(so->aa[0].ab, 3));
			__CHK__(!bridge_get_bit(so->aa[0].ab, 4));
			__CHK__(bridge_get_bit(so->aa[0].ab, 5));
			s26_ac_delete(so, uc.ui.ac);
			memstack_cleanup(sost);
		}
		s26_delete(si);
	}
	__END__

	__START__
	{
		RENEW(s30);
		{
			si->i = 243454;
			si->l = -1022010155451104042ll;
			CREPACK(s30, 12);
			__CHK__(so->i == si->i);
			__CHK__(so->l == si->l);
			s30_ac_delete(so, uc.ui.ac);
		}
		s30_delete(si);
	}
	__END__

	__START__
	{
		RENEW(s31);
		{
			__CHK__(!si->aa && !si->aa_end);
			si->b = 24;
			__CHK__(s31_new_rc(si));
			si->rc->i = 243454;
			si->rc->l = -1022010155451104042ll;
			__CHK__(s31_new_rd(si));
			(*si->rd)[0].i = 243455;
			(*si->rd)[0].l = -1022010155451104043ll;
			(*si->rd)[1].i = 243456;
			(*si->rd)[1].l = -1022010155451104044ll;
			CREPACK(s31, 38);
			__CHK__(!so->aa && !so->aa_end);
			__CHK__(so->b == si->b);
			__CHK__(so->rc->i == si->rc->i);
			__CHK__(so->rc->l == si->rc->l);
			__CHK__((*so->rd)[0].i == (*si->rd)[0].i);
			__CHK__((*so->rd)[0].l == (*si->rd)[0].l);
			__CHK__((*so->rd)[1].i == (*si->rd)[1].i);
			__CHK__((*so->rd)[1].l == (*si->rd)[1].l);
			s31_ac_delete(so, uc.ui.ac);
		}
		{
			__CHK__(s31_new_aa(si, 2));
			si->b = 22;
			si->aa[0].i = 112233500;
			si->aa[0].l = 1234567890451104042ll;
			si->aa[1].i = 445566778;
			si->aa[1].l = 1234567847365987432ll;
			CREPACK(s31, 62);
			__CHK__(bridge_array_count(so->aa) == bridge_array_count(si->aa));
			__CHK__(so->aa[0].i == si->aa[0].i);
			__CHK__(so->aa[0].l == si->aa[0].l);
			__CHK__(so->aa[1].i == si->aa[1].i);
			__CHK__(so->aa[1].l == si->aa[1].l);
			__CHK__(so->b == si->b);
			__CHK__(so->rc->i == si->rc->i);
			__CHK__(so->rc->l == si->rc->l);
			__CHK__((*so->rd)[0].i == (*si->rd)[0].i);
			__CHK__((*so->rd)[0].l == (*si->rd)[0].l);
			__CHK__((*so->rd)[1].i == (*si->rd)[1].i);
			__CHK__((*so->rd)[1].l == (*si->rd)[1].l);
			s31_ac_delete(so, uc.ui.ac);
		}
		s31_delete(si);
	}
	__END__

	__START__
	{
		RENEW(s35);
		{
			__CHK__(!si->aa1 && !si->aa1_end);
			__CHK__(!si->aa2 && !si->aa2_end);
			__CHK__(!si->aa3 && !si->aa3_end);
			si->f = -545.334f;
			CREPACK(s35, 7);
			__CHK__(so->f == si->f);
			__CHK__(!so->aa1 && !so->aa1_end);
			__CHK__(!so->aa2 && !so->aa2_end);
			__CHK__(!so->aa3 && !so->aa3_end);
			s35_ac_delete(so, uc.ui.ac);
		}
		{
			__CHK__(s35_new_aa1(si, 2));
			__CHK__(s35_new_aa2(si, 1));
			__CHK__(s35_new_aa3(si, 3));
			si->f = 22.3442f;
			si->aa1[0].c = 45;
			si->aa1[1].c = 53;
			si->aa2[0].u = 4545452;
			si->aa3[0].u1 = 4657432;
			si->aa3[0].u2 = -876843;
			si->aa3[0].ss1.u = -87684;
			si->aa3[0].u3 = -8768;
			si->aa3[1].u1 = 67894352;
			si->aa3[1].u2 = 5465452;
			si->aa3[1].ss1.u = -8764;
			si->aa3[1].u3 = -878;
			si->aa3[2].u1 = -68584352;
			si->aa3[2].u2 = 3635622;
			si->aa3[2].ss1.u = -864;
			si->aa3[2].u3 = -88;
			CREPACK(s35, 61);
			__CHK__(so->f == si->f);
			__CHK__(bridge_array_count(so->aa1) == bridge_array_count(si->aa1));
			__CHK__(bridge_array_count(so->aa2) == bridge_array_count(si->aa2));
			__CHK__(bridge_array_count(so->aa3) == bridge_array_count(si->aa3));
			__CHK__(so->aa1[0].c == si->aa1[0].c);
			__CHK__(so->aa1[1].c == si->aa1[1].c);
			__CHK__(so->aa2[0].u == si->aa2[0].u);
			__CHK__(so->aa3[0].u1 == si->aa3[0].u1);
			__CHK__(so->aa3[0].u2 == si->aa3[0].u2);
			__CHK__(so->aa3[1].u1 == si->aa3[1].u1);
			__CHK__(so->aa3[1].u2 == si->aa3[1].u2);
			__CHK__(so->aa3[2].u1 == si->aa3[2].u1);
			__CHK__(so->aa3[2].u2 == si->aa3[2].u2);
			s35_ac_delete(so, uc.ui.ac);
		}
		s35_delete(si);
	}
	__END__

	__START__
	{
		RENEW(string_list);
		{
			__CHK__(!si->onext);
			si->s = bridge_ref_str("aa");
			__CHK__(si->s);
			si->onext = string_list_new();
			__CHK__(si->onext);
			__CHK__(!si->onext->onext);
			si->onext->s = bridge_ref_str("bb");
			__CHK__(si->onext->s);
			si->onext->onext = string_list_new();
			__CHK__(si->onext->onext);
			__CHK__(!si->onext->onext->onext);
			si->onext->onext->s = bridge_ref_str("cc");
			__CHK__(si->onext->onext->s);
			REPACK(string_list, 14);
			__CHK__(so->s && !strcmp(so->s, "aa"));
			__CHK__(so->onext);
			__CHK__(so->onext->s && !strcmp(so->onext->s, "bb"));
			__CHK__(so->onext->onext);
			__CHK__(so->onext->onext->s && !strcmp(so->onext->onext->s, "cc"));
			__CHK__(!so->onext->onext->onext);
			string_list_ac_delete(so, uc.ui.ac);
		}
		{
			char str_buf[] = "aabbcc0d";
			unsigned i = 0;
			struct string_list **tail = &si->onext->onext->onext;
			for (; i < 100; i++) {
				str_buf[sizeof(str_buf) - 2] = (char)('0' + i%10);
				if (i/10)
					str_buf[sizeof(str_buf) - 3] = (char)('0' + i/10);
				*tail = string_list_new();
				if (!*tail)
					break;
				(*tail)->s = bridge_copy_str_buf(str_buf);
				if (!(*tail)->s)
					break;
				if ((*tail)->onext)
					break;
				tail = &(*tail)->onext;
			}
			__CHK__(i == 100);
		}
		REPACK(string_list, 1026);
		{
			unsigned c = 0;
			const struct string_list *i = si;
			const struct string_list *o = so;
			for (; i; c++) {
				if (!o || !o->s)
					break;
				if (strcmp(i->s, o->s))
					break;
				o = o->onext;
				i = i->onext;
			}
			__CHK__(!i == !o);
			__CHK__(c == 3 + 100);
			string_list_ac_delete(so, uc.ui.ac);
		}
		string_list_delete(si);
	}
	__END__

	__START__
	{
		RENEW(sTree);
		{
			__CHK__(!si->oleft);
			__CHK__(!si->oright);
			si->name = bridge_ref_str("node1");
			__CHK__(si->name);
			si->value = 1;
			CREPACK(sTree, 13);
			__CHK__(!so->oleft);
			__CHK__(!so->oright);
			__CHK__(so->name && !strcmp(so->name, si->name));
			__CHK__(so->value == si->value);
			sTree_ac_delete(so, uc.ui.ac);
		}
		sTree_delete(si);
	}
	__END__

	__START__
	{
		RENEW(sTree);
		{
			__CHK__(!si->oleft);
			__CHK__(!si->oright);
			si->name = bridge_ref_str("root20");
			__CHK__(si->name);
			si->value = 20;
		}
		{
			si->oleft = sTree_new();
			__CHK__(si->oleft);
			__CHK__(!si->oleft->oleft);
			__CHK__(!si->oleft->oright);
			si->oleft->name = bridge_ref_str("left10");
			__CHK__(si->oleft->name);
			si->oleft->value = 10;
		}
		{
			si->oright = sTree_new();
			__CHK__(si->oright);
			__CHK__(!si->oright->oleft);
			__CHK__(!si->oright->oright);
			si->oright->name = bridge_ref_str("right30");
			__CHK__(si->oright->name);
			si->oright->value = 30;
		}
		CREPACK(sTree, 39);
		{
			__CHK__(so->oleft);
			__CHK__(so->oright);
			__CHK__(so->name && !strcmp(so->name, si->name));
			__CHK__(so->value == si->value);
			__CHK__(so->value == 20);
		}
		{
			__CHK__(!so->oleft->oleft);
			__CHK__(!so->oleft->oright);
			__CHK__(so->oleft->name && !strcmp(so->oleft->name, si->oleft->name));
			__CHK__(so->oleft->value == si->oleft->value);
			__CHK__(so->oleft->value == 10);
		}
		{
			__CHK__(!so->oright->oleft);
			__CHK__(!so->oright->oright);
			__CHK__(so->oright->name && !strcmp(so->oright->name, si->oright->name));
			__CHK__(so->oright->value == si->oright->value);
			__CHK__(so->oright->value == 30);
		}
		sTree_ac_delete(so, uc.ui.ac);
		{
			si->oleft->oleft = sTree_new();
			__CHK__(si->oleft->oleft);
			__CHK__(!si->oleft->oleft->oleft);
			__CHK__(!si->oleft->oleft->oright);
			si->oleft->oleft->name = bridge_ref_str("leftleft5");
			__CHK__(si->oleft->oleft->name);
			si->oleft->oleft->value = 5;
		}
		{
			si->oleft->oright = sTree_new();
			__CHK__(si->oleft->oright);
			__CHK__(!si->oleft->oright->oleft);
			__CHK__(!si->oleft->oright->oright);
			si->oleft->oright->name = bridge_ref_str("leftright15");
			__CHK__(si->oleft->oright->name);
			si->oleft->oright->value = 15;
		}
		{
			si->oright->oleft = sTree_new();
			__CHK__(si->oright->oleft);
			__CHK__(!si->oright->oleft->oleft);
			__CHK__(!si->oright->oleft->oright);
			si->oright->oleft->name = bridge_ref_str("rightleft25");
			__CHK__(si->oright->oleft->name);
			si->oright->oleft->value = 25;
		}
		{
			si->oright->oright = sTree_new();
			__CHK__(si->oright->oright);
			__CHK__(!si->oright->oright->oleft);
			__CHK__(!si->oright->oright->oright);
			si->oright->oright->name = bridge_ref_str("rightright35");
			__CHK__(si->oright->oright->name);
			si->oright->oright->value = 35;
		}
		CREPACK(sTree, 107);
		{
			__CHK__(so->oleft);
			__CHK__(so->oright);
			__CHK__(so->name && !strcmp(so->name, si->name));
			__CHK__(so->value == si->value);
			__CHK__(so->value == 20);
		}
		{
			__CHK__(so->oleft->oleft);
			__CHK__(so->oleft->oright);
			__CHK__(so->oleft->name && !strcmp(so->oleft->name, si->oleft->name));
			__CHK__(so->oleft->value == si->oleft->value);
			__CHK__(so->oleft->value == 10);
		}
		{
			__CHK__(so->oright->oleft);
			__CHK__(so->oright->oright);
			__CHK__(so->oright->name && !strcmp(so->oright->name, si->oright->name));
			__CHK__(so->oright->value == si->oright->value);
			__CHK__(so->oright->value == 30);
		}
		{
			__CHK__(!so->oleft->oleft->oleft);
			__CHK__(!so->oleft->oleft->oright);
			__CHK__(so->oleft->oleft->name && !strcmp(so->oleft->oleft->name, si->oleft->oleft->name));
			__CHK__(so->oleft->oleft->value == si->oleft->oleft->value);
			__CHK__(so->oleft->oleft->value == 5);
		}
		{
			__CHK__(!so->oleft->oright->oleft);
			__CHK__(!so->oleft->oright->oright);
			__CHK__(so->oleft->oright->name && !strcmp(so->oleft->oright->name, si->oleft->oright->name));
			__CHK__(so->oleft->oright->value == si->oleft->oright->value);
			__CHK__(so->oleft->oright->value == 15);
		}
		{
			__CHK__(!so->oright->oleft->oleft);
			__CHK__(!so->oright->oleft->oright);
			__CHK__(so->oright->oleft->name && !strcmp(so->oright->oleft->name, si->oright->oleft->name));
			__CHK__(so->oright->oleft->value == si->oright->oleft->value);
			__CHK__(so->oright->oleft->value == 25);
		}
		{
			__CHK__(!so->oright->oright->oleft);
			__CHK__(!so->oright->oright->oright);
			__CHK__(so->oright->oright->name && !strcmp(so->oright->oright->name, si->oright->oright->name));
			__CHK__(so->oright->oright->value == si->oright->oright->value);
			__CHK__(so->oright->oright->value == 35);
		}
		sTree_ac_delete(so, uc.ui.ac);
		sTree_delete(si);
	}
	__END__

	return 0;

fail:
	return -1;
}
