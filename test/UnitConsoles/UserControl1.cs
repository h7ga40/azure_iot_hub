using SimBase;
using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Threading;
using System.Windows.Forms;
using UnitConsoles.Properties;

namespace UnitConsoles
{
	public partial class UserControl1 : UserControl, IUnitConsole
	{
		IEnvironment m_Environment;
		string m_Args;
		IUnitSim m_Device;
		IUnitInterface m_UnitInterface;

		public UserControl1(IEnvironment env, string args)
		{
			m_Environment = env;
			m_Args = args;

			InitializeComponent();

			webBrowser1.ObjectForScripting = new TerminalHost(this);
		}

		public IUnitSim Device { get { return m_Device; } }

		public IUnitSim Prepare()
		{
			m_Device = m_Environment.CreateUnit(m_Args);
			m_Device.Prepare();
			Stdio += XTerm_Stdio;
			foreach (var uif in m_Device.Interfaces) {
				if (uif.TypeName != "TSIO")
					continue;
				m_UnitInterface = uif;
				uif.Output += UnitInterface_Output;
				break;
			}
			//m_Device.Stdio += IUnitSim_Stdio;
			m_Device.UnitExit += IUnitSim_Tarminated;
			return m_Device;
		}

		public void Activate(bool active)
		{
		}

		protected override void OnLoad(EventArgs e)
		{
			webBrowser1.DocumentText = Resources.xterm_html;
			base.OnLoad(e);
		}

		private void Close()
		{
			if (m_Device.IsAlive) {
				m_Device.Exit();
				Thread.Sleep(100);
				bool Cancel = m_Device.IsAlive;
			}

			Stdio -= XTerm_Stdio;
			m_UnitInterface.Output -= UnitInterface_Output;
			//m_Device.Stdio -= IUnitSim_Stdio;
			m_Device.UnitExit -= IUnitSim_Tarminated;
		}

		private void IUnitSim_Tarminated(IUnitSim sender)
		{
			BeginInvoke(new MethodInvoker(() => {
				Close();
			}));
		}

		private void AddScript(WebBrowser wb, string scriptSrc)
		{
			var head = wb.Document.GetElementsByTagName("head")[0];
			var script = wb.Document.CreateElement("script");
			script.SetAttribute("type", "text/javascript");
			script.InnerHtml = scriptSrc;
			head.InsertAdjacentElement(HtmlElementInsertionOrientation.AfterEnd, script);
		}

		private void AddStyle(WebBrowser wb, string styleSrc)
		{
			var head = wb.Document.GetElementsByTagName("head")[0];
			var style = wb.Document.CreateElement("style");
			style.InnerHtml = styleSrc;
			head.InsertAdjacentElement(HtmlElementInsertionOrientation.AfterEnd, style);
		}

		private void WebBrowser1_DocumentCompleted(object sender, WebBrowserDocumentCompletedEventArgs e)
		{
			AddStyle(webBrowser1, Resources.xterm_css);
			AddScript(webBrowser1, Resources.xterm_js);
			AddScript(webBrowser1, Resources.fit_js);
			webBrowser1.Document.InvokeScript("start_xterm");

			//m_Device.Start();
		}

		private void ExitApplication(object sender, EventArgs e)
		{
			Application.Idle += Application_Idle;
		}

		private void Application_Idle(object sender, EventArgs e)
		{
			Application.Idle -= Application_Idle;

			Close();
		}

		private void XTerm_Stdio(object sender, StdioEventArgs e)
		{
			//m_Device.WriteStdin(e.Text);
			m_UnitInterface.Input(Encoding.UTF8.GetBytes(e.Text));
		}

		//AutoResetEvent _Event = new AutoResetEvent(false);
		Decoder decoder = Encoding.UTF8.GetDecoder();

		private void UnitInterface_Output(IUnitInterface sender, byte[] data)
		{
			BeginInvoke(new MethodInvoker(() => {
				var decoded = new char[data.Length + 2];
				var chars = decoder.GetChars(data, 0, data.Length, decoded, 0);
				Array.Resize(ref decoded, chars);
				var s = new String(decoded);
				OnTermData(s);
				Console.Write(s);
				//_Event.Set();
			}));
			//_Event.WaitOne();
		}

		public void OnTermData(string text)
		{
			((TerminalHost)webBrowser1.ObjectForScripting).write(text);
		}

		public event EventHandler<StdioEventArgs> Stdio;

		internal void WriteStdin(string text)
		{
			Stdio?.Invoke(this, new StdioEventArgs(StdioType.In, text));
		}
	}

	[System.Runtime.InteropServices.ComVisible(true)]
	public class TerminalHost
	{
		public dynamic term;
		internal UserControl1 View;

		internal TerminalHost(UserControl1 view)
		{
			View = view;
			Out = new XTermWriter(this);
		}

		internal XTermWriter Out { get; private set; }

		public object new_object()
		{
			return new Dictionary<string, object>();
		}

		public void object_add(object dic_, string name, object item)
		{
			var dic = (Dictionary<string, object>)dic_;
			dic.Add(name, item);
		}

		public void console_log(object text)
		{
			if (text == null)
				System.Diagnostics.Debug.WriteLine("null");
			else
				System.Diagnostics.Debug.WriteLine(text.ToString());
		}

		public void console_warn(object text)
		{
			if (text == null)
				System.Diagnostics.Debug.WriteLine("null");
			else
				System.Diagnostics.Debug.WriteLine(text.ToString());
		}

		public object Error { get; set; }

		public object new_error()
		{
			return ((dynamic)Error).call();
		}

		public void on_data(object data)
		{
			View.WriteStdin((string)data);
		}

		internal void write(string text)
		{
			term?.write(text);
		}
	}

	class XTermWriter : TextWriter
	{
		private TerminalHost term;
		private Encoding encoding;

		public XTermWriter(TerminalHost term)
		{
			this.term = term;
			encoding = new UTF8Encoding(false);
		}

		public override Encoding Encoding {
			get {
				return encoding;
			}
		}

		public override void WriteLine(string value)
		{
			term.write(value + NewLine);
		}
	}

	public enum StdioType
	{
		In,
		Out,
		Error,
	}

	public class StdioEventArgs : EventArgs
	{
		public StdioType Type { get; private set; }
		public string Text { get; private set; }

		public StdioEventArgs(StdioType type, string text)
		{
			Type = type;
			Text = text;
		}
	}
}
