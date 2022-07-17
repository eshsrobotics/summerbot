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


See Also
--------
* [OpenCV C++ API](https://docs.opencv.org/4.x/d9/d6a/group__aruco.html)
   - The official Python APi appears to be undocumented, but the functions are all the 
        same as the C++ version, so this is still useful as a reference.
* [Background on what ArUco markers are and how to generate them](https://docs.opencv.org/4.x/d5/dae/tutorial_aruco_detection.html)