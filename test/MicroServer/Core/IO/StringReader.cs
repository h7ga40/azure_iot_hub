using System;
using System.IO;
using System.Runtime.InteropServices;

namespace MicroServer.IO
{
	[Serializable]
	public class StringReader : TextReader
	{
		private string source;
		private int nextChar;
		private readonly int sourceLength;
		private static char[] cr_lf;

		public StringReader(string s)
		{
			if (s == null)
				throw new ArgumentNullException("s");

			source = s;
			nextChar = 0;
			sourceLength = s.Length;
		}

		public override void Close()
		{
			Dispose(true);
		}

		protected override void Dispose(bool disposing)
		{
			source = null;
			base.Dispose(disposing);
		}

		public override int Peek()
		{
			if (source == null)
				ObjectDisposedException();

			if (nextChar >= sourceLength)
				return -1;
			return source[nextChar];
		}

		public override int Read()
		{
			if (source == null)
				ObjectDisposedException();

			if (nextChar >= sourceLength)
				return -1;
			return source[nextChar++];
		}

		// The method will read up to count characters from the StringReader
		// into the buffer character array starting at position index. Returns
		// the actual number of characters read, or zero if the end of the string
		// has been reached and no characters are read.

		//public override int Read([In, Out] char[] buffer, int index, int count)
		public override int Read([Out] char[] buffer, int index, int count)
		{
			if (source == null)
				ObjectDisposedException();

			if (buffer == null)
				throw new ArgumentNullException("buffer");
			if (buffer.Length - index < count)
				throw new ArgumentException();
			if (index < 0 || count < 0)
				throw new ArgumentOutOfRangeException();

			int charsToRead;

			// reordered to avoir possible integer overflow
			if (nextChar > sourceLength - count)
				charsToRead = sourceLength - nextChar;
			else
				charsToRead = count;

			source.CopyTo(nextChar, buffer, index, charsToRead);

			nextChar += charsToRead;

			return charsToRead;
		}

		public override string ReadLine()
		{
			// Reads until next \r or \n or \r\n, otherwise return null

			if (source == null)
				ObjectDisposedException();

			if (nextChar >= source.Length)
				return null;

			if (cr_lf == null)
				cr_lf = new char[] { '\n', '\r' };

			var readto = source.IndexOfAny(cr_lf, nextChar);

			if (readto == -1)
				return ReadToEnd();

			var consecutive = source[readto] == '\r'
				&& readto + 1 < source.Length
				&& source[readto + 1] == '\n';

			var nextLine = source.Substring(nextChar, readto - nextChar);
			nextChar = readto + ((consecutive) ? 2 : 1);
			return nextLine;
		}

		public override string ReadToEnd()
		{
			if (source == null)
				ObjectDisposedException();
			var toEnd = source.Substring(nextChar, sourceLength - nextChar);
			nextChar = sourceLength;
			return toEnd;
		}

		private static void ObjectDisposedException()
		{
			throw new ObjectDisposedException("StringReader cannot read from a closed StringReader");
		}
	}
}
