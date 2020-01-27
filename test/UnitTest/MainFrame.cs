using Bridge;
using SimBase;
using SimEnv;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Text.RegularExpressions;
using System.Windows.Forms;
using System.Xml.Linq;
using uITron3;

namespace SystemSim
{
	public partial class MainFrame : Form, IEnvironment
	{
		private List<UnitInterfaceInfo> m_UnitInterfaceInfos = new List<UnitInterfaceInfo>();
		private List<UnitSimInfo> m_UnitSimInfos = new List<UnitSimInfo>();
		private List<UnitConsoleInfo> m_UnitConsoles = new List<UnitConsoleInfo>();
		private SimRunner m_SimRunner;
		private List<string> m_CreateUnits = new List<string>();
		private List<IUnitSim> m_Units = new List<IUnitSim>();
		private PcapIF m_PcapIF;
		private EthernetLoger m_EthernetLoger;
		private PacketViewer m_PacketViewer;
		private Unit m_GateWay;

		public MainFrame()
		{
			InitializeComponent();

			m_CreateUnits.Add("UnitConsoles.UserControl1(app_iothub_client)");
			m_PcapIF = new PcapIF(true);

			m_EthernetLoger = new EthernetLoger();
			m_PcapIF.Add(m_EthernetLoger);

			ethernetLogItemBindingSource.DataSource = m_EthernetLoger.EthernetLog;
			ethernetLogItemBindingSource.ResetBindings(true);

			m_PacketViewer = new PacketViewer();

			packetViewItemBindingSource.DataSource = m_PacketViewer.PacketFields;
			packetViewItemBindingSource.ResetBindings(true);
		}

		private void MainFrame_Load(object sender, EventArgs e)
		{
			var path = Path.GetDirectoryName(Application.ExecutablePath);

			var dir = new DirectoryInfo(path);
			foreach (var file in dir.EnumerateFiles()) {
				var name = file.Name;
				var ext = Path.GetExtension(name);
				if (ext.ToLower() != ".dll")
					continue;

				DllUnitSim unitSim = null;
				try {
					unitSim = new DllUnitSim(name);
				}
				catch (Exception) {
					if (unitSim != null)
						unitSim.Dispose();
					unitSim = null;
				}

				if (unitSim == null)
					continue;

				if (unitSim.IsAlive) {
					unitSim.Prepare();
					var u = new UnitSimInfo(name, typeof(SimEnv.DllUnitSim).FullName, Path.GetFileNameWithoutExtension(name), true);
					m_UnitSimInfos.Add(u);
					foreach (var i in unitSim.DllInterfaces) {
						u.Interfaces.Add(new UnitInterfaceInstanceInfo(u.FileName, i.TypeName, i.InterfaceName));
					}
					unitSim.Dispose();
					continue;
				}

				Assembly asm = null;
				try {
					asm = Assembly.LoadFrom(name);
				}
				catch (Exception) {
					asm = null;
				}

				if (asm != null) {
					if (Assembly.GetEntryAssembly().GetReferencedAssemblies().FirstOrDefault(
						(a) => { return String.Compare(a.FullName, asm.FullName) == 0; }) != null)
						continue;

					Type[] types;
					try {
						types = asm.GetExportedTypes();
					}
					catch (Exception) {
						types = new Type[0];
					}

					foreach (var type in types) {
						if (!type.IsClass)
							continue;

						foreach (var i in type.GetInterfaces()) {
							if (i == typeof(SimBase.IUnitInterface)) {
								var ctor = type.GetConstructor(new Type[] { typeof(SimBase.IUnitSim), typeof(string), typeof(int) });
								if (ctor != null)
									m_UnitInterfaceInfos.Add(new UnitInterfaceInfo(name, type.FullName));
							}
							if (i == typeof(SimBase.IUnitSim)) {
								var ctor = type.GetConstructor(new Type[] { typeof(string) });
								if (ctor != null) {
									var u = new UnitSimInfo(name, type.FullName, type.Name, false);
									m_UnitSimInfos.Add(u);
									EnumMemberInterface(u, type);
								}
							}
							if (i == typeof(IUnitConsole)) {
								var ctor = type.GetConstructor(new Type[] { typeof(IEnvironment), typeof(string) });
								if (ctor != null)
									m_UnitConsoles.Add(new UnitConsoleInfo(name, type.FullName));
							}
						}
					}
					continue;
				}
			}

			ip_addr addr = new ip_addr(0xC0A88954), mask = new ip_addr(0xFFFFFF00);

			m_PcapIF.Init(addr.addr, mask.addr);

			timer1.Enabled = true;
		}

		private void MainFrame_FormClosing(object sender, FormClosingEventArgs e)
		{
			if (m_SimRunner == null)
				return;

			m_SimRunner.Terminate();
		}

		private void EnumMemberInterface(UnitSimInfo u, Type type)
		{
			foreach (var m in type.GetProperties(BindingFlags.Public)) {
				var t = m.DeclaringType;
				foreach (var i in t.GetInterfaces()) {
					if (i == typeof(SimBase.IUnitInterface)) {
						u.Interfaces.Add(new UnitInterfaceInstanceInfo(t.Assembly.Location, t.FullName, m.Name));
					}
				}
			}
		}

		private void RunBtn_Click(object sender, EventArgs e)
		{
			if (m_SimRunner != null)
				return;

			m_SimRunner = new SimRunner(this);

			var random = new Random();
			var macAddr = ((ulong)(random.NextDouble() * 0x1000000000000UL) & 0xFEFFFFFFFFFFUL) | 0x020000000000UL;
			uint addr = 0xC0A88901, subNet = 0xFFFFFF00;
			var gateWay = new GateWay(addr, subNet);
			m_GateWay = new Unit("GateWay", gateWay, macAddr, gateWay.Addr, gateWay.SubNet);
			m_Units.Add(m_GateWay);

			foreach (var args in m_CreateUnits) {
				var unit = m_SimRunner.CreateUnit(args);
				if (unit != null)
					m_Units.Add(unit);
			}

			foreach (var unit in m_Units) {
				foreach (var uif in unit.Interfaces) {
					if (uif is IEtherConnection eif) {
						m_PcapIF.Add(eif);
					}
				}
			}

			foreach (var unit in m_Units) {
				unit.Start();
			}

			m_SimRunner.Start();
		}

		private void StopBtn_Click(object sender, EventArgs e)
		{
			foreach (var unit in m_Units) {
				unit.Exit();
			}
		}

		public SimBase.IUnitSim CreateUnit(string ctorText)
		{
			foreach (var u in m_UnitSimInfos) {
				if (u.UnitName != ctorText)
					continue;

				if (u.Dll)
					return new DllUnitSim(ctorText);

				var asm = Assembly.LoadFrom(u.FileName);
				if (asm != null) {
					var type = asm.GetType(u.TypeName);
					var ctor = type.GetConstructor(new Type[] { typeof(string) });
					return ctor.Invoke(new object[] { ctorText }) as SimBase.IUnitSim;
				}
			}

			return null;
		}

		public IUnitConsole CreateUnitConsole(string ctorText)
		{
			string typeName;
			string args;

			var m = Regex.Match(ctorText, @"([A-Za-z_][A-Za-z0-9_]+)\.([A-Za-z_][A-Za-z0-9_]+)\((.+)\)");
			if (m.Success) {
				typeName = m.Groups[1].Value + "." + m.Groups[2].Value;
				args = m.Groups[3].Value;
			}
			else {
				typeName = ctorText;
				args = "";
			}

			IUnitConsole console = null;
			foreach (var c in m_UnitConsoles) {
				if (typeName != c.TypeName)
					continue;

				try {
					var asm = Assembly.LoadFrom(c.FileName);
					var cls = asm.GetType(c.TypeName);
					var ctor = cls.GetConstructor(new Type[] { typeof(IEnvironment), typeof(string) });
					console = (IUnitConsole)ctor.Invoke(new object[] { this, args });
				}
				catch (Exception) {
					console = null;
				}
			}
			if (console == null)
				return null;

			var cotrol = (Control)console;
			var page = new TabPage(args);
			cotrol.Dock = DockStyle.Fill;
			page.Controls.Add(cotrol);
			tabControl1.TabPages.Add(page);
			return console;
		}

		public void OnTerminated()
		{
		}

		public void Log(SimBase.IUnitSim sender, int kind, byte[] data)
		{
			System.Diagnostics.Debug.WriteLine(Encoding.Default.GetString(data));
		}

		public void UnitExit(SimBase.IUnitSim sender)
		{
			foreach (var uif in sender.Interfaces) {
				if (uif is IEtherConnection eif) {
					m_PcapIF.Remove(eif);
				}
			}

			m_Units.Remove(sender);

			if (m_SimRunner == null)
				return;

			if (m_SimRunner.Units.Count == 0) {
				m_SimRunner.Terminate();
				m_SimRunner = null;
			}
		}

		private void Timer1_Tick(object sender, EventArgs e)
		{
			if (m_EthernetLoger.Changed) {
				m_EthernetLoger.Changed = false;

				var lastRow = (dataGridView1.CurrentRow == null)
					|| ((dataGridView1.RowCount - 1) == dataGridView1.CurrentRow.Index);

				ethernetLogItemBindingSource.ResetBindings(false);

				if (lastRow) {
					dataGridView1.CurrentCell = dataGridView1[dataGridView1.CurrentCell.ColumnIndex, dataGridView1.RowCount - 1];
				}
			}
		}

		private void DataGridView1_CurrentCellChanged(object sender, EventArgs e)
		{
			if (tabControl1.SelectedTab != tabPage2)
				return;

			var grid = (DataGridView)sender;
			var row = grid.CurrentRow;
			if (row == null)
				return;

			var item = (EthernetLogItem)row.DataBoundItem;

			m_PacketViewer.SetPacket(item.Packet);

			packetViewItemBindingSource.ResetBindings(false);

			dataGridView2.CurrentCell = dataGridView2[dataGridView2.CurrentCell.ColumnIndex, dataGridView2.RowCount - 1];
		}

		IUnitConsole ActiveConsole;

		private void TabControl1_SelectedIndexChanged(object sender, EventArgs e)
		{
			var page = tabControl1.SelectedTab;
			IUnitConsole console = null;

			if (page == tabPage2) {
				DataGridView1_CurrentCellChanged(dataGridView1, EventArgs.Empty);
			}
			else if (page.Controls.Count == 1) {
				console = page.Controls[0] as IUnitConsole;
				console?.Activate(true);
			}

			ActiveConsole?.Activate(false);
			ActiveConsole = console;
		}
	}

	internal class DllUnitInterface : SimBase.IUnitInterface
	{
		private DllUnitSim m_UnitSim;
		private readonly int kind;
		private readonly string name;
		private readonly string type;
		private Action<SimBase.IUnitInterface, byte[]> output;

		public DllUnitInterface(DllUnitSim unitSim, string name, string type, int kind)
		{
			m_UnitSim = unitSim;
			this.name = name;
			this.type = type;
			this.kind = kind;
		}

		public int IFKind => kind;

		public string TypeName => type;

		public string InterfaceName => name;

		public SimBase.IUnitSim UnitSim => m_UnitSim;

		public Action<SimBase.IUnitInterface, byte[]> Output { get => output; set => output = value; }

		public virtual void Push(byte[] data)
		{
			output?.Invoke(this, data);
		}

		public virtual void Input(byte[] data)
		{
			m_UnitSim.Input(kind, data);
		}
	}

	internal class DllUnitEthernetIf : DllUnitInterface, IEtherConnection
	{
		public DllUnitEthernetIf(DllUnitSim unitSim, string name, string type, int kind)
			: base(unitSim, name, type, kind)
		{
		}

		public IHub Hub { get; set; }

		public void Receive(EtherPacket packet)
		{
			var data = new byte[packet.Length];
			Buffer.BlockCopy(packet.data, packet.offset, data, 0, packet.Length);
			UnitSim.Input(IFKind, data);
		}

		public override void Push(byte[] data)
		{
			Hub.Send(this, new EtherPacket(data));
		}
	}

	internal class DllUnitSim : SimEnv.DllUnitSim
	{
		private List<DllUnitInterface> m_DllInterfaces = new List<DllUnitInterface>();
		private List<SimBase.IUnitInterface> m_Interfaces = new List<SimBase.IUnitInterface>();

		public DllUnitSim(string dllName) : base(dllName)
		{
		}

		public override IList<SimBase.IUnitInterface> Interfaces => m_Interfaces.AsReadOnly();

		public IList<DllUnitInterface> DllInterfaces => m_DllInterfaces.AsReadOnly();

		protected override void AddInterfaceFromDll(string name, string type, int kind)
		{
			DllUnitInterface uif;

			if (type == "TEthernet") {
				uif = new DllUnitEthernetIf(this, name, type, kind);
			}
			else {
				uif = new DllUnitInterface(this, name, type, kind);
			}

			m_DllInterfaces.Add(uif);
			m_Interfaces.Add(uif);
		}

		protected override void RemoveInterfaceFromDll(string name, string type, int kind)
		{
			var uif = m_DllInterfaces.FirstOrDefault((i) => (i.InterfaceName == name) && (i.IFKind == kind));
			if (uif == null)
				return;
			m_DllInterfaces.Remove(uif);
			m_Interfaces.Remove(uif);
		}

		protected override void Push(int kind, byte[] Data)
		{
			var uif = m_DllInterfaces.FirstOrDefault((i) => i.IFKind == kind);
			if (uif == null)
				return;
			uif.Push(Data);
		}
	}

	internal class UnitInterfaceInfo
	{
		public string FileName { get; }
		public string TypeName { get; }

		public UnitInterfaceInfo(string fileName, string typeName)
		{
			FileName = fileName;
			TypeName = typeName;
		}
	}

	internal class UnitInterfaceInstanceInfo
	{
		public string FileName { get; }
		public string TypeName { get; }
		public string InterfaceName { get; }

		public UnitInterfaceInstanceInfo(string fileName, string typeName, string interfaceName)
		{
			FileName = fileName;
			TypeName = typeName;
			InterfaceName = interfaceName;
		}
	}

	internal class UnitSimInfo
	{
		private readonly List<UnitInterfaceInstanceInfo> m_Interfaces = new List<UnitInterfaceInstanceInfo>();

		public string FileName { get; }
		public string TypeName { get; }
		public string UnitName { get; }
		public bool Dll { get; }
		public IList<UnitInterfaceInstanceInfo> Interfaces => m_Interfaces;

		public UnitSimInfo(string fileName, string typeName, string unitName, bool dll)
		{
			FileName = fileName;
			TypeName = typeName;
			UnitName = unitName;
			Dll = dll;
		}
	}

	internal class UnitConsoleInfo
	{
		public string FileName { get; }
		public string TypeName { get; }

		public UnitConsoleInfo(string fileName, string typeName)
		{
			FileName = fileName;
			TypeName = typeName;
		}
	}

	public class App
	{
		public static TextWriter Out => Console.Out;

		internal static void Write(string text)
		{
			Out.Write(text);
		}

		internal static void Write(string text, params object[] args)
		{
			Out.Write(text, args);
		}

		internal static void WriteLine()
		{
			Out.WriteLine();
		}

		internal static void WriteLine(string text)
		{
			Out.WriteLine(text);
		}

		internal static void WriteLine(string text, params object[] args)
		{
			Out.WriteLine(text, args);
		}
	}

	public class Dir
	{
		internal static void chdir(string path)
		{
			path = path.Replace("/", "\\");
			Environment.CurrentDirectory = path;
		}

		internal static void chdir(string path, Action p)
		{
			var dir = Environment.CurrentDirectory;
			try {
				path = path.Replace("/", "\\");
				Environment.CurrentDirectory = path;
				p();
			}
			finally {
				Environment.CurrentDirectory = dir;
			}
		}

		internal static string pwd()
		{
			return Environment.CurrentDirectory.Replace("\\", "/");
		}
	}

	public class File
	{
		private System.IO.FileStream m_File;

		internal static File open(string fname, string v)
		{
			var mode = System.IO.FileMode.Open;
			var access = new System.IO.FileAccess();

			if (v.Contains('r')) {
				access |= System.IO.FileAccess.Read;
			}
			if (v.Contains('w')) {
				access |= System.IO.FileAccess.Write;
				mode = System.IO.FileMode.Create;
			}
			if (v.Contains('a')) {
				mode = System.IO.FileMode.Append;
			}

			var ret = new File {
				m_File = new System.IO.FileStream(fname.Replace("/", "\\"), mode, access)
			};

			return ret;
		}

		internal static void unlink(string fname)
		{
			System.IO.File.Delete(fname.Replace("/", "\\"));
		}

		internal static bool exist(string fname)
		{
			return System.IO.File.Exists(fname.Replace("/", "\\"));
		}

		internal void close()
		{
			m_File.Close();
			m_File = null;
		}

		internal void print(string text)
		{
			var data = System.Text.Encoding.UTF8.GetBytes(text);
			m_File.Write(data, 0, data.Length);
		}

		internal static string join(params string[] paths)
		{
			return paths.Join("/");
		}

		internal static string absolute_path(string path)
		{
			return System.IO.Path.GetFullPath(path).Replace("\\", "/");
		}
	}
}
