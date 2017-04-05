/********************************************************************************
* bitbridge - another serialization/rpc mechanism
* Copyright (C) 2008-2017 Michael M. Builov, https://github.com/mbuilov/bitbridge
* Licensed under GPL version 3 or any later version, see COPYING
********************************************************************************/

/* example2.java */

/* example 2:
 - serialize and then deserialize list of simple structures

 1) fill test persons list
 2) serialize it
 3) deserialize it
*/

import static bitbridge.Bridge.BridgeException;
import static bitbridge.Bridge.BridgeIterator;
import static ex2.gen_example2.person;

final class example2 {

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
			persons = persons.get_onext();
		}
	}

	public static void main(String[] args) {
		boolean err = true;
		{
			person persons = create_persons_list(100);
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
