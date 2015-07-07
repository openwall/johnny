function Component()
{
    Component.prototype.createOperations = function()
    {
        component.createOperations();
        if (systemInfo.productType === "windows") {
            component.addOperation("CreateShortcut", "@TargetDir@/johnny.exe", "@StartMenuDir@/johnny.lnk",
                                   "workingDirectory=@TargetDir@", "iconPath=@TargetDir@/platforms/johnny.ico");
            component.addOperation("CreateShortcut", "@TargetDir@/johnny.exe", "@DesktopDir@/johnny.lnk",
                                   "workingDirectory=@TargetDir@", "iconPath=@TargetDir@/platforms/johnny.ico");
        }
    }
}
