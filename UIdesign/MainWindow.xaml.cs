using System;
using System.Runtime.InteropServices;
using System.Windows;
using System.Windows.Input;

namespace UIdesign
{
    /// <summary>
    /// MainWindow.xaml 的交互逻辑
    /// </summary>
    public partial class MainWindow : Window
    {
        // 载入算法dll
        [DllImport("DiscreteMathProgram_0.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr CalculateForm([MarshalAs(UnmanagedType.LPWStr)] string expression, int maxsize,
            int operate); // 操作 0: 主析取范式 1: 主合取范式

        // 释放空间函数
        [DllImport("DiscreteMathProgram_0.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern void FreeMemory(IntPtr p);

        public MainWindow()
        {
            InitializeComponent();
            // Box.Text = Marshal.PtrToStringAuto(CalculateForm("(┐P∧Q)→R", 100, 0))!;
            ExpressionBox.Focusable = true;
        }

        private void SetFocus()
        {
            ExpressionBox.Focus();
            ExpressionBox.Select(ExpressionBox.Text.Length, 0);
            ExpressionBox.ScrollToEnd();
        }

        // 按键输入
        private void OrButton_OnClick(object sender, RoutedEventArgs e)
        {
            ExpressionBox.Text += "∨";
            SetFocus();
        }


        private void SingleAButton_OnClick(object sender, RoutedEventArgs e)
        {
            ExpressionBox.Text += "→";
            SetFocus();
        }

        private void NotButton_OnClick(object sender, RoutedEventArgs e)
        {
            ExpressionBox.Text += "┐";
            SetFocus();
        }


        private void AndButton_OnClick(object sender, RoutedEventArgs e)
        {
            ExpressionBox.Text += "∧";
            SetFocus();
        }

        private void RBracketButton_OnClick(object sender, RoutedEventArgs e)
        {
            ExpressionBox.Text += ")";
            SetFocus();
        }

        private void LBracketButton_OnClick(object sender, RoutedEventArgs e)
        {
            ExpressionBox.Text += "(";
            SetFocus();
        }

        private void DoubleAButton_OnClick(object sender, RoutedEventArgs e)
        {
            ExpressionBox.Text += "⇄";
            SetFocus();
        }

        // 窗体退出
        private void ExitButton_OnClick(object sender, RoutedEventArgs e)
        {
            this.Close();
        }

        // 清空
        private void AcButton_OnClick(object sender, RoutedEventArgs e)
        {
            AnswerBlock.Text = "";
            ExpressionBox.Text = "";
            SetFocus();
        }

        // 计算主合取范式
        private void AnsConButton_OnClick(object sender, RoutedEventArgs e)
        {
            IntPtr p = CalculateForm(ExpressionBox.Text, 10000000, 1);
            AnswerBlock.Text = Marshal.PtrToStringAuto(p)!;
            FreeMemory(p);
        }

        // 计算主析取范式
        private void AnsDisButton_OnClick(object sender, RoutedEventArgs e)
        {
            IntPtr p = CalculateForm(ExpressionBox.Text, 10000000, 0);
            AnswerBlock.Text = Marshal.PtrToStringAuto(p)!;
            FreeMemory(p);
        }


        // 窗体拖拽
        private void MainWindow_OnMouseMove(object sender, MouseEventArgs e)
        {
            if (e.LeftButton == MouseButtonState.Pressed)
            {
                this.DragMove();

            }
        }

        private void ExpressionBox_OnMouseMove(object sender, MouseEventArgs e)
        {
            if (e.LeftButton == MouseButtonState.Pressed)
            {
                e.Handled = true;
            }
        }



        // 限制输入大写字母
        public static bool IsInputChar(KeyEventArgs e)
        {
            if (e.Key >= Key.A && e.Key <= Key.Z ||
                e.Key == Key.Delete || e.Key == Key.Back || e.Key == Key.Left || e.Key == Key.Right)
            {
                //按下了Alt、ctrl、shift等修饰键
                if (e.KeyboardDevice.Modifiers != ModifierKeys.None)
                {
                    e.Handled = true;
                }
                else
                {
                    return true;
                }

            }
            else //按下了字符等其它功能键
            {
                e.Handled = true;
            }

            return false;
        }

        private void ExpressionBox_OnPreviewKeyDown(object sender, KeyEventArgs e)
        {
            IsInputChar(e);
        }


        private void AnswerBlock_OnMouseMove(object sender, MouseEventArgs e)
        {
            if (e.LeftButton == MouseButtonState.Pressed)
            {
                e.Handled = true;
            }
        }
    }
}
