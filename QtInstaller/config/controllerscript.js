/// This is the script which handles the parts of the installer not related to any of the packages.


function Controller()
{
    if (systemInfo.productType === "windows") {
        installer.setDefaultPageVisible(QInstaller.StartMenuSelection, false);
    }
    
}

Controller.prototype.IntroductionPageCallback = function()
{
    gui.clickButton(buttons.NextButton);
}

