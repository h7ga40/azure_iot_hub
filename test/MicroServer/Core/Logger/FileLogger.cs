
using MicroServer.Utilities;
using System;
using System.IO;

namespace MicroServer.Logging
{
	/// <summary>
	/// FileLogger class
	/// </summary>
	/// <remarks>Implements a file-based data logger for use in the Logger static class.</remarks>
	public class FileLogger : ILogger
	{
		public FileLogger()
		{
			isDisposed = false;
			sync = new object();
			streamWriter = null;
		}

		~FileLogger()
		{
			Dispose(false);
		}

		public void Dispose()
		{
			Dispose(true);
			GC.SuppressFinalize(this);
		}

		protected void Dispose(bool disposing)
		{
			if (isDisposed)
				return;

			if (disposing) {
				if (streamWriter != null) {
					streamWriter.Flush();
					streamWriter.Close();
					streamWriter.Dispose();
				}
			}

			streamWriter = null;
		}

		public void Open(string directory, string fileName, string fileExt = "log")
		{
			lock (sync) {
				if (streamWriter != null)
					streamWriter.Close();

				string fullPath = null;
				var name = fileName;
				for (var i = 1; i < 100; ++i) {
					fileName = String.Concat(name, i.ToString("D2"), ".", fileExt);
					fullPath = Path.Combine(directory, fileName);
					if (File.Exists(fullPath)) {
						if (i == 99) {
							fileName = String.Concat(name, "01", ".", fileExt);
							fullPath = Path.Combine(directory, fileName);
							break;
						}
					}
					else {
						break;
					}
				}

				if (!StringUtility.IsNullOrEmpty(fullPath)) {
					Directory.CreateDirectory(directory);
					streamWriter = new StreamWriter(File.Create(fullPath));
				}

				if (streamWriter != null) {
					streamWriter.WriteLine(String.Concat("Log file opened: ", DateTime.Now.ToString("MM/dd/yyyy | HH:mm:ss.fff")));
					streamWriter.Flush();
				}
			}
		}

		public void Close()
		{
			lock (sync) {
				if (streamWriter != null) {
					streamWriter.Flush();
					streamWriter.Close();
					streamWriter = null;
				}
			}
		}

		public void WriteError(string message)
		{
			WriteToLog(String.Concat("  ", message));
		}

		public void WriteError(object source, string message, Exception ex)
		{
			WriteToLog(String.Concat("[", DateTime.Now.ToString("MM/dd/yyyy HH:mm:ss"), "] ", source.ToString(), " | Error | ", message, " | ", ex.StackTrace.ToString()));
		}

		public void WriteInfo(string message)
		{
			WriteToLog(String.Concat("  ", message));
		}

		public void WriteInfo(object source, string message)
		{
			WriteToLog(String.Concat("[", DateTime.Now.ToString("MM/dd/yyyy HH:mm:ss"), "] ", source.ToString(), " | Info | ", message));
		}

		public void WriteDebug(string message)
		{
			WriteToLog(String.Concat("  ", message));
		}

		public void WriteDebug(object source, string message)
		{
			WriteToLog(String.Concat("[", DateTime.Now.ToString("MM/dd/yyyy HH:mm:ss"), "] ", source.ToString(), " | Debug | ", message));
		}

		private void WriteToLog(string message)
		{
			lock (sync) {
				if (streamWriter != null) {
					streamWriter.WriteLine(message);
					streamWriter.Flush();
				}
			}
		}

		private readonly bool isDisposed;
		private readonly object sync = null;
		private StreamWriter streamWriter = null;
	}
}
