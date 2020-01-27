using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;

namespace Bridge
{
	public partial class WebConsole : UserControl
	{
		ResourceReader m_ResourceReader;
		string m_DllName;
		Icon m_Icon;

		static WebConsole()
		{
			// http://www.osadasoft.com/c-webbrowser%e3%82%b3%e3%83%b3%e3%83%88%e3%83%ad%e3%83%bc%e3%83%ab%e3%81%ae%e3%83%ac%e3%83%b3%e3%83%80%e3%83%aa%e3%83%b3%e3%82%b0%e3%83%a2%e3%83%bc%e3%83%89%e3%82%92%e3%83%87%e3%83%95%e3%82%a9/
			try {
				var regkey = Microsoft.Win32.Registry.CurrentUser.CreateSubKey(
					@"Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_BROWSER_EMULATION");
				regkey.SetValue(Path.GetFileName(Application.ExecutablePath).ToLower(), 11001);
				regkey.Close();

				//regkey = Microsoft.Win32.Registry.CurrentUser.CreateSubKey(
				//	@"Software\Microsoft\Internet Explorer\Main\FeatureControl\FEATURE_DOCUMENT_COMPATIBLE_MODE");
				//regkey.SetValue(Path.GetFileName(Application.ExecutablePath).ToLower(), 11001);
				//regkey.Close();
			}
			catch (Exception) {
			}
		}

		public WebConsole()
		{
			InitializeComponent();
		}

		public Icon Icon { get { return m_Icon; } }
		public ScriptingHost ObjectForScripting { get { return (ScriptingHost)webBrowser1.ObjectForScripting; } }
		public event EventHandler<EventArgs> DocumentLoaded;

		public void Open(ResourceReader resourceReader, ScriptingHost scriptingHost)
		{
			m_ResourceReader = resourceReader;
			webBrowser1.ObjectForScripting = scriptingHost;
			m_DllName = resourceReader.DllName;
		}

		public void Navigate(string path)
		{
			webBrowser1.Navigate(new System.Uri("res://" + m_DllName + ".dll/23/" + path, System.UriKind.Absolute));
			webBrowser1.DocumentTitleChanged += new EventHandler(webBrowser1_DocumentTitleChanged);
		}

		public Script Script { get; private set; }

		protected virtual bool PrepareScript(Uri Url)
		{
			return Url.Scheme != "about";
		}

		protected virtual Script NewScript(ScriptingHost scriptingHost)
		{
			return new Script(scriptingHost);
		}

		public void SetZoom(int zoom)
		{
			((dynamic)webBrowser1.ActiveXInstance).ExecWB(63, 1, zoom, 0);
		}

		protected virtual void DocumentCompleted(Uri Url)
		{
			if (Url.Scheme == "about")
				return;

			Icon favicon = m_ResourceReader.GetFavicon(Url);
			if (favicon != null)
				m_Icon = favicon;

			DocumentLoaded?.Invoke(this, EventArgs.Empty);
		}

		private void WebBrowser1_DocumentCompleted(object sender, WebBrowserDocumentCompletedEventArgs e)
		{
			if (PrepareScript(e.Url))
				Script = NewScript((ScriptingHost)webBrowser1.ObjectForScripting);

			DocumentCompleted(e.Url);
		}

		public ResourceReader ResourceReader { get { return m_ResourceReader; } }

		private void webBrowser1_DocumentTitleChanged(object sender, EventArgs e)
		{
			string text = webBrowser1.DocumentTitle;
			if (text != "")
				Text = text;
		}

		public object InvokeScript(string scriptName)
		{
			return webBrowser1.Document.InvokeScript(scriptName);
		}

		public object InvokeScript(string scriptName, object[] args)
		{
			return webBrowser1.Document.InvokeScript(scriptName, args);
		}
	}
}
