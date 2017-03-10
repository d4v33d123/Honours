import random as rnd


def frange(start, stop, step):
    i = start
    while i < stop:
        yield i
        i += step

def main():
    file = open("traindat25.txt", "w")

    # now we have to write each line of the code
    # f = angle to waypoint
    # s = distance to side
    # j = speed
    # k = tire angle
    # !!!!!!!!!!!!!!!!!!! putting zero as a desired output value is an issue because how do you multiply to get to 0??!!!!
    for f in frange(0, 1, 0.1):  #0.025 for traindat.txt |||| 0.1 for traindat2.txt |||| 0.2 for traindat3.txt
        for s in frange(0, 1, 0.1): #0.2 for traindat3 |||| 0.1 for traindat&2
            for j in frange(0, 1, 0.1):
                for k in frange(0, 1, 0.1):
                    if( f >= 0.25 and f <= 0.75):
                        for ff in frange(f, (f+0.1), 0.1):
                            if(f >= 0.4 and f <= 0.6):
                                if( s > 0.3 and s < 0.7):
                                    if(k > 0.3 and k < 0.7):
                                        file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(noise(ff, 0.1), noise(s, 0.1), noise(j, 0.1), noise(k, 0.1), noise(0.1, 0.1), noise(0.1, 0.1), noise(0.9, -0.1), noise(0.1, 0.1)))
                                    elif(k <= 0.3):
                                        file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(noise(ff, 0.1), noise(s, 0.1), noise(j, 0.1), noise(k, 0.1), noise(0.1, 0.1), noise(0.1, 0.1), noise(0.9, -0.1), noise(0.1, 0.1)))
                                    else:
                                        file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(noise(ff, 0.1), noise(s, 0.1), noise(j, 0.1), noise(k, 0.1), noise(0.1, 0.1), noise(0.1, 0.1), noise(0.9, -0.1), noise(0.1, 0.1)))
                                elif(s <= 0.3):
                                    if(s >= 0.2):
                                        file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(noise(ff, 0.1), noise(s, 0.1), noise(j, 0.1), noise(k, 0.1), noise(0.1, 0.1), noise(0.9, -0.1), noise(0.9, -0.1), noise(0.1, 0.1)))
                                    else:
                                        if(s < 0.2):
                                            if(j > 0.3):
                                                file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(noise(ff, 0.1), noise(s, 0.1), noise(j, 0.1), noise(k, 0.1), noise(0.1, 0.1), noise(0.9, -0.1), noise(0.1, 0.1), noise(0.1, 0.1)))
                                            elif(j <= 0.3):
                                                file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(noise(ff, 0.1), noise(s, 0.1), noise(j, 0.1), noise(k, 0.1), noise(0.1, 0.1), noise(0.9, -0.1), noise(0.9, -0.1), noise(0.1, 0.1)))
                                elif(s >= 0.7):
                                    if(s <= 0.8):
                                        file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(noise(ff, 0.1), noise(s, 0.1), noise(j, 0.1), noise(k, 0.1), noise(0.9, -0.1), noise(0.1, 0.1), noise(0.9, -0.1), noise(0.1, 0.1)))
                                    else:
                                        if(s > 0.8):
                                            if(j > 0.3):
                                                file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(noise(ff, 0.1), noise(s, 0.1), noise(j, 0.1), noise(k, 0.1), noise(0.9, -0.1), noise(0.1, 0.1), noise(0.1, 0.1), noise(0.1, 0.1)))
                                            elif(j <= 0.3):
                                                file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(noise(ff, 0.1), noise(s, 0.1), noise(j, 0.1), noise(k, 0.1), noise(0.9, -0.1), noise(0.1, 0.1), noise(0.9, -0.1), noise(0.1, 0.1)))
                            elif(f > 0.6):
                                if( s > 0.3 and s < 0.7):
                                    if(k > 0.3 and k < 0.7):
                                        file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(noise(ff, 0.1), noise(s, 0.1), noise(j, 0.1), noise(k, 0.1), noise(0.1, 0.1), noise(0.9, -0.1), noise(0.9, -0.1), noise(0.1, 0.1)))
                                    elif(k <= 0.3):
                                        file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(noise(ff, 0.1), noise(s, 0.1), noise(j, 0.1), noise(k, 0.1), noise(0.1, 0.1), noise(0.9, -0.1), noise(0.9, -0.1), noise(0.1, 0.1)))
                                    else:
                                        file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(noise(ff, 0.1), noise(s, 0.1), noise(j, 0.1), noise(k, 0.1), noise(0.1, 0.1), noise(0.9, -0.1), noise(0.9, -0.1), noise(0.1, 0.1)))
                                elif(s <= 0.3):
                                    if(s >= 0.2):
                                        file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(noise(ff, 0.1), noise(s, 0.1), noise(j, 0.1), noise(k, 0.1), noise(0.1, 0.1), noise(0.9, -0.1), noise(0.9, -0.1), noise(0.1, 0.1)))
                                    else:
                                        if(s < 0.2):
                                            if(j > 0.3):
                                                file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(noise(ff, 0.1), noise(s, 0.1), noise(j, 0.1), noise(k, 0.1), noise(0.1, 0.1), noise(0.9, -0.1), noise(0.9, -0.1), noise(0.1, 0.1)))
                                            elif(j <= 0.3):
                                                file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(noise(ff, 0.1), noise(s, 0.1), noise(j, 0.1), noise(k, 0.1), noise(0.1, 0.1), noise(0.9, -0.1), noise(0.9, -0.1), noise(0.1, 0.1)))
                                elif(s >= 0.7):
                                    if(s <= 0.8):
                                        file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(noise(ff, 0.1), noise(s, 0.1), noise(j, 0.1), noise(k, 0.1), noise(0.9, -0.1), noise(0.1, 0.1), noise(0.9, -0.1), noise(0.1, 0.1)))
                                    else:
                                        if(s > 0.8):
                                            if(j > 0.3):
                                                file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(noise(ff, 0.1), noise(s, 0.1), noise(j, 0.1), noise(k, 0.1), noise(0.1, 0.1), noise(0.1, 0.1), noise(0.9, -0.1), noise(0.1, 0.1)))
                                            elif(j <= 0.3):
                                                file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(noise(ff, 0.1), noise(s, 0.1), noise(j, 0.1), noise(k, 0.1), noise(0.1, 0.1), noise(0.1, 0.1), noise(0.9, -0.1), noise(0.1, 0.1)))

                            else:
                                if( s > 0.3 and s < 0.7):
                                    if(k > 0.3 and k < 0.7):
                                        file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(noise(ff, 0.1), noise(s, 0.1), noise(j, 0.1), noise(k, 0.1), noise(0.9, -0.1), noise(0.1, 0.1), noise(0.9, -0.1), noise(0.1, 0.1)))
                                    elif(k <= 0.3):
                                        file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(noise(ff, 0.1), noise(s, 0.1), noise(j, 0.1), noise(k, 0.1), noise(0.9, -0.1), noise(0.1, 0.1), noise(0.9, -0.1), noise(0.1, 0.1)))
                                    else:
                                        file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(noise(ff, 0.1), noise(s, 0.1), noise(j, 0.1), noise(k, 0.1), noise(0.9, -0.1), noise(0.1, 0.1), noise(0.9, -0.1), noise(0.1, 0.1)))
                                elif(s <= 0.3):
                                    if(s >= 0.2):
                                        file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(noise(ff, 0.1), noise(s, 0.1), noise(j, 0.1), noise(k, 0.1), noise(0.9, -0.1), noise(0.9, -0.1), noise(0.9, -0.1), noise(0.1, 0.1)))
                                    else:
                                        if(s < 0.2):
                                            if(j > 0.3):
                                                file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(noise(ff, 0.1), noise(s, 0.1), noise(j, 0.1), noise(k, 0.1), noise(0.1, 0.1), noise(0.1, 0.1), noise(0.9, -0.1), noise(0.1, 0.1)))
                                            elif(j <= 0.3):
                                                file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(noise(ff, 0.1), noise(s, 0.1), noise(j, 0.1), noise(k, 0.1), noise(0.1, 0.1), noise(0.1, 0.1), noise(0.9, -0.1), noise(0.1, 0.1)))
                                elif(s >= 0.7):
                                    if(s <= 0.8):
                                        file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(noise(ff, 0.1), noise(s, 0.1), noise(j, 0.1), noise(k, 0.1), noise(0.9, -0.1), noise(0.1, 0.1), noise(0.9, -0.1), noise(0.1, 0.1)))
                                    else:
                                        if(s > 0.8):
                                            if(j > 0.3):
                                                file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(noise(ff, 0.1), noise(s, 0.1), noise(j, 0.1), noise(k, 0.1), noise(0.9, -0.1), noise(0.1, 0.1), noise(0.9, -0.1), noise(0.1, 0.1)))
                                            elif(j <= 0.3):
                                                file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(noise(ff, 0.1), noise(s, 0.1), noise(j, 0.1), noise(k, 0.1), noise(0.9, -0.1), noise(0.1, 0.1), noise(0.9, -0.1), noise(0.1, 0.1)))
                                                
                    elif(f > 0.75): 
                        if(k > 0.3 and k < 0.7):
                            file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(noise(f, 0.1), noise(s, 0.1), noise(j, 0.1), noise(k, 0.1), noise(0.1, 0.1), noise(0.9, -0.1), noise(0.1, 0.1), noise(0.9, -0.1)))
                        elif(k <= 0.3):
                            file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(noise(f, 0.1), noise(s, 0.1), noise(j, 0.1), noise(k, 0.1), noise(0.1, 0.1), noise(0.9, -0.1), noise(0.1, 0.1), noise(0.9, -0.1)))
                        else:
                            file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(noise(f, 0.1), noise(s, 0.1), noise(j, 0.1), noise(k, 0.1), noise(0.1, 0.1), noise(0.9, -0.1), noise(0.1, 0.1), noise(0.9, -0.1)))

                    elif(f < 0.25):
                        if(k > 0.3 and k < 0.7):
                            file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(noise(f, 0.1), noise(s, 0.1), noise(j, 0.1), noise(k, 0.1), noise(0.9, -0.1), noise(0.1, 0.1), noise(0.1, 0.1), noise(0.9, -0.1)))
                        elif(k <= 0.3):
                            file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(noise(f, 0.1), noise(s, 0.1), noise(j, 0.1), noise(k, 0.1), noise(0.9, -0.1), noise(0.1, 0.1), noise(0.1, 0.1), noise(0.9, -0.1)))
                        else:
                            file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(noise(f, 0.1), noise(s, 0.1), noise(j, 0.1), noise(k, 0.1), noise(0.9, -0.1), noise(0.1, 0.1), noise(0.1, 0.1), noise(0.9, -0.1)))
                                
                    


def noise(val, rng):
    randnum = rnd.uniform(0,rng)
    val += randnum
    return val

main()
