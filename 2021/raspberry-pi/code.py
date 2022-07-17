#!/usr/bin/env python

# This is the driver for the vision solution code for the summmerbot.
import argparse
import numpy as np
import cv2 as cv
import cv2.aruco as aruco


# # Select a marker from the predefined dictionary and save it to a file.

# img = cv.imread('marker23.png')

# corners, ids, rejectedImgpoints = aruco.detectMarkers(img, ARU_DICT, parameters=ARU_PARAM)

# print(f"Marker IDs detected: {ids}")




# px = img [0,0]
# print(px)

def parse_arguments():
    """
    Parses arguments and makes you happy.
    """
    argparser = argparse.ArgumentParser(prog="code", description="Vision component of 2021 summerbot.")
    argparser.add_argument("-g",
                           "--generate",                           
                           type=int, 
                           metavar="MARKER_ID", 
                           help="Generate an AruCo marker image using the given ID from 0-49.")
    argparser.add_argument("-o", 
                           "--output",
                           default="aruco.png",
                           metavar="FILE_NAME",
                           help="This is to find out the name of the file which has the marker saved.")
    arg_list = argparser.parse_args()
    
    if arg_list.generate:
        if arg_list.generate >= 50 or arg_list.generate < 0:
            print(f"Error \"{arg_list.generate}\" is not in the suitable range: 0-49")
            exit(1)
        generate_aruco_marker(marker_id=arg_list.generate, file_name=arg_list.output)
        print(f"Wrote \"{arg_list.output}\"")
    else:
        # -g was not passed in, so -o is an error.
        if arg_list.output:
            print("Error: -o is not valid unless -g is present")
            exit(1)

def generate_aruco_marker(marker_id, file_name):
    """
    Pulls an AruCo marker image from the AruCo marker dictionary and saves it into disk.
    
    Arguments:
    - marker_id: This is the ID of the Aruco Marker to generate from DICT_4X4_50.
    - file_name: This is the name of the file which has the marker saved into it.
    """
    ARU_PIXEL_SIZE = 300  # This number is the size of the marker image in pixels.
    ARU_BORDER_WIDTH = 1 # This is the width of the border (TODO: is this in pixels or grid cells?)
    # In order for aruco to work, we need to find the predefined dictionary.
    ARU_DICT = aruco.Dictionary_get(aruco.DICT_4X4_50)
    ARU_PARAM = aruco.DetectorParameters_create()

    # Writing the image for disk.
    marker_image = aruco.drawMarker(ARU_DICT, marker_id, ARU_PIXEL_SIZE)
    cv.imwrite(file_name, marker_image)

if __name__ == "__main__":
    parse_arguments()