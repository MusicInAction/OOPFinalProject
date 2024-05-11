#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "database.hpp"

#include "Track.hpp"
#include "Track_Playcount.hpp"
#include "Albums.hpp"
#include "Artists.hpp"
#include "WindowsAccount.hpp"

#include "Track-odb.hxx"
#include "Track_Playcount-odb.hxx"
#include "Albums-odb.hxx"
#include "Artists-odb.hxx"
#include "WindowsAccount-odb.hxx"

#include <qDebug>
#include <QMessageBox>

// So this is the report page. It is a simple page that displays the report of the
// The recently played tracks, albums, and artists. It also displays the total number of
// tracks, albums, and artists played. It also displays the total time spent listening to music
// Which is just all the song durations added up. It also displays the average time spent listening
// To music, which is the total time spent listening to music divided by the total number of tracks played.

void MainWindow::LoadTrackManagementPage() {
    ui->mainStackedWidget->setCurrentWidget(ui->formsPage);

    // Query the database for the total tracks played by the current WindowsAccount
    odb::sqlite::database database_context = db.getDatabase();
    odb::transaction t(database_context.begin());

    auto model = new QStandardItemModel(this);
    ui->reportListView->setModel(model);

    std::string currentTabText = ui->managementTab_fp->tabText(ui->managementTab_fp->currentIndex()).toStdString();

    if (currentTabText == "Genres") {

    }
    else if (currentTabText == "Albums") {

    }
    else if (currentTabText == "Tracks") {

    }
    else {
        return;
    }

    t.commit(); // Don't need the database anymore beyond this point
}