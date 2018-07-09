from socket import create_connection

MAZE_SERVER = ('54.69.145.229', 16000)
RECV_SIZE = 8192

dr = [-1, 0, 0, 1]
dc = [0, -1, 1, 0]
ds = ['^', '<', '>', 'V']

def isInside(r, c, R, C):
	return 0 < r and r < R and 0 < c and c < C

def solveMaze(G):
	R = len(G)
	C = len(G[0])
	pos = []
	for r in range(R):
		for c in range(C):
			if G[r][c] == '>':
				pos.append((r, c))
			if G[r][c] == 'X':
				pos.append((r, c))
	queue = [(pos[0], '')]
	check = [[False] * C for _ in range(R)]
	while len(queue):
		u = queue.pop(0)
		r = u[0][0]
		c = u[0][1]
		s = u[1]
		for i in range(4):
			rr = r + dr[i]
			cc = c + dc[i]
			ss = s + ds[i]
			if not isInside(rr, cc, R, C):
				continue
			if check[rr][cc]:
				continue
			if G[rr][cc] == '#':
				continue
			if G[rr][cc] == 'X':
				return ss
			check[rr][cc] = True
			queue.append(((rr, cc), ss))
	

def main():
    conn = create_connection(MAZE_SERVER)
    response = conn.recv(RECV_SIZE)
    while True:
        print response
        if "Now " not in response:
            return

        response_lines = response.splitlines()
        find_delim = [x for x in response_lines if x.startswith('Now')][0]
        maze_lines = response_lines[response_lines.index(find_delim)+2:-1]
        maze_text = '\n'.join(maze_lines)

        # Do your thing here with either maze_text or maze_lines.
        # solution = raw_input("Your solution: ")
	solution = solveMaze(maze_lines)
	print solution
        if not len(solution):
            return
        conn.send(solution)

        response = conn.recv(RECV_SIZE)


if __name__ == '__main__':
    main()
