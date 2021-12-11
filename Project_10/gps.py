from gps3 import gps3

def main(*args):
    gps_socket = gps3.GPSDSocket()
    data_stream = gps3.DataStream()
    gps_socket.connect()
    gps_socket.watch()
    for new_data in gps_socket:
        if new_data:
            data_stream.unpack(new_data)
            print(data_stream.TPV)


if __name__ == "__main__":
   main()
