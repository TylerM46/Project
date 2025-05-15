import pigpio
import time

pi = pigpio.pi()
if not pi.connected:
    exit()

#setting up arrays
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




# Call the callback to read the GPIO pin at high speed
cb = pi.callback(GPIO_PIN, pigpio.EITHER_EDGE, read_gpio)




#Sleep Time + Set timings values
duration = 0.5  # seconds
bitlength = 50    #every 0.1 seconds resync, 0.1/50μs = 2000 bits (might even do more resyncs), 20khz is 50μs, μs seems to be unit of time array, T = 0.00005s
time.sleep(duration) #duration in seconds   
# Stop the callback and pigpio connection
cb.cancel()
pi.stop()





#Start time, Count number of bit lengths, work out curent remainder  
#This Is Psuedo Sampling Every Bit interval 
#Time is continuous line, you add onto remainder to as see how many new bits the time spans
#take those reading points as the current value, leave the left over
#test value for how large remainder
for i in range(len(timearray)-1):       #!!POSSIBLY ADD OFF SET TO READING SO LESS NEAR CROSS ZONE!!
    if i==0:                            
        timestart=timearray[0]          
        remainder=0                        
        resyncCounter = 0               
        resynctrigger =0                

    #calculate time lengths    
    itime = timearray[i+1] - timearray[i]  #length of this mode (0,1)
    totaltime = itime + remainder          #time passed since last reading
    
    #bit length and appending to data array
    bitCoverage = totaltime // bitlength   #number of bits of current mode    
    for b in range(bitCoverage):
        if (dataarray[i] == 1): #inversion in system
            data.append(0)      #add corresponding number of 0s 
        else:
            data.append(1)      #add corresponding number of 1s

    remainder = totaltime - bitCoverage*bitlength #update remainder


    #check resync   
    resyncCounter += itime        #add current length      
    if(resyncCounter > bitlength*1000):    #after 1000 bits resync the to tick change time (might make more frequent)  
        if (remainder> 14):                #Lets do 1000T = 0.05s or 50*1000=50,000μs
            #add bit perhaps?
            resynctrigger += 1
        remainder = 0                       #same as setting T start to current swtich    



#original method.
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
            


readdata = ''.join(map(str,data))
print(readdata)    

#implement 10 reduction


#extraction of data:
def extractdata(binary_string, pattern):
    positions = []   #array of instance positions 
    start = 0

    # Find all start positions of the pattern
    while True:
        idx = binary_string.find(pattern, start)
        if idx == -1:
            break
        positions.append(idx)
        start = idx + 1  # allow overlapping

    # Extract sequences between each pair of occurrences
    results = []
    for i in range(len(positions) - 1):
        start_idx = positions[i] + len(pattern)
        end_idx = positions[i + 1]
        results.append(binary_string[start_idx:end_idx])

    return results

# Example usage
pattern = "10101"
segments = extractdata(readdata, pattern)

print(segments)



