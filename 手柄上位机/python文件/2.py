import cv2
import numpy as np
import time
import math
import serial

# 打开摄像头
cap = cv2.VideoCapture("rtsp://admin:admin@172.20.10.3/live/chn=0")  # 0表示默认的摄像头，如果有多个摄像头，可以选择其他编号

min_black_rect_count = 30
u = n = 0
image_count = 0
c1 = [0,360]
c2 = [0,720]
color = (0, 255, 0)  # 绿色
datasend = ""


#串口发送
def com(coms,choicedcom=7):

    label = coms.encode('utf-8')
    #print(label, top, left, bottom, right)
            
    import serial

    ser = serial.Serial(f'COM{choicedcom}', baudrate=115200, timeout=1)
    # 假设 label 是您要发送的字符串
            

    # 将字符串编码为字节字符串
    byte_message = label

    # 发送字节字符串到串口

    if not ser.is_open:
    # 打开串口
        ser.open()
    ser.write(byte_message)
    received_data = ser.readline()
    
    # 将字节数据解码为字符串
    decoded_data = received_data.decode('utf-8')
    
    # 打印接收到的数据
    print("Received:", decoded_data)
    ser.close()
#com("当你看到这段代码说明已经能够传输了",4)




while True:
    # 读取摄像头的帧
    ret, frame = cap.read()
    xc1 = xc2 = 0
    lower_blue = np.array([0, 0, 128], dtype=np.uint8)
    upper_blue = np.array([50, 50, 255], dtype=np.uint8)

    # 创建蓝色掩码
    blue_mask = cv2.inRange(frame, lower_blue, upper_blue)

    # 将蓝色区域设置为白色
    result_frame = frame.copy()
    result_frame[blue_mask > 0] = [255, 255, 255]  # 设置白色
    
    # 将帧转换为灰度图像
    gray_frame = cv2.cvtColor(result_frame, cv2.COLOR_BGR2GRAY)
    
    # 二值化处理
    _, binary_frame = cv2.threshold(gray_frame, 100, 255, cv2.THRESH_BINARY)
    
    # 按下'q'键退出循环
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break
    elif cv2.waitKey(1) & 0xFF == ord('a'):
        # 保存图片
        image_count += 1
        filename = f'C:\\test\\方法2\\black_rect_{image_count}.png'                            
        cv2.imwrite(filename, frame)
        print(f'Saved {filename}')
    frame_width = 1920      #int(cap.get(3))  # 获取宽度属性
    frame_height = 1080     #int(cap.get(4))  # 获取高度属性
    #print(f'分辨率: {frame_width} x {frame_height}')
    ##
    #
    ##下面画x框
    center_x = 960
    rect_width = 1920           #长
    rect_height = 27            #宽
    top_left = (0, int(frame_height*1/3) - rect_height)
    bottom_right = (1920, int(frame_height*1/3)+ rect_height // 2)
    
    thickness = 2  # 矩形边框线条的粗细
    cv2.rectangle(frame, top_left, bottom_right, color, thickness)
    cv2.rectangle(binary_frame, top_left, bottom_right, color, thickness)
    color2 = (255,0 , 255)
    color3 = (255,165,100)
    #第二个框
    top_left2 = (0, int(frame_height*2/3) - rect_height)
    bottom_right2 = (1920, int(frame_height*2/3)+ rect_height // 2)
    cv2.rectangle(frame, top_left2, bottom_right2, color, thickness)
    cv2.rectangle(binary_frame, top_left2, bottom_right2, color, thickness)
    
    # 遍历1x27的矩阵
    for x in range(top_left[0], bottom_right[0]):
        roi = binary_frame[top_left[1]:bottom_right[1], x:x+1]
        roi2 = binary_frame[top_left2[1]:bottom_right2[1], x:x+1]
        
        if np.count_nonzero(roi == 0) >= roi.shape[0] // 1.3:  # 检查是否全黑
            if n == 0:
                x1 = x
            n += 1
        else:
            if 300>= n >= min_black_rect_count:
                x2 = x
                #print(f'Found black rect from x={x1} to x={x2}')
                cv2.rectangle(frame,(x1,int(frame_height*1/3)) , (x2,int(frame_height*1/3)), color2, thickness)
                xc1 = (x1+x2)//2
                
                cv2.circle(frame, ((x1+x2)//2,int(frame_height*1/3)), 8, color3, thickness=cv2.FILLED)
                '''cv2.putText(frame, f'Found black rect from x={x1} to x={x2}',
                            (frame_width - 1700, 500),
                            cv2.FONT_HERSHEY_SIMPLEX, 2, (0, 0, 255), 2)'''
            x1 = x2 = n = 0
        if np.count_nonzero(roi2 == 0) >= roi2.shape[0] // 1.3:  # 检查是否全黑
            if u == 0:
                a1 = x
            u += 1
        else:
            if 300>= u >= min_black_rect_count:
                a2 = x
                #print(f'Found black rect from x={x1} to x={x2}')
                cv2.rectangle(frame,(a1,int(frame_height*2/3)) , (a2,int(frame_height*2/3)), color2, thickness)
                xc2 = (a1+a2)//2
                print(xc1,xc2)
                k = (xc2-xc1)/(360)
                d = abs(360-k*xc1-360)/(1+k**2)**0.5
                angle_radians = math.atan2(k, 1)
                s = math.degrees(angle_radians)
                datasend = "LP {:.2f} {:.2f}".format(s,d)
                
                

                cv2.circle(frame, ((a1+a2)//2,int(frame_height*2/3)), 8, color3, thickness=cv2.FILLED)
            a2 = a1 = u = 0
    print("一帧过去了")
    print(datasend)
    #com(datasend,4)

    
    
            #print("与目标点的距离:", distance_to_target)
            #print("与y轴的角度（度数）:", angle_to_y_axis_degrees)
    #cv2.line(frame,tuple(c1), tuple(c2), color, thickness=2)


            ##
            #
            #
            ##该区域为与下位机通信代码

            #com()

            ##
            #
            #
            ##

    frame = cv2.resize(frame, (1200, 800))
    
    # 显示原始图像和处理后的图像
    cv2.imshow('Original Frame', frame)
    cv2.imshow('Binary Frame', binary_frame)
# 释放摄像头并关闭所有窗口
cap.release()
cv2.destroyAllWindows()
