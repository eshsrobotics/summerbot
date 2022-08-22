#!/usr/bin/env python

# This is the driver for the vision solution code for the summmerbot.
import os.path
import argparse
import sys

import numpy as np
import cv2 as cv
import cv2.aruco as aruco

ARU_PIXEL_SIZE = 300  # This number is the size of the marker image in pixels.
ARU_BORDER_WIDTH = 1 # This is the width of the border (TODO: is this in pixels or grid cells?)

# In order for aruco to work, we need to find the predefined dictionary.
ARU_DICT = aruco.Dictionary_get(aruco.DICT_4X4_50)
ARU_PARAM = aruco.DetectorParameters_create()

# The dimensions for the ChArUco Boards.
#
# Also used to determine if there are enough Aruco squares in an image of a
# ChArUco Board for it to be valid.  There are R * C squares in the checkerboard,
# and half of them -- the white ones -- contain ArUco markers.
CHARUCO_ROWS = 8
CHARUCO_COLUMNS = 8
CHARUCO_THRESHOLD = int((CHARUCO_COLUMNS * CHARUCO_ROWS)/2 * 0.60)

def calibrate(charuco_photo_list):
    """
    Calibrates your camera. Meaning? Meaning we return two matrices: the distCoeffs, which is short 
    for distortionCoefficients, and the cameraMatrix. We do this by utilizing a series of photos.
    These photos were passed into the function, and each of them must contain a ChArUco board generated
    by the -g board.
    
    Please note that most of the information listed above came from this URL: 
    https://docs.opencv.org/3.4/df/d4a/tutorial_charuco_detection.html
    """
    # Need to accumulate the instersections (CharcoCorners) and the 
    # ids of those intersections (CharucoIds) in order to feed the data
    # into calibrateCharucoCorners().
    allCharucoCorners = []
    allCharucoIds = []
    board = get_charuco_board()
    photo_dimensions = None
    
    # We need to display _an_ image at the end of this function call.  However,
    # more than one function could have been passed into us through the 
    # charuco_photo_list.  So _for now_, we concentrate on rendering all the
    # marker metadata on the first image that we receive.  The others can wait.  :).
    IMAGE_INDEX_TO_DISPLAY = 0
    scratch_image = cv.imread(charuco_photo_list[IMAGE_INDEX_TO_DISPLAY])
    
    for i in range(len(charuco_photo_list)):
        photo_file_name = charuco_photo_list[i]
        if not os.path.exists(photo_file_name):
            print(f"Error: \"{photo_file_name}\" does not exist.")
            continue
        
        photo = cv.imread(photo_file_name)
        
        # Convert the photo to grayscale.
        gray = cv.cvtColor(photo, cv.COLOR_BGR2GRAY)
        
        # Detect all the ArUco markers in the grayscale ChArUco image.
        corners, ids, rejectedImgpoints = aruco.detectMarkers(gray, 
                                                              ARU_DICT, 
                                                              parameters=ARU_PARAM)
                                                              
        # Get the corners of the big charuco board(s) using interpolation.
        
        _, charucoCorners, charucoIds = \
            cv.aruco.interpolateCornersCharuco(markerCorners=corners, 
                                               markerIds=ids,
                                               image=gray,
                                               board=board)
        if charucoCorners is not None and charucoIds is not None:
            allCharucoCorners.append(charucoCorners)
            allCharucoIds.append(charucoIds)

        # Draw all ArUco markers identified in the ChAruCo board.
        cv.aruco.drawDetectedMarkers(scratch_image, corners, ids)

        if i == 0:
            photo_dimensions = gray.shape
    
    print(photo_dimensions)
       
    # Get the distCoeffs and the cameraMatrix.
    print(f"The length of the charucoCorners is: {len(allCharucoCorners)}")
    print(f"The length of the charucoIds is: {len(allCharucoIds)}")
    _, cameraMatrix, distCoeffs, _, _ = \
        cv.aruco.calibrateCameraCharuco(allCharucoCorners, 
                                        allCharucoIds, 
                                        board, 
                                        photo_dimensions, 
                                        None, 
                                        None)
    print(f"cameraMatrix={cameraMatrix}distCoeffs={cameraMatrix}")
     
    # Determines if ChArUco board has valid 3-dimensional position information.
    valid, rvec, tvec = cv.aruco.estimatePoseCharucoBoard(charucoCorners, 
                                                          charucoIds, 
                                                          board,
                                                          cameraMatrix,
                                                          distCoeffs,
                                                          None,
                                                          None)
    print(f"Valid {'is' if valid is True else 'is not'} true.")
 
    # Display the aruco markers within the ChArUco board embedded within the first image in our list.
    # This helps provide visual confirmation of the calibration process.
    photo_with_markers = cv.resize(scratch_image, 
                                   (int(scratch_image.shape[1]/3), int(scratch_image.shape[0]/3)), 
                                   interpolation=cv.INTER_AREA)

    cv.imshow("photo_ids", photo_with_markers)
    cv.waitKey(0)
    cv.destroyAllWindows()
    
        
def parse_arguments():
    """
    Parses arguments and makes you happy.
    """
    argparser = argparse.ArgumentParser(prog="code", description="Vision component of 2021 summerbot.")
    modegroup = argparser.add_argument_group('Mode Options', 'You must select one of the following options: ')
    modegroup.add_argument("-t",
                           "--test",
                           metavar="IMAGE",
                           help="Takes a photo as input and detects all of the aruco markers in the photo")
    modegroup.add_argument("-g",
                           "--generate",                           
                           type=str,
                           metavar="MARKER | \"board\"",
                           help="Generate an AruCo marker image using the given ID from 0-49, or generates a ChArUco board.")
    modegroup.add_argument("-c",
                           "--calibrate",
                           nargs='+',
                           metavar="PHOTO",
                           help="Generates camera calibration data based on one or more ChArUco board photos.")
    modegroup_2 = argparser.add_argument_group('Other Options')
    modegroup_2.add_argument("-o", 
                           "--output",
                           default="aruco.png",
                           metavar="FILE_NAME",
                           help="This is to find out the name of the file which has the marker saved.")
    arg_list = argparser.parse_args()
    
    if arg_list.generate is not None:
        if arg_list.generate == "board":
            print("Generating ChArUco")
            charuco_board = get_charuco_board()
            board_img = charuco_board.draw((WIDTH_PIXELS, HEIGHT_PIXELS))
            cv.imwrite(arg_list.output, board_img)
        else:
            value = int(arg_list.generate) # TODO: Handle Parse Errors.
            if value >= 50 or value < 0:
                print(f"Error \"{value}\" is not in the suitable range: 0-49")
                exit(1)
            generate_aruco_marker(marker_id=value, file_name=arg_list.output)
        print(f"Wrote \"{arg_list.output}\"")
    
    elif arg_list.test is not None:
        detect_all_markers(arg_list.test)
    elif arg_list.calibrate is not None:
        calibrate(arg_list.calibrate)
    else:
        # -g was not passed in, so -o is an error.
        # if arg_list.output:
        #     print("Error: -o is not valid unless -g is present")
        #     exit(1)
        print("Error: You must pass in at least one of these four arguments: -t, -r, -g, or -c.")
        print(f"Execute {sys.argv[0]} --help for more information.")

def generate_aruco_marker(marker_id, file_name):
    """
    Pulls an AruCo marker image from the AruCo marker dictionary and saves it into disk.
    
    Arguments:
    - marker_id: This is the ID of the Aruco Marker to generate from DICT_4X4_50.
    - file_name: This is the name of the file which has the marker saved into it.
    """
    # Writing the image for disk.
    marker_image = aruco.drawMarker(ARU_DICT, marker_id, ARU_PIXEL_SIZE)
    cv.imwrite(file_name, marker_image)

def detect_all_markers(image_file_name):
    """
    Detects all the AruCo markers in the image, and returns the Marker ID and also the 
    three-dimensional space of the marker.
    """ 
    img = cv.imread(image_file_name)
    corners, ids, rejectedImgpoints = aruco.detectMarkers(img, ARU_DICT, parameters=ARU_PARAM)
    
    copy_of_img = img.copy()
    scale_image(copy_of_img, 0.1)
    cv.aruco.drawDetectedMarkers(copy_of_img, corners, ids)
    cv.imshow("string", copy_of_img)
    cv.waitKey(0)
    cv.destroyAllWindows()

    print(f"Marker IDs detected: {ids}")
    
def scale_image(img, scale_factor):
    width = img.shape[1]
    height = img.shape[0]
    print(f"Original dimensions: {width} x {height}")
    new_width = int(width * scale_factor)
    new_height = int(height * scale_factor)
    print(f"New dimensions: {new_width} x {new_height}")
    
    # Bug: We expected this function to change the dimensions of the
    # input image (img.shape[].)  It turns out that the dimensions are
    # not being modified, so the displayed image is still too large.  This
    # is not acceptable, and we don't know why this is happening yet.
    #
    # References: https://www.tutorialkart.com/opencv/python/opencv-python-resize-image/    
    cv.resize(img, (new_width, new_height), dst=img, interpolation=cv.INTER_AREA)

    print(f"Modified dimensions: {img.shape[1]} x {img.shape[0]}")
    
def get_charuco_board():
    MARKER_LENGTH = 0.7
    SIDE_LENGTH = 1.2
    WIDTH_PIXELS = 900
    HEIGHT_PIXELS = 900
    charuco_board = cv.aruco.CharucoBoard_create(CHARUCO_COLUMNS, CHARUCO_ROWS, SIDE_LENGTH, MARKER_LENGTH, ARU_DICT)
    return charuco_board

if __name__ == "__main__":
    parse_arguments()