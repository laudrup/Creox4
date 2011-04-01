#include <QCheckBox>
#include <q3frame.h>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QLayout>
#include <QVariant>
#include <QToolTip>
#include <q3whatsthis.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>
#include <klocale.h>
#include "crnewpresetfolderdialog.h"

CrNewPresetFolderDialog::CrNewPresetFolderDialog(QWidget* parent,
												 const char* name,
												 bool modal, Qt::WFlags fl )
 : QDialog( parent, name, modal, fl )
{
	if(!name)
	{
		setName( "CrNewPresetFolderDialog" );
	}

	resize(248, 112);
	setSizePolicy(QSizePolicy(QSizePolicy::SizeType(3),
							  QSizePolicy::SizeType(3),
							  sizePolicy().hasHeightForWidth()));

	setCaption( i18n( "New Preset Folder" ) );
	setSizeGripEnabled( false );
	CrNewPresetFolderDialogLayout = new Q3VBoxLayout( this );
	CrNewPresetFolderDialogLayout->setSpacing( 3 );
	CrNewPresetFolderDialogLayout->setMargin( 3 );

	m_mainFrame = new Q3Frame( this, "m_mainFrame" );
	m_mainFrame->setFrameShape( Q3Frame::StyledPanel );
	m_mainFrame->setFrameShadow( Q3Frame::Raised );
	m_mainFrameLayout = new Q3VBoxLayout( m_mainFrame );
	m_mainFrameLayout->setSpacing( 3 );
	m_mainFrameLayout->setMargin( 6 );

	m_textLabel = new QLabel( m_mainFrame, "m_textLabel" );
	m_textLabel->setSizePolicy( QSizePolicy( QSizePolicy::SizeType(1),
											 QSizePolicy::SizeType(0),
											 m_textLabel->sizePolicy().
														hasHeightForWidth() ) );
	m_textLabel->setText( i18n( "<b>New Preset Folder Name:</b>" ) );
	m_mainFrameLayout->addWidget( m_textLabel );

	m_folderNameLineEdit = new QLineEdit( m_mainFrame, "m_folderNameLineEdit" );
	m_folderNameLineEdit->setMaximumSize( QSize( 32767, 32767 ) );
	m_folderNameLineEdit->setMouseTracking( false );
	m_folderNameLineEdit->setFocusPolicy( Qt::StrongFocus );
        // XXX!
	//m_folderNameLineEdit->setAutoMask( false );
	m_folderNameLineEdit->setText( i18n( "New Folder" ) );
	m_folderNameLineEdit->setCursorPosition( 0 );
	m_folderNameLineEdit->setEdited( false );
	m_mainFrameLayout->addWidget( m_folderNameLineEdit );

	m_rootFolderCheckBox = new QCheckBox( m_mainFrame, "m_rootFolderCheckBox" );
	m_rootFolderCheckBox->setText( i18n( "Root Folder" ) );
	m_mainFrameLayout->addWidget( m_rootFolderCheckBox );
	CrNewPresetFolderDialogLayout->addWidget( m_mainFrame );
	QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
	CrNewPresetFolderDialogLayout->addItem( spacer );

	m_buttonLayout = new Q3HBoxLayout;
	m_buttonLayout->setSpacing( 6 );
	m_buttonLayout->setMargin( 0 );
	QSpacerItem* spacer_2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
	m_buttonLayout->addItem( spacer_2 );

	m_okButton = new QPushButton( this, "m_okButton" );
	m_okButton->setText( i18n( "Create" ) );
	m_okButton->setDefault( true );
	m_buttonLayout->addWidget( m_okButton );

	m_cancelButton = new QPushButton( this, "m_cancelButton" );
	m_cancelButton->setText( i18n( "Cancel" ) );
	m_buttonLayout->addWidget( m_cancelButton );
	QSpacerItem* spacer_3 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
	m_buttonLayout->addItem( spacer_3 );
	CrNewPresetFolderDialogLayout->addLayout( m_buttonLayout );

	// signals and slots connections
	connect( m_okButton, SIGNAL( released() ), this, SLOT( accept() ) );
	connect( m_cancelButton, SIGNAL( released() ), this, SLOT( reject() ) );
	connect( m_folderNameLineEdit, SIGNAL( returnPressed() ), this, SLOT( accept() ) );

	// tab order
	setTabOrder( m_folderNameLineEdit, m_rootFolderCheckBox );
	setTabOrder( m_rootFolderCheckBox, m_okButton );
	setTabOrder( m_okButton, m_cancelButton );
}

/*
 *  Destroys the object and frees any allocated resources
 */
CrNewPresetFolderDialog::~CrNewPresetFolderDialog()
{
	// no need to delete child widgets, Qt does it all for us
}
