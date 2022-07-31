1. Capture the image
2. Determine the distance of the marker
3. Determine speed based on the distance of the marker
4. If the position of the marker is positive on the x axis, turn the camera clockwise, if negative, turn counter-clockwise
5. Use the position and distance to determine the drive and turn speed


Installation & Running
----------------------
1. Visit https://www.python.org/downloads/release/python-3105/ and download a python installer.
    - On school computers, download the Windows installer (64-bit) file .
2. Run `pip3 install -r requirements.txt`.
3. To run the code, type `python3 code.py`.


ChArUco Calibration Process
---------------------------

As we understand it, [the procedure](https://github.com/kyle-bersani/opencv-examples/blob/master/CalibrationByCharucoBoard/CalibrateCamera.py)
appears to be:

1. Print out a ChAruco board
2. Take pictures of it at different angles
3. Set allCharucoCorners = [], allCharucoIds = []
4. For each picture:
    1. Convert image to grayscale:
        ```python
        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        ```
    2. Detect all the ArUco markers in the grayscale ChArUco image:
        ```python
        corners, ids, rejected = aruco.detectMarkers(image=gray,
                                                     dictionary=ARUCO_DICT)
        ```
    3. _Optional_: Draw borders around each ArUco image in the frame
        ```python
        view_img = aruco.drawDetectedMarkers(image=img,
                                             corners=corners)
        ```
    4. Get the ChAruco boards corners using the embedded aruco board corners:
        ```python
        corner_count, charuco_corners, charuco_ids = 
            aruco.interpolateCornersCharuco(markerCorners=corners,
                                            markerIds=ids,
                                            image=gray,
                                            board=CHARUCO_BOARD)
        ```
    5. Use the corner count to determine if the photo is a valid photo of a ChArUco.
        ```python
        if corner_count > CHARUCO_THRESHOLD:
        ```
    6. Add the corners and aruco ids to the calibration arrays
        ```python
            allCharucoCorners.append(charuco_corners)
            allCharucoIds.append(charuco_ids)
        ```       
5. Use the appended aruco corners and ids to call the camera calibration function
        ```python
            calibration, cameraMatrix, distCoeffs, rvecs, tvecs = 
                aruco.calibrateCameraCharuco(charucoCorners=corners_all,
                                             charucoIds=ids_all,
                                             board=CHARUCO_BOARD,
                                             imageSize=image_size,
                                             cameraMatrix=None,
                                             distCoeffs=None)
        ```
6. Done. Results are stored in `cameraMatrix` & `distCoeffs`.

See Also
--------
* [OpenCV C++ API](https://docs.opencv.org/4.x/d9/d6a/group__aruco.html)
   - The official Python APi appears to be undocumented, but the functions are all the
        same as the C++ version, so this is still useful as a reference.
* [Background on what ArUco markers are and how to generate them](https://docs.opencv.org/4.x/d5/dae/tutorial_aruco_detection.html)