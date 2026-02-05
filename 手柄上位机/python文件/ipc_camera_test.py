import cv2
import numpy as np
import time
import math



null = 0


video = 'rtsp://admin:admin@172.20.10.3/live/chn=0'#摄像头ip
while True:
    cap = cv2.VideoCapture(video)
    if(cap.isOpened()==True):
        time.sleep(3)
        break
    else:
        continue
while (1):
    ret, frame = cap.read()
    #frame_rate = int(cap.get(cv2.CAP_PROP_FPS))
    #print(f"视频帧速率: {frame_rate} 帧/秒")
    if frame is None:
        null += 1
        if null >=500: break
        continue
    else:
        null = 0
        # 转化为灰度图
        if ret == False:  # 如果是最后一帧这个值为False
            break
        
        cv2.imshow("dst",frame)
        if cv2.waitKey(10) & 0xFF == ord('q'):
            break

        
        
# 释放清理
cap.release()
cv2.destroyAllWindows()
