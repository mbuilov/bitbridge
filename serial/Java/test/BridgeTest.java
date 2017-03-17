/********************************************************************************
* bitbridge - another serialization/rpc mechanism
* Copyright (C) 2008-2017 Michael M. Builov, https://github.com/mbuilov/bitbridge
* Licensed under GPL version 3 or any later version, see COPYING
********************************************************************************/

/* BridgeTest.java */

public final class BridgeTest {

	static final int LOOP_COUNT = 1;

	static int test1(int loop_count) {
		int err = 0; /* ok */
		for (int i = 0; i < loop_count; i++) {
			try {
				new TestFn1().test_fn1(/*big:*/true);
			}
			catch (Exception ex) {
				System.out.println("\n!error: " + ex.getMessage());
				err = 1; /* failed */
				break;
			}
			try {
				new TestFn1().test_fn1(/*big:*/false);
			}
			catch (Exception ex) {
				err = 2; /* failed */
				break;
			}
		}
		return err;
	}

	public static void main(String[] args) {
		int err = test1(LOOP_COUNT);
		if (err != 0)
			System.err.println("\n!FAILED TEST: " + err);
		else
			System.out.println("ALL TESTS PASSED");
	}
}
