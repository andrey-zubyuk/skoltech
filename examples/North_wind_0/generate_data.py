#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import numpy as np

A = np.zeros((15, 17, 10, 10))
dx = 9
dy = 10
wind_speed = 1
for x in range(A.shape[0]):
    for y in range(A.shape[1]):
        for td in range(A.shape[2]):
            for ts in range(A.shape[3]):
                if x == dx and (y - dy) / wind_speed == td - ts:
                    A[x, y, td, ts] = 1
np.save("A.npy", A)

n = np.array([1.2] * A.shape[2], dtype = float)
with open("n.csv", "w") as f:
    np.savetxt(f, n, fmt = "%7g")
