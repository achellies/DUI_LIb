
// ----- ExeScript Options Begin -----
// ScriptType: window,invoker
// DestDirectory: temp
// Icon: default
// ----- ExeScript Options End -----
try
{
	var shell = new ActiveXObject("WScript.shell");
	var regPath = "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\9.0\\Setup\\VC\\ProductDir";
	var vcPath = shell.RegRead(regPath);
	var projPath = vcPath + "vcprojects\\";
	var wizPath = vcPath + "VCWizards\\";
	var fso = new ActiveXObject("Scripting.FileSystemObject");
	var wizFolder = "DuilibWizard\\";

	if (fso.FileExists(projPath + "DuilibWizard.ico"))
		fso.DeleteFile(projPath + "DuilibWizard.ico", true);
	if (fso.FileExists(projPath + "DuilibWizard.vsz"))
		fso.DeleteFile(projPath + "DuilibWizard.vsz", true);
	if (fso.FileExists(projPath + wizFolder + "DuilibWizard.vsdir"))
		fso.DeleteFile(projPath + wizFolder + "DuilibWizard.vsdir", true);
	
	if (fso.FolderExists(projPath + wizFolder))
		fso.DeleteFolder(projPath + wizFolder, true);

	if (fso.FolderExists(wizPath + wizFolder))
		fso.DeleteFolder(wizPath + wizFolder, true);
	
	WScript.Echo("Uninstall successfully !");
}
catch(e)
{
	throw(e);
}