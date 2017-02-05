from pyczmq import zmq, zctx, zsocket, zmsg, zframe
import sys, getopt

def addNode(ipAddress):
    m = zmsg.new()
    zmsg.addstr(m, "JOIN")
    zmsg.addstr(ipAddress)

    socket = zsocket.new("ipc:///tmp/discoverycontrol", zsocket.DEALER)

    zmsg.send(m, socket)


def main(argv):
    try:
        opts, args = getopt.getopt(argv,"hi:o:",["ifile=","ofile="])
    except getopt.GetoptError:
        print 'addDhtNode.py -a <IP address>'
        sys.exit(2)

    for opt, arg in opts:
        if opt == '-h':
            print 'addDhtNode.py -a <IP address>'
            sys.exit()
        elif opt in ("-a", "--ifile"):
            address = arg
            addNode(address)

if __name__ == "__main__":
    main(sys.argv[1:])