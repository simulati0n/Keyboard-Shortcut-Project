from PyQt5.QtWidgets import QApplication, QMainWindow, QVBoxLayout, QPushButton, QLineEdit, QLabel, QWidget, QListWidget

class ShortcutApp(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Shortcut Manager")
        self.setGeometry(100, 100, 400, 300)

        self.layout = QVBoxLayout()

        self.shortcut_input = QLineEdit(self)
        self.shortcut_input.setPlaceholderText("Enter Shortcut (e.g., Ctrl+Alt+Y)")
        self.layout.addWidget(self.shortcut_input)

        self.action_input = QLineEdit(self)
        self.action_input.setPlaceholderText("Enter Action (e.g., open browser)")
        self.layout.addWidget(self.action_input)

        self.add_button = QPushButton("Add Shortcut", self)
        self.add_button.clicked.connect(self.add_shortcut)
        self.layout.addWidget(self.add_button)

        self.shortcuts_list = QListWidget(self)
        self.layout.addWidget(self.shortcuts_list)

        container = QWidget()
        container.setLayout(self.layout)
        self.setCentralWidget(container)

    def add_shortcut(self):
        shortcut = self.shortcut_input.text()
        action = self.action_input.text()
        if shortcut and action:
            self.shortcuts_list.addItem(f"{shortcut} -> {action}")
            self.shortcut_input.clear()
            self.action_input.clear()

if __name__ == "__main__":
    app = QApplication([])
    window = ShortcutApp()
    window.show()
    app.exec()
