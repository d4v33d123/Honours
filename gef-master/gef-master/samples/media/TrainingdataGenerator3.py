import random as rnd


def frange(start, stop, step):
    i = start
    while i < stop:
        yield i
        i += step

def main():
    file = open("traindat28.txt", "w")

    # now we have to write each line of the code
    # f = angle to waypoint
    # s = distance to side
    # j = speed
    # k = tire angle
    # !!!!!!!!!!!!!!!!!!! putting zero as a desired output value is an issue because how do you multiply to get to 0??!!!!
    for f in frange(0, 1, 0.05):  #0.025 for traindat.txt |||| 0.1 for traindat2.txt |||| 0.2 for traindat3.txt
        for s in frange(0, 1, 0.1): #0.2 for traindat3 |||| 0.1 for traindat&2
            for j in frange(0, 1, 0.1):
                for k in frange(0, 1, 0.1):
                    if( f >= 0.3 and f <= 0.7):
                        for ff in frange(f, (f+0.1), 0.1):
                            if(f >= 0.45 and f <= 0.55):#if the waypoint angle is centred
                                if( s > 0.4 and s < 0.6):#if the car is in the middle of the track
                                    file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(noise(ff, 0.1), noise(s, 0.1), noise(j, 0.1), noise(k, 0.1), 0.1, 0.1, 0.9, 0.1))
                                elif(s >= 0.6 and s <= 0.8):# if the car is to the left of the track 
                                    file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(noise(ff, 0.1), noise(s, 0.1), noise(j, 0.1), noise(k, 0.1), 0.9, 0.1, 0.9, 0.1))
                                elif(s > 0.8):
                                    file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(noise(ff, 0.1), noise(s, 0.1), noise(j, 0.1), noise(k, 0.1), 0.9, 0.1, 0.9, 0.1))
                                elif(s <= 0.4 and s >= 0.2):# if the car is to the right of the track
                                    file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(noise(ff, 0.1), noise(s, 0.1), noise(j, 0.1), noise(k, 0.1), 0.1, 0.9, 0.9, 0.1))
                                else:
                                    file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(noise(ff, 0.1), noise(s, 0.1), noise(j, 0.1), noise(k, 0.1), 0.1, 0.9, 0.9, 0.1))

                                    
                            elif(f > 0.55):# if the car is angled to a higher degree than the waypoint
                                if( s > 0.3 and s < 0.7):
                                   file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(noise(ff, 0.1), noise(s, 0.1), noise(j, 0.1), noise(k, 0.1), 0.1, 0.9, 0.9, 0.1))
                                elif(s >= 0.7):
                                    file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(noise(ff, 0.1), noise(s, 0.1), noise(j, 0.1), noise(k, 0.1), 0.1, 0.9, 0.9, 0.1))
                                elif(s <= 0.3):
                                    file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(noise(ff, 0.1), noise(s, 0.1), noise(j, 0.1), noise(k, 0.1), 0.9, 0.1, 0.9, 0.1))

                            else:# if the car is angled to a lower degree than the waypoint
                                if( s > 0.3 and s < 0.7):
                                    file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(noise(ff, 0.1), noise(s, 0.1), noise(j, 0.1), noise(k, 0.1), 0.9, 0.1, 0.9, 0.1))
                                elif(s >= 0.7):
                                    file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(noise(ff, 0.1), noise(s, 0.1), noise(j, 0.1), noise(k, 0.1), 0.1, 0.9, 0.9, 0.1))
                                elif(s <= 0.3):
                                    file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(noise(ff, 0.1), noise(s, 0.1), noise(j, 0.1), noise(k, 0.1), 0.9, 0.1, 0.9, 0.1))
                                                
                    elif(f < 0.3): # if the car is angled to a higher degree than the waypoint
                        if(f > 0.1):
                            if(s > 0.3 and s < 0.7):
                                file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(noise(f, 0.1), noise(s, 0.1), noise(j, 0.1), noise(k, 0.1), 0.1, 0.9, 0.1, 0.9))
                            elif(s <= 0.3):
                                file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(noise(f, 0.1), noise(s, 0.1), noise(j, 0.1), noise(k, 0.1), 0.1, 0.9, 0.1, 0.9))
                            else:
                                file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(noise(f, 0.1), noise(s, 0.1), noise(j, 0.1), noise(k, 0.1), 0.9, 0.1, 0.9, 0.1))
                        else:
                            file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(noise(f, 0.1), noise(s, 0.1), noise(j, 0.1), noise(k, 0.1), 0.1, 0.9, 0.1, 0.9))
                    elif(f > 0.7): # if the car is angled to a lower degree than the waypoint
                        if(f < 0.9):
                            if(s > 0.3 and s < 0.7):
                                file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(noise(f, 0.1), noise(s, 0.1), noise(j, 0.1), noise(k, 0.1), 0.9, 0.1, 0.1, 0.9))
                            elif(s >= 0.7):
                                file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(noise(f, 0.1), noise(s, 0.1), noise(j, 0.1), noise(k, 0.1), 0.9, 0.1, 0.1, 0.9))
                            else:
                                file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(noise(f, 0.1), noise(s, 0.1), noise(j, 0.1), noise(k, 0.1), 0.1, 0.9, 0.9, 0.1))
                        else:
                            file.write('%f   %f   %f   %f   %f   %f   %f   %f\n' %(noise(f, 0.1), noise(s, 0.1), noise(j, 0.1), noise(k, 0.1), 0.9, 0.1, 0.1, 0.9))
                                
                    


def noise(val, rng):
    randnum = rnd.uniform(0,rng)
    val += randnum
    return val

main()
