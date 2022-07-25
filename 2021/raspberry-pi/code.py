#!/usr/bin/env python

# This is the driver for the vision solution code for the summmerbot.
import argparse
import numpy as np
import cv2 as cv
import cv2.aruco as aruco

ARU_PIXEL_SIZE = 300                               # This number is the size of the marker image in pixels.
ARU_BORDER_WIDTH = 1                               # This is the width of the border (TODO: is this in pixels or grid cells?)
ARU_DICT = aruco.Dictionary_get(aruco.DICT_4X4_50) # In order for aruco to work, we need to find the predefined dictionary.
ARU_PARAM = aruco.DetectorParameters_create()

# px = img [0,0]
# print(px)


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
                           type=int,
                           metavar="MARKER_ID",
                           help="Generate an AruCo marker image using the given ID from 0-49.")
    modegroup_2 = argparser.add_argument_group('Other Options')
    modegroup_2.add_argument("-o",
                           "--output",
                           default="aruco.png",
                           metavar="FILE_NAME",
                           help="This is to find out the name of the file which has the marker saved.")
    arg_list = argparser.parse_args()

    if arg_list.generate is not None:
        if arg_list.generate >= 50 or arg_list.generate < 0:
            print(f"Error \"{arg_list.generate}\" is not in the suitable range: 0-49")
            exit(1)
        generate_aruco_marker(marker_id=arg_list.generate, file_name=arg_list.output)
        print(f"Wrote \"{arg_list.output}\"")

    elif arg_list.test is not None:
        detect_all_markers(arg_list.test, view_results=True)
    else:
        # -g was not passed in, so -o is an error.
        if arg_list.output:
            print("Error: -o is not valid unless -g is present")
            exit(1)


def generate_aruco_marker(marker_id, file_name):
    """
    Pulls an AruCo marker image from the AruCo marker dictionary and saves it
    into disk.

    Arguments:
    - marker_id: This is the ID of the Aruco Marker to generate from
                 DICT_4X4_50.
    - file_name: This is the name of the file which has the marker saved into
                 it.
    """
    # Writing the image for disk.
    marker_image = aruco.drawMarker(ARU_DICT, marker_id, ARU_PIXEL_SIZE)
    cv.imwrite(file_name, marker_image)


def detect_all_markers(image_file_name, view_results: bool):
    """
    Detects all ArUco markers in the given image and returns interesting data
    about them.

    Arguments:
    - image_file_name: The input image.
    - view_results: If True, an interactive window will appear that shows the
                    source image and the detected and rejected markers.

    Returns:
    - Returns a list of (id, x, y, z) tuples with:
      * id: The ID of the marker that was successfully detected
      * x: The horizontal position of the marker's center relative to the
           center of the image.  Negative values are left of center.
      * y: The vertical position of the marker's center relative to the center
          of the image.  Negative values are above the center.
      * z: The depth of the marker's center relative to the center of the
           image.  These values will always be positive.

      Together, (x, y, z) form a translation vector between the center of the
      image and the center of the marker.
    """
    img = cv.imread(image_file_name)

    copy_of_img = img.copy()
    copy_of_img = scale_image(copy_of_img, 0.33)
    corners, ids, rejectedImgpoints = aruco.detectMarkers(copy_of_img, ARU_DICT, parameters=ARU_PARAM)

    cv.aruco.drawDetectedMarkers(copy_of_img, corners, ids)

    print(f"Marker IDs detected: {ids}")
    for i, corner in enumerate(corners):
        print(f"Marker {i}:")
        for point in corner:
            print(f"Corner {i}: x={point[0]}, y={point[1]}")

    if view_results:
        cv.imshow("string", copy_of_img)
        cv.waitKey(0)
        cv.destroyAllWindows()


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
    final_img = cv.resize(img, (new_width, new_height),
                          interpolation=cv.INTER_AREA)

    print(f"Modified dimensions: {img.shape[1]} x {img.shape[0]}")
    return final_img


if __name__ == "__main__":
    parse_arguments()
