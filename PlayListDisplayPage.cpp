// Logic for the playlist-view page
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "Albums.hpp"
#include "Albums-odb.hxx"

#include "Artists.hpp"
#include "Artists-odb.hxx"

#include "Track_Playlist.hpp"
#include "Track_Playlist-odb.hxx"

void MainWindow::LoadPlayListDisplayPage(const QModelIndex& index) {
    QSize icon_size = QSize(50, 50);
    QSize card_size = QSize(14, 70);


    db = *new database();
    db.setDatabase("userdata");
    odb::sqlite::database database_context = db.getDatabase();
    odb::transaction t(database_context.begin());

	auto model = new QStandardItemModel(this);
    // Remove edit capabilities
	ui->details_listView_ld->setModel(model);
    // Set the stylesheet
    ui->details_listView_ld->setStyleSheet("QStandardItem { background-color: #14273f; color: white; border: 1px solid #1c1c1c; }");

    // Get the content of the `index`
    QStringList playlistInfo = index.data().toString().split("\n");

    // Set the labels
    // Title of playlist
    ui->album_playlist_title_ld->setText(playlistInfo[0]);

    // Creator of playlist / Year of album
	ui->artist_ld->setText(playlistInfo[1]);

    // Blank this out. It's unused
	ui->published_ld->setText("");


	// Is this an album?
    Albums* track_album = database_context.query_one<Albums>(odb::query<Albums>::title == playlistInfo[0].toStdString() && odb::query<Albums>::release_date == playlistInfo[1].toStdString());

	// The default playlist
	if (track_album == NULL) {
        // Deal with the playlist
		return;
	}

	// Setup our connections for album tracks
    connect(ui->details_listView_ld, &QListView::doubleClicked, [=](const QModelIndex& index) {
        PlayTrack(index);
     });

    // We assume after this point, we're dealing with an album
	// Get the tracks associated with the album
    odb::result<Track> tracks = database_context.query<Track>(odb::query<Track>::album_id == track_album->Id());

    // Add the albums to the list
    for (odb::result<Track>::iterator trackIt = tracks.begin(); trackIt != tracks.end(); trackIt++) {
        Track track = *(trackIt);
		Artists track_artist = *(track.ArtistId());

        TrackImage track_image = track.Image();
        // Check to see if it's 16 bytes long. If it is, it's an empty image (probably a bug)
        if (!track_image.Data() || track_image.Size() == 16) {
            image.load(":/otherfiles/assets/images/album.png"); // Replace with default image
        }
        else {
            image.loadFromData(QByteArray::fromRawData(track_image.Data(), track_image.Size() == 16 ? 0 : track_image.Size()), "JPG"); // Pretty much all of the images are JPGs
        }

        // Information about the track
        image = image.scaled(60, 60); // Downscale the image to 60x60
        image = image.convertToFormat(QImage::Format_Indexed8); // Convert the image to an indexed 8-bit image
        pixmap = QPixmap::fromImage(image).scaled(icon_size); // Standardize the icon size across all the tracks
        image = *(new QImage);

		
        // Check `album_Image_ld` for a pixmap otherwise set it to the image
        if (ui->album_Image_ld->pixmap().isNull() || (ui->album_Image_ld->pixmap().data_ptr() != pixmap.data_ptr())) {
            ui->album_Image_ld->setPixmap(pixmap);
        }

        // Create a QStandardItem for the track
        trackView = new QStandardItem(QIcon(pixmap), QString::fromLatin1((track.Title().empty() ? track.FileName() : track.Title()) + "\n"));
        QString albumRow = QString::fromStdString(track_album->Title());
        QString artistRow = QString::fromStdString(track_artist.Name());

        // Append the album and artist below the title
        trackView->setText(trackView->text() + albumRow + "\n" + artistRow);

		// Remove edit capabilities
		trackView->setEditable(false);

        // Resize the trackView to the specified size
        trackView->setSizeHint(card_size);

        // Have the image fit the trackView
        ui->details_listView_ld->setIconSize(icon_size);
        model->appendRow(trackView);
    }

    // Commit the transaction
    t.commit();
    ui->mainStackedWidget->setCurrentIndex(0);
}