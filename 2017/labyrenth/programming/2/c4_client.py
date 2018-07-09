import socket

C4_SERVER = ('52.40.187.77', 16000)
RECV_SIZE = 4096


def main():
    conn = socket.create_connection(C4_SERVER)
    response = conn.recv(RECV_SIZE)
    while True:
        print response
        if 'Skeelz' not in response:
            return

        # Do a thing here.

        move = raw_input("")
        if not len(move):
            return
        conn.send(move)
        response = conn.recv(RECV_SIZE)


if __name__ == '__main__':
    main()
