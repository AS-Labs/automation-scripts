import psutil

def get_connections():
    """
    Get all servers with established connections to the current server.
    """
    current_server = '192.168.0.188'  # Replace with the IP of the current server

    connection_count = {}
    for conn in psutil.net_connections():
        if conn.status == 'ESTABLISHED' and conn.raddr:
            source, source_port = conn.laddr
            target, target_port = conn.raddr

            if source == current_server:
                key = (target, target_port)
                if key in connection_count:
                    connection_count[key] += 1
                else:
                    connection_count[key] = 1

    formatted_connections = []
    for (target, target_port), count in connection_count.items():
        formatted_connections.append([current_server, target, target_port, count])

    return formatted_connections

# Main program
if __name__ == '__main__':
    server_connections = get_connections()

    # Print the formatted connections
    for connection in server_connections:
        print(f"{connection[0]},{connection[1]},{connection[2]},{connection[3]}")

