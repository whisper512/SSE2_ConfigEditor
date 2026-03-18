#include "SSE2_ConfigEditorMainWnd.h"

#include <QTimer>
#include <QFileDialog>
#include <QMessageBox>

SSE2_ConfigEditorMainWnd::SSE2_ConfigEditorMainWnd(QWidget* parent)
    : QMainWindow(parent),
    m_pActionOpenGamePath(nullptr),
    m_pActionWriteConfig(nullptr),
    m_pActionSaveBackup(nullptr),
    m_strGamePath(""),
    m_eFaction(Faction_TL),
    m_iTitanNum(1),
    m_iSuperCapitalshipNum(1),
    m_istarStarbase(4),
    m_iplanetStarbase(1)

{
    ui.setupUi(this);

    InitApplication();
}

SSE2_ConfigEditorMainWnd::~SSE2_ConfigEditorMainWnd()
{
}

void SSE2_ConfigEditorMainWnd::InitApplication()
{
    QTimer::singleShot(0, this, [this]()
        {
            InitMainWnd();
            IntiEditor();
            InitMenu();
            ConnectSlots();
        });
}

void SSE2_ConfigEditorMainWnd::InitMainWnd()
{
    this->setWindowIcon(QIcon(":/SSE2_ConfigEditorMainWnd/res/blackcat.png"));
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowMaximizeButtonHint);
    this->show();
}

void SSE2_ConfigEditorMainWnd::InitMenu()
{
    m_pActionOpenGamePath = new QAction(tr("打开游戏路径"), this);
    m_pActionOpenGamePath->setShortcut(QKeySequence::Open);
    connect(m_pActionOpenGamePath, &QAction::triggered, this, &SSE2_ConfigEditorMainWnd::OnOpenGamePath);
    ui.menu_File->addAction(m_pActionOpenGamePath);

    m_pActionWriteConfig = new QAction(tr("写入配置"), this);
    m_pActionWriteConfig->setShortcut(QKeySequence::Save);
    connect(m_pActionWriteConfig, &QAction::triggered, this, &SSE2_ConfigEditorMainWnd::OnEditConfig);
    ui.menu_File->addAction(m_pActionWriteConfig);

    m_pActionSaveBackup = new QAction(tr("保存备份"), this);
    m_pActionSaveBackup->setShortcut(QKeySequence::SaveAs);
    connect(m_pActionSaveBackup, &QAction::triggered, this, &SSE2_ConfigEditorMainWnd::OnSaveBackup);
    ui.menu_File->addAction(m_pActionSaveBackup);
}

void SSE2_ConfigEditorMainWnd::ConnectSlots()
{
    connect(ui.comboBox_faction, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SSE2_ConfigEditorMainWnd::OnFactionChanged);

    connect(ui.lineEdit_Titan, &QLineEdit::editingFinished, this, &SSE2_ConfigEditorMainWnd::OnEditFinished);
    connect(ui.lineEdit_SuperCapitalship, &QLineEdit::editingFinished, this, &SSE2_ConfigEditorMainWnd::OnEditFinished);
    connect(ui.lineEdit_starStarbase, &QLineEdit::editingFinished, this, &SSE2_ConfigEditorMainWnd::OnEditFinished);
    connect(ui.lineEdit_planetStarbase, &QLineEdit::editingFinished, this, &SSE2_ConfigEditorMainWnd::OnEditFinished);
}

void SSE2_ConfigEditorMainWnd::ReadConfig()
{
    if (m_strGamePath.isEmpty())
    {
        return;
    }

    QString strConfigFile;
    switch (m_eFaction)
    {
    case Faction_TL:
        strConfigFile = QString("%1/entities/%2.player").arg(m_strGamePath).arg(TL_PlayerConfig);
        break;
    case Faction_TR:
        strConfigFile = QString("%1/entities/%2.player").arg(m_strGamePath).arg(TR_PlayerConfig);
        break;
    case Faction_VL:
        strConfigFile = QString("%1/entities/%2.player").arg(m_strGamePath).arg(VL_PlayerConfig);
        break;
    case Faction_VR:
        strConfigFile = QString("%1/entities/%2.player").arg(m_strGamePath).arg(VR_PlayerConfig);
        break;
    case Faction_AL:
        strConfigFile = QString("%1/entities/%2.player").arg(m_strGamePath).arg(AL_PlayerConfig);
        break;
    case Faction_AR:
        strConfigFile = QString("%1/entities/%2.player").arg(m_strGamePath).arg(AR_PlayerConfig);
        break;
    default:
        return;
    }

    QFile file(strConfigFile);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "无法打开配置文件:" << strConfigFile;
        return;
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    if (jsonDoc.isNull() || !jsonDoc.isObject())
    {
        qDebug() << "JSON 文件格式错误:" << strConfigFile;
        return;
    }

    QJsonObject jsonObj = jsonDoc.object();
    QJsonObject unitLimitsObj = jsonObj.value("unit_limits").toObject();

    // 读取 global 数组中的数据
    QJsonArray globalArray = unitLimitsObj.value("global").toArray();
    for (const QJsonValue& value : globalArray)
    {
        QJsonObject item = value.toObject();
        QString tag = item.value("tag").toString();
        int unitLimit = item.value("unit_limit").toInt();

        if (tag == "titan")
        {
            m_iTitanNum = unitLimit;
        }
        else if (tag == "super_capital_ship")
        {
            m_iSuperCapitalshipNum = unitLimit;
        }
    }

    // 读取 star 数组中的数据
    QJsonArray starArray = unitLimitsObj.value("star").toArray();
    for (const QJsonValue& value : starArray)
    {
        QJsonObject item = value.toObject();
        QString tag = item.value("tag").toString();
        int unitLimit = item.value("unit_limit").toInt();

        if (tag == "starbase")
        {
            m_istarStarbase = unitLimit;
        }
    }

    // 读取 planet 数组中的数据
    QJsonArray planetArray = unitLimitsObj.value("planet").toArray();
    for (const QJsonValue& value : planetArray)
    {
        QJsonObject item = value.toObject();
        QString tag = item.value("tag").toString();
        int unitLimit = item.value("unit_limit").toInt();

        if (tag == "starbase")
        {
            m_iplanetStarbase = unitLimit;
        }
    }


    UpdateData();
}

void SSE2_ConfigEditorMainWnd::OnFactionChanged(int index)
{
    m_eFaction = (Faction)index;
    ReadConfig();
    
}

void SSE2_ConfigEditorMainWnd::OnOpenGamePath()
{
    QString strFilePath = QFileDialog::getOpenFileName(
        this,
        tr("选择游戏可执行文件"),
        "",
        tr("可执行文件 (*.exe)")
    );

    if (!strFilePath.isEmpty())
    {
        QFileInfo fileInfo(strFilePath);
        m_strGamePath = fileInfo.absolutePath();
    }

    ReadConfig();
}

void SSE2_ConfigEditorMainWnd::OnEditConfig()
{
    if (m_strGamePath.isEmpty())
    {
        return;
    }

    QString strConfigFile;
    switch (m_eFaction)
    {
    case Faction_TL:
        strConfigFile = QString("%1/entities/%2.player").arg(m_strGamePath).arg(TL_PlayerConfig);
        break;
    case Faction_TR:
        strConfigFile = QString("%1/entities/%2.player").arg(m_strGamePath).arg(TR_PlayerConfig);
        break;
    case Faction_VL:
        strConfigFile = QString("%1/entities/%2.player").arg(m_strGamePath).arg(VL_PlayerConfig);
        break;
    case Faction_VR:
        strConfigFile = QString("%1/entities/%2.player").arg(m_strGamePath).arg(VR_PlayerConfig);
        break;
    case Faction_AL:
        strConfigFile = QString("%1/entities/%2.player").arg(m_strGamePath).arg(AL_PlayerConfig);
        break;
    case Faction_AR:
        strConfigFile = QString("%1/entities/%2.player").arg(m_strGamePath).arg(AR_PlayerConfig);
        break;
    default:
        return;
    }

    QFile file(strConfigFile);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "无法打开配置文件:" << strConfigFile;
        return;
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    if (jsonDoc.isNull() || !jsonDoc.isObject())
    {
        qDebug() << "JSON 文件格式错误:" << strConfigFile;
        return;
    }

    QJsonObject jsonObj = jsonDoc.object();
    QJsonObject unitLimitsObj = jsonObj.value("unit_limits").toObject();

    // 更新 global 数组中的数据
    QJsonArray globalArray = unitLimitsObj.value("global").toArray();
    for (int i = 0; i < globalArray.size(); ++i)
    {
        QJsonObject item = globalArray[i].toObject();
        QString tag = item.value("tag").toString();

        if (tag == "titan")
        {
            item["unit_limit"] = m_iTitanNum;
            globalArray[i] = item;
        }
        else if (tag == "super_capital_ship")
        {
            item["unit_limit"] = m_iSuperCapitalshipNum;
            globalArray[i] = item;
        }
    }
    unitLimitsObj["global"] = globalArray;

    // 更新 star 数组中的数据
    QJsonArray starArray = unitLimitsObj.value("star").toArray();
    for (int i = 0; i < starArray.size(); ++i)
    {
        QJsonObject item = starArray[i].toObject();
        QString tag = item.value("tag").toString();

        if (tag == "starbase")
        {
            item["unit_limit"] = m_istarStarbase;
            starArray[i] = item;
        }
    }
    unitLimitsObj["star"] = starArray;

    // 更新 planet 数组中的数据
    QJsonArray planetArray = unitLimitsObj.value("planet").toArray();
    for (int i = 0; i < planetArray.size(); ++i)
    {
        QJsonObject item = planetArray[i].toObject();
        QString tag = item.value("tag").toString();

        if (tag == "starbase")
        {
            item["unit_limit"] = m_iplanetStarbase;
            planetArray[i] = item;
        }
    }
    unitLimitsObj["planet"] = planetArray;

    // 更新 JSON 对象
    jsonObj["unit_limits"] = unitLimitsObj;

    // 写入文件
    jsonDoc.setObject(jsonObj);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        qDebug() << "无法写入配置文件:" << strConfigFile;
        return;
    }

    file.write(jsonDoc.toJson());
    file.close();

    QMessageBox::information(this, tr("提示"), tr("配置文件已成功修改！"));
}

void SSE2_ConfigEditorMainWnd::OnSaveBackup()
{
}

void SSE2_ConfigEditorMainWnd::UpdateData()
{
    ui.lineEdit_Titan->setText(QString::number(m_iTitanNum));
    ui.lineEdit_SuperCapitalship->setText(QString::number(m_iSuperCapitalshipNum));
    ui.lineEdit_starStarbase->setText(QString::number(m_istarStarbase));
    ui.lineEdit_planetStarbase->setText(QString::number(m_iplanetStarbase));

}


void SSE2_ConfigEditorMainWnd::IntiEditor()
{
    ui.lineEdit_Titan->setValidator(new QIntValidator(0, 50, this));
    ui.lineEdit_SuperCapitalship->setValidator(new QIntValidator(0, 50, this));
    ui.lineEdit_starStarbase->setValidator(new QIntValidator(0, 50, this));
    ui.lineEdit_planetStarbase->setValidator(new QIntValidator(0, 50, this));

    ui.comboBox_faction->addItem(tr("贸易联盟忠诚派"));
    ui.comboBox_faction->addItem(tr("贸易联盟反叛派"));
    ui.comboBox_faction->addItem(tr("瓦萨里忠诚派"));
    ui.comboBox_faction->addItem(tr("瓦萨里反叛派"));
    ui.comboBox_faction->addItem(tr("圣临忠诚派"));
    ui.comboBox_faction->addItem(tr("圣临反叛派"));
}

void SSE2_ConfigEditorMainWnd::OnEditFinished()
{
    m_iTitanNum = ui.lineEdit_Titan->text().toInt();
    m_iSuperCapitalshipNum = ui.lineEdit_SuperCapitalship->text().toInt();
    m_istarStarbase = ui.lineEdit_starStarbase->text().toInt();
    m_iplanetStarbase = ui.lineEdit_planetStarbase->text().toInt();
}