using MicroServer.Logging;
using System;
using System.Collections;
using System.IO;

namespace MicroServer.Net.Http.Files
{
	/// <summary>
	/// Serves files from the hard drive.
	/// </summary>
	public class DiskFileService : IFileService
	{
		private readonly string _basePath;
		private readonly string _rootUri;

		/// <summary>
		/// Initializes a new instance of the <see cref="CompositeFileService"/> class.
		/// </summary>
		/// <param name="rootFilePath">Path to serve files from.</param>
		/// <param name="rootUri">Serve all files which are located under this URI</param>
		/// <example>
		/// <code>
		/// var diskFiles = new DiskFileService("/public/", @"\sd\www\public\");
		/// var module = new FileModule(diskFiles);
		/// 
		/// var moduleManager = new ModuleManager();
		/// moduleManager.Add(module);
		/// </code>
		/// </example>
		public DiskFileService(string rootUri, string rootFilePath)
		{
			if (rootUri == null) throw new ArgumentNullException("rootUri");
			if (rootFilePath == null) throw new ArgumentNullException("rootFilePath");
			if (!Directory.Exists(rootFilePath)) {
				try {
					Directory.CreateDirectory(rootFilePath);
				}
				catch {
					Logger.WriteInfo(this, "Error creating directory: " + rootFilePath);
				}

				if (!Directory.Exists(rootFilePath)) {
					throw new ArgumentOutOfRangeException("rootFilePath", "Failed to find path " + rootFilePath);
				}
				else {
					Logger.WriteInfo(this, "Created directory: " + rootFilePath);
				}
			}

			_rootUri = rootUri;
			_basePath = rootFilePath;
		}

		#region IFileService Members

		/// <summary>
		/// Get a file
		/// </summary>
		/// <param name="context">Context used to locate and return files</param>
		public virtual bool GetFile(FileContext context)
		{
			var fullPath = GetFullPath(context.Request.UriRewrite);
			return GetFile(context, fullPath);
		}

		/// <summary>
		/// Get a file
		/// </summary>
		/// <param name="context">Context used to locate and return files</param>
		/// <param name="fullPath">Full path used to locate files</param>
		public virtual bool GetFile(FileContext context, string fullPath)
		{
			if (fullPath == null || !File.Exists(fullPath))
				return false;

			var fi = new FileInfo(fullPath);
			var date = fi.LastWriteTimeUtc;

			// browser ignores second fractions
			date = date.AddTicks(-(date.Ticks % TimeSpan.TicksPerSecond));

			if (date <= context.BrowserCacheDate) {
				context.SetNotModified(fullPath, date);
				return true;
			}

			try {
				var stream = new FileStream(fullPath, FileMode.Open, FileAccess.Read, FileShare.ReadWrite);
				context.SetFile(fullPath, stream, date);
				return true;
			}
			catch {
				return false;
			}
		}

		private string GetFullPath(Uri uri)
		{
			if (!uri.AbsolutePath.StartsWith(_rootUri))
				return String.Empty;

			var relativeUri = uri.AbsolutePath.TrimStart(_rootUri.ToCharArray());
			if (relativeUri == null || relativeUri == String.Empty) {
				return _basePath;
			}
			else {
				return Path.Combine(_basePath, relativeUri.Replace("/", Path.DirectorySeparatorChar.ToString()).Split('?')[0]);
			}
		}

		/// <summary>
		/// Gets if the specified url corresponds to a directory serving files
		/// </summary>
		/// <param name="uri">Uri</param>
		/// <returns></returns>
		public bool IsDirectory(Uri uri)
		{
			var path = GetFullPath(uri);
			return path != null && Directory.Exists(path);
		}

		/// <summary>
		/// Get all files that exists in the specified directory
		/// </summary>
		/// <param name="uri">Uri</param>
		/// <returns></returns>
		public ArrayList GetFiles(Uri uri)
		{
			var path = GetFullPath(uri);
			if (path == null || !Directory.Exists(path))
				return null;

			var results = new ArrayList();
			foreach (var file in Directory.GetFiles(path)) {
				var mimeType = MimeTypeProvider.Instance.Get(Path.GetFileName(file));
				if (mimeType == null)
					continue;

				var info = new FileInfo(file);
				results.Add(new FileInformation {
					LastModifiedAtUtc = info.LastWriteTimeUtc,
					Name = Path.GetFileName(file),
					Size = (int)info.Length
				});
			}
			return results;
		}

		/// <summary>
		/// Gets a list of all sub directories
		/// </summary>
		/// <param name="uri">URI (as requested by the HTTP client) which should correspond to a directory.</param>
		/// <returns></returns>
		public ArrayList GetDirectories(Uri uri)
		{
			var path = GetFullPath(uri);
			if (path == null || !Directory.Exists(path))
				return null;

			var results = new ArrayList();

			foreach (var directory in Directory.GetDirectories(path)) {
				if (directory.StartsWith("."))
					continue;

				var info = new DirectoryInfo(directory);
				results.Add(new DirectoryInformation {
					Root = info.Root,
					Parent = info.Parent,
					LastModifiedAtUtc = info.LastWriteTimeUtc,
					Name = Path.GetFileName(directory),

				});
			}

			return results;
		}

		#endregion
	}
}