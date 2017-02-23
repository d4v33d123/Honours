
def frange(start, stop, step):
    i = start
    while i < stop:
        yield i
        i += step

def main():
    file = open("traindat2.txt", "w")

    # now we have to write each line of the code
    # f = angle to waypoint
    # s = distance to side
    # j = speed
    # k = tire angle
    # !!!!!!!!!!!!!!!!!!! putting zero as a desired output value is an issue because how do you multiply to get to 0??!!!!
    for f in frange(0, 1, 0.1):  #0.025 for traindat.txt |||| 0.1 for traindat2.txt |||| 0.2 for traindat3.txt
        for s in frange(0, 1, 0.1): #0.2 for traindat3 |||| 0.1 for traindat&2
            for j in frange(0, 1, 0.2):
                for k in frange(0, 1, 0.2):
                    if( f > 0.4 and f < 0.6):
                        if( s > 0.4 and s < 0.6):
                            file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(f, s, j, k, 0.1, 0.1, 1, 0.1))
                        elif(s < 0.4):
                            if(s > 0.2):
                                file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(f, s, j, k, 0.1, 1, 1, 0.1))
                            else:
                                if(s < 0.2 and j > 0.3):
                                    file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(f, s, j, k, 0.1, 1, 0.1, 0.1))
                                else:
                                    file.write('%f   %f   %f   %f   %f   %f   %f   %fi\n' %(f, s, j, k, 0.1, 1, 1, 0.1))
                        elif(s > 0.6):
                            if(s < 0.8):
                                file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(f, s, j, k, 1, 0.1, 1, 0.1))
                            else:
                                if(s > 0.8 and j > 0.3):
                                    file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(f, s, j, k, 1, 0.1, 0.1, 0.1))
                                else:
                                    file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(f, s, j, k, 1, 0.1, 1, 0.1))
                    elif(f < 0.4):
                        if(f > 0.2 and f < 0.4):
                            file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(f, s, j, k, 0.1, 1, 0.1, 0.1))
                        else:
                            file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(f, s, j, k, 0.1, 1, 0.1, 1))
                    elif(f > 0.6):
                        if(f < 0.8 and f > 0.6):
                            file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(f, s, j, k, 1, 0.1, 0.1, 0.1))
                        else:
                            file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(f, s, j, k, 1, 0.1, 0.1, 1))

main()
