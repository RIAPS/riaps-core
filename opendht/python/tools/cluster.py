#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Copyright (C) 2015-2016 Savoir-faire Linux Inc.
# Author(s): Adrien Béraud <adrien.beraud@savoirfairelinux.com>
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

import os, sys, time, cmd
import subprocess
import signal
import argparse
import threading

import opendht as dht

class NodeCluster(object):
    nodes = []
    node_uid = 0

    @staticmethod
    def run_node(ip4, ip6, p, bootstrap=None, is_bootstrap=False, logfile=None):
        print("run_node", ip4, ip6, p, bootstrap, logfile)
        n = dht.DhtRunner()
        n.run(ipv4=ip4 if ip4 else "", ipv6=ip6 if ip6 else "", port=p, is_bootstrap=is_bootstrap)
        if logfile:
            n.enableFileLogging(logfile)
        if bootstrap:
            n.bootstrap(bootstrap[0], bootstrap[1])
        time.sleep(.01)
        return ((ip4, ip6, p), n, id)

    @staticmethod
    def find_ip(iface):
        if not iface or iface == 'any':
            return ('0.0.0.0','')

        if_ip4 = netifaces.ifaddresses(iface)[netifaces.AF_INET][0]['addr']
        if_ip6 = netifaces.ifaddresses(iface)[netifaces.AF_INET6][0]['addr']
        return (if_ip4, if_ip6)

    def __init__(self, iface=None, ip4=None, ip6=None, port=4000, bootstrap=None, first_bootstrap=False, logfile=None):
        NodeCluster.iface = iface
        self.port = port
        ips = NodeCluster.find_ip(iface)
        self.logfile = logfile
        self.ip4 = ip4 if ip4 else ips[0]
        self.ip6 = ip6 if ip6 else ips[1]
        self.bootstrap = bootstrap
        if bootstrap:
            self.bootstrap = (bootstrap.hostname, str(bootstrap.port) if bootstrap.port else "4222")
        else:
            print("Using fallback bootstrap", self.ip4, self.port)
            self.bootstrap = ((self.ip4, str(self.port)))
        if first_bootstrap:
            print("Starting bootstrap node")
            self.nodes.append(NodeCluster.run_node(self.ip4, self.ip6, self.port, self.bootstrap, is_bootstrap=True))
            self.bootstrap = ((self.ip4, str(self.port)))
            self.port += 1
        #print(self.ip4, self.ip6, self.port)

    def front(self):
        if len(self.nodes) == 0:
            return None
        return self.nodes[0][1]

    def get(self, i=None):
        if not self.nodes:
            return None

        if i is None:
            l = list(self.nodes)
            random.shuffle(l)
            return l[0][1]
        else:
            return self.nodes[i][1]

    def getNodeInfoById(self, id=None):
        if id:
            for n in self.nodes:
                if n[1].getNodeId() == id:
                    return n
        return None

    def launch_node(self):
        node_logfile = (self.logfile + str(self.node_uid) + '.log') if self.logfile else None
        n = NodeCluster.run_node(self.ip4, self.ip6, self.port, self.bootstrap, logfile=node_logfile)
        self.nodes.append(n)
        self.port += 1
        self.node_uid += 1
        return n

    def end_node(self, id=None, shutdown=False, last_msg_stats=None):
        """
        Ends a running node.

        @param id: The 40 hex chars id of the node.
        @type  id: bytes

        @return: If a node was deleted or not.
        @rtype : boolean
        """
        lock = threading.Condition()
        def shutdown_cb():
            nonlocal lock
            NodeCluster.Log.log('Done.')
            with lock:
                lock.notify()

        if not self.nodes:
            return
        elif id:
            n = self.getNodeInfoById(id)
            if n:
                if shutdown:
                    with lock:
                        print('Waiting for node to shutdown... ')
                        n[1].shutdown(shutdown_cb)
                        lock.wait()
                    if last_msg_stats:
                        last_msg_stats.append(self.getMessageStats())
                n[1].join()
                self.nodes.remove(n)
                print(id, 'deleted !')
                return True
            else:
                return False
        else:
            n = self.nodes.pop()
            n[1].join()
            return True

    def replace_node(self, id=None, shutdown=False, last_msg_stats=None):
        random.shuffle(self.nodes)
        deleted = self.end_node(id=id, shutdown=shutdown, last_msg_stats=last_msg_stats)
        if deleted:
            self.launch_node()

    def resize(self, n):
        n = min(n, 500)
        l = len(self.nodes)
        if n == l:
            return
        if n > l:
            print("Launching", n-l, "nodes", self.ip4, self.ip6)
            for i in range(l, n):
                self.launch_node()
        else:
            print("Ending", l-n, "nodes", self.ip4, self.ip6)
            #random.shuffle(self.nodes)
            for i in range(n, l):
                self.end_node()

    def close(self):
    	self.resize(0)

    def getMessageStats(self):
        stats = np.array([0,0,0,0,0])
        for n in self.nodes:
            stats +=  np.array(n[1].getNodeMessageStats())
        stats_list = [len(self.nodes)]
        stats_list.extend(stats.tolist())
        return stats_list

class ClusterShell(cmd.Cmd):
    intro = 'Welcome to the OpenDHT node cluster control. Type help or ? to list commands.\n'
    prompt = '>> '
    net = None
    node = None
    log = False
    def __init__(self, network):
        super(ClusterShell, self).__init__()
        self.net = network
    def setNode(self, node=None, num=0):
        if node == self.node:
            return
        if self.node:
            self.node.disableLogging()
        self.node = node
        if self.node:
            self.prompt = '('+str(num)+') >> '
            if self.log:
                self.node.enableLogging()
        else:
            self.prompt = '>> '
    def do_exit(self, arg):
        self.close()
        return True
    def do_node(self, arg):
        if not arg:
            setNode()
        else:
            nodenum = int(arg)
            if nodenum > len(self.net.nodes) or nodenum < 1:
                print("Invalid node number:", nodenum, " (accepted: 1-", len(self.net.nodes), ")")
            else:
                self.setNode(self.net.nodes[nodenum-1][1], nodenum)
    def do_ll(self, arg):
        if self.node:
            print('Node', self.node.getNodeId().decode())
        else:
            print(len(self.net.nodes), 'nodes running.')
    def do_ls(self, arg):
        if self.node:
            print(self.node.getSearchesLog(0))
        else:
            print('No node selected.')
    def do_log(self, arg):
        if self.node:
            self.log = not self.log
            if self.log:
                self.node.enableLogging()
            else:
                self.node.disableLogging()
    def do_EOF(self, line):
        self.close()
        return True
    def close(self):
        if self.net:
            self.net.close()
            self.net = None

if __name__ == '__main__':
    import argparse
    from urllib.parse import urlparse
    net = None
    try:
        parser = argparse.ArgumentParser(description='Create a dht network of -n nodes')
        parser.add_argument('-n', '--node-num', help='number of dht nodes to run', type=int, default=32)
        parser.add_argument('-I', '--iface', help='local interface to bind', default='any')
        parser.add_argument('-p', '--port', help='start of port range (port, port+node_num)', type=int, default=4000)
        parser.add_argument('-b', '--bootstrap', help='bootstrap address')
        parser.add_argument('-d', '--daemonize', help='daemonize process', action='store_true')
        parser.add_argument('-l', '--log', help='log file prefix')
        args = parser.parse_args()

        if args.bootstrap:
            args.bootstrap = urlparse('dht://'+args.bootstrap)

        net = NodeCluster(iface=args.iface, port=args.port, bootstrap=args.bootstrap, logfile=args.log)
        net.resize(args.node_num)
        if args.daemonize:
            import daemon, lockfile
            #pidfile=lockfile.FileLock('/var/run/dhtcluster.'+str(args.port))
            with daemon.DaemonContext():
                while True:
                    time.sleep(2)
        else:
            ClusterShell(net).cmdloop()
    except Exception as e:
        traceback.print_tb(e.__traceback__)
        print(type(e).__name__+':', e, file=sys.stderr)
    finally:
        if net:
            net.resize(0)
