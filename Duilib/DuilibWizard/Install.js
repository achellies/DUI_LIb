try
{
	var shell = new ActiveXObject("WScript.shell");
	var regPath = "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\9.0\\Setup\\VC\\ProductDir";
	var vcPath = shell.RegRead(regPath);
	var projPath = vcPath + "vcprojects\\";
	var wizPath = vcPath + "VCWizards\\";
	var fso = new ActiveXObject("Scripting.FileSystemObject");
	var wizFolder = "DuilibWizard\\";
	
	if (!fso.FolderExists(projPath + wizFolder))
		fso.CreateFolder(projPath + wizFolder); 
		
	if (!fso.FolderExists(wizPath + wizFolder))
		fso.CreateFolder(wizPath + wizFolder);
	
	fso.CopyFile("DuilibWizard.ico", projPath, true);
	fso.CopyFile("DuilibWizard.vsz", projPath, true);
	fso.CopyFile("DuilibWizard.vsdir", projPath + wizFolder, true);
	
	fso.CopyFolder("Scripts", wizPath + wizFolder, true);
	fso.CopyFolder("Templates", wizPath + wizFolder, true);
	
	WScript.Echo("Duilib Wizard Install Successfully !");
}
catch(e)
{
	throw(e);
}