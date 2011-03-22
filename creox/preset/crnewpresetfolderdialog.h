#ifndef CRNEWPRESETFOLDERDIALOG_H
#define CRNEWPRESETFOLDERDIALOG_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QCheckBox;
class QFrame;
class QLabel;
class QLineEdit;
class QPushButton;

class CrNewPresetFolderDialog : public QDialog
{
	Q_OBJECT

public:
	CrNewPresetFolderDialog(QWidget* parent = 0, const char* name = 0,
							bool modal = FALSE, WFlags fl = 0 );
	~CrNewPresetFolderDialog();

	QFrame* m_mainFrame;
	QLabel* m_textLabel;
	QLineEdit* m_folderNameLineEdit;
	QCheckBox* m_rootFolderCheckBox;
	QPushButton* m_okButton;
	QPushButton* m_cancelButton;

protected:
	QVBoxLayout* CrNewPresetFolderDialogLayout;
	QVBoxLayout* m_mainFrameLayout;
	QHBoxLayout* m_buttonLayout;
};

#endif // CRNEWPRESETFOLDERDIALOG_H
