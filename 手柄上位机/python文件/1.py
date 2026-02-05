#该代码是用于改变水下图像的色调的
import cv2
import numpy as np

# RTSP URL for the camera
rtsp_url = "rtsp://admin:admin@172.20.10.3/live/chn=0"

# Open the RTSP stream
cap = cv2.VideoCapture(rtsp_url)

if not cap.isOpened():
    print("Error: Could not open video stream.")
    exit()

while True:
    ret, frame = cap.read()

    if not ret:
        print("Error: Could not read frame.")
        break

    # Apply image enhancement (similar to your previous code)
    img_hls = cv2.cvtColor(frame, cv2.COLOR_BGR2HLS)
    img_hls[:, :, 1] = np.where(img_hls[:, :, 1] <= 150, img_hls[:, :, 1])
