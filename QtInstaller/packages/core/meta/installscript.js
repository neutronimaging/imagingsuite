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
        if (systemInfo.productType === "windows")
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

    if (systemInfo.productType === "windows") {
        if (installer.addWizardPage(component, "ShortcutWidget", QInstaller.StartMenuSelection)) {
            var widget = gui.pageWidgetByObjectName("DynamicShortcutWidget");
            if (widget != null) {
                widget.createDesktopShortcut.checked = true;
                widget.createStartMenuShortcut.checked = true;

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
	
    if (systemInfo.productType === "windows") {
        var shortcutpage = component.userInterface("ShortcutWidget");
        if (shortcutpage && shortcutpage.createDesktopShortcut.checked) {
            component.addElevatedOperation("CreateShortcut", "@TargetDir@/MuhRec.exe", "@DesktopDir@/MuhRec.lnk");
        }
        if (shortcutpage && shortcutpage.createStartMenuShortcut.checked) {
            component.addElevatedOperation("CreateShortcut", "@TargetDir@/MuhRec.exe", "@StartMenuDir@/MuhRec.lnk",
                "workingDirectory=@TargetDir@");
        }   
    }
}
