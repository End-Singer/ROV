using System;//基本的系统类
using System.Net;
using System.Net.Sockets;
//using System.Security.Cryptography.Xml;
using System.Windows.Forms;//窗体应用程序的类
using SharpDX.XInput;//
using System.Text;
using static System.Windows.Forms.AxHost;//Xbox 控制器的输入库
using System.Reflection.Emit;
using System.IO.Ports;
using System.Diagnostics;
using SharpDX;
using System.Runtime.InteropServices;
using 实验4.Properties;
using System.Threading;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Windows.Forms.DataVisualization.Charting;
using System.Text.RegularExpressions;
//using System.IO.Ports;//串口的库

namespace 实验4
{
    public partial class Form1 : Form
    {
        private Controller controller;//xbox控制对象
        private Gamepad currentGamepadState;//xbox状态
        private System.Windows.Forms.Timer sendTimer = new System.Windows.Forms.Timer();//用来向串口自动发送手柄信息

        public Form1()//绘制一个剧本，但还未实例化
        {
            InitializeComponent();//初始化（并生成）窗体组件
            InitializeController();//初始化（并调用）xbox控制器
            InitializeSocket();//传送给bang-pi
            ConnectToServer();
            timer1.Start();
            sendTimer.Interval = 100; // 设置定时器间隔（以毫秒为单位，这里是1秒）
            sendTimer.Tick += new EventHandler(sendTimer_Tick); // 添加定时器 Tick 事件处理程序            
            InitializeStateSendTimer();
            
        }

        /// <summary>检查手柄是否连接
        /// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// </summary>
        private void InitializeController()
        {
            controller = new Controller(UserIndex.One);//实例化xbox控制器，UserIndex.One表示连接的第一个手柄
            //为了防止摇杆报错,污染输出       
            b[14] = 999;
            b[16] = 999;
        }
        
        private bool isConnected = true;

        private DateTime time1; // 记录计时器启动时间1
        private DateTime time2; // 记录计时器启动时间2
        int cdcs = 0;
        public void changecolor()
        {
            if (isConnected)
            {
                // 当 isConnected 变为 True 时加载并设置指定的图片
                pictureBox2.Image = Properties.Resources.真正的绿灯; // 替换为您的图片路径
            
            }
            else
            {
                pictureBox2.Image = Properties.Resources.真正的红灯; // 替换为您的图片路径
                //pictureBox2.Image = System.Drawing.Image.FromFile("Resources\\真正的红灯.png"); // 替换为您的图片路径
                // 在 isConnected 为 False 时可以执行其他操作，或者清空图像

            }
        }
        

        private double GetLeftTriggerValue()
        {
            // 获取左扳机（左触发器）的按下程度
            float L = currentGamepadState.LeftTrigger / 255f;
            return (double)Math.Round(L, 1)*100;
        }

        private double GetRightTriggerValue()
        {
            // 获取右扳机（右触发器）的按下程度
            float R = currentGamepadState.RightTrigger / 255f;
            return (double)Math.Round(R, 1)*100;
        }
        private bool IsButtonPressed(GamepadButtonFlags button)//判断某个按钮是否按下，返回是否
        {
            return (currentGamepadState.Buttons & button) == button;//&表示按位and
        }

        //下面是一个关键的方法timer1_Tick,这个处理时间的方法被Form1.Designer.cs预定了，
        //每隔一段时间（随时）监视手柄状况并呈现，我们将过去和现在的手柄信息储存在列表b和a中
        int[] a = new int[30];//手柄基本信息20个变量
        int[] b = new int[30];
        

        public double yjnangle(double angle0, double a1, double b1)
        {
            if ((a1 > 0 && b1 > 0) || (a1 == 0 && b1 > 0))
            {
                angle0 = 90 - angle0;
            }
            else if ((a1 < 0 && b1 > 0) || (a1 < 0 && b1 == 0))
            {
                angle0 = -angle0 + 90;
            }
            else if (a1 < 0 && b1 < 0)
            {
                angle0 = -270 - angle0;
            }
            else if ((a1 > 0 && b1 < 0) || (a1 > 0 && b1 == 0) || (a1 == 0 && b1 < 0))
            {
                angle0 = 90 - angle0;
            }

            return angle0;
        }
        public double sffw(double angle0, double a1, double b1 )
        {
            String f = "Z";
            if (a1 == b1 & a1 == 0)
            {
                angle0 = 999;
            }
            else
            {
                if (angle0 <= 135 && angle0 >= 45)
            {
                angle0 = 0;
                f = "N";
            }
            else if (angle0 >= -135 && angle0 <= -45)
            {
                angle0 = 180;
                f = "S";
            }
            else if (angle0 <= 45 && angle0 >= -45)
            {
                angle0 = 90;
                f = "E";
            }
            else 
            {
                angle0 = -90;
                f = "W";
            }

            }
            
            

            return angle0;
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            try
            {
                UpdateControllerState();
                isConnected = true;
                Console.WriteLine("重新连接手柄成功");
            }
            catch (SharpDXException ex)
            {
                isConnected = false;
                Console.WriteLine("连接手柄时发生错误：");
            }
            changecolor();

            // 更新手柄状态
            float x1 = currentGamepadState.LeftThumbX;
            float y1 = currentGamepadState.LeftThumbY;
            float x2 = currentGamepadState.RightThumbX;
            float y2 = currentGamepadState.RightThumbY;

            // 处理摇杆数据
            if (Math.Abs(x1) <= 1600) x1 = 0;
            if (Math.Abs(y1) <= 1600) y1 = 0;
            if (Math.Abs(x2) <= 1600) x2 = 0;
            if (Math.Abs(y2) <= 1600) y2 = 0;

            double angle1 = 999, angle2 = 999;
            double abs1 = 0, abs2 = 0;

            if (x1 != 0 || y1 != 0)
            {
                angle1 = Math.Round(Math.Atan2(y1, x1), 4) * 180 / 3.1415;
                abs1 = Math.Sqrt(x1 * x1 + y1 * y1) / 410;
            }

            if (x2 != 0 || y2 != 0)
            {
                angle2 = Math.Round(Math.Atan2(y2, x2), 4) * 180 / 3.1415;
                abs2 = Math.Sqrt(x2 * x2 + y2 * y2) / 410;
            }

            // 更新摇杆显示
            label4.Text = angle1.ToString("F0") + " " + (int)abs1;
            label5.Text = angle2.ToString("F0") + " " + (int)abs2;

            // 更新扳机显示
            label6.Text = GetLeftTriggerValue().ToString();
            label7.Text = GetRightTriggerValue().ToString();

            // 更新按键状态
            GamepadButtonFlags pressedButtons = currentGamepadState.Buttons;
            for (int i = 0; i < 20; i++)
            {
                a[i] = (int)(pressedButtons & (GamepadButtonFlags)(1 << i));
            }

            // 更新摇杆和扳机状态
            a[14] = (int)angle1;
            a[15] = (int)abs1;
            a[16] = (int)angle2;
            a[17] = (int)abs2;
            a[18] = (int)GetLeftTriggerValue();
            a[19] = (int)GetRightTriggerValue();
        }
        private System.Windows.Forms.Timer stateSendTimer = new System.Windows.Forms.Timer();

        private void InitializeStateSendTimer()
        {
            stateSendTimer.Interval = 333; // 333ms，每秒发送3次
            stateSendTimer.Tick += new EventHandler(StateSendTimer_Tick);
            stateSendTimer.Start();
        }

        private void StateSendTimer_Tick(object sender, EventArgs e)
        {
            try
            {
                // 构建按键状态消息
                StringBuilder message = new StringBuilder();
                for (int i = 0; i < 20; i++)
                {
                    message.Append($"JSB {i} {(a[i] != 0 ? "Press" : "Release")}\n");
                }

                // 添加摇杆和扳机状态
                message.Append($"JSV1 {a[14]} {a[15]}\n");
                message.Append($"JSV2 {a[16]} {a[17]}\n");
                message.Append($"BJ {a[18]} {a[19]}\n");

                // 发送消息
                SendMessage(message.ToString());
            }
            catch (Exception ex)
            {
                Console.WriteLine("发送按键状态时发生错误：" + ex.Message);
            }
        }

        //}
        /// <summary>IP发送信息
        /// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// </summary>
        //树莓派服务器的 IP 地址和端口号
        private const string RaspberryPiIp = "192.168.66.2";
        private const int RaspberryPiPort = 5001;
        private Socket clientSocket;
        private void UpdateControllerState()//获取label3.Text并储存到currentGamepadState中
        {
            SharpDX.XInput.State state = controller.GetState();
            currentGamepadState = state.Gamepad;
            string controllerInfo = label3.Text;
            //SendMessage(controllerInfo);
        }
        //下面是另一群重要的方法，目的是向bang-pi发送储存在label3.Text中的手柄信息
        //初始化socket
        private void InitializeSocket()
        {
            clientSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
        }
        //连接到服务器
        private void ConnectToServer()
        {
            try
            {
                // 连接到树莓派服务器
                IPAddress ipAddress = IPAddress.Parse(RaspberryPiIp);
                IPEndPoint remoteEP = new IPEndPoint(ipAddress, RaspberryPiPort);
                clientSocket.Connect(remoteEP);
                Console.WriteLine("已连接到服务器");
                StartReceivingMessages();
            }
            
            catch (Exception ex)
            {
                Console.WriteLine("连接服务器时发生错误：" + ex.Message);
            }
        }
        private void SendMessage(string message)
        {
            try
            {
                // 将消息转换为字节数组
                byte[] buffer = Encoding.UTF8.GetBytes(message);

                // 发送消息到服务器
                clientSocket.Send(buffer);
                Console.WriteLine("已发送消息：" + message);
            }
            catch (Exception ex)
            {
                Console.WriteLine("发送消息时发生错误：" + ex.Message);
            }
        }
        private void DisconnectFromServer()
        {
            clientSocket.Close();
            Console.WriteLine("已断开与服务器的连接");
        }


        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            DisconnectFromServer();
        }
        private void button1_Click(object sender, EventArgs e)
        {
            string message = richTextBox1.Text;

            // 发送到树莓派
            SendMessage(message);

            // 清空richTextBox1
            richTextBox1.Clear();
        }
        private void StartReceivingMessages()
        {
            Thread receiveThread = new Thread(new ThreadStart(ReceiveMessages));
            receiveThread.IsBackground = true;
            receiveThread.Start();
        }

        private void ReceiveMessages()
        {
            try
            {
                while (clientSocket.Connected)
                {
                    byte[] buffer = new byte[1024];  // 假设每次读取1024字节
                    int bytesRead = clientSocket.Receive(buffer);
                    if (bytesRead > 0)
                    {
                        string receivedMessage = Encoding.UTF8.GetString(buffer, 0, bytesRead);

                        // 将接收到的信息更新到richTextBox2
                        Invoke(new Action(() =>
                        {
                            richTextBox2.AppendText(receivedMessage + Environment.NewLine);
                            richTextBox2.SelectionStart = richTextBox2.Text.Length;
                            richTextBox2.ScrollToCaret(); // 自动滚动到最底部
                        }));
                    }
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine("接收消息时发生错误：" + ex.Message);
            }
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {

        }

        private void richTextBox1_TextChanged_2(object sender, EventArgs e)
        {

        }


        /////////////////////////////////////////////////////////////////添加绘制折线图
        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        // 用于存储PID和PIDOUT的数据点
        private List<double> pidData = new List<double>();
        private List<double> pidOutData = new List<double>();
        


        private void richTextBox2_TextChanged_1(object sender, EventArgs e)
        {
            // 获取richTextBox的内容
            string text = richTextBox2.Text;

            // 正则表达式匹配PID或PIDOUT后的数字
            Regex pidRegex = new Regex(@"PID\s+(\d+(\.\d+)?)");
            Regex pidOutRegex = new Regex(@"PIDOUT\s+(\d+(\.\d+)?)");

            // 清空现有的数据
            pidData.Clear();
            pidOutData.Clear();

            // 匹配并提取PID数据
            MatchCollection pidMatches = pidRegex.Matches(text);
            foreach (Match match in pidMatches)
            {
                if (double.TryParse(match.Groups[1].Value, out double pidValue))
                {
                    pidData.Add(pidValue);
                }
            }

            // 匹配并提取PIDOUT数据
            MatchCollection pidOutMatches = pidOutRegex.Matches(text);
            foreach (Match match in pidOutMatches)
            {
                if (double.TryParse(match.Groups[1].Value, out double pidOutValue))
                {
                    pidOutData.Add(pidOutValue);
                }
            }

            // 更新图表数据
            UpdateChartData();


        }

        // 提取以指定字符串开头的数字值
        

        // 绘制折线图
        
        /// <summary>2023.9.3添加了右侧的串口模式
        /// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        ///   //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /// </summary>
        private SerialPort serialPort = new SerialPort();
       

        private void RefreshAvailablePorts()
        {
            // 清空已有的串口列表
            port_cbb.Items.Clear();

            // 获取计算机上可用的串口列表
            string[] availablePorts = SerialPort.GetPortNames();

            // 将可用串口添加到 ComboBox
            port_cbb.Items.AddRange(availablePorts);

            if (port_cbb.Items.Count > 0)
            {
                port_cbb.SelectedIndex = 0; // 默认选择第一个串口
            }
        }

        private void open_btn_Click(object sender, EventArgs e)
        {
            MessageBox.Show($"串口参数：{serialPort.PortName}, 波特率：{serialPort.BaudRate}, 数据位：{serialPort.DataBits}, 校验位：{serialPort.Parity}, 停止位：{serialPort.StopBits}");

            if (!serialPort.IsOpen)
            {
                // 配置串口参数
                serialPort.PortName = port_cbb.SelectedItem.ToString();
                serialPort.BaudRate = 9600; // 设置波特率
                serialPort.DataBits = 8;    // 设置数据位
                serialPort.Parity = Parity.None; // 设置校验位
                serialPort.StopBits = StopBits.One; // 设置停止位

                try
                {
                    serialPort.Open(); // 打开串口
                    open_btn.Text = "关闭串口";
                }
                catch (Exception ex)
                {
                    MessageBox.Show("无法打开串口：" + ex.Message);
                }
            }
            else
            {
                serialPort.Close(); // 关闭串口
                open_btn.Text = "打开串口";
            }
        }

        private void send_btn_Click(object sender, EventArgs e)
        {
            MessageBox.Show("发送按钮被点击了。"); // 调试信息
            if (serialPort.IsOpen)
            {
                string dataToSend = send_txb.Text;
                serialPort.Write(dataToSend); // 发送数据
            }
            else
            {
                MessageBox.Show("串口未打开。");
            }
            if (serialPort.IsOpen)
            {
                string dataToSend = send_txb.Text;
                serialPort.Write(dataToSend); // 发送数据
            }
            else
            {
                MessageBox.Show("串口未打开。");
            }
        }
        //线程冲突
        /*private void serialPort_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            if (serialPort.IsOpen)
            {
               string receivedData = serialPort.ReadExisting();
               receive_rtb.AppendText(receivedData); // 显示接收到的数据
            }
        }*/
        private void serialPort_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            string data = serialPort.ReadExisting(); // 读取所有可用的数据

            // 在主线程上更新 UI
            this.Invoke((MethodInvoker)delegate
            {
                receive_rtb.AppendText(data); // 将接收到的数据添加到 RichTextBox 中
            });
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            RefreshAvailablePorts();
            serialPort.DataReceived += new SerialDataReceivedEventHandler(serialPort_DataReceived);
            if (port_cbb.Items.Count > 0)
            {
                port_cbb.SelectedIndex = 0; // 默认选择第一个串口
            }
            changecolor();


        }
        private String bclhandle = " ";
        private String bclbutton = " ";
        private String bclbanji = " ";
        private void receive_rtb_TextChanged(object sender, EventArgs e)
        {
            
        }
        // 更新图表数据
        private void UpdateChartData()
        {
            // 清空现有数据
            chart1.Series["PID"].Points.Clear();
            chart2.Series["PIDOUT"].Points.Clear();

            // 将PID数据添加到图表
            for (int i = 0; i < pidData.Count; i++)
            {
                chart1.Series["PID"].Points.AddXY(i, pidData[i]);
            }

            // 将PIDOUT数据添加到图表
            for (int i = 0; i < pidOutData.Count; i++)
            {
                chart2.Series["PIDOUT"].Points.AddXY(i, pidOutData[i]);
            }

            // 刷新图表显示
            chart1.Invalidate();
            chart2.Invalidate();
        }
        private void sendTimer_Tick(object sender, EventArgs e)
        {
            if (serialPort.IsOpen)
            {
                string dataToSend = label3.Text;
                //string dataToSend = bclhandle + bclbutton + bclbanji;
                serialPort.Write(dataToSend); // 发送数据
            }
        }
        private bool isSending = false;
        private void autoxboxsend_Click(object sender, EventArgs e)
        {
            if (!isSending)
            {
                if (!serialPort.IsOpen)
                {
                    MessageBox.Show("串口未打开。");
                    return;
                }

                isSending = true;
                autoxboxsend_btn.Text = "停止发送";

                sendTimer.Start(); // 启动定时器，开始发送数据
            }
            else
            {
                isSending = false;
                autoxboxsend_btn.Text = "开始发送";

                sendTimer.Stop(); // 停止定时器，停止发送数据
            }
        }

        private void chart1_Click(object sender, EventArgs e)
        {

        }
    }
}

