#include <QDesktopWidget>
#include <QMessageBox>
#include <QVariant>
#include <QSettings>
#include <QInputDialog>
#include <QHeaderView>

#include <cmath>

#include "maingui.h"
#include "ui_maingui.h"
#include "chattab.h"
#include "config.h"
#include "contactlist.h"
#include "audioinfo.h"
#include "moviedelegate.h"

QPointer<Main> Main::m_instance;

Main::Main(QWidget* parent)
		: QMainWindow(parent), ui(new Ui::Main), m_tabs(0), m_contactmodel(0), m_terminal(0), m_statsVisible(false)
{
	m_audiodevices = new AudioDevices;
	m_settings_contactlist = new SettingsContactList;
}

QList<int> modifySizes(int diff, QList<int> sizes)
{
	if (sizes.size() > 0)
		sizes[0] -= diff;
	if (sizes.size() > 1)
		sizes[1] += diff;
	return sizes;
}

void Main::init()
{
	ui->setupUi(this);
	m_tabs = new Tabs(ui->tabs);
	QObject::connect(m_tabs.data(), &Tabs::tabTitleChanged, this, &Main::onTabTitleChanged);
	onTabTitleChanged(QApplication::applicationName());

	// settings
	QSettings settings;

	// window icon, size and position
	this->setWindowIcon(Config::instance()->icon("icon"));
	resize(settings.value("window/size", QSize(900, 400)).toSize());
	this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->size(),
	qApp->desktop()->availableGeometry()));

	// contacts
	ui->splitter->setStretchFactor(0, 0);
	ui->splitter->setStretchFactor(1, 100);
	log.debug("minimumSizeHint: %1", QString::number(ui->contactlist->sizeHint().width()));
	m_contactmodel = new ContactModel(this);
	ui->contactlist->setModel(m_contactmodel);
	ui->contactlist->setItemDelegate(new MovieDelegate(*ui->contactlist, 0));
	QObject::connect(ui->contactlist, &QTableView::clicked, this, &Main::onContactSelected);
	QObject::connect(this, &Main::shown, ContactList(), &List::Contacts::onResetContacts);
	ui->contactlist->verticalHeader()->setDefaultSectionSize(fontMetrics().lineSpacing() + 5);
	ui->contactlist->horizontalHeader()->setDefaultSectionSize(fontMetrics().lineSpacing() + 5);
	ui->contactlist->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui->contactlist->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

	QObject::connect(ui->actionAddContact, &QAction::triggered, this, &Main::onMenuAddContact);

	// terminal
	m_terminal = new Terminal;
	bool showTerminal = settings.value("window/show-terminal", true).toBool();
	if (showTerminal)
		m_tabs->addTab(m_terminal);
	QObject::connect(ui->actionShowTerminal, &QAction::triggered, this, &Main::onMenuShowTerminal);

	// stats
	ui->stats->hide();
	QObject::connect(ui->volumeInput, &QSlider::valueChanged, this, &Main::onSliderVolumeInput);
	QObject::connect(ui->volumeOutput, &QSlider::valueChanged, this, &Main::onSliderVolumeOutput);

	// audio devices settings
	QObject::connect(ui->actionAudioDevices, &QAction::triggered, this, &Main::onMenuAudioDevices);
	QObject::connect(ui->actionSettingsContactList, &QAction::triggered, this, &Main::onMenuSettingsContactList);

	// menu
	QObject::connect(ui->actionReloadContacts, &QAction::triggered, ContactList(), &List::Contacts::onResetContacts);
	QObject::connect(ui->actionAbout, &QAction::triggered, this, &Main::onMenuAbout);
	QObject::connect(ui->actionAboutQt, &QAction::triggered, this, &Main::onMenuAboutQt);

	// tabs
	m_tabs->onTabChanged(ui->tabs->currentIndex());

	//ui->groupBox->setFlat(true);
	//ui->groupBox->setAlignment(Qt::AlignHCenter);
}

Main* Main::instance()
{
	bool initializing = !m_instance;
	if (initializing) {
		static QMutex mutex;
		mutex.lock();
		m_instance = new Main;
		mutex.unlock();
	}
	if (initializing) {
		m_instance->init();
	}
	return m_instance;
}

Tabs* Main::tabs()
{
	return instance()->m_tabs;
}

Main::~Main()
{
	delete ui;
}

QString Main::id() const
{
	return "Main";
}

void Main::show()
{
	QMainWindow::show();
	emit shown();
}

void Main::resizeEvent(QResizeEvent* event)
{
	QMainWindow::resizeEvent(event);
	QSettings settings;
	settings.setValue("window/size", event->size());
}

void Main::onMenuAbout()
{
	QMessageBox::about(this, "About", "Version " + QCoreApplication::applicationVersion());
}

void Main::onMenuAboutQt()
{
	QMessageBox::aboutQt(this, "About Qt");
}

void Main::onContactSelected(const QModelIndex & index)
{
	Contact contact(m_contactmodel->getContact(index));
	m_tabs->addContactTab(contact);
	m_tabs->openContactTab(contact);
}

void Main::onTabTitleChanged(QString tabtitle)
{
	setWindowTitle(tabtitle + " - Build #" + Config::instance()->build());
}

void Main::onMenuShowTerminal()
{
	m_tabs->openTab(m_terminal);
}

void Main::onMenuAudioDevices()
{
	m_tabs->openTab(m_audiodevices);
}

void Main::onMenuSettingsContactList()
{
	m_tabs->openTab(m_settings_contactlist);
}

void Main::onShowStatsToggled(bool checked)
{
	QSettings settings;
	settings.setValue("window/show-stats", checked);
	ui->stats->setVisible(m_statsVisible && checked);
}

void Main::showStats()
{
	m_statsVisible = true;
	QSettings settings;
	bool showStats = settings.value("window/show-stats", true).toBool();
	ui->stats->setVisible(m_statsVisible && showStats);
}

void Main::hideStats()
{
	m_statsVisible = false;
	ui->stats->hide();
}

void Main::onStatsContact(Contact contact)
{
	ui->username->setText(contact.username());
	ui->computername->setText(contact.computername());
	ui->address->setText(contact.address().toString());
	ui->port->setText(QString::number(contact.port()));
}
void Main::onStatsDurationInput(int ms)
{
	QTime time(ms / 1000 / 60 / 60, (ms / 1000 / 60) % 60, (ms / 1000) % 60, ms % 1000);
//	time.addMSecs(ms);
	log.debug("time: %1, ms=%2", time.toString("HH:mm:ss"), ms);
	ui->statsDurationInput->setTime(time);
}
void Main::onStatsDurationOutput(int ms)
{
	QTime time(ms / 1000 / 60 / 60, (ms / 1000 / 60) % 60, (ms / 1000) % 60, ms % 1000);
//	time.addMSecs(ms);
	log.debug("time: %1, ms=%2", time.toString("HH:mm:ss"), ms);
	ui->statsDurationOutput->setTime(time);
}
void Main::onStatsLatencyInput(int ms)
{
	ui->statsLatencyInput->setText(QString::number(ms) + "ms");
}
void Main::onStatsLatencyOutput(int ms)
{
	ui->statsLatencyOutput->setText(QString::number(ms) + "ms");
}
qreal weighted(qreal level)
{
	level = (level - 0.015) / (1 - 0.015);
	if (level < 0)
		level = 0;
	level = std::pow(level, 0.6);

	static qreal loudestValueMeasured = 0.5;
	if (level > loudestValueMeasured) {
		loudestValueMeasured = qMax(level, 0.7);
		level = 1.0;
	} else {
		level /= loudestValueMeasured;
	}

	return level;
}
void Main::onStatsLevelInput(qreal _level)
{
	int level = (int) (weighted(_level) * 100);
	static int lastLevel = 0;
	if (AudioInfo::DO_DEBUG)
		log.debug("level (in): %1", level);
	level = level >= lastLevel ? level : (lastLevel + level) / 2;
	if (level != lastLevel) {
		ui->statsLevelInput->setValue(level);
		lastLevel = level;
	}
}
void Main::onStatsLevelOutput(qreal _level)
{
	int level = (int) (weighted(_level) * 100);
	static int lastLevel = 0;
	if (AudioInfo::DO_DEBUG)
		log.debug("level (out): %1", level);
	level = level >= lastLevel ? level : (lastLevel + level) / 2;
	if (level != lastLevel) {
		ui->statsLevelOutput->setValue(level);
		lastLevel = level;
	}
}
void Main::onStatsFormatInput(QAudioFormat format)
{
	ui->codecInput->setText(
			QString("PCM %1 kHz, %2 bit, %3").arg(QString::number(format.sampleRate() / 1000.0),
					QString::number(format.sampleSize()), format.channelCount() == 1 ? "mono" : "stereo"));
}
void Main::onStatsFormatOutput(QAudioFormat format)
{
	ui->codecOutput->setText(
			QString("PCM %1 kHz, %2 bit, %3").arg(QString::number(format.sampleRate() / 1000.0),
					QString::number(format.sampleSize()), format.channelCount() == 1 ? "mono" : "stereo"));
}
void Main::onSliderVolumeInput(int value)
{
	emit volumeChangedInput((qreal) value / 100);
}

void Main::onSliderVolumeOutput(int value)
{
	emit volumeChangedOutput((qreal) value / 100);
}

void Main::onMenuAddContact()
{
	bool ok;
	QString text = QInputDialog::getText(this, tr("Add Contact"), tr("Host name or IP address:"), QLineEdit::Normal, "",
			&ok);
	if (ok && !text.isEmpty()) {
		QString hostname;
		quint16 port = Config::instance()->DEFAULT_PORT;
		QStringList splitted = text.split(':');
		if (splitted.size() == 2) {
			hostname = splitted[0];
			bool ok;
			port = splitted[1].toUInt(&ok);
			if (!ok) {
				port = Config::instance()->DEFAULT_PORT;
			}
		} else if (splitted.size() == 1) {
			hostname = splitted[0];
		}
		if (hostname.size() > 0) {
			Host host(hostname, port);
			Config::instance()->addHost(host, Config::KNOWN_HOST);
		}
	}
}

Settings* Main::settingsContactList()
{
	return m_settings_contactlist->settings();
}
