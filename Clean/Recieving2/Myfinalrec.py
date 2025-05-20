import pigpio
import time
import numpy as np
from PIL import Image

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
        data.append(abs(dataarray[i]-1))
            

    remainder = totaltime - bitCoverage*bitlength #update remainder


    #check resync   
    resyncCounter += itime        #add current length      
    if(resyncCounter > bitlength*100):    #after 100 bits resync the to tick change time (might make more frequent)  
        if (remainder> bitlength/3):                #Lets do 100T = 0.005s or 50*100=5000μs
            data.append(abs(dataarray[i]-1))      #add corresponding number of bits
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

#extraction of data:


def binary_to_image(binary_string, width, height, channels):
    # Convert binary string to byte array
    byte_array = bytearray()
    for i in range(0, len(binary_string), 3):
        byte = binary_string[i:i+3]
        byte_array.append(int(byte, 2) * 32)  # Scale back to original color range

    # Convert byte array to numpy array
    img_array = np.array(byte_array, dtype=np.uint8).reshape((height, width, channels))

    # Create and save image
    img = Image.fromarray(img_array, mode='RGB')
    
    img.save('./flip.jpg')
    print("made it")

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

def flip_every_5th_bit(binary_str):
    """
    Flips every 5th bit (i.e., indices 4, 9, 14, ...) back to correct errors.
    """
    bit_list = list(binary_str)
    for i in range(4, len(bit_list), 5):
        bit_list[i] = '0' if bit_list[i] == '1' else '1'
    return ''.join(bit_list)


# Example usage
pattern = "110010101010101010101100"
segments = extractdata(readdata, pattern)

#Checks if data found
if not segments:
    raise ValueError("No data segments found — check signal or pattern.")

#print(segments,len(segments[0]))
sent = flip_every_5th_bit(segments[0])

width = 25 # Replace with actual width
height = 25  # Replace with actual height
channels = 3  # RGB

#checks data length
expected_length = width * height * channels * 3
if len(sent) != expected_length:
    raise ValueError(f"Incorrect bit length. Got {len(sent)} bits, expected {expected_length}.")


binary_to_image(sent, width, height, channels)

print(sent, len(sent))


