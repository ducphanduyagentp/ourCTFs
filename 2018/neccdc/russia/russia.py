import cv2
import numpy as np

img = cv2.imread('go_trump.jpg')
ans = np.zeros((504, 504, 3), np.uint8)

for i in range(30):
	offset = 8 * i
	for l in range(504):
		for w in range(8):
			ans[offset * 2 + w][l] = img[offset + w][offset + l]
	for l in range(1000 - 504 - offset):
		for w in range(8):
			ans[offset * 2 + 8 + w][l] = img[offset + w][offset + 504 + l]
	for l in range(offset + 8):
		for w in range(8):
			ans[offset * 2+ 8 + w][-(offset+8-l)] = img[offset + 8 + w][l]
cv2.imwrite('go_trump_ans.jpg', ans)
cv2.imshow('gg', ans)
cv2.waitKey(0)
cv2.destroyAllWindows()


