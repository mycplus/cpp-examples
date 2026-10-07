// qt_widgets.cpp - a small Qt 6 window with a signal connected to a lambda.
// The test runs it with QT_QPA_PLATFORM=offscreen, clicks the button from
// code and saves what the window looks like to a PNG.
#include <QApplication>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#include <cstdio>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    QWidget window;
    window.setWindowTitle("Greeter");
    auto* name = new QLineEdit("Ada");
    auto* button = new QPushButton("Greet");
    auto* label = new QLabel("Press the button");
    auto* layout = new QVBoxLayout(&window);   // the layout owns the widgets
    layout->addWidget(name);
    layout->addWidget(button);
    layout->addWidget(label);

    QObject::connect(button, &QPushButton::clicked, label, [name, label] {
        label->setText("Hello, " + name->text() + "!");
    });

    window.resize(240, 120);
    window.show();
    button->click();                            // emits clicked()
    std::printf("label after click: %s\n", label->text().toUtf8().constData());

    if (argc > 1) {                             // e.g. qt_widgets window.png
        const bool saved = window.grab().save(QString::fromLocal8Bit(argv[1]));
        std::printf("saved %s: %s\n", argv[1], saved ? "yes" : "no");
    }
    return 0;                                   // app.exec() would start the event loop
}
