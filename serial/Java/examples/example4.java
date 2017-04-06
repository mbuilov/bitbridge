/********************************************************************************
* bitbridge - another serialization/rpc mechanism
* Copyright (C) 2008-2017 Michael M. Builov, https://github.com/mbuilov/bitbridge
* Licensed under GPL version 3 or any later version, see COPYING
********************************************************************************/

/* example4.java */

/* example 4:
 - serialize and then deserialize tree of structures

 1) fill test persons tree
 2) serialize it
 3) deserialize it
*/

import static bitbridge.Bridge.BridgeException;
import static bitbridge.Bridge.BridgeIterator;
import static ex4.gen_example4.tree;
import static ex4.gen_example4.person;
import static ex4.gen_example4.car;

final class example4 {

	static byte[] serialize(tree t) throws BridgeException {
		return t.pack(/*head:*/0, /*tail:*/0, /*big:*/false);
	}

	static tree deserialize(BridgeIterator it, byte[] mem) throws BridgeException {
		return tree.unpack(it, mem, mem.length, /*big:*/false);
	}

	static car create_car(int k) {
		return new car().
			set_name(
				0 == (k & 3) ? "porsche" :
				1 == (k & 3) ? "mitsubishi" :
				2 == (k & 3) ? "linfan" :
				"lada"
			).set_rmodel(
				0 == (k % 3) ? "truck" :
				1 == (k % 3) ? "hatchback" :
				"cabriolet"
			).set_diesel(0 == (k & 1)).
				set_turbo(0 == (k & 3));
	}

	static person create_person(int id) {
		person p = new person();
		p.set_id(id);
		p.set_name("person " + id);
		{
			int n_cars = (int)(((long)id & 0xFFFFFFFFl) % 11);
			if (n_cars != 0) {
				car[] cars = p.new_acars(n_cars);
				for (int i = 0; i < cars.length; i++)
					cars[i] = create_car(id + i);
			}
		}
		p.set_male(0 != (id & 1));
		p.set_rbirth_month_year(
			(short)((id*3) % 12),
			(short)((id*2) % 9999)
		);
		return p;
	}

	static tree create_tree(int id, int count) {
		tree root = new tree();
		tree t = root;
		for (;;) {
			t.set_rp(create_person(id++));
			if (0 == --count)
				return root;
			if (count > 1) {
				int left = count >> 1;
				t.set_oleft(create_tree(id, left));
				id += left;
				count -= left;
			}
			t = t.new_oright();
		}
	}

	static boolean compare_cars(car c1, car c2) {
		if (!c1.get_name().equals(c2.get_name())) {
			System.err.print("wrong deserialized car name!");
			return false;
		}
		if (!c1.rmodel_get().equals(c2.rmodel_get())) {
			System.err.print("wrong deserialized model name!");
			return false;
		}
		if (c1.get_diesel() != c2.get_diesel()) {
			System.err.print("wrong deserialized diesel property!");
			return false;
		}
		if (c1.get_turbo() != c2.get_turbo()) {
			System.err.print("wrong deserialized turbo property!");
			return false;
		}
		return true;
	}

	static boolean compare_persons(person p1, person p2) {
		if (p1.get_id() != p2.get_id()) {
			System.err.print("wrong deserialized person id!");
			return false;
		}
		if (!p1.get_name().equals(p2.get_name())) {
			System.err.print("wrong deserialized person name!");
			return false;
		}
		// note: p1.get_acars() may return null if acars array was not allocated in create_person()
		// note: p2.get_acars() never returns null after deserialization
		if ((null == p1.get_acars() ? 0 : p1.get_acars().length) != p2.get_acars().length) {
			System.err.print("wrong number of cars!");
			return false;
		}
		if (null != p1.get_acars()) {
			for (int i = 0, n = p1.get_acars().length; i < n; i++) {
				if (!compare_cars(p1.acars_get(i), p2.acars_get(i)))
					return false;
			}
		}
		if (p1.get_male() != p2.get_male()) {
			System.err.print("wrong deserialized male property()!");
			return false;
		}
		if (p1.rbirth_month_year_get(0) != p2.rbirth_month_year_get(0)) {
			System.err.print("wrong deserialized birth month!");
			return false;
		}
		if (p1.rbirth_month_year_get(1) != p2.rbirth_month_year_get(1)) {
			System.err.print("wrong deserialized birth year!");
			return false;
		}
		return true;
	}

	static boolean compare_trees(tree t1, tree t2) {
		for (;;) {
			if ((null == t1) != (null == t2)) {
				System.err.print("wrong number of persons in deserialized tree!");
				return false;
			}
			if (null == t1)
				return true;
			if (!compare_persons(t1.get_rp(), t2.get_rp()))
				return false;
			if (!compare_trees(t1.get_oleft(), t2.get_oleft()))
				return false;
			t1 = t1.get_oright();
			t2 = t2.get_oright();
		}
	}

	static void print_car(car c) {
		System.out.print("\n\tcar:");
		System.out.print("\n\t\tname = " + c.get_name());
		System.out.print("\n\t\tmodel = " + c.rmodel_get());
		System.out.print("\n\t\tdiesel = " + (c.get_diesel() ? "true" : "false"));
		System.out.print("\n\t\tturbo = " + (c.get_turbo() ? "true" : "false"));
	}

	static void print_person(person p) {
		System.out.print("\nperson:");
		System.out.print("\n\tid = " + p.get_id());
		System.out.print("\n\tname = " + p.get_name());
		{
			for (car c : p.get_acars())
				print_car(c);
		}
		System.out.print("\n\tgender = " + (p.get_male() ? "male" : "female"));
		System.out.print("\n\tbirth month = " + p.rbirth_month_year_get(0));
		System.out.print("\n\tbirth year = " + p.rbirth_month_year_get(1));
	}

	static void print_tree(tree t) {
		while (null != t) {
			print_person(t.get_rp());
			print_tree(t.get_oleft());
			t = t.get_oright();
		}
	}

	public static void main(String[] args) {
		boolean err = true;
		{
			tree t = create_tree(0, 10000);
			{
				byte[] mem = null;
				try {
					mem = serialize(t);
				}
				catch (BridgeException ex) {
					System.err.print("failed to serialize, error: " + ex);
				}
				if (mem != null) {
					tree t2 = null;
					BridgeIterator it = new BridgeIterator(0);
					try {
						t2 = deserialize(it, mem);
					}
					catch (BridgeException ex) {
						System.err.print("failed to deserialize, error: " + ex);
					}
					if (t2 != null) {
						if (mem.length != it.at)
							System.err.print("wrong packed size!");
						else if (compare_trees(t, t2)) {
							if (args.length < 2)
								print_tree(t2);
							err = false; /* ok */
						}
					}
				}
			}
		}
		if (err)
			System.err.print("\nfailed\n");
		else if (args.length < 3)
			System.out.print("\nok\n");
	}
}
