/********************************************************************************
* bitbridge - another serialization/rpc mechanism
* Copyright (C) 2008-2017 Michael M. Builov, https://github.com/mbuilov/bitbridge
* Licensed under GPL version 3 or any later version, see COPYING
********************************************************************************/

/* example1.java */

/* example 1:
 - serialize and then deserialize simple structure

 1) fill test person structure
 2) serialize it
 3) deserialize it
*/

import static bitbridge.Bridge.BridgeException;
import static bitbridge.Bridge.BridgeIterator;

final class example1 {

	static byte[] serialize(gen_example.person p) throws BridgeException {
		return p.pack(/*head:*/0, /*tail:*/0, /*big:*/false);
	}

	static gen_example.person deserialize(BridgeIterator it, byte[] mem) throws BridgeException {
		return gen_example.person.unpack(it, mem, mem.length, /*big:*/false);
	}

	static void print_person(gen_example.person p) {
		System.out.print("\nperson:");
		System.out.print("\n\tid = " + p.get_id());
		System.out.print("\n\tname = " + p.get_name());
	}

	public static void main(String[] args) {
		boolean err = true;
		{
			gen_example.person p = new gen_example.person();
			p.set_id(123);
			p.set_name("person name");
			{
				byte[] mem = null;
				try {
					mem = serialize(p);
				}
				catch (BridgeException ex) {
					System.err.print("failed to serialize, error: " + ex);
				}
				if (mem != null) {
					gen_example.person p2 = null;
					BridgeIterator it = new BridgeIterator(0);
					try {
						p2 = deserialize(it, mem);
					}
					catch (BridgeException ex) {
						System.err.print("failed to deserialize, error: " + ex);
					}
					if (p2 != null) {
						if (mem.length != it.at)
							System.err.print("wrong packed size!");
						else if (p.get_id() != p2.get_id())
							System.err.print("wrong deserialized person id!");
						else if (!p.get_name().equals(p2.get_name()))
							System.err.print("wrong deserialized person name!");
						else {
							if (args.length < 2)
								print_person(p2);
							err = false; /* ok */
						}
					}
				}
			}
		}
		if (err)
			System.out.print("\nfailed\n");
		else if (args.length < 3)
			System.out.print("\nok\n");
	}
}
