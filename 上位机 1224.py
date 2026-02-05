# -*- coding: gbk -*-
import pygame
import serial
import sys
import threading
import time


class GamepadToSTM32:
    def __init__(self, port='COM10', baudrate=9600, debug_mode=False):
        self.debug_mode = debug_mode
        self.last_send_time = 0  # 记录上次发送时间
        self.send_interval = 0.05  # 50ms发送间隔（秒）

        # 初始化串口
        try:
            self.ser = serial.Serial(
                port=port,
                baudrate=baudrate,
                bytesize=serial.EIGHTBITS,
                parity=serial.PARITY_NONE,
                stopbits=serial.STOPBITS_ONE,
                timeout=1,
                xonxoff=False,
                rtscts=False,
                dsrdtr=False
            )
            print(f"串口连接成功: {port}, 波特率: {baudrate}")
        except serial.SerialException as e:
            print(f"串口连接失败: {e}")
            sys.exit(1)

        # 初始化Pygame和手柄
        pygame.init()
        pygame.joystick.init()

        if pygame.joystick.get_count() == 0:
            print("未检测到手柄，请连接手柄后重新运行程序")
            sys.exit(1)

        self.joystick = pygame.joystick.Joystick(0)
        self.joystick.init()
        print(f"手柄连接: {self.joystick.get_name()}")

        # 按照你的要求设置按键映射
        self.button_config = {
            'A': (1, '[B1]'),
            'B': (2, '[B2]'),
            'Y': (3, '[B3]'),
            'X': (4, '[B4]'),
            'RB': (9, '[B9]'),
            'LB': (10, '[B10]'),
            'DOWN': (5, '[B5]'),
            'RIGHT': (6, '[B6]'),
            'UP': (7, '[B7]'),
            'LEFT': (8, '[B8]'),
        }

        # 硬件映射
        self.hardware_mapping = {
            'A': 0,
            'B': 1,
            'X': 2,
            'Y': 3,
            'LB': 4,
            'RB': 5,
        }

        # 方向键映射
        self.hat_mapping = {
            (0, 1): 'UP',
            (0, -1): 'DOWN',
            (-1, 0): 'LEFT',
            (1, 0): 'RIGHT',
        }

        self.running = True
        self.button_states = [False] * self.joystick.get_numbuttons()
        self.hat_state = (0, 0)

        # 打印配置信息
        print(f"按钮总数: {self.joystick.get_numbuttons()}")
        print(f"帽子开关总数: {self.joystick.get_numhats()}")
        print(f"轴总数: {self.joystick.get_numaxes()}")

        print("\n按键映射配置:")
        print("-" * 40)
        for name, (logic_id, cmd) in self.button_config.items():
            hw_id = self.hardware_mapping.get(name, "帽子开关")
            print(f"{name:6} -> {cmd:6} (逻辑ID: {logic_id:2}, 硬件ID: {hw_id})")
        print("-" * 40)
        print(f"发送间隔: {self.send_interval*1000}ms")

        if debug_mode:
            print("\n!!! 调试模式已开启 !!!")

        # 启动接收线程
        self.receive_thread = threading.Thread(target=self.receive_data, daemon=True)
        self.receive_thread.start()

    def can_send(self):
        """检查是否达到发送时间间隔"""
        current_time = time.time()
        if current_time - self.last_send_time >= self.send_interval:
            self.last_send_time = current_time
            return True
        return False

    def read_gamepad_data(self):
        pygame.event.pump()

        # 1. 检查普通按钮（A,B,X,Y,LB,RB）
        for btn_name in ['A', 'B', 'X', 'Y', 'LB', 'RB']:
            hw_id = self.hardware_mapping.get(btn_name)
            if hw_id is not None and hw_id < self.joystick.get_numbuttons():
                current_state = self.joystick.get_button(hw_id) == 1

                if current_state and not self.button_states[hw_id]:
                    # 检查是否可以发送
                    if self.can_send():
                        _, cmd = self.button_config[btn_name]
                        if self.debug_mode:
                            print(f"[调试] {btn_name}按下 - 发送:{cmd}")
                        return cmd

                self.button_states[hw_id] = current_state

        # 2. 检查方向键（帽子开关）
        if self.joystick.get_numhats() > 0:
            current_hat = self.joystick.get_hat(0)

            if current_hat != self.hat_state:
                for hat_value, direction in self.hat_mapping.items():
                    if current_hat == hat_value:
                        # 检查是否可以发送
                        if self.can_send():
                            _, cmd = self.button_config[direction]
                            if self.debug_mode:
                                print(f"[调试] {direction}按下 - 发送:{cmd}")
                            self.hat_state = current_hat
                            return cmd

                if current_hat == (0, 0):
                    self.hat_state = current_hat

        return None

    def send_data(self, data_str):
        if self.ser and self.ser.is_open:
            self.ser.write(data_str.encode('gbk'))
            print(f"发送: {data_str}")

    def receive_data(self):
        """接收串口数据"""
        buffer = ""
        while self.running:
            try:
                if self.ser and self.ser.is_open:
                    data = self.ser.read(self.ser.in_waiting or 1)
                    if data:
                        try:
                            text = data.decode('gbk', errors='ignore')
                        except:
                            text = data.decode('utf-8', errors='ignore')
                        buffer += text

                        while '[' in buffer and ']' in buffer:
                            start = buffer.find('[')
                            end = buffer.find(']', start)
                            if end != -1:
                                packet = buffer[start:end + 1]
                                print(f"接收: {packet}")
                                buffer = buffer[end + 1:]
                            else:
                                break
            except:
                pass
            pygame.time.wait(10)

    def run(self):
        print("\n" + "=" * 60)
        print("Xbox手柄控制程序 - 已启动")
        print("=" * 60)
        print("当前按键映射:")
        print("  A     -> [B1]    B     -> [B2]")
        print("  Y     -> [B3]    X     -> [B4]")
        print("  DOWN  -> [B5]    RIGHT -> [B6]")
        print("  UP    -> [B7]    LEFT  -> [B8]")
        print("  RB    -> [B9]    LB    -> [B10]")
        print("=" * 60)
        print(f"发送限制: 每{self.send_interval*1000}ms最多发送一次")
        print("按下手柄按钮发送对应指令，按Ctrl+C停止程序")
        print("=" * 60)

        try:
            while self.running:
                data_str = self.read_gamepad_data()
                if data_str:
                    self.send_data(data_str)
                # 主循环仍保持适当延迟，降低CPU使用率
                pygame.time.wait(10)

        except KeyboardInterrupt:
            print("\n程序被用户中断")
        except Exception as e:
            print(f"发生错误: {e}")
        finally:
            self.close()

    def close(self):
        self.running = False
        if self.ser and self.ser.is_open:
            self.ser.close()
        pygame.quit()
        print("\n资源已释放")


def find_serial_ports():
    """查找可用的串口"""
    import serial.tools.list_ports
    ports = serial.tools.list_ports.comports()
    if not ports:
        print("未找到可用串口")
        return []

    print("可用串口列表:")
    for i, port in enumerate(ports):
        print(f"  [{i}] {port.device} - {port.description}")
    return [port.device for port in ports]


if __name__ == "__main__":
    print("Xbox手柄转串口控制程序")
    print("-" * 40)

    # 查找串口
    ports = find_serial_ports()

    if not ports:
        print("未找到可用串口，程序退出")
        sys.exit(1)

    # 选择串口
    if len(ports) == 1:
        selected_port = ports[0]
        print(f"\n自动选择唯一串口: {selected_port}")
    else:
        print("\n请输入要使用的串口编号 (0, 1, 2...): ")
        try:
            port_index = int(input().strip())
            if 0 <= port_index < len(ports):
                selected_port = ports[port_index]
            else:
                print("输入错误，使用默认COM10")
                selected_port = 'COM10'
        except:
            print("输入错误，使用默认COM10")
            selected_port = 'COM10'

    print(f"\n使用串口: {selected_port}")

    # 创建并运行控制器
    try:
        controller = GamepadToSTM32(
            port=selected_port,
            baudrate=9600,
            debug_mode=False
        )
        controller.run()
    except Exception as e:
        print(f"启动失败: {e}")