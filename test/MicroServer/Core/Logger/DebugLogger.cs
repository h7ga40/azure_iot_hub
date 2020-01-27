
using System;
using System.Diagnostics;

namespace MicroServer.Logging
{
	/// <summary>
	/// DebugLogger class
	/// </summary>
	/// <remarks>Implements a Debug (Output) window based data logger for use in the Logger static class.</remarks>
	public class DebugLogger : ILogger
	{
		public void Dispose()
		{
		}

		public void WriteError(string message)
		{
			WriteToLog(message);
		}

		public void WriteError(object source, string message, Exception ex)
		{
			WriteToLog(String.Concat("[", DateTime.Now.ToString("MM/dd/yyyy HH:mm:ss"), "] ", source.ToString(), " | Error | ", message, " | ", ex.StackTrace.ToString()));
		}

		public void WriteInfo(string message)
		{
			WriteToLog(message);
		}

		public void WriteInfo(object source, string message)
		{
			WriteToLog(String.Concat("[", DateTime.Now.ToString("MM/dd/yyyy HH:mm:ss"), "] ", source.ToString(), " | Info | ", message));
		}

		public void WriteDebug(string message)
		{
			WriteToLog(message);
		}

		public void WriteDebug(object source, string message)
		{
			WriteToLog(String.Concat("[", DateTime.Now.ToString("MM/dd/yyyy HH:mm:ss"), "] ", source.ToString(), " | Debug | ", message));
		}

		private void WriteToLog(string message)
		{
			Debug.Print(message);
		}
	}
}
