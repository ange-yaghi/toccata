import matplotlib
import numpy as np
import matplotlib.pyplot as plt


class NewtonsMethod(object):
    def __init__(self):
        pass
    
    @staticmethod
    def transform(p, s, t):
        return p * s + t

    @staticmethod
    def cost(r_set, p_set, s, t):
        n = len(r_set)
        total = 0.0
        for i in range(n):
            p = p_set[i]
            r = r_set[i]
            p_t = NewtonsMethod.transform(p, s, t)

            # (r - p_t) * (r - p_t)
            diff_2 = r * r - 2 * p_t * r + p_t * p_t
            total += diff_2

        return total

    @staticmethod
    def cost_gradient(r_set, p_set, s, t):
        n = len(r_set)
        gradient = [0.0, 0.0]
        for i in range(n):
            p_t = NewtonsMethod.transform(p_set[i], s, t)
            p = p_set[i]
            r = r_set[i]

            gradient[0] += 2 * p * (p_t - r)
            gradient[1] += 2 * (p_t - r)

        return gradient

    @staticmethod
    def cost_hessian(p_set):
        n = len(p_set)
        H = [[0.0, 0.0], [0.0, 0.0]]
        for i in range(n):
            p = p_set[i]

            H[0][0] += 2 * p * p
            H[1][0] += 2 * p

            H[0][1] += 2 * p
            H[1][1] += 2

        return H

    @staticmethod
    def solve(r_set, p_set, s_start=1.0, t_start=0.0, iteration_limit=16, epsilon=1e-4):
        if len(r_set) == 1:
            return s_start, s_start * p_set[0] - r_set[0]
        elif len(r_set) == 0:
            return s_start, t_start

        H = NewtonsMethod.cost_hessian(p_set)

        x = [s_start, t_start]
        s, t = x[0], x[1]
        for _ in range(iteration_limit):
            grad = NewtonsMethod.cost_gradient(r_set, p_set, s, t)
            #print("grad = {}".format(grad))

            delta = -np.linalg.solve(H, grad)
            x = x + delta
            s, t = x[0], x[1]

            #print("{}, {} with delta={}".format(s, t, delta))

            if abs(delta[0]) < epsilon and abs(delta[1]) < epsilon:
                break
        
        return x[0], x[1]


if __name__ == "__main__":
    #r = [0.2, 0.3]
    #p = [0.1, 0.9]

    r = [0.0, 1.0]
    p = [0.2, 0.9]

    s, t = NewtonsMethod.solve(r, p, s_start=0.4, t_start=0.5)

    print((s, t))
