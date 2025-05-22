from PyQt5.QtWidgets import QApplication, QFileDialog

def open_file_dialog():
    app = QApplication([])
    dialog = QFileDialog()
    dialog.setNameFilter("PreProcessing Modules (*.dylib)")
    if dialog.exec_():
        selected_files = dialog.selectedFiles()
        print("Selected files:", selected_files)

if __name__ == "__main__":
    open_file_dialog()
