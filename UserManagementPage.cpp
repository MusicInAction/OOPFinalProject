// Logic for the all-albums page
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "WindowsAccount.hpp"
#include "WindowsAccount-odb.hxx"

#include "Roles.hpp"
#include "Roles-odb.hxx"


// Go to the User Management Page
void MainWindow::LoadUserManagementPage() {
	// Populate the `userListView` with the users
	// And label them with their roles
	db = *new database();
	db.setDatabase("userdata");
	odb::sqlite::database database_context = db.getDatabase();
	odb::transaction t(database_context.begin());

	auto model = new QStandardItemModel(this);
	ui->userListView->setModel(model);
	// Set the context menu to appear when the user right-clicks an item
	ui->userListView->setContextMenuPolicy(Qt::CustomContextMenu);
	// Open a context menu when the user right-clicks an item
	connect(ui->userListView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(ShowUserContextMenu(QPoint)));


	// Query for the users
	odb::result<Windows_Account> users = database_context.query<Windows_Account>();

	// Add the users to the list
	for (odb::result<Windows_Account>::iterator it = users.begin(); it != users.end(); it++) {
		Windows_Account user = *(it);
		Roles user_role = *(user.AccessLevel());

		// Get the role of the user
		odb::result<Roles> roles = database_context.query<Roles>(odb::query<Roles>::id == user_role.Id());
		Roles role = *(roles.begin());

		// Add the user to the list
		QStandardItem* item = new QStandardItem(QString::fromStdString(user.Name() + "\n" + role.Name()));
		item->setToolTip(role.Name().c_str());
		model->appendRow(item);
	}

	// Set the current index to the user management page
	ui->mainStackedWidget->setCurrentWidget(ui->editUsersPage);
}

// Show the context menu for the user list
void MainWindow::ShowUserContextMenu(QPoint pos) {
	QPoint item = ui->userListView->mapToGlobal(pos);
	QMenu* userContextMenu = new QMenu(ui->userListView);
	QAction* deleteUser = userContextMenu->addAction("Delete this user");
	QAction* banUser = userContextMenu->addAction("Ban this user");
	userContextMenu->addAction(deleteUser);
	userContextMenu->addAction(banUser);
	QAction* rightClickItem = userContextMenu->exec(item);

	// Delete the user when the user clicks the delete button
	if (rightClickItem == deleteUser) {
		QModelIndex index = ui->userListView->currentIndex();
		QStandardItemModel* model = (QStandardItemModel*)ui->userListView->model();
		QStandardItem* item = model->itemFromIndex(index);
		std::string user_name = item->text().toStdString();
		std::string user_name_only = user_name.substr(0, user_name.find("\n"));

		// Delete the user from the database
		odb::sqlite::database database_context = db.getDatabase();
		odb::transaction t(database_context.begin());

		// Query for the user
		Windows_Account* user_to_delete = database_context.query_one<Windows_Account>(odb::query<Windows_Account>::username == user_name_only);

		try {
			// Delete cascade
			database_context.erase(user_to_delete);
		}
		catch (odb::exception& e) {
			qDebug() << e.what();
		}

		t.commit(); // Save the changes
		LoadUserManagementPage(); // Reload the page
	}

	// Ban the user when the user clicks the ban button
	if (rightClickItem == banUser) {
		QModelIndex index = ui->userListView->currentIndex();
		QStandardItemModel* model = (QStandardItemModel*)ui->userListView->model();
		QStandardItem* item = model->itemFromIndex(index);
		std::string user_name = item->text().toStdString();
		std::string user_name_only = user_name.substr(0, user_name.find("\n"));

		// Ban the user
		odb::sqlite::database database_context = db.getDatabase();
		odb::transaction t(database_context.begin());

		// Query for the user
		Windows_Account* user_to_ban = database_context.query_one<Windows_Account>(odb::query<Windows_Account>::username == user_name_only);
		// Query for the role
		Roles* banned_role = database_context.query_one<Roles>(odb::query<Roles>::name == "Banned");
		user_to_ban->SetAccessLevel(banned_role); // Set the user's access level to banned
		
		try {
			database_context.update(user_to_ban);
		}
		catch (odb::exception& e) {
			qDebug() << e.what();
		}
		
		t.commit(); // Save the changes
		LoadUserManagementPage(); // Reload the page
	}
}
