# Untitled - By: doee - 周三 7月 7 2021

import sensor, image, time,lcd,struct,math
from pyb import UART

sensor.reset()
#sensor.set_pixformat(sensor.RGB565)
#sensor.set_framesize(sensor.QVGA)
sensor.set_pixformat(sensor.GRAYSCALE) # or sensor.GRAYSCALE
sensor.set_framesize(sensor.QQVGA2) # Special 128x160 framesize for LCD Shield.

sensor.skip_frames(time = 2000)
sensor.set_auto_gain(False) # must be turned off for color tracking
sensor.set_auto_whitebal(False) # must be turned off for color tracking

clock = time.clock()

uart = UART(3, 115200, timeout_char=0)                         # init with given baudrate
uart.init(115200, bits=8, parity=None, stop=1, timeout=0,timeout_char=0,read_buf_len=256) # init with given parameters

lcd.init() # Initialize the lcd screen.

#def uart_DataSend():
def find_target(blobs):
    for blob in blobs:
        w = blob.rect()[2];
        h = blob.rect()[3];
        x = blob.rect()[0];
        y = blob.rect()[1];
        if w > 3 and w < 9 and h > 3 and h < 9 and x >= 34 and x<=104 and y >= 18 and y <= 119:
            target = blob
            return target
def find_max(blobs):
    max_size=0
    for blob in blobs:
        if blob.pixels() > max_size:
            max_blob=blob
            max_size = blob.pixels()
    return max_blob
# Color Tracking Thresholds (Grayscale Min, Grayscale Max)
# The below grayscale threshold is set to only find extremely bright white areas.
thresholds = (102, 255)
while(True):
    clock.tick()
    img = sensor.snapshot()
    # print(clock.fps())
    blobs = img.find_blobs([thresholds])
    if blobs:
        target=find_target(blobs)
        if target:
        #for blob in img.find_blobs([thresholds[threshold_index]], pixels_threshold=200, area_threshold=200, merge=True):
            # These values are stable all the time.
            img.draw_rectangle(target.rect())
            img.draw_cross(target.cx(), target.cy())
            data=bytearray([target.cx(),target.cy()])
            uart.write(data+'\r\n')
            # Note - the blob rotation is unique to 0-180 only.
            img.draw_keypoints([(target.cx(), target.cy(), int(math.degrees(target.rotation())))], size=20)
        else:
            data=bytearray([-1,-1])
            uart.write(data+'\r\n')
    else:
        data=bytearray([-1,-1])
        uart.write(data+'\r\n')
    # lcd.display(img) # Take a picture and display the image.
    print(clock.fps())
        #uart.readinto(buf)  # read and store into the given buffer
