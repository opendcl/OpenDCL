// ResetNoImpersonate.js <msi-file>
// Unsets the "NoImpersonate" bit for deferred custom actions

// Constant values from Windows Installer
var msiOpenDatabaseModeTransact = 1;

var msiViewModifyInsert         = 1;
var msiViewModifyUpdate         = 2;
var msiViewModifyAssign         = 3;
var msiViewModifyReplace        = 4;
var msiViewModifyDelete         = 6;



if (WScript.Arguments.Length != 1)
{
	WScript.StdErr.WriteLine(WScript.ScriptName + " expected 1 argument");
	WScript.Quit(1);
}

var filespec = WScript.Arguments(0);
var installer = WScript.CreateObject("WindowsInstaller.Installer");
var database = installer.OpenDatabase(filespec, msiOpenDatabaseModeTransact);

var sql
var view
var record
var msidbCustomActionTypeInScript = 1024
var msidbCustomActionTypeNoImpersonate = 2048

try
{
	WScript.Echo("Unsetting 'NoImpersonate' flag for deferred custom actions in\r\n  '" + filespec + "'...");
	sql = "SELECT Action, Type FROM `CustomAction`";
	view = database.OpenView(sql);
	view.Execute();
	record = view.Fetch();
	while (record)
	{
		if (record.IntegerData(2) &  msidbCustomActionTypeInScript)
		{
			record.IntegerData(2) &= ~msidbCustomActionTypeNoImpersonate;
			view.Modify(msiViewModifyUpdate, record);
		}
		record = view.Fetch();
	}
	view.Close();
	database.Commit();
}
catch(e)
{
	WScript.StdErr.WriteLine(e);
	WScript.Quit(1);
}
