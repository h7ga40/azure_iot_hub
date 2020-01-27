var fs = WScript.CreateObject("Scripting.FileSystemObject");

args = WScript.Arguments;

if(args.length >= 2)
{
	var src_file = args(0);
	var dst_file = args(1);

	if(fs.FileExists(src_file))
	{
		if(fs.FileExists(dst_file))
		{
			var src = fs.GetFile(src_file);
			var dst = fs.GetFile(dst_file);

			if(src.DateLastModified > dst.DateLastModified)
			{
				fs.CopyFile(src_file, dst_file, true);

				WScript.StdOut.Write(dst_file);
				WScript.StdOut.WriteLine("���R�s�[���܂����B");
			}
		}
		else
		{
			fs.CopyFile(src_file, dst_file);

			WScript.StdOut.Write(dst_file);
			WScript.StdOut.WriteLine("���R�s�[���܂����B");
		}
	}
	else
	{
		if(fs.FileExists(dst_file))
		{
			var dst = fs.GetFile(dst_file);
			
			fs.DeleteFile(dst_file);

			WScript.StdOut.Write(dst_file);
			WScript.StdOut.WriteLine("���폜���܂����B");
		}
		else
		{
			WScript.StdOut.WriteLine("�w�肳�ꂽ�t�@�C��������܂���B");
			WScript.StdOut.WriteLine(src_file);
			WScript.StdOut.WriteLine(dst_file);
		}
	}
}

