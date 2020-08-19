import matplotlib
import numpy as np
import matplotlib.pyplot as plt

from newtons_method import NewtonsMethod

def transform(p, s, t):
    return p * s + t

def sinkhorn_knopp_normalize(m, N, K, iterations=1024):
    for _ in range(iterations):
        for i in range(N):
            sum = 0.0
            for a in range(K + 1):
                sum += m[a][i]

            for a in range(K + 1):
                m[a][i] /= sum

        for a in range(K):
            sum = 0.0
            for i in range(N + 1):
                sum += m[a][i]

            for i in range(N + 1):
                m[a][i] /= sum

def centroid(points):
    accum = 0.0
    for point in points:
        accum += point

    return accum / len(points)

def print_correspondence_matrix(m, r_set, p_set, s, t, N, K):
    for i in range(N):
        for a in range(K):
            print("{} -> {} : {}".format(transform(p_set[a], s, t), r_set[i], m[a][i]))

    for a in range(K):
        i = N
        print("{} //// outlier = {}".format(transform(p_set[a], s, t), m[a][i]))

def get_correspondence_matrix(r_set, p_set, N, K, s, t, T, T0, centroid_r, centroid_p):
    m = []
    for i in range(K + 1):
        m.append([0.001] * (N + 1))

    for i in range(N):
        for a in range(K):
            delta = r_set[i] - transform(p_set[a], s, t)
            m[a][i] += (1 / np.sqrt(T)) * np.exp(-delta * delta / (2.0 * T))

    # Outliers
    for i in range(N):
        a = K
        delta = r_set[i] - transform(centroid_p, s, t)

        m[a][i] += 0.01
        #m[a][i] += (1.0 / T0) * np.exp(-delta * delta / (2.0 * T0))

    for a in range(K):
        i = N
        delta = centroid_r - transform(p_set[a], s, t)

        m[a][i] += 0.01

        #m[a][i] += (1.0 / T0) * np.exp(-delta * delta / (2.0 * T0))

    #print("m-before: {}".format(m))
    #print_correspondence_matrix(m, r_set, p_set, s, t, N, K)
    sinkhorn_knopp_normalize(m, N, K)
    #print("m-after: {}".format(m))
    return m

def estimated_positions(r_set, p_set, s, t, m, N, K):
    OUTLIER_EPSILON = 0.7

    x = []
    v = []

    for a in range(K):
        m_sum = 0.0
        for i in range(N):
            m_sum += m[a][i]

        if m_sum > OUTLIER_EPSILON:            
            x_a = 0.0
            for i in range(N):
                 x_a += r_set[i] * m[a][i]

            x.append((x_a / m_sum) * 0.5 + 0.5 * transform(p_set[a], s, t))
            v.append(p_set[a])

    return x, v

def generate_point_sets(r_set, p_set, N, K, m):
    x = []
    v = []

    for i in range(N):
        for a in range(K):
            weight = np.sqrt(m[a][i])

            x.append(weight * r_set[i])
            v.append(weight * p_set[a])

            print("{} -> {} [{}]".format(p_set[a], r_set[i], weight))

    return x, v

def largest_distance(r, p, N, K):
    max_r = r[N - 1]
    min_r = r[0]

    max_p = p[K - 1]
    min_p = p[0]

    overall_max = max(max_p, max_r)
    overall_min = min(min_p, min_r)

    return overall_max - overall_min

def min_distance(r, N):
    smallest = r[1] - r[0]
    for i in range(1, N - 1):
        i_next = i + 1
        if r[i_next] - r[i] < smallest:
            smallest = r[i_next] - r[i]

    return smallest


r_set = [0.0, 5.0, 6.0]
p_set = [0.5 * 2, 5.5 * 2, 6.5 * 2, 7 * 2]

centroid_r = centroid(r_set)
centroid_p = centroid(p_set)

annealing_rate = 0.93

N = len(r_set)
K = len(p_set)
T0 = np.power(largest_distance(r_set, p_set, N, K), 2)
T_final = np.power(min_distance(r_set, N), 2)

#T_final = 0.001

T_final = 0.001
T = 100
s, t = 1.0, 0.0
ds, dt = 1.0, 0.0

while T > T_final:
    p_current = []
    for i in range(K):
        p_current.append(transform(p_set[i], s, t))

    m = get_correspondence_matrix(r_set, p_set, N, K, s, t, T, T0, centroid_r, centroid_p)
    x, v = estimated_positions(r_set, p_set, s, t, m, N, K)
    #x, v = generate_point_sets(r_set, p_set, N, K, m)

    print("====")
    print("T = {}".format(T))
    print("x = {}".format(x))
    print("v = {}".format(v))
    print("p = {}".format(p_current))

    s, t = NewtonsMethod.solve(x, v, s_start=s, t_start=t)
    T = T * annealing_rate

    np.set_printoptions(suppress=True)
    print("m:\n{}".format(np.matrix(m)))
    print("s, t: {}, {}".format(s, t))

    input("")
