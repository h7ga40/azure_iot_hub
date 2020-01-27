using System;
using System.Collections;
using System.Collections.Generic;
using System.Dynamic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using Bridge.Html5;

namespace Bridge
{
	public delegate int LoadEvent(ScriptingHost sender, string url, object data);

	/// <summary>
	/// WebBrowserのJavaScriptから呼ばれる関数の定義
	/// </summary>
	/// <remarks>
	/// JavaScriptから呼び出す際の引数の数や型で関数の選択がうまく行かないので、
	/// ここでリダイレクトするため、引数の型はobject型で定義する。
	/// </remarks>
	[ComVisible(true)]
	public class ScriptingHost
	{
		public object bridge;
		WebConsole _View;
		bool _Async;

		public dynamic instance;

		public ScriptingHost(WebConsole view)
		{
			_View = view;
		}

		public WebConsole View { get { return _View; } }
		public Script Script { get { return _View.Script; } }
		public Document Document { get { return _View.Script.Document; } }
		public string Type { get; private set; }
		public string Url { get; private set; }
		public string ContentType { get; private set; }
		public string Username { get; private set; }
		public string Password { get; private set; }

		public object CreateXMLHttpRequest()
		{
			var result = new ScriptingHost(_View);
			result.OnLoad = OnLoad;
			return result;
		}

		public object new_array()
		{
			return new JsArray<object>();
		}

		public void array_add(object array_, object item)
		{
			var array = (JsArray<object>)array_;
			array.Push(item);
		}

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

		public void onerror(string msg, string src, int line, int column, Dictionary<string, object> excpetion)
		{
			var kvs = new string[excpetion.Count];
			int i = 0;
			foreach (var kv in excpetion) {
				kvs[i++] = kv.Key + ":\"" + kv.Value.ToString() + "\"";
			}

			System.Diagnostics.Debug.WriteLine(GetType().Name + "." + System.Reflection.MethodBase.GetCurrentMethod().Name
				+ " " + msg + " " + src + " " + line + " " + column + "\n" + String.Join(",\n", kvs));
		}

		private void SetReadyState(WebBrowserReadyState readyState)
		{
			instance.readyState = readyState;
			try {
				if ((instance.onreadystatechange != null) && !(instance.onreadystatechange is DBNull))
					instance.onreadystatechange.call(null, true);
			}
			catch (COMException e) {
				System.Diagnostics.Debug.WriteLine(e.Message);
			}
		}

		public LoadEvent OnLoad;

		private void Load(object data)
		{
			switch (System.IO.Path.GetExtension(Url)) {
			case "html": ContentType = "text/html"; break;
			case "css": ContentType = "text/css"; break;
			case "js": ContentType = "text/javascript"; break;
			case "xml": ContentType = "text/xml"; break;
			case "gif": ContentType = "image/gif"; break;
			case "jpeg": ContentType = "image/jpeg"; break;
			case "png": ContentType = "image/png"; break;
			case "svg": ContentType = "image/svg+xml"; break;
			case "json": ContentType = "application/json"; break;
			default: ContentType = "text/plane"; break;
			}

			if ((data != null) && !(data is DBNull)) {
				var ret = OnLoad == null ? 404 : OnLoad(this, Url, data);
				// 0以下の場合は
				if (ret > 0) {
					instance.status = ret;
					SetReadyState(WebBrowserReadyState.Complete);
				}
				return;
			}

			var url = Url;
			if (url.StartsWith("./"))
				url = url.Substring(2);

			byte[] response;
			if (!_View.ResourceReader.GetResource(url, out response)) {
				instance.status = 404;
				SetReadyState(WebBrowserReadyState.Complete);
				return;
			}
			instance.response = response;
			instance.responseText = Encoding.UTF8.GetString(instance.response);
			instance.status = 200;
			SetReadyState(WebBrowserReadyState.Complete);

			try {
				if ((instance.onload != null) && !(instance.onload is DBNull))
					instance.onload.call(this, null);
			}
			catch (COMException e) {
				System.Diagnostics.Debug.WriteLine(e.Message);
			}
		}

		public void LoadDane(int status)
		{
			instance.status = status;
			SetReadyState(WebBrowserReadyState.Complete);

			try {
				if (status == 200) {
					if ((instance.onload != null) && !(instance.onload is DBNull))
						instance.onload.call(this, null);
				}
				else {
					if ((instance.onload != null) && !(instance.onload is DBNull))
						instance.onload.call(this, null);
				}
			}
			catch (COMException e) {
				System.Diagnostics.Debug.WriteLine(e.Message);
			}
		}

		public void open(string type, string url, bool async, string username, string password)
		{
			Type = type;
			Url = url;
			_Async = async;
			Username = username;
			Password = password;

			instance.status = 404;
			SetReadyState(WebBrowserReadyState.Loading);
		}

		public void send(object data)
		{
			SetReadyState(WebBrowserReadyState.Loaded);

			if (_Async)
				_View.BeginInvoke(new MethodInvoker(() => { Load(data); }));
			else
				Load(data);
		}

		Dictionary<string, object> headers = new Dictionary<string, object>();

		public void setRequestHeader(string name, object value)
		{
			headers[name] = value;
		}

		public string getAllResponseHeaders()
		{
			return
				"Content-Type: " + ContentType + "\n" +
				"Last-Modified: Tue, 11 Nov 2014 00:00:00 GMT\n" +
				"Accept-Ranges: bytes\n" +
				"Server: Microsoft-IIS/8.0\n" +
				"Date: " + DateTime.Now.ToString("R");
		}

		public bool isDefined(object obj, string name)
		{
			var members = obj.GetType().GetMember(name);
			return members.Length != 0;
		}

		public dynamic GetElementInstance(object element_)
		{
			var element = (Element)element_;
			return element.instance;
		}

		public object InvokeHandler(object handler_, object arguments_)
		{
			var handler = handler_ as Delegate;
			var arguments = arguments_ as JsArray<object>;
			try {
				return handler.DynamicInvoke(arguments.ToArray());
			}
			catch (Exception e) {
				System.Diagnostics.Debug.WriteLine(e.ToString());
				return null;
			}
		}

		public Element ToElement(dynamic element)
		{
			return Element.Create(Script, element);
		}

		public string NullOrString(object text)
		{
			return ((text == null) || (text is DBNull)) ? "null" : text.ToString();
		}

		public void abort(string text)
		{
			if ((instance.onabort != null) && !(instance.onabort is DBNull))
				instance.onabort.call(this);
		}
	}

	public class Script
	{
		public static DBNull Undefined = DBNull.Value;
		public dynamic Bridge;
		public ScriptingHost ScriptHost;
		public Document Document { get; private set; }

		public Script(ScriptingHost scriptHost)
		{
			ScriptHost = scriptHost;
			Bridge = ScriptHost.bridge;
			if (Bridge != null) {
				Bridge.instance = ScriptHost;
				Document = new Document(this);
			}
		}

		public static dynamic NewObject()
		{
			return new Microsoft.JScript.JSObject();
		}

		public static dynamic NewArray(params object[] items)
		{
			var a = new Microsoft.JScript.JSObject();
			foreach (var i in items)
				Microsoft.JScript.ArrayPrototype.push(a, i);
			return a;
		}

		public static void Push(object a, object first, params object[] items)
		{
			Microsoft.JScript.ArrayPrototype.push(a, first);
			foreach (var i in items)
				Microsoft.JScript.ArrayPrototype.push(a, i);
		}

		public dynamic NewFunc(Delegate d)
		{
			System.Diagnostics.Debug.Assert(d != null);
			return Bridge.NewFunc(d);
		}

		public dynamic New(string name, object[] args)
		{
			var a = Script.NewArray();
			foreach (var i in args)
				Script.Push(a, i);
			return Bridge.New(name, a);
		}

		public static int ParseInt(string value, int radix = 10)
		{
			return (int)Microsoft.JScript.GlobalObject.parseInt(value, radix);
		}

		public static double ParseFloat(string value)
		{
			return Microsoft.JScript.GlobalObject.parseFloat(value);
		}

		public static bool IsNaN(object num)
		{
			return Microsoft.JScript.GlobalObject.isNaN(num);
		}

		public object Get(object scope, string name)
		{
			return Bridge.Get(scope, name);
		}

		public T Get<T>(object scope, string name)
		{
			var ret = Bridge.Get(scope, name);
			if ((ret == null) || (ret is DBNull))
				return default(T);
			return (T)ret;
		}

		public void Set(object scope, string name, object value)
		{
			Bridge.Set(scope, name, value);
		}

		public string Replace(string str, dynamic regex, string dst)
		{
			return Bridge.Replace(str, regex, dst);
		}

		public string Split(string str, dynamic regex)
		{
			return Bridge.Split(str, regex);
		}

		public object Match(string str, dynamic regex)
		{
			return Bridge.Match(str, regex);
		}

		public Element CreateElement(string tagname)
		{
			return Element.Create(this, Bridge.CreateElement(tagname));
		}

		public Element CreateTextNode(string text)
		{
			return Element.Create(this, Bridge.CreateTextNode(text));
		}

		public HTMLElement GetElementById(string id)
		{
			return HTMLElement.Create(this, Bridge.GetElementById(id));
		}

		public void PreventDefault(Event @event)
		{
			Bridge.PreventDefault(@event.instance);
		}

		public void StopPropagation(Event @event)
		{
			Bridge.StopPropagation(@event.instance);
		}

		public string Stringify(object value)
		{
			return Bridge.Stringify(value);
		}

		public object Parse(string text)
		{
			return Bridge.Parse(text);
		}

		public static string EncodeURI(string url)
		{
			return Microsoft.JScript.GlobalObject.encodeURI(url);
		}

		public static string DecodeURI(string url)
		{
			return Microsoft.JScript.GlobalObject.decodeURI(url);
		}

		public dynamic NewRegExp(string pattern, string flag)
		{
			return Bridge.NewRegExp(pattern, flag);
		}

		public static string RegExpEscape(string s)
		{
			return Microsoft.JScript.GlobalObject.escape(s);
		}

		public Element ParseXML(string data)
		{
			return new Element(this, Bridge.ParseXML(data));
		}

		public static string[] GetFieldNames(object comObj)
		{
			JsArray<string> result = new JsArray<string>();

			IDispatch dispatch = comObj as IDispatch;
			if (dispatch == null)
				return result.ToArray();

			System.Runtime.InteropServices.ComTypes.ITypeInfo typeInfo = null;
			try {
				if (dispatch.GetTypeInfo(0, 0, out typeInfo) != 0)
					return result.ToArray();

				IntPtr _typeAttr;
				typeInfo.GetTypeAttr(out _typeAttr);
				System.Runtime.InteropServices.ComTypes.TYPEATTR typeAttr = (System.Runtime.InteropServices.ComTypes.TYPEATTR)Marshal.PtrToStructure(_typeAttr, typeof(System.Runtime.InteropServices.ComTypes.TYPEATTR));
				typeInfo.ReleaseTypeAttr(_typeAttr);

				for (int i = 0; i < typeAttr.cVars; i++) {
					IntPtr _varDesc;
					typeInfo.GetVarDesc(i, out _varDesc);
					System.Runtime.InteropServices.ComTypes.VARDESC varDesc = (System.Runtime.InteropServices.ComTypes.VARDESC)Marshal.PtrToStructure(_varDesc, typeof(System.Runtime.InteropServices.ComTypes.VARDESC));
					typeInfo.ReleaseVarDesc(_varDesc);

					int j;
					string[] names = new string[5];
					typeInfo.GetNames(varDesc.memid, names, names.Length, out j);
					Array.Resize(ref names, j);

					foreach (string name in names) {
						result.Push(name);
					}
				}
			}
			finally {
				if (typeInfo != null) Marshal.ReleaseComObject(typeInfo);
			}

			return result.ToArray();
		}

		public static string[] GetFuncNames(object comObj)
		{
			JsArray<string> result = new JsArray<string>();

			IDispatch dispatch = comObj as IDispatch;
			if (dispatch == null)
				return result.ToArray();

			System.Runtime.InteropServices.ComTypes.ITypeInfo typeInfo = null;
			try {
				if (dispatch.GetTypeInfo(0, 0, out typeInfo) != 0)
					return result.ToArray();

				IntPtr _typeAttr;
				typeInfo.GetTypeAttr(out _typeAttr);
				System.Runtime.InteropServices.ComTypes.TYPEATTR typeAttr = (System.Runtime.InteropServices.ComTypes.TYPEATTR)Marshal.PtrToStructure(_typeAttr, typeof(System.Runtime.InteropServices.ComTypes.TYPEATTR));
				typeInfo.ReleaseTypeAttr(_typeAttr);

				for (int i = 0; i < typeAttr.cFuncs; i++) {
					IntPtr _funcDesc;
					typeInfo.GetFuncDesc(i, out _funcDesc);
					System.Runtime.InteropServices.ComTypes.FUNCDESC funcDesc = (System.Runtime.InteropServices.ComTypes.FUNCDESC)Marshal.PtrToStructure(_funcDesc, typeof(System.Runtime.InteropServices.ComTypes.FUNCDESC));
					typeInfo.ReleaseFuncDesc(_funcDesc);

					int j;
					string[] names = new string[5];
					typeInfo.GetNames(funcDesc.memid, names, names.Length, out j);
					Array.Resize(ref names, j);

					foreach (string name in names) {
						result.Push(name);
					}
				}
			}
			finally {
				if (typeInfo != null) Marshal.ReleaseComObject(typeInfo);
			}

			return result.ToArray();
		}

		public static bool HasMember(object obj, string name)
		{
			var a = GetFieldNames(obj);
			return a.Contains(name);
		}

		public void console_log(string text)
		{
			ScriptHost.console_log((object)text);
		}

		public void console_warn(string text)
		{
			ScriptHost.console_warn((object)text);
		}
	}

	public class Union<T1, T2>
	{
		object value;

		public Union(object value)
		{
			if (value is T1 || value is T2)
				this.value = value;
			else
				throw new ArgumentException();
		}

		public object Value { get { return value; } }

		public T As<T>() { if (value is T) return (T)value; else return default(T); }

		public bool Is<T>() { return value is T; }

		public static implicit operator Union<T1, T2>(T1 value)
		{
			return new Union<T1, T2>(value);
		}

		public static implicit operator Union<T1, T2>(T2 value)
		{
			return new Union<T1, T2>(value);
		}

		public static explicit operator T1(Union<T1, T2> any)
		{
			return any == null ? default(T1) : (T1)any.value;
		}

		public static explicit operator T2(Union<T1, T2> any)
		{
			return any == null ? default(T2) : (T2)any.value;
		}

		public override string ToString()
		{
			return value.ToString();
		}
	}

	public class Union<T1, T2, T3>
	{
		object value;

		public Union(object value)
		{
			if (value is T1 || value is T2 || value is T3)
				this.value = value;
			else
				throw new ArgumentException();
		}

		public object Value { get { return value; } }

		public T As<T>() { if (value is T) return (T)value; else return default(T); }

		public bool Is<T>() { return value is T; }

		public static implicit operator Union<T1, T2, T3>(T1 value)
		{
			return new Union<T1, T2, T3>(value);
		}

		public static implicit operator Union<T1, T2, T3>(T2 value)
		{
			return new Union<T1, T2, T3>(value);
		}

		public static implicit operator Union<T1, T2, T3>(T3 value)
		{
			return new Union<T1, T2, T3>(value);
		}

		public static explicit operator T1(Union<T1, T2, T3> any)
		{
			return (T1)any.value;
		}

		public static explicit operator T2(Union<T1, T2, T3> any)
		{
			return (T2)any.value;
		}

		public static explicit operator T3(Union<T1, T2, T3> any)
		{
			return (T3)any.value;
		}

		public override string ToString()
		{
			return value.ToString();
		}
	}

	public class Union<T1, T2, T3, T4>
	{
		object value;

		public Union(object value)
		{
			if (value is T1 || value is T2 || value is T3 || value is T4)
				this.value = value;
			else
				throw new ArgumentException();
		}

		public object Value { get { return value; } }

		public T As<T>() { if (value is T) return (T)value; else return default(T); }

		public bool Is<T>() { return value is T; }

		public static implicit operator Union<T1, T2, T3, T4>(T1 value)
		{
			return new Union<T1, T2, T3, T4>(value);
		}

		public static implicit operator Union<T1, T2, T3, T4>(T2 value)
		{
			return new Union<T1, T2, T3, T4>(value);
		}

		public static implicit operator Union<T1, T2, T3, T4>(T3 value)
		{
			return new Union<T1, T2, T3, T4>(value);
		}

		public static implicit operator Union<T1, T2, T3, T4>(T4 value)
		{
			return new Union<T1, T2, T3, T4>(value);
		}

		public static explicit operator T1(Union<T1, T2, T3, T4> any)
		{
			return (T1)any.value;
		}

		public static explicit operator T2(Union<T1, T2, T3, T4> any)
		{
			return (T2)any.value;
		}

		public static explicit operator T3(Union<T1, T2, T3, T4> any)
		{
			return (T3)any.value;
		}

		public static explicit operator T4(Union<T1, T2, T3, T4> any)
		{
			return (T4)any.value;
		}

		public override string ToString()
		{
			return value.ToString();
		}
	}

	public static class StringExtention
	{
		public static string Replace(this string str, Text.RegularExpressions.Regex regex, string dst)
		{
			var Script = regex.Script;
			return Script.Replace(str, regex.instance, dst);
		}

		public static string[] Split(this string str, Text.RegularExpressions.Regex regex)
		{
			var Script = regex.Script;
			var ret = Script.Split(str, regex.instance);
			if ((ret == null) || (ret is DBNull))
				return null;
			int len = Script.Get(ret, "length");
			var result = new JsArray<string>(len);
			for (int i = 0; i < len; i++) {
				result.Push(Script.Get<string>(ret, i.ToString()));
			}
			return result.ToArray();
		}

		public static string[] Match(this string str, Text.RegularExpressions.Regex regex)
		{
			var Script = regex.Script;
			var ret = Script.Match(str, regex.instance);
			if ((ret == null) || (ret is DBNull))
				return null;
			int len = Script.Get(ret, "length");
			var result = new JsArray<string>(len);
			for (int i = 0; i < len; i++) {
				result.Push(Script.Get<string>(ret, i.ToString()));
			}
			return result.ToArray();
		}

		public static string Join(this string[] str, string sep)
		{
			return String.Join(sep, str);
		}

		public static string CharAt(this string str, int index)
		{
			return str[index].ToString();
		}

		public static string[] Split(this string str, string sep)
		{
			return str.Split(new string[] { sep }, StringSplitOptions.None);
		}

		public static string Substr(this string str, int start)
		{
			return str.Substring(start);
		}

		public static int LocaleCompare(this string str, string b)
		{
			return str.CompareTo(b);
		}
	}

	public static class StringListExtention
	{
		public static string Join(this IEnumerable<string> list, string separator)
		{
			return String.Join(separator, list);
		}
	}

	public interface IJsArray<T> : IEnumerable<T>
	{
		int Length { get; }
		T this[int index] { get; set; }
		void Push(T item);
		void PushRange(IEnumerable<T> items);
	}

	public interface IReadOnlyJsArray<out T> : IEnumerable<T>
	{
		int Length { get; }
		T this[int index] { get; }
	}

	public class JsArray<T> : IJsArray<T>, IReadOnlyJsArray<T>
	{
		private List<T> _List;

		public JsArray()
		{
			_List = new List<T>();
		}

		public JsArray(int capacity)
		{
			_List = new List<T>(capacity);
		}

		public JsArray(IEnumerable<T> collection)
		{
			_List = new List<T>(collection);
		}

		public int Length { get { return _List.Count; } }

		public bool Contains(T item)
		{
			return _List.Contains(item);
		}

		public T this[int index] {
			get { return _List[index]; }
			set { _List[index] = value; }
		}

		public void Push(T item)
		{
			_List.Add(item);
		}

		public void PushRange(IEnumerable<T> items)
		{
			_List.AddRange(items);
		}

		public void Pop()
		{
			_List.RemoveAt(_List.Count - 1);
		}

		public void Add(T item)
		{
			_List.Add(item);
		}

		public JsArray<T> Concat(IEnumerable<T> items)
		{
			var result = new JsArray<T>(this);
			result._List.AddRange(items);
			return result;
		}

		public IEnumerator<T> GetEnumerator()
		{
			return _List.GetEnumerator();
		}

		IEnumerator IEnumerable.GetEnumerator()
		{
			return _List.GetEnumerator();
		}

		public int IndexOf(T item)
		{
			return _List.IndexOf(item);
		}

		public void Splice(int start, int len)
		{
			if (start == 0 && len == _List.Count)
				_List.Clear();
			else {
				for (int i = 0; i < len; i++) {
					_List.RemoveAt(start);
				}
			}
		}

		public void Sort(Comparison<T> comparison)
		{
			_List.Sort(comparison);
		}

		public static implicit operator T[] (JsArray<T> target)
		{
			return target._List.ToArray();
		}

		public bool Remove(T item)
		{
			return _List.Remove(item);
		}
	}

	public static class JsArrayExtention
	{
		public static void Concat<T>(this List<T> first, IEnumerable<T> second)
		{
			first.AddRange(second);
		}
	}

	public class ExternalAttribute : Attribute
	{

	}

	public class FieldPropertyAttribute : Attribute
	{

	}

	public class NameAttribute : Attribute
	{
		public NameAttribute(bool charcase)
		{
		}

		public NameAttribute(string name)
		{

		}
	}

	public class Location
	{
		public dynamic instance;
		public Script Script { get; private set; }

		public string parentId { get { return instance.parentId; } }
		public string inputName { get { return instance.inputName; } }
		public object coordinate { get { return instance.coordinate; } }

		public Location(Script script, object instance)
		{
			Script = script;
			this.instance = instance;
		}

		public void construct(string parentId, string inputName, object coordinate)
		{
			instance = Script.New("Blockly.Location", new[] { parentId, inputName, coordinate });
			instance.parentId = parentId;
			instance.inputName = inputName;
			instance.coordinate = coordinate;
		}
	}

	/// <summary> 
	/// Exposes objects, methods and properties to programming tools and other 
	/// applications that support Automation. 
	/// </summary> 
	[ComImport()]
	[Guid("00020400-0000-0000-C000-000000000046")]
	[InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
	interface IDispatch
	{
		[PreserveSig]
		int GetTypeInfoCount(out int Count);

		[PreserveSig]
		int GetTypeInfo(
			[MarshalAs(UnmanagedType.U4)] int iTInfo,
			[MarshalAs(UnmanagedType.U4)] int lcid,
			out System.Runtime.InteropServices.ComTypes.ITypeInfo typeInfo);

		[PreserveSig]
		int GetIDsOfNames(
			ref Guid riid,
			[MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.LPWStr)]
			string[] rgsNames,
			int cNames,
			int lcid,
			[MarshalAs(UnmanagedType.LPArray)] int[] rgDispId);

		[PreserveSig]
		int Invoke(
			int dispIdMember,
			ref Guid riid,
			uint lcid,
			ushort wFlags,
			ref System.Runtime.InteropServices.ComTypes.DISPPARAMS pDispParams,
			out object pVarResult,
			ref System.Runtime.InteropServices.ComTypes.EXCEPINFO pExcepInfo,
			IntPtr[] pArgErr);
	}
}

namespace Bridge.Html5
{
	public class Node
	{
		public dynamic instance;
		public Script Script { get; internal set; }
		public string TextContent { get; }

		public Node()
		{
		}

		public Node(Script script, object instance)
		{
			Script = script;
			this.instance = instance;
		}

		public string NodeName { get { return instance.nodeName; } }
		public string NodeValue { get { return instance.nodeValue; } }
		public NodeList<Node> ChildNodes { get { return new NodeList<Node>(Script, instance.childNodes); } }

		public void AppendChild(Node parameter)
		{
			instance.appendChild(parameter.instance);
		}
	}

	public class NodeList<T> : IEnumerable<T> where T : Node, new()
	{
		public dynamic instance;
		public Script Script { get; private set; }

		public NodeList(Script script, object instance)
		{
			Script = script;
			this.instance = instance;
		}

		public T this[int index] {
			get {
				return Element.Create(Script, Script.Get(instance, index.ToString()));
			}
		}

		public int Length { get { return instance.length; } }

		public class Enumerator : IEnumerator<T>
		{
			private NodeList<T> nodeList;
			private int index = -1;

			public Enumerator(NodeList<T> jquery)
			{
				this.nodeList = jquery;
			}

			T IEnumerator<T>.Current {
				get {
					var node = new T();
					node.Script = nodeList.Script;
					node.instance = nodeList.Script.Get(nodeList.instance, index.ToString());
					return node;
				}
			}

			public object Current {
				get {
					return ((IEnumerator<T>)this).Current;
				}
			}

			public void Dispose()
			{
			}

			public bool MoveNext()
			{
				index++;
				return index < nodeList.Length;
			}

			public void Reset()
			{
				index = -1;
			}
		}

		public IEnumerator<T> GetEnumerator()
		{
			return new Enumerator(this);
		}

		IEnumerator IEnumerable.GetEnumerator()
		{
			return GetEnumerator();
		}
	}

	public enum ElementType
	{
		Anchor = 0,
		Area = 1,
		Audio = 2,
		Base = 3,
		Body = 4,
		BR = 5,
		Button = 6,
		Canvas = 7,
		DataList = 8,
		Div = 9,
		DList = 10,
		Embed = 11,
		FieldSet = 12,
		Form = 13,
		Head = 14,
		H1 = 15,
		H2 = 16,
		H3 = 17,
		H4 = 18,
		H5 = 19,
		H6 = 20,
		HR = 21,
		Html = 22,
		IFrame = 23,
		Image = 24,
		Input = 25,
		Keygen = 26,
		Label = 27,
		Legend = 28,
		LI = 29,
		Link = 30,
		Map = 31,
		Media = 32,
		Meta = 33,
		Meter = 34,
		Mod = 35,
		Object = 36,
		OList = 37,
		OptGroup = 38,
		Option = 39,
		Output = 40,
		Paragraph = 41,
		Param = 42,
		Pre = 43,
		Progress = 44,
		Quote = 45,
		Script = 46,
		Select = 47,
		Source = 48,
		Span = 49,
		Style = 50,
		TableCaption = 51,
		TableCell = 52,
		TableCol = 53,
		TableDataCell = 54,
		Table = 55,
		TableHeaderCell = 56,
		TableRow = 57,
		TableSection = 58,
		TextArea = 59,
		Title = 60,
		Track = 61,
		UList = 62,
		Video = 63
	}

	public class Element : Node
	{
		public string InnerHTML { get { return instance.innerHTML; } set { instance.innerHTML = value; } }

		public string OuterHTML { get { return instance.outerHTML; } }

		public Element()
		{
		}

		public Element(Script script, object instance)
			: base(script, instance)
		{
			if (instance is ElementType) {
				string tagname = ToTagName((ElementType)instance);
				this.instance = Script.CreateElement(tagname).instance;
			}
		}

		public static string ToTagName(ElementType instance)
		{
			var tagname = instance.ToString().ToLower();
			switch (tagname) {
			case "anchor": tagname = "a"; break;
			case "datalist": tagname = "dl"; break;
			case "olist": tagname = "ol"; break;
			case "paragraph": tagname = "p"; break;
			case "quote": tagname = "q"; break;
			case "tablecaption": tagname = "caption"; break;
			case "tabledatacell": tagname = "td"; break;
			case "tableheadercell": tagname = "th"; break;
			case "tablerow": tagname = "tr"; break;
			case "ulist": tagname = "ul"; break;
			}
			return tagname;
		}

		public static Element Create(Script script, dynamic instance)
		{
			if ((instance == null) || (instance is DBNull))
				return null;
			return new Element(script, instance);
		}

		public string GetAttribute(string name)
		{
			var ret = instance.getAttribute(name);
			if ((ret == null) || (ret is DBNull))
				return null;
			return (string)ret;
		}

		public void SetAttribute(string name, string value)
		{
			instance.setAttribute(name, value);
		}

		public NodeList<Element> GetElementsByTagName(string tagname)
		{
			return new NodeList<Element>(Script, instance.getElementsByTagName(tagname));
		}
	}

	public class Text : Node
	{
		public Text(Script script, object instance)
			: base(script, instance)
		{
			if (instance is string) {
				this.instance = Script.CreateTextNode((string)instance).instance;
			}
		}
	}

	public class Document
	{
		public Script Script { get; private set; }

		public Document(Script script)
		{
			Script = script;
		}

		public Element CreateElement(string tagname)
		{
			return Script.CreateElement(tagname);
		}

		public Element CreateTextNode(string text)
		{
			return Script.CreateTextNode(text);
		}

		public HTMLElement GetElementById(string id)
		{
			return Script.GetElementById(id);
		}
	}

	public class HTMLElement : Element
	{
		public HTMLElement(Script script, object instance)
			: base(script, instance)
		{
		}

		public new static HTMLElement Create(Script script, dynamic instance)
		{
			if ((instance == null) || (instance is DBNull))
				return null;
			return new HTMLElement(script, instance);
		}
	}

	public class SVGMatrix
	{
		public dynamic instance;

		public SVGMatrix(dynamic instance)
		{
			this.instance = instance;
		}
	}

	public class Event
	{
		public dynamic instance;
		public Script Script { get; private set; }

		public Event(Script script, object instance)
		{
			Script = script;
			this.instance = instance;
		}

		public void PreventDefault()
		{
			Script.PreventDefault(this);
		}

		public void StopPropagation()
		{
			Script.StopPropagation(this);
		}
	}

	public class JSON
	{
		public static string Stringify(object value)
		{
			return Codeplex.Data.DynamicJson.Serialize(value);
		}

		public static object Parse(string text)
		{
			return Codeplex.Data.DynamicJson.Parse(text);
		}
	}
}

namespace Bridge.Text.RegularExpressions
{
	public class Regex
	{
		public dynamic instance;
		public Script Script { get; private set; }

		public Regex(Script script, object instance)
		{
			Script = script;
			this.instance = instance;
		}

		public Regex(Script script, string patern, string flag = "")
		{
			Script = script;
			instance = Script.NewRegExp(patern, flag);
		}

		public bool Global { get { return instance.global; } }
		public bool IgnoreCase { get { return instance.ignoreCase; } }
		public int LastIndex { get { return instance.lastIndex; } set { instance.lastIndex = value; } }
		public bool Multiline { get { return instance.multiline; } }
		public string Pattern { get { return instance.pattern; } }
		public string Source { get { return instance.source; } }

		public static string Escape(string s)
		{
			return Script.RegExpEscape(s);
		}

		public Matches Exec(string s)
		{
			return new Matches(Script, instance.exec(s));
		}

		public bool Test(string s)
		{
			return instance.test(s);
		}
	}

	public class Matches
	{
		public dynamic instance;
		public Script Script { get; private set; }

		public Matches(Script script, object instance)
		{
			Script = script;
			this.instance = instance;
		}

		public int index { get { return instance.index; } }

		public string this[int i] { get { return Script.Get(instance, i.ToString()); } }

		public static implicit operator string(Matches m)
		{
			return m.instance;
		}
	}
}

namespace Bridge.jQuery2
{
	public class jQuery : IEnumerable
	{
		public dynamic instance;
		Script Script;

		public jQuery(Script script, object instance = null)
		{
			Script = script;
			this.instance = instance;
		}

		public jQuery New(string val)
		{
			return new jQuery(Script, Script.Bridge.jqNew(val));
		}

		public jQuery New(Element val)
		{
			return new jQuery(Script, Script.Bridge.jqNew(val.instance));
		}

		public jQuery New(ElementType val)
		{
			return new jQuery(Script, Script.Bridge.jqNew("<" + Element.ToTagName(val) + ">"));
		}

		public jQuery Select(string selector, jQuery parent = null)
		{
			if (parent == null)
				return new jQuery(Script, Script.Bridge.Select(selector));
			else
				return new jQuery(Script, Script.Bridge.Select2(selector, parent.instance));
		}

		public string Attr(string attr)
		{
			return Script.Bridge.Attr(instance, attr);
		}

		public void Attr(string attr, string val)
		{
			Script.Bridge.Attr2(instance, attr, val);
		}

		public void ReplaceWith(jQuery val)
		{
			Script.Bridge.ReplaceWith(instance, val.instance);
		}

		public string Val()
		{
			return Script.Bridge.Val(instance);
		}

		public void Val(string val)
		{
			Script.Bridge.Val1(instance, val);
		}

		public jQuery Get()
		{
			return new jQuery(Script, Script.Bridge.jqGet(instance));
		}

		public Element Get(int val)
		{
			return new Element(Script, Script.Bridge.jqGet2(instance, val));
		}

		public jQuery Parent()
		{
			return new jQuery(Script, Script.Bridge.Parent(instance));
		}

		public void Append(Element ele)
		{
			Script.Bridge.Append(instance, ele.instance);
		}

		public string Text()
		{
			return Script.Bridge.Text(instance);
		}

		public void Text(string text)
		{
			Script.Bridge.Text2(instance, text);
		}

		public string Html()
		{
			return Script.Bridge.Html(instance);
		}

		public void Html(string html)
		{
			Script.Bridge.Html2(instance, html);
		}

		public void RemoveAttr(string attr)
		{
			Script.Bridge.RemoveAttr(instance, attr);
		}

		public void Click(object state, Action<jQueryMouseEvent> handler)
		{
			Script.Bridge.Click(instance, state, Script.NewFunc(new Action<object>((e) => {
				handler(new jQueryMouseEvent(Script, e));
			})));
		}

		public jQuery ButtonToggle()
		{
			return new jQuery(Script, Script.Bridge.ButtonToggle(instance));
		}

		public bool Is(string val)
		{
			return Script.Bridge.Is(instance, val);
		}

		public jQuery Children()
		{
			return new jQuery(Script, Script.Bridge.Children(instance));
		}

		public jQuery Find(string selector)
		{
			return new jQuery(Script, Script.Bridge.Find(instance, selector));
		}

		public jQuery Remove()
		{
			return new jQuery(Script, Script.Bridge.Remove(instance));
		}

		public jQuery Append(jQuery content)
		{
			return new jQuery(Script, Script.Bridge.Append(instance, content.instance));
		}

		public jQuery Append(string content)
		{
			return new jQuery(Script, Script.Bridge.Append(instance, content));
		}

		public jQuery Show()
		{
			return new jQuery(Script, Script.Bridge.Show(instance));
		}

		public jQuery Hide()
		{
			return new jQuery(Script, Script.Bridge.Hide(instance));
		}

		public jQuery On(string events, Action<jQueryMouseEvent> handler)
		{
			return new jQuery(Script, Script.Bridge.On(instance, events, Script.NewFunc(new Action<object>((e) => {
				handler(new jQueryMouseEvent(Script, e));
			}))));
		}

		public int Length { get { return Script.Get<int>(instance, "length"); } }

		public class Enumerator : IEnumerator<Element>
		{
			private jQuery jquery;
			private int index = -1;

			public Enumerator(jQuery jquery)
			{
				this.jquery = jquery;
			}

			Element IEnumerator<Element>.Current {
				get { return new Element(jquery.Script, jquery.Script.Get(jquery.instance, index.ToString())); }
			}

			public object Current {
				get {
					return ((IEnumerator<Element>)this).Current;
				}
			}

			public void Dispose()
			{
			}

			public bool MoveNext()
			{
				index++;
				return index < jquery.Length;
			}

			public void Reset()
			{
				index = -1;
			}
		}

		IEnumerator IEnumerable.GetEnumerator()
		{
			return new Enumerator(this);
		}
	}

	public class jQueryMouseEvent : Html5.Event
	{
		public object Data { get { return instance.data; } }

		public jQueryMouseEvent(Script script, object instance)
			: base(script, instance)
		{

		}
	}

	public class AjaxOptions
	{
		public string Url;
		public string DataType;
		public Action<object, string, jqXHR> Success;
		public Action<jqXHR, string, string> Error;
	}

	public class jqXHR
	{

	}
}
