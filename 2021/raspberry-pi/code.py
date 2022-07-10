# This is the driver for the vision solution code for the summmerbot.
import numpy as np
import cv2 as cv
import cv2.aruco as aruco

ARU_ID = 15          # The range of the dictionary should be between 0 - 49.
ARU_PIXEL_SIZE = 300  # This number is the size of the marker image in pixels.
ARU_BORDER_WIDTH = 1 # This is the width of the border (TODO: is this in pixels or grid cells?)

# In order for aruco to work, we need to find the predefined dictionary.
ARU_DICT = aruco.Dictionary_get(aruco.DICT_4X4_50)
ARU_PARAM = aruco.DetectorParameters_create()

# Writing the image for disk.
marker_image = aruco.drawMarker(ARU_DICT, ARU_ID, ARU_PIXEL_SIZE)
cv.imwrite("foo.png", marker_image)

# # Select a marker from the predefined dictionary and save it to a file.

# img = cv.imread('marker23.png')

# corners, ids, rejectedImgpoints = aruco.detectMarkers(img, ARU_DICT, parameters=ARU_PARAM)

# print(f"Marker IDs detected: {ids}")




# px = img [0,0]
# print(px)
