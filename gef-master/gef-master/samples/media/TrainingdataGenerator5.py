import random as rnd


def frange(start, stop, step):
    i = start
    while i < stop:
        yield i
        i += step

def noise(val, rng):
    randnum = rnd.uniform(0,rng)
    val += randnum
    return val


def main():
    file = open("traindat31.txt", "w")
    for i in range(0, 1000, 1):
        f = noise(rnd.uniform(0,0.9),0.1)
        s = noise(rnd.uniform(0,0.9),0.1)
        #v = noise(rnd.uniform(0,0.9),0.1)
        #k = noise(rnd.uniform(0,0.9),0.1)

        if( f >= 0.3 and f <= 0.7):
            if(f >= 0.4 and f <= 0.6):#if the waypoint angle is centred
                if( s > 0.2 and s < 0.8):#if the car is in the middle of the track
                    file.write('%f   %f   %f   %f   %f   %f\n' %(f, s, noise(0.1, 0.1), noise(0.1, 0.1), noise(1.0, -0.1), noise(0.1, 0.1)))
                elif(s <= 0.2 and s >= 0.1):# if the car is to the left of the track 
                    file.write('%f   %f   %f   %f   %f   %f\n' %(f, s,noise(1.0, -0.1), noise(0.1, 0.1), noise(1.0, -0.1), noise(0.1, 0.1)))
                elif(s < 0.1):
                    file.write('%f   %f   %f   %f   %f   %f\n' %(f, s, noise(1.0, -0.1), noise(0.1, 0.1), noise(1.0, -0.1), noise(0.1, 0.1)))
                elif(s >= 0.8 and s <= 0.9):# if the car is to the right of the track
                    file.write('%f   %f   %f   %f   %f   %f\n' %(f, s, noise(0.1, 0.1), noise(1.0, -0.1), noise(1.0, -0.1), noise(0.1, 0.1)))
                else:
                    file.write('%f   %f   %f   %f   %f   %f\n' %(f, s, noise(0.1, 0.1), noise(1.0, -0.1), noise(1.0, -0.1), noise(0.1, 0.1)))

                    
            elif(f > 0.6):# if the car is angled to a higher degree than the waypoint
                if( s > 0.3 and s < 0.7):
                   file.write('%f   %f   %f   %f   %f   %f\n' %(f, s,  noise(0.1, 0.1), noise(1.0, -0.1), noise(1.0, -0.1), noise(0.1, 0.1)))
                elif(s >= 0.7):
                    file.write('%f   %f   %f   %f   %f   %f\n' %(f, s, noise(0.1, 0.1), noise(1.0, -0.1), noise(1.0, -0.1), noise(0.1, 0.1)))
                elif(s <= 0.3):
                    file.write('%f   %f   %f   %f   %f   %f\n' %(f, s, noise(1.0, -0.1), noise(0.1, 0.1), noise(1.0, -0.1), noise(0.1, 0.1)))

            else:# if the car is angled to a lower degree than the waypoint
                if( s > 0.3 and s < 0.7):
                    file.write('%f   %f   %f   %f   %f   %f\n' %(f, s, noise(1.0, -0.1), noise(0.1, 0.1), noise(1.0, -0.1), noise(0.1, 0.1)))
                elif(s >= 0.7):
                    file.write('%f   %f   %f   %f   %f   %f\n' %(f, s, noise(0.1, 0.1), noise(1.0, -0.1), noise(1.0, -0.1), noise(0.1, 0.1)))
                elif(s <= 0.3):
                    file.write('%f   %f   %f   %f   %f   %f\n' %(f, s, noise(1.0, -0.1), noise(0.1, 0.1), noise(1.0, -0.1), noise(0.1, 0.1)))
                                
        elif(f < 0.3): # if the car is angled to a higher degree than the waypoint
            if(f > 0.1):
                if(s > 0.3 and s < 0.7):
                    file.write('%f   %f   %f   %f   %f   %f\n' %(f, s, noise(0.1, 0.1), noise(1.0, -0.1), noise(0.1, 0.1), noise(1.0, -0.1)))
                elif(s <= 0.3):
                    file.write('%f   %f   %f   %f   %f   %f\n' %(f, s, noise(1.0, -0.1), noise(0.1, 0.1), noise(1.0, -0.1), noise(0.1, 0.1)))
                else:
                    file.write('%f   %f   %f   %f   %f   %f\n' %(f, s, noise(1.0, -0.1), noise(0.1, 0.1), noise(1.0, -0.1), noise(0.1, 0.1)))
            else:
                file.write('%f   %f   %f   %f   %f   %f\n' %(f, s, noise(0.1, 0.1), noise(1.0, -0.1), noise(0.1, 0.1), noise(1.0, -0.1)))
        elif(f > 0.7): # if the car is angled to a lower degree than the waypoint
            if(f < 0.9):
                if(s > 0.3 and s < 0.7):
                    file.write('%f   %f   %f   %f   %f   %f\n' %(f, s, noise(0.1, 0.1), noise(1.0, -0.1), noise(1.0, -0.1), noise(0.1, 0.1)))
                elif(s >= 0.7):
                    file.write('%f   %f   %f   %f   %f   %f\n' %(f, s, noise(1.0, -0.1), noise(0.1, 0.1), noise(0.1, 0.1), noise(1.0, -0.1)))
                else:
                    file.write('%f   %f   %f   %f   %f   %f\n' %(f, s, noise(0.1, 0.1), noise(1.0, -0.1), noise(1.0, -0.1), noise(0.1, 0.1)))
            else:
                file.write('%f   %f   %f   %f   %f   %f\n' %(f, s, noise(0.1, 0.1), noise(1.0, -0.1), noise(0.1, 0.1), noise(1.0, -0.1)))
                    
        

        



main()
