var fs = WScript.CreateObject("Scripting.FileSystemObject");

function Compare(a, b)
{
	var stream = WScript.CreateObject('ADODB.Stream');
	//stream.Type=1;//1=binary,2=text
	stream.type = 2;	//adTypeText
	stream.charset = "ascii";

	stream.Open();
	stream.LoadFromFile(fs.GetAbsolutePathName(a));
	//var fa = stream.Read(-1);//-1:read all , -2 : read line
	var fa = stream.ReadText();
	stream.Close();

	stream.Open();
	stream.LoadFromFile(fs.GetAbsolutePathName(b));
	//var fb = stream.Read(-1);
	var fb = stream.ReadText();
	stream.Close();

	if (fa.length != fb.length)
		return false;

	for (var i = 0; i < fa.length; i++) {
		if(fa.charCodeAt(i) != fb.charCodeAt(i))
			return false;
	}

	return true;
}

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

			if(!Compare(src, dst))
			{
				fs.CopyFile(src_file, dst_file, true);

				WScript.StdOut.Write(dst_file);
				WScript.StdOut.WriteLine("をコピーしました。");
			}
		}
		else
		{
			fs.CopyFile(src_file, dst_file);

			WScript.StdOut.Write(dst_file);
			WScript.StdOut.WriteLine("をコピーしました。");
		}
	}
	else
	{
		if(fs.FileExists(dst_file))
		{
			var dst = fs.GetFile(dst_file);
			
			fs.DeleteFile(dst_file);

			WScript.StdOut.Write(dst_file);
			WScript.StdOut.WriteLine("を削除しました。");
		}
		else
		{
			WScript.StdOut.WriteLine("指定されたファイルがありません。");
			WScript.StdOut.WriteLine(src_file);
			WScript.StdOut.WriteLine(dst_file);
		}
	}
}

