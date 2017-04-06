/********************************************************************************
* bitbridge - another serialization/rpc mechanism
* Copyright (C) 2008-2017 Michael M. Builov, https://github.com/mbuilov/bitbridge
* Licensed under GPL version 3 or any later version, see COPYING
********************************************************************************/

/* example3.java */

/* example 3:
 - serialize and then deserialize list of simple structures

 1) fill test persons list
 2) serialize it
 3) deserialize it
*/

import static bitbridge.Bridge.BridgeException;
import static bitbridge.Bridge.BridgeIterator;
import static ex3.gen_example3.person;
import static ex3.gen_example3.hobby;

final class example3 {

	static byte[] serialize(person persons) throws BridgeException {
		return persons.pack(/*head:*/0, /*tail:*/0, /*big:*/false);
	}

	static person deserialize(BridgeIterator it, byte[] mem) throws BridgeException {
		return person.unpack(it, mem, mem.length, /*big:*/false);
	}

	static person create_persons_list(int count) {
		person head = null;
		person tail = null;
		for (int n = 0; n < count; n++) {
			person p = new person();
			if (null != tail)
				tail.set_onext(p);
			else
				head = p;
			tail = p;
			p.set_id(n);
			p.set_name("person " + n);
			if (0 == (n % 3)) {
				hobby h = p.new_ohobby();
				switch (n % 5) {
					case 0: h.set_name("football"); break;
					case 1: h.set_name("volleyball"); break;
					case 2: h.set_name("basketball"); break;
					case 3: h.set_name("handball"); break;
					case 4: h.set_name("swimming"); break;
				}
			}
			p.set_male((n & 1) != 0);
			{
				int[] birth_month_year = p.new_rbirth_month_year();
				birth_month_year[0] = (n*3) % 12;
				birth_month_year[1] = (n*2) % 9999;
			}
		}
		return head;
	}

	static boolean compare_persons_lists(person p1, person p2) {
		for (;;) {
			if ((null == p1) != (null == p2)) {
				System.err.print("wrong number of persons in deserialized list!");
				return false;
			}
			if (null == p1)
				break;
			if (p1.get_id() != p2.get_id()) {
				System.err.print("wrong deserialized person id!");
				return false;
			}
			if (!p1.get_name().equals(p2.get_name())) {
				System.err.print("wrong deserialized person name!");
				return false;
			}
			if ((null != p1.get_ohobby()) != (null != p2.get_ohobby())) {
				System.err.print("wrong person hobby in deserialized list");
				return false;
			}
			if (p1.get_ohobby() != null && !p1.get_ohobby().get_name().equals(p2.get_ohobby().get_name())) {
				System.err.print("wrong hobby name in deserialized list");
				return false;
			}
			if (p1.get_male() != p2.get_male()) {
				System.err.print("wrong male bit in deserialized list");
				return false;
			}
			if (p1.get_rbirth_month_year()[0] != p2.get_rbirth_month_year()[0]) {
				System.err.print("wrong birth month in deserialized list");
				return false;
			}
			if (p1.get_rbirth_month_year()[1] != p2.get_rbirth_month_year()[1]) {
				System.err.print("wrong birth year in deserialized list");
				return false;
			}
			p1 = p1.get_onext();
			p2 = p2.get_onext();
		}
		return true; /* ok */
	}

	static void print_persons(person persons) {
		while (null != persons) {
			System.out.print("\nperson:");
			System.out.print("\n\tid = " + persons.get_id());
			System.out.print("\n\tname = " + persons.get_name());
			if (persons.get_ohobby() != null)
				System.out.print("\n\thobby = " + persons.get_ohobby().get_name());
			System.out.print("\n\tgender = " + (persons.get_male() ? "male" : "female"));
			System.out.print("\n\tbirth month = " + persons.get_rbirth_month_year()[0]);
			System.out.print("\n\tbirth year = " + persons.get_rbirth_month_year()[1]);
			persons = persons.get_onext();
		}
	}

	public static void main(String[] args) {
		boolean err = true;
		{
			person persons = create_persons_list(100000);
			{
				byte[] mem = null;
				try {
					mem = serialize(persons);
				}
				catch (BridgeException ex) {
					System.err.print("failed to serialize, error: " + ex);
				}
				if (mem != null) {
					person persons2 = null;
					BridgeIterator it = new BridgeIterator(0);
					try {
						persons2 = deserialize(it, mem);
					}
					catch (BridgeException ex) {
						System.err.print("failed to deserialize, error: " + ex);
					}
					if (persons2 != null) {
						if (mem.length != it.at)
							System.err.print("wrong packed size!");
						else if (compare_persons_lists(persons, persons2)) {
							if (args.length < 2)
								print_persons(persons2);
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
