var fs = WScript.CreateObject("Scripting.FileSystemObject");

args = WScript.Arguments;

if(args.length >= 2)
{
	var istrm = fs.OpenTextFile(args(0)); // cfg1_out.map
	var ostrm = fs.CreateTextFile(args(1)); // cfg1_out.syms

	var line;
	line = istrm.ReadLine();
	var re = /^\s+Address\s+Publics by Value\s+Rva\+Base\s+Lib\:Object$/;
	while(line.search(re) == -1){
		line = istrm.ReadLine();
		// ‹ó‰ñ‚µ
	}

	while(!istrm.AtEndOfStream){
		line = istrm.ReadLine();

		var newline;
		re=/\s+$/;
		newline = line.replace(re,"");
		re=/^\s*[0-9|a-f]+:[0-9|a-f]+\s+([0-9|a-z|A-Z|\?@_]+)\s+([0-9|a-f]+)\s+(f i|f  |   )\s+.+$/;
		if(newline.search(re) == -1)
			continue;

		var sectype;
		sectype = newline.replace(re,"$3");

		if((sectype == "f i") || (sectype == "f  ")){
			newline = newline.replace(re,"$2 T $1");
		}
		//else if(sectype == "rodata"){
		//	newline = newline.replace(re,"$2 R $1");
		//}
		else{
			newline = newline.replace(re,"$2 A $1");
		}

		ostrm.WriteLine(newline);
	}
	istrm.Close();
	ostrm.Close();
}
