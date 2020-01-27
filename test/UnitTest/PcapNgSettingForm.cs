using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Net;
using System.Text;
using System.Windows.Forms;

namespace MT
{
	public partial class PcapNgSettingForm : Form
	{
		//接続先初期設定値
		static readonly string[] AccessPointInit = new string[]{
		 "1", "0","192.168.10.100","10005"
		};
		private DisplaySetting m_Set;
		//設定値
		public DisplaySetting Set { get { return m_Set; } set { m_Set = value; } }
		public DisplaySetting WriteSet;
		/// <summary>
		/// 入力前退避
		/// </summary>
		string m_EditText;

		public PcapNgSettingForm(DisplaySetting Set)
		{
			InitializeComponent();
			this.Set = new DisplaySetting(Set);
			SetText();
		}

		private void SetText()
		{
			byte[] ipaddr = Set.IPAddr.GetAddressBytes();
			IP1.Text = ipaddr[0].ToString();
			IP2.Text = ipaddr[1].ToString();
			IP3.Text = ipaddr[2].ToString();
			IP4.Text = ipaddr[3].ToString();
			UnitName.Text = Set.UnitName;
			UDPPort.Text = Set.UDPPort.ToString();
			settingItemBindingSource.DataSource = Set.AccessPointList;
			settingItemBindingSource.ResetBindings(true);
		}

		private void SettingForm_Load(object sender, EventArgs e)
		{
		}

		/// <summary>
		/// 
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		private void CancelButton_Click(object sender, EventArgs e)
		{
			DialogResult = DialogResult.Cancel;
			Close();
		}

		/// <summary>
		/// 値が0～255外の場合、エラー表示する
		/// </summary>
		/// <param name="sender"></param>
		/// <param name="e"></param>
		private void IPCheck(object sender, EventArgs e)
		{
			TextBox IP = sender as TextBox;
			byte ip;
			if (IP.Text.Length == 0 || !byte.TryParse(IP.Text, out ip) && ip <= 255)
			{
				IP.BackColor = Color.Pink;
			}
			else
			{
				IP.BackColor = Color.White;
			}
			if (m_EditText != IP.Text)
			{
				if (IP1.BackColor != Color.Pink
					&& IP2.BackColor != Color.Pink
					&& IP3.BackColor != Color.Pink
					&& IP4.BackColor != Color.Pink)
				{
					Set.IPAddr = IPAddress.Parse(IP1.Text + "." + IP2.Text + "." + IP3.Text + "." + IP4.Text);
				}
			}
		}

		private void TextBox_Enter(object sender, EventArgs e)
		{
			TextBox Edit = sender as TextBox;
			m_EditText = Edit.Text;
		}

		private void UDPPort_TextChanged(object sender, EventArgs e)
		{
			TextBox Port = sender as TextBox;
			UInt16 ip;
			if (Port.Text.Length == 0 || !UInt16.TryParse(Port.Text, out ip) && ip <= 0xFFFF)
			{
				Port.BackColor = Color.Pink;
			}
			else
			{
				Port.BackColor = Color.White;
			}

			if (m_EditText != Port.Text && UDPPort.BackColor != Color.Pink)
			{
				Set.UDPPort = UInt16.Parse(UDPPort.Text);
			}
		}

		private void UnitName_TextChanged(object sender, EventArgs e)
		{
			TextBox Port = sender as TextBox;
			Set.UnitName = UnitName.Text;
		}

		private void OKButton_Click(object sender, EventArgs e)
		{
			DialogResult = DialogResult.OK;
			Close();
		}
	}

	public class DisplaySetting
	{
		private List<AccessPoint> m_AccessPointList;
		public IPAddress m_IPAddr;
		public UInt16 m_UDPPort;

		public DisplaySetting()
		{
			AccessPointList = new List<AccessPoint>();
		}

		public DisplaySetting(DisplaySetting display)
		{
			this.IPAddr = display.IPAddr;
			this.UDPPort = display.UDPPort;
			this.UnitName = display.UnitName;
			this.AccessPointList = display.AccessPointList;
		}

		public AccessPoint[] GetCheckedArray()
		{
			return Array.FindAll(AccessPointList.ToArray(), (apoint) => apoint.Check);
		}

		public void Add(AccessPoint apoint)
		{
			AccessPointList.Add(apoint);
		}

		public List<AccessPoint> AccessPointList { get { return m_AccessPointList; } set { m_AccessPointList = value; } }
		/// <summary>
		/// 装置側IPアドレス
		/// </summary>
		public IPAddress IPAddr { get { return m_IPAddr; } set { m_IPAddr = value; } }
		/// <summary>
		/// 装置側UDPポート
		/// </summary>
		public UInt16 UDPPort { get { return m_UDPPort; } set { m_UDPPort = value; } }
		/// <summary>
		/// 装置名
		/// </summary>
		public string UnitName { get; set; }
	}

	/// <summary>
	/// 接続先設定
	/// </summary>
	public class AccessPoint
	{
		private byte m_Number;
		private string m_AccsessName;
		private bool m_Check;
		private ushort m_PortNo;
		private IPAddress m_IPaddr;

		public AccessPoint(byte number, bool check, IPAddress ipadd, ushort portno)
		{
			Number = number;
			m_AccsessName = number.ToString();
			Check = check;
			m_IPaddr = ipadd;
			PortNo = portno;
		}

		public AccessPoint(byte number, string name, bool check, IPAddress ipadd, ushort portno)
		{
			Number = number;
			AccessName = name;
			Check = check;
			m_IPaddr = ipadd;
			PortNo = portno;
		}

		/// <summary>番号</summary>
		public byte Number {
			get { return m_Number; }
			set { m_Number = value; }
		}

		/// <summary>名前</summary>
		public string AccessName {
			get { return m_AccsessName; }
			set { m_AccsessName = value; }
		}

		/// <summary>有効／無効</summary>
		public bool Check {
			get { return m_Check; }
			set { m_Check = value; }
		}

		public IPAddress IPAddr {
			get { return m_IPaddr; }
			set { m_IPaddr = value; }
		}

		/// <summary>ポート</summary>
		public ushort PortNo {
			get { return m_PortNo; }
			set { m_PortNo = value; }
		}
	}
}
