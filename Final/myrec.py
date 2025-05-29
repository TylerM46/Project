import pigpio
import time
import numpy as np
from PIL import Image

# Constants (must match transmitter)
GPIO_PIN = 26
BIT_LENGTH = 50               # microseconds per bit
DURATION = 0.5                # sample duration
PILOT = "110010101010101010101100"
HEADER = "1010101100"
PACKET_SIZE = 450             # bits per packet
TOTAL_BITS = 5625             # total expected payload bits
HEADER_TOLERANCE = 2          # allowed bit errors in header match
SEARCH_WINDOW = 20            # search window for next header

# Setup pigpio
pi = pigpio.pi()
if not pi.connected:
    exit()
pi.set_mode(GPIO_PIN, pigpio.INPUT)

# Collect edges
timearray = []
dataarray = []

def read_gpio(gpio, level, tick):
    timearray.append(tick)
    dataarray.append(level)

cb = pi.callback(GPIO_PIN, pigpio.EITHER_EDGE, read_gpio)
time.sleep(DURATION)
cb.cancel()
pi.stop()

# Convert edge data to bitstream
data = []
remainder = 0
for i in range(len(timearray) - 1):
    itime = timearray[i + 1] - timearray[i]
    totaltime = itime + remainder
    bitCoverage = totaltime // BIT_LENGTH
    for _ in range(bitCoverage):
        data.append(abs(dataarray[i] - 1))  # Inverted logic
    remainder = totaltime - bitCoverage * BIT_LENGTH

readdata = ''.join(map(str, data))

# Utility: compute Hamming distance
def hamming_distance(a, b):
    return sum(x != y for x, y in zip(a, b))

# Find pilot pattern to sync start
def find_pilot(data, pilot):
    idx = data.find(pilot)
    if idx == -1:
        raise ValueError("Pilot pattern not found")
    return idx + len(pilot)

# Try to find a near-match to the packet header
def find_next_header(data, start_idx, header, tolerance, window):
    for i in range(start_idx, min(len(data) - len(header), start_idx + window)):
        segment = data[i:i+len(header)]
        if hamming_distance(segment, header) <= tolerance:
            return i + len(header)  # Return index after header
    return -1

# Extract all packets to form exact payload
def extract_payload(data, pilot, header, total_bits, packet_size):
    cursor = find_pilot(data, pilot)
    payload = ""
    while len(payload) < total_bits:
        header_start = find_next_header(data, cursor, header, HEADER_TOLERANCE, SEARCH_WINDOW)
        if header_start == -1:
            raise ValueError("Packet header not found or too corrupted")
        packet_data = data[header_start:header_start + packet_size]
        if len(packet_data) < packet_size:
            raise ValueError("Incomplete packet at stream end")
        payload += packet_data
        cursor = header_start + packet_size
    return payload[:total_bits]

# Undo flipping of every 5th bit
def flip_every_5th_bit(binary_str):
    flipped = list(binary_str)
    for i in range(4, len(flipped), 5):
        flipped[i] = '0' if flipped[i] == '1' else '1'
    return ''.join(flipped)

# Convert final binary string to image
def binary_to_image(binary_string, width, height, channels):
    byte_array = bytearray()
    for i in range(0, len(binary_string), 3):
        chunk = binary_string[i:i+3]
        byte_array.append(int(chunk, 2) * 32)
    img_array = np.array(byte_array, dtype=np.uint8).reshape((height, width, channels))
    img = Image.fromarray(img_array, mode='RGB')
    img.save('./flip.jpg')
    print("Image saved as flip.jpg")

# Main processing logic
try:
    print("Searching for pilot and headers...")
    sent = extract_payload(readdata, PILOT, HEADER, TOTAL_BITS, PACKET_SIZE)
    print("Payload extracted, flipping every 5th bit...")
    sent = flip_every_5th_bit(sent)
    if len(sent) != TOTAL_BITS:
        raise ValueError(f"Wrong bit length: got {len(sent)} instead of {TOTAL_BITS}")
    print("Reconstructing image...")
    binary_to_image(sent, 25, 25, 3)
    print("Success: Image reconstructed from transmission.")
except Exception as e:
    print("Error:", e)
