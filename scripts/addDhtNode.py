
import getopt
import sys
import zmq

def addNode(ipAddress):
    frame1 = zmq.Frame(b"JOIN")
    frame2 = zmq.Frame(str.encode(ipAddress))

    msg_parts = [frame1, frame2]
    ctx = zmq.Context.instance()
    socket = ctx.socket(zmq.DEALER)
    socket.connect("ipc:///tmp/discoverycontrol")
    socket.send_multipart(msg_parts)
    socket.close()


def main(argv):
    try:
        opts, args = getopt.getopt(argv,"a:",["address="])
    except getopt.GetoptError:
        print('addDhtNode.py -a <IP address>')
        sys.exit(2)

    for opt, arg in opts:
        if opt == '-h':
            print('addDhtNode.py -a <IP address>')
            sys.exit()
        elif opt in ("-a"):
            address = arg
            addNode(address)


if __name__ == "__main__":
    main(sys.argv[1:])