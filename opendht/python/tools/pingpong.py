#!/usr/bin/env python3
# Copyright (c) 2015-2016 Savoir-faire Linux Inc.
# Author: Adrien Béraud <adrien.beraud@savoirfairelinux.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; If not, see <http://www.gnu.org/licenses/>.

import opendht as dht
import time

ping_node = dht.DhtRunner()
ping_node.run(port = 5632)
#ping_node.enableLogging()
#ping_node.bootstrap("bootstrap.ring.cx", "4222")

pong_node = dht.DhtRunner()
pong_node.run(port = 5633)
#pong_node.enableLogging()
pong_node.bootstrap("192.168.49.55", "5632");
#pong_node.bootstrap("192.168.49.55", "5632");

loc_ping = dht.InfoHash.get("toto99")
loc_pong = dht.InfoHash.get(str(loc_ping))

i = 0
MAX = 8192

def pong(node, h):
	global i
	print("got ping", h, i)
	time.sleep(0.0075)
	i += 1
	if i < MAX:
		node.put(h, dht.Value(b"hey"), lambda ok, nodes: print("over", ok))
	return True

ping_node.listen(loc_ping, lambda v: pong(pong_node, loc_pong))
pong_node.listen(loc_pong, lambda v: pong(ping_node, loc_ping))

pong_node.put(loc_pong, dht.Value(b"hey"), lambda ok, nodes: print("i over", ok))

while (i < MAX):
	time.sleep(1)
