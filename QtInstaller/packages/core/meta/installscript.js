/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the FOO module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

var Dir = new function () {
    this.toNativeSparator = function (path) {
        if (systemInfo.kernelType === "winnt")
            return path.replace(/\//g, '\\');
        return path;
    }
};

function Component() {
    // constructor
    if (installer.isInstaller()) {
        component.loaded.connect(this, Component.prototype.loaded);
    }
}

Component.prototype.loaded = function () {
    if (installer.addWizardPage(component, "CustomIntroductionPage", QInstaller.TargetDirectory)) {
        var page = gui.pageByObjectName("DynamicCustomIntroductionPage");
        if (page != null) {
            page.entered.connect(Component.prototype.dynamicCustromIntroductionPageEntered);
        }
    }
    console.log("component loaded");
    if (systemInfo.kernelType === "winnt") {
        console.log("System is windows")
        if (installer.addWizardPage(component, "ShortcutWidget", QInstaller.StartMenuSelection)) {
            var widget = gui.pageWidgetByObjectName("DynamicShortcutWidget");
            if (widget != null) {
                widget.createDesktopShortcut.checked = true;
                widget.createStartMenuShortcut.checked = true;

                widget.windowTitle = "Create shortcuts";
            }
        }
    } else if (systemInfo.kernelType == "linux") {
        console.log("System is linux");
        if (installer.addWizardPage(component, "SymlinkWidget", QInstaller.StartMenuSelection)) {
            var widget = gui.pageWidgetByObjectName("DynamicSymlinkWidget");
            if (widget != null) {
                widget.createDesktopShortcut.checked = true;
                widget.createUserLink.checked = true;
                widget.createSystemLink.checked = false;

                widget.windowTitle = "Create shortcuts";
            }
        }
    }
}

Component.prototype.dynamicCustromIntroductionPageEntered = function ()
{
    var pageWidget = gui.pageWidgetByObjectName("DynamicCustomIntroductionPage");
    if (pageWidget != null) {
        pageWidget.m_pageLabel.text = "Welcome to the MuhRec setup.";
        installer.setDefaultPageVisible(QInstaller.Introduction, false);
    }
}

Component.prototype.createOperations = function()
{
    try {
        // call the base create operations function
        component.createOperations();
    } catch (e) {
        console.log(e);
    }
	
    if (systemInfo.kernelType === "winnt") {
        var shortcutpage = component.userInterface("ShortcutWidget");
        if (shortcutpage && shortcutpage.createDesktopShortcut.checked) {
            component.addElevatedOperation("CreateShortcut", "@TargetDir@/MuhRec.exe", "@DesktopDir@/MuhRec.lnk");
        }
        if (shortcutpage && shortcutpage.createStartMenuShortcut.checked) {
            component.addElevatedOperation("CreateShortcut", "@TargetDir@/MuhRec.exe", "@UserStartMenuProgramsPath@/@StartMenuDir@/MuhRec.lnk",
                "workingDirectory=@TargetDir@");
        }   
    } else if (systemInfo.kernelType === "linux") {
        var shortcutpage = component.userInterface("SymlinkWidget");
        if (shortcutpage && shortcutpage.createDesktopShortcut.checked) {
            component.addOperation("CreateDesktopEntry", "MuhRec", 
                "Type=Application\n
                 Name=@HomeDir@/.local/share/applications/MuhRec.desktop\n
                 Comment=Neutron Tomography Software\n
                 Path=/opt/bin\n
                 Exec=MuhRec\n
                 Icon=@TargetDir@/resources/muh4_icon.svg\n
                 Categories=Science"
                     );
            component.addOperation("CreateDesktopEntry", "ImageViewer", 
                "Type=Application\n
                 Name=@HomeDir@/.local/share/applications/ImageViewer.desktop\n
                 Comment=Neutron Tomography Image Viewer\n
                 Path=/opt/bin\n
                 Exec=ImageViewer\n
                 Icon=@TargetDir@/resources/viewer_icon.svg\n
                 Categories=Science"
                     );

        }
        if (shortcutpage && shortcutpage.createUserLink.checked) {
            console.log("Creating Symlinks in @HomeDir@/.local/bin");
            component.addOperation("CreateLink", "@HomeDir@/.local/bin/muhrec", "@TargetDir@/bin/MuhRec");
            component.addOperation("CreateLink", "@HomeDir@/.local/bin/imageviewer", "@TargetDir@/bin/ImageViewer");
        }
        if (shortcutpage && shortcutpage.createSystemLink.checked) {
            console.log("Creating Symlinks in @RootDir@usr/local/bin");
            component.addElevatedOperation("CreateLink", "@RootDir@usr/local/bin/muhrec", "@TargetDir@/bin/MuhRec");
            component.addElevatedOperation("CreateLink", "@RootDir@usr/local/bin/imageviewer", "@TargetDir@/bin/ImageViewer");
        }
    }
}
