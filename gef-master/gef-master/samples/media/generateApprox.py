import random as rnd
import math as math


def frange(start, stop, step):
    i = start
    while i < stop:
        yield i
        i += step

def main():
    file = open("ApproxData.txt", "w")

    for f in frange(0, 1.05, 0.05):
        file.write('%f    %f\n'%( f, g(f)))



def g(val):
    return(0.2 + (0.8 * (val + (0.7 * math.sin(2*math.pi*val)))))

main()
