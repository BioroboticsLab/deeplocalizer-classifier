#! /usr/bin/env python3

import sys
import numpy as np
import matplotlib.pyplot as plt
steps = 100
gtlabels_prob_file = sys.argv[1]

print("loading data from file: " + gtlabels_prob_file)
with open(gtlabels_prob_file) as f:
    gtlabels_prob = np.loadtxt(f, delimiter=',')

true_pos_rate = []
false_pos_rate = []
is_pos = gtlabels_prob[:, 0] == 1
is_neg = gtlabels_prob[:, 0] == 0
n_is_pos = float(is_pos.sum())
n_is_neg = float(is_neg.sum())

for i in range(steps):
    says_is_pos = gtlabels_prob[:, 1] > float(i)/steps
    true_pos_rate.append(is_pos[says_is_pos].sum() / n_is_pos)
    false_pos_rate.append(1 - is_neg[~says_is_pos].sum() / n_is_neg)

axes = plt.gca()
axes.set_xlim([0., 1.])
axes.set_ylim([0., 1.])

plt.plot(false_pos_rate, true_pos_rate)
plt.show()

