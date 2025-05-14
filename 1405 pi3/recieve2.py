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
    print(f"GPIO {gpio} level {level} at tick {tick}")
    timearray.append(tick)
    dataarray.append(level)

# Set up the callback to read the GPIO pin at high speed
cb = pi.callback(GPIO_PIN, pigpio.EITHER_EDGE, read_gpio)

# Run for a specified duration
duration = 0.05  # seconds
time.sleep(duration)

# Stop the callback and pigpio connection
cb.cancel()
pi.stop()



for i in range(len(timearray)-1):
    itime = timearray[i+1] - timearray[i]
    inumber = (itime+5)//10
    print(itime, inumber, dataarray[i])
    for p in range(inumber):
        data.append(dataarray[i])
        
print(data)        
