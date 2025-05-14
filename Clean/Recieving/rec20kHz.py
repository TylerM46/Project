import pigpio
import time

pi = pigpio.pi()
if not pi.connected:
    exit()

GPIO_PIN = 26  # Replace with your GPIO pin number
timearray = []
dataarray = []
data = []
# Set the GPIO pin as an input
pi.set_mode(GPIO_PIN, pigpio.INPUT)

# Define a callback function to read the GPIO pin
def read_gpio(gpio, level, tick):
    #print(f"GPIO {gpio} level {level} at tick {tick}")
    timearray.append(tick)
    dataarray.append(level)

# Set up the callback to read the GPIO pin at high speed
cb = pi.callback(GPIO_PIN, pigpio.EITHER_EDGE, read_gpio)

# Run for a specified duration
duration = 0.5  # seconds
time.sleep(duration)

# Stop the callback and pigpio connection
cb.cancel()
pi.stop()

for i in range(len(timearray)-1):       #20khz is 50μs, μs seems to be unit of time array, T = 0.00005s
    if i==0:                            #every 0.1 seconds resync, 0.1/50μs = 2000 bits (might even do more resyncs), Lets do 1000T = 0.05s or 50*1000=50,000μs
        timestart=timearray[0]          #work out time difference, sum it to remainder then allocate bits (same as sampling every....)
        remainder=0                     #So the thinking is time is continuous line, you add onto remainder to as see how many new bits the time spans,   
        resyncCounter = 0               #take those reading points as the current value, leave the left over

    #calculate time length (length of that mode )    
    itime = timearray[i+1] - timearray[i]
    totaltime = itime + remainder
    resyncCounter += itime 
    for 
        

    #if (dataarray[i] == 1):    
        
    if(resyncCounter > 50000):#check resync
        if (remainder> 15):
            #add bit perhaps?



#for i in range(len(timearray)-1):
#    itime = timearray[i+1] - timearray[i]    #works out time difference between bit switching 
#    inumber = (itime+25)//50                 #divides the length by bit length  
#    print(itime, inumber, dataarray[i])      #prints time difference, correponding bit length, 1 or 0? 
#    if(15>inumber>25):                       #
#        print("here")
#    for p in range(inumber): #bitflipping
#        if (dataarray[i] == 1):
#            data.append(0)
#        else:
#            data.append(1)
            
result = ''.join(map(str,data))
print(result)    
