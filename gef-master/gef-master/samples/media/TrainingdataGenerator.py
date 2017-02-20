
def frange(start, stop, step):
    i = start
    while i < stop:
        yield i
        i += step

def main():
    file = open("traindat.txt", "w")

    # now we have to write each line of the code
    # f = angle to waypoint
    # s = distance to side
    # j = speed
    # k = tire angle
    for f in frange(0, 1, 0.025):
        for s in frange(0, 1, 0.2):
            for j in frange(0, 1, 0.2):
                for k in frange(0, 1, 0.1):
                    if( f > 0.4 and f < 0.6):
                        if( s > 0.4 and s < 0.6):
                            file.write('%f   %f   %f   %f   %i   %i   %i   %i\n' %(f, s, j, k, 0, 0, 1, 0))
                        elif(s < 0.4):
                            if(s > 0.2):
                                file.write('%f   %f   %f   %f   %i   %i   %i   %i\n' %(f, s, j, k, 0, 1, 1, 0))
                            else:
                                if(s < 0.2 and j > 0.3):
                                    file.write('%f   %f   %f   %f   %i   %i   %i   %i\n' %(f, s, j, k, 0, 1, 0, 0))
                                else:
                                    file.write('%f   %f   %f   %f   %i   %i   %i   %i\n' %(f, s, j, k, 0, 1, 1, 0))
                        elif(s > 0.6):
                            if(s < 0.8):
                                file.write('%f   %f   %f   %f   %i   %i   %i   %i\n' %(f, s, j, k, 1, 0, 1, 0))
                            else:
                                if(s > 0.8 and j > 0.3):
                                    file.write('%f   %f   %f   %f   %i   %i   %i   %i\n' %(f, s, j, k, 1, 0, 0, 0))
                                else:
                                    file.write('%f   %f   %f   %f   %i   %i   %i   %i\n' %(f, s, j, k, 1, 0, 1, 0))
                    elif(f < 0.4):
                        if(f < 0.2 and f > 0.3):
                            file.write('%f   %f   %f   %f   %i   %i   %i   %i\n' %(f, s, j, k, 0, 1, 0, 0))
                        else:
                            file.write('%f   %f   %f   %f   %i   %i   %i   %i\n' %(f, s, j, k, 0, 1, 1, 0))
                    elif(f > 0.6):
                        if(f < 0.2 and f > 0.3):
                            file.write('%f   %f   %f   %f   %i   %i   %i   %i\n' %(f, s, j, k, 1, 0, 0, 0))
                        else:
                            file.write('%f   %f   %f   %f   %i   %i   %i   %i\n' %(f, s, j, k, 1, 0, 1, 0))

main()
