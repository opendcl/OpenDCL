// MSIREPFixup.js <msi-file>
// Performs a post-build fixup of an msi to resequence the RemoveExistingProducts action


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
var newSequence = 1450;

try
{
	WScript.Echo("Resequencing RemoveExistingProducts action in\r\n  '" + filespec + "'...");
	// Modify the Sequence property of the RemoveExistingProducts action
	sql = "SELECT Action, Sequence FROM `InstallExecuteSequence` WHERE Action = 'InstallInitialize'";
	view = database.OpenView(sql);
	view.Execute();
	record = view.Fetch();
	if (record)
		newSequence = record.IntegerData(2) - 1;
	view.Close();
	sql = "SELECT Action, Sequence FROM `InstallExecuteSequence` WHERE Action = 'RemoveExistingProducts'";
	view = database.OpenView(sql);
	view.Execute();
	record = view.Fetch();
	record.IntegerData(2) = newSequence;
	view.Modify(msiViewModifyUpdate, record);
	view.Close();
	database.Commit();
}
catch(e)
{
	WScript.StdErr.WriteLine(e);
	WScript.Quit(1);
}
