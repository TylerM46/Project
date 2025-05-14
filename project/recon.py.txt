import numpy as np
from PIL import Image

# Example binary string (replace with your actual binary string)
binary_string = "1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,..."  # Add your binary data here
binary_values = binary_string.split(',')

# Convert binary values to pixel values
pixel_values = [int(''.join(binary_values[i:i+8]), 2) for i in range(0, len(binary_values), 8)]

# Replace with actual dimensions and channels
width, height, channels = 100, 100, 3

# Convert pixel values to numpy array
pixel_array = np.array(pixel_values, dtype=np.uint8)
pixel_array = pixel_array.reshape((height, width, channels))

# Create and save the image
image = Image.fromarray(pixel_array, 'RGB')
image.save('reconstructed_image.jpg')
image.show()