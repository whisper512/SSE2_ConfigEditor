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
    m_iplanetStarbase(1),
    m_iDefaultCredits(1000),
    m_iDefaultMetal(400),
    m_iDefaultCrystal(250)

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


    ui.label_Maxsupply6->setVisible(false);
    ui.lineEdit_Maxsupply6->setVisible(false);
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
    
    ParseUnitLimitConfigFromJson(jsonDoc);
    ParseMaxSupplyConfigFromJson(jsonDoc);
    ParseDefaultStartingAssetsFromJson(jsonDoc);

}

void SSE2_ConfigEditorMainWnd::ParseUnitLimitConfigFromJson(const QJsonDocument& jsonDoc)
{
    if (jsonDoc.isNull() || !jsonDoc.isObject())
    {
        qDebug() << "JSON 文件格式错误";
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

    UpdateUnitsLimitData();
}

void SSE2_ConfigEditorMainWnd::ParseMaxSupplyConfigFromJson(const QJsonDocument& jsonDoc)
{
    if (jsonDoc.isNull() || !jsonDoc.isObject())
    {
        qDebug() << "JSON 文件格式错误";
        return;
    }

    QJsonObject jsonObj = jsonDoc.object();
    QJsonObject maxSupplyObj = jsonObj.value("max_supply").toObject();
    QJsonArray levelsArray = maxSupplyObj.value("levels").toArray();

    m_listMaxSupply.clear();
    for (const QJsonValue& value : levelsArray)
    {
        QJsonObject item = value.toObject();
        int maxSupply = item.value("max_supply").toInt();
        m_listMaxSupply.append(maxSupply);
    }
    UpdateMaxSupplyData();

}


void SSE2_ConfigEditorMainWnd::OnFactionChanged(int index)
{
    m_eFaction = (Faction)index;
    ReadConfig();
    
    if (m_eFaction == Faction_VL || m_eFaction == Faction_VR)
    {
        ui.label_Maxsupply6->setVisible(true);
        ui.lineEdit_Maxsupply6->setVisible(true);

        ui.label_default_starting_credit->setVisible(false);
        ui.lineEdit_default_starting_credit->setVisible(false);
    }
    else
    {
        ui.label_Maxsupply6->setVisible(false);
        ui.lineEdit_Maxsupply6->setVisible(false);
        ui.label_default_starting_credit->setVisible(true);
        ui.lineEdit_default_starting_credit->setVisible(true);
    }
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

    // 写入单位限制配置
    WriteUnitLimitConfigToJson();
    // 写入最大供应量配置
    WriteMaxSupplyConfigToJson();

    WriteDefaultStartingAssetsToJson();

    QMessageBox::information(this, tr("提示"), tr("配置文件已成功修改！"));
}

void SSE2_ConfigEditorMainWnd::OnSaveBackup()
{
}

void SSE2_ConfigEditorMainWnd::UpdateUnitsLimitData()
{
    ui.lineEdit_Titan->setText(QString::number(m_iTitanNum));
    ui.lineEdit_SuperCapitalship->setText(QString::number(m_iSuperCapitalshipNum));
    ui.lineEdit_starStarbase->setText(QString::number(m_istarStarbase));
    ui.lineEdit_planetStarbase->setText(QString::number(m_iplanetStarbase));
}

void SSE2_ConfigEditorMainWnd::WriteUnitLimitConfigToJson()
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
}

void SSE2_ConfigEditorMainWnd::UpdateMaxSupplyData()
{
    if (m_eFaction == Faction_TL || m_eFaction == Faction_TR || m_eFaction == Faction_AL|| m_eFaction == Faction_AR)
    {
        if (m_listMaxSupply.size() == 6)
        {
            ui.lineEdit_Maxsupply0->setText(QString::number(m_listMaxSupply[0]));
            ui.lineEdit_Maxsupply1->setText(QString::number(m_listMaxSupply[1]));
            ui.lineEdit_Maxsupply2->setText(QString::number(m_listMaxSupply[2]));
            ui.lineEdit_Maxsupply3->setText(QString::number(m_listMaxSupply[3]));
            ui.lineEdit_Maxsupply4->setText(QString::number(m_listMaxSupply[4]));
            ui.lineEdit_Maxsupply5->setText(QString::number(m_listMaxSupply[5]));
        }
        else
        {
            QMessageBox::warning(this, tr("警告"), tr("max_supply 数据不完整，请检查配置文件！"));
        }
    }
    else
    {
        if (m_listMaxSupply.size() == 7)
        {
            ui.lineEdit_Maxsupply0->setText(QString::number(m_listMaxSupply[0]));
            ui.lineEdit_Maxsupply1->setText(QString::number(m_listMaxSupply[1]));
            ui.lineEdit_Maxsupply2->setText(QString::number(m_listMaxSupply[2]));
            ui.lineEdit_Maxsupply3->setText(QString::number(m_listMaxSupply[3]));
            ui.lineEdit_Maxsupply4->setText(QString::number(m_listMaxSupply[4]));
            ui.lineEdit_Maxsupply5->setText(QString::number(m_listMaxSupply[5]));
            ui.lineEdit_Maxsupply6->setText(QString::number(m_listMaxSupply[6]));
        }
        else
        {
            QMessageBox::warning(this, tr("警告"), tr("max_supply 数据不完整，请检查配置文件！"));
        }
    }
}

void SSE2_ConfigEditorMainWnd::WriteMaxSupplyConfigToJson()
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
    QJsonObject maxSupplyObj = jsonObj.value("max_supply").toObject();

    // 从界面获取当前值
    m_listMaxSupply.clear();
    m_listMaxSupply.append(ui.lineEdit_Maxsupply0->text().toInt());
    m_listMaxSupply.append(ui.lineEdit_Maxsupply1->text().toInt());
    m_listMaxSupply.append(ui.lineEdit_Maxsupply2->text().toInt());
    m_listMaxSupply.append(ui.lineEdit_Maxsupply3->text().toInt());
    m_listMaxSupply.append(ui.lineEdit_Maxsupply4->text().toInt());
    m_listMaxSupply.append(ui.lineEdit_Maxsupply5->text().toInt());

    // 瓦萨里派系有第7个等级
    if (m_eFaction == Faction_VL || m_eFaction == Faction_VR)
    {
        m_listMaxSupply.append(ui.lineEdit_Maxsupply6->text().toInt());
    }

    // 更新 levels 数组
    QJsonArray levelsArray;
    for (int maxSupply : m_listMaxSupply)
    {
        QJsonObject item;
        item["max_supply"] = maxSupply;
        levelsArray.append(item);
    }

    maxSupplyObj["levels"] = levelsArray;
    jsonObj["max_supply"] = maxSupplyObj;

    // 写入文件
    jsonDoc.setObject(jsonObj);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        qDebug() << "无法写入配置文件:" << strConfigFile;
        return;
    }

    file.write(jsonDoc.toJson());
    file.close();
}

void SSE2_ConfigEditorMainWnd::ParseDefaultStartingAssetsFromJson(const QJsonDocument& jsonDoc)
{
    if (jsonDoc.isNull() || !jsonDoc.isObject())
    {
        qDebug() << "JSON 文件格式错误";
        return;
    }

    QJsonObject jsonObj = jsonDoc.object();
    QJsonObject defaultStartingAssetsObj = jsonObj.value("default_starting_assets").toObject();

    // 瓦萨里派系没有credits字段
    if (m_eFaction != Faction_VL && m_eFaction != Faction_VR)
    {
        m_iDefaultCredits = defaultStartingAssetsObj.value("credits").toInt(1000);
    }

    m_iDefaultMetal = defaultStartingAssetsObj.value("metal").toInt(400);
    m_iDefaultCrystal = defaultStartingAssetsObj.value("crystal").toInt(250);

    UpdateDefaultStartingAssetsData();
}

void SSE2_ConfigEditorMainWnd::UpdateDefaultStartingAssetsData()
{
    // 瓦萨里派系没有credits字段
    if (m_eFaction != Faction_VL && m_eFaction != Faction_VR)
    {
        ui.lineEdit_default_starting_credit->setText(QString::number(m_iDefaultCredits));
    }

    ui.lineEdit_default_starting_metal->setText(QString::number(m_iDefaultMetal));
    ui.lineEdit_default_starting_crystal->setText(QString::number(m_iDefaultCrystal));
}

void SSE2_ConfigEditorMainWnd::WriteDefaultStartingAssetsToJson()
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
    QJsonObject defaultStartingAssetsObj = jsonObj.value("default_starting_assets").toObject();

    // 从界面获取当前值
    // 瓦萨里派系没有credits字段
    if (m_eFaction != Faction_VL && m_eFaction != Faction_VR)
    {
        m_iDefaultCredits = ui.lineEdit_default_starting_credit->text().toInt();
        defaultStartingAssetsObj["credits"] = m_iDefaultCredits;
    }

    m_iDefaultMetal = ui.lineEdit_default_starting_metal->text().toInt();
    m_iDefaultCrystal = ui.lineEdit_default_starting_crystal->text().toInt();

    defaultStartingAssetsObj["metal"] = m_iDefaultMetal;
    defaultStartingAssetsObj["crystal"] = m_iDefaultCrystal;

    jsonObj["default_starting_assets"] = defaultStartingAssetsObj;

    // 写入文件
    jsonDoc.setObject(jsonObj);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        qDebug() << "无法写入配置文件:" << strConfigFile;
        return;
    }

    file.write(jsonDoc.toJson());
    file.close();
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

    // 瓦萨里派系没有credits字段
    if (m_eFaction != Faction_VL && m_eFaction != Faction_VR)
    {
        m_iDefaultCredits = ui.lineEdit_default_starting_credit->text().toInt();
    }

    m_iDefaultMetal = ui.lineEdit_default_starting_metal->text().toInt();
    m_iDefaultCrystal = ui.lineEdit_default_starting_crystal->text().toInt();
}