/*
* Quantum, A free and open-source download manager that is written in C++
* Copyright (C) 2025  Yovsky <Yovsky@proton.me>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "src/backend/backend.h"
#include "src/core/databasemanager.h"
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QQmlContext>
#include <QLockFile>

int main(int argc, char *argv[])
{
    QQuickStyle::setStyle("Fusion");

    QApplication a(argc, argv);

    const QString lockFilePath = QDir::temp().absoluteFilePath("com.guenane.quantum.lock");
    QLockFile lockFile(lockFilePath);

    if (!lockFile.tryLock())
    {
        QMessageBox::critical(nullptr, "Quantum already running", "Another instance of quantum is already running.\nRecheck and try again.");
        return 0;
    }

    a.setWindowIcon(QIcon(":/qml/assets/icons/icon.ico"));

    Backend backend;

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("backend", &backend);

    engine.load(QUrl("qrc:/qml/Main.qml"));

    qRegisterMetaType<downloadInformations>("DownloadStatus");

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "Quantum_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    return a.exec();
}
