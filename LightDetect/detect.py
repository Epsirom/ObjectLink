# -*-encoding: utf-8 -*-
#
# @author Epsirom

import cv2
import random
import numpy as np


class LightDetector():

    def __init__(self, path, patch_radius=3, ):
        self.img = cv2.imread(path)
        self.patch_radius = patch_radius
        # grey = cv2.cvtColor(self.img, cv2.COLOR_RGB2GRAY)
        # contours, hierarchy = cv2.findContours(grey, cv2.RETR_TREE, cv2.CHAIN_APPROX_NONE)
        # for i in xrange(0, len(contours)):
        #     cv2.drawContours(grey, contours, i, (255, 255, 255))
        # cv2.imshow("contour", grey)
        # cv2.waitKey()
        # print contours

    def get_patch(self, row, col, img=None):
        if img is None:
            img = self.img
        r = self.patch_radius
        shape = img.shape
        return img[max(0, row - r):min(shape[0], row + r + 1), max(0, col - r):min(shape[1], col + r + 1)]

    @staticmethod
    def pixel_dist(p1, p2):
        return reduce(lambda x, y: x + y, map(lambda x, y: (x - y) * (x - y), p1, p2))

    @staticmethod
    def medium(p):
        s = [0, 0, 0]
        for row in xrange(0, p.shape[0]):
            for col in xrange(0, p.shape[1]):
                for c in xrange(0, 3):
                    s[c] += p[row][col][c]
        for c in xrange(0, 3):
            s[c] /= (p.shape[0] * p.shape[1])
        return s

    @staticmethod
    def cover_patch(p, c):
        for row in xrange(0, p.shape[0]):
            for col in xrange(0, p.shape[1]):
                p[row][col] = c

    @staticmethod
    def patch_shadow_tangent(p):
        shape = p.shape
        cluster = np.arange(shape[0] * shape[1]).resize(shape[0], shape[1])

        return (0, 0), (p.shape[0] - 1, p.shape[1] - 1)

    @staticmethod
    def point_similarity(p1, p2):
        s = [0, 0, 0]
        for c in xrange(0, 3):
            s[c] = (p1[c] + 0.1) / (p2[c] + 0.1)
        m = sum(s) / 3
        return sum(map(lambda k: (k - m) * (k - m), s))

    def split_con(self):
        r = self.patch_radius
        shape = self.con.shape
        con = self.con
        block_sum = list()
        block_count = list()
        block_map = np.ones(shape, dtype=np.uint32) * -1
        dir_map = [(0, 1), (1, 0), (0, -1), (-1, 0)]
        for row in xrange(r + 1, shape[0] - r):
            print "split {}".format(row)
            for col in xrange(r + 1, shape[1] - r):
                if con[row][col] < 1 and block_map[row][col] < 0:
                    bfs = [(row, col)]
                    idx = len(block_sum)
                    # if idx > 91:
                    #     print 'a'
                    block_map[row][col] = idx
                    block_sum.append([0, 0, 0])
                    block_count.append(0)
                    head = 0
                    tail = 1
                    while head < tail:
                        p = bfs[head]
                        block_count[idx] += 1
                        for c in xrange(0, 3):
                            block_sum[idx][c] += self.medium[p[0]][p[1]][c]
                        for d in dir_map:
                            tp = (p[0] + d[0], p[1] + d[1])
                            if tp[0] >= r + 1 and tp[0] < shape[0] - r and tp[1] >= r + 1 and tp[1] < shape[1] - r:
                                if con[tp[0]][tp[1]] < 1 and block_map[tp[0]][tp[1]] < 0:
                                    block_map[tp[0]][tp[1]] = idx
                                    bfs.append(tp)
                                    tail += 1
                        head += 1
        self.blocks = map(lambda x, y: map(lambda k: 1.0 * k / y, x), block_sum, block_count)
        self.block_map = block_map
        # cv2.waitKey()

    def get_light_direction(self):
        dirs = [(1, 0), (1, 1), (0, 1), (-1, 1), (-1, 0), (-1, -1), (0, -1), (1, -1)]
        ratios = [1, 1.414, 1, 1.414, 1, 1.414, 1, 1.414]
        hits = [0] * 8
        r = self.patch_radius
        con = self.con
        shape = self.con.shape
        for row in xrange(r + 1, shape[0] - r):
            print "get light direction {}".format(row)
            for col in xrange(r + 1, shape[1] - r):
                if con[row][col] >= 1:
                    for idx, d in enumerate(dirs):
                        h = 1
                        while 1:
                            p = (row + d[0] * h, col + d[1] * h)
                            if p[0] > r and p[0] < shape[0] - r and p[1] > r and p[1] < shape[1] - r and con[p[0]][p[1]] < 1:
                                block_avg = self.blocks[self.block_map[p[0]][p[1]]]
                                pixel = self.medium[p[0]][p[1]]
                                if pixel[0] <= block_avg[0] and pixel[1] <= block_avg[1] and pixel[2] <= block_avg[2] and self.point_similarity(pixel, block_avg) < 0.01:
                                    h += 1
                                else:
                                    break
                            else:
                                break
                        h -= 1
                        hits[idx] += h * ratios[idx]
        print hits
        return dirs[max((v, i) for i, v in enumerate(hits))[1]]

    def draw_shadow(self):
        img = self.img.copy()
        shape = img.shape
        r = self.patch_radius
        # for row in xrange(r, shape[0] - r, r):
        #     print row
        #     for col in xrange(r, shape[1] - r, r):
        #         m = self.medium(self.get_patch(row, col, img))
        #         self.cover_patch(self.get_patch(row, col), m)
        # for row in xrange(r, shape[0] - r):
        #     print row
        #     for col in xrange(r, shape[1] - r):
        #         img[row][col] = self.medium(self.get_patch(row, col))
                # pt1, pt2 = self.patch_shadow_tangent(self.get_patch(row, col, img))
                # if pt1 is not None:
                #     cv2.line(self.get_patch(row, col), pt1, pt2, (0, 0, 255))
        # img = cv2.imread('medium.bmp')
        img = cv2.blur(self.img, (self.patch_radius * 2 + 1, self.patch_radius * 2 + 1), (-1, -1))
        self.medium = img
        t = np.zeros((shape[0], shape[1]), )
        # m = 0
        for row in xrange(r + 1, shape[0] - r):
            print "calculate similarity {}".format(row)
            for col in xrange(r + 1, shape[1] - r):
                t[row][col] = max(self.point_similarity(img[row][col], img[row - 1][col]),
                                  self.point_similarity(img[row][col], img[row][col - 1]))
                # if t[row][col] > m:
                #     m = t[row][col]
                # s = self.patch_similarity(img[row][col], img[row - 1][col])
                # if s > 0.00001 and s < 0.00003:
                #     self.img[row][col] = (0, 0, 255)
                # s = self.patch_similarity(img[row][col], img[row][col - 1])
                # if s > 0.00001 and s < 0.00003:
                #     self.img[row][col] = (0, 0, 255)

        line = sorted(t.reshape((t.size,)))[-t.size / 5]
        for row in xrange(r + 1, shape[0] - r):
            for col in xrange(r + 1, shape[1] - r):
                t[row][col] = 1 if t[row][col] >= line else 0#if t[row][col] / m * 100 > 0.5 else 0
        t = cv2.dilate(t, cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (5, 5)))
        self.con = t#[r:t.shape[0]-r, r:t.shape[1]-r]
        self.split_con()
        cv2.imwrite('map.bmp', (self.block_map + 1) * 1.0 / len(self.blocks))
        print self.get_light_direction()
        cv2.imshow('tmp', t)#cv2.blur(t, (self.patch_radius * 2 + 1, self.patch_radius * 2 + 1), (-1, -1)))
        cv2.waitKey()
        x = 1
        while 1:
            x += 1
            print x
            z = t.copy()
            for row in xrange(r + 1, shape[0] - r):
                # print row
                for col in xrange(r + 1, shape[1] - r):
                    z[row][col] = min(1, z[row][col] / m * x)
            cv2.imshow("tmp", cv2.blur(z, (self.patch_radius * 2 + 1, self.patch_radius * 2 + 1), (-1, -1)))
            cv2.waitKey()
        cv2.imwrite('tmp.bmp', t)
        # cv2.imshow("shadow", self.img)
        # cv2.imwrite("medium.bmp", img)
        # cv2.imwrite("b.png", self.img)
        cv2.waitKey()


if __name__ == '__main__':
    LightDetector('./ld_full.jpg', 3).draw_shadow()
