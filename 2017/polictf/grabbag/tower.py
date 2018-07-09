from pwn import *


dr = [-1, 0, 0, 1]
dc = [0, -1, 1, 0]
ds = ['w', 'a', 'd', 's']

def isInside(r, c, R, C):
	return 0 < r and r < R and 0 < c and c < C

def solveMaze(G, start, goal):
	R = len(G)
	C = len(G[3])
	print start, goal, R, C
	pos = []
	pos.append((R - start[1] - 1, start[0] + 1))
	queue = [(pos[0], '', tuple(start))]
	check = [[False] * C for _ in range(R)]
	check[pos[0][0]][pos[0][1]] = True
	while len(queue):
		u = queue.pop(0)
		r = u[0][0]
		c = u[0][1]
		s = u[1]
		x = u[2][0]
		y = u[2][1]
		for i in range(4):
			rr = r + dr[i]
			cc = c + dc[i]
			ss = s + ds[i]
			xx = x + dc[i]
			yy = y - dr[i]

			if not isInside(rr, cc, R, C):
				continue
			if i == 3 and G[r][c] == '_':
				continue
			if i == 1 or i == 2:
				if G[rr][cc] == '|':
					continue
				rr += dr[i]
				cc += dc[i]
				if not isInside(rr, cc, R, C):
					continue
			else:
				if G[rr][cc] == '_' and i == 0:
					continue
			if check[rr][cc]:
				continue
			check[rr][cc] = True
			if xx == goal[0] and yy == goal[1]:
				return ss
			queue.append(((rr, cc), ss, (xx, yy)))
	

def main():
	maze = remote('tower.chall.polictf.it', 31337)
	lines = maze.recvuntil('(the lower left cell is 0,0)\n').split('\n')
	lines = [line.strip() for line in lines]
	start = lines[-5].strip('start:').split(',')
	goal = lines[-4].strip('goal:').split(',')
	start = [int(x) for x in start]
	goal = [int(x) for x in goal]
	lines = lines[1:-5]
	print start, goal
	for line in lines:
		print line
	solution = solveMaze(lines, start, goal)
	print solution
	maze.sendline(solution)
	print maze.recv()

if __name__ == '__main__':
	main()
