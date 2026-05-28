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
    m_iDefaultCrystal(250),
    m_iCurrentShipIndex(-1),
    m_iCurrentLevelIndex(-1)

{
    ui.setupUi(this);
    setFixedSize(this->width(), this->height());
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
            InitData();
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

    m_pActionTip = new QAction(tr("提示"), this);
    connect(m_pActionTip, &QAction::triggered, this, &SSE2_ConfigEditorMainWnd::OnTip);
    ui.menu_Help->addAction(m_pActionTip);

    ui.label_Maxsupply6->setVisible(false);
    ui.lineEdit_Maxsupply6->setVisible(false);
}

void SSE2_ConfigEditorMainWnd::InitData()
{
    m_listTradeCapitalshipInfo[0].eCapitalship = Capitalship_A;
    m_listTradeCapitalshipInfo[1].eCapitalship = Capitalship_B;
    m_listTradeCapitalshipInfo[2].eCapitalship = Capitalship_C;
    m_listTradeCapitalshipInfo[3].eCapitalship = Capitalship_D;
    m_listTradeCapitalshipInfo[4].eCapitalship = Capitalship_E;

    m_listVasariCapitalshipInfo[0].eCapitalship = Capitalship_A;
    m_listVasariCapitalshipInfo[1].eCapitalship = Capitalship_B;
    m_listVasariCapitalshipInfo[2].eCapitalship = Capitalship_C;
    m_listVasariCapitalshipInfo[3].eCapitalship = Capitalship_D;
    m_listVasariCapitalshipInfo[4].eCapitalship = Capitalship_E;

    m_listAdventCapitalshipInfo[0].eCapitalship = Capitalship_A;
    m_listAdventCapitalshipInfo[1].eCapitalship = Capitalship_B;
    m_listAdventCapitalshipInfo[2].eCapitalship = Capitalship_C;
    m_listAdventCapitalshipInfo[3].eCapitalship = Capitalship_D;
    m_listAdventCapitalshipInfo[4].eCapitalship = Capitalship_E;
}

void SSE2_ConfigEditorMainWnd::refreshCapitalshipCombox()
{
    ui.comboBox_CapitalShip->clear();
    if (m_eFaction == Faction_AL || m_eFaction == Faction_AR)
    {
        ui.comboBox_CapitalShip->addItem("光辉");
        ui.comboBox_CapitalShip->addItem("先祖");
        ui.comboBox_CapitalShip->addItem("宁静");
        ui.comboBox_CapitalShip->addItem("超度");
        ui.comboBox_CapitalShip->addItem("天启");
    }
    else if (m_eFaction == Faction_TL || m_eFaction == Faction_TR)
    {
        ui.comboBox_CapitalShip->addItem("科尔");
        ui.comboBox_CapitalShip->addItem("艾肯");
        ui.comboBox_CapitalShip->addItem("瓦索");
        ui.comboBox_CapitalShip->addItem("玛莎");
        ui.comboBox_CapitalShip->addItem("杜诺夫");
    }
    else if (m_eFaction == Faction_VL || m_eFaction == Faction_VR)
    {
        ui.comboBox_CapitalShip->addItem("克尔图");
        ui.comboBox_CapitalShip->addItem("加拉苏");
        ui.comboBox_CapitalShip->addItem("基兰查");
        ui.comboBox_CapitalShip->addItem("瓦诺克斯");
        ui.comboBox_CapitalShip->addItem("安特可");
    }
}

void SSE2_ConfigEditorMainWnd::ConnectSlots()
{
    connect(ui.comboBox_faction, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SSE2_ConfigEditorMainWnd::OnFactionChanged);

    connect(ui.lineEdit_Titan, &QLineEdit::editingFinished, this, &SSE2_ConfigEditorMainWnd::OnEditFinished);
    connect(ui.lineEdit_SuperCapitalship, &QLineEdit::editingFinished, this, &SSE2_ConfigEditorMainWnd::OnEditFinished);
    connect(ui.lineEdit_starStarbase, &QLineEdit::editingFinished, this, &SSE2_ConfigEditorMainWnd::OnEditFinished);
    connect(ui.lineEdit_planetStarbase, &QLineEdit::editingFinished, this, &SSE2_ConfigEditorMainWnd::OnEditFinished);

    connect(ui.comboBox_CapitalShip, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SSE2_ConfigEditorMainWnd::OnCapitalshipTypeOrLevelChanged);
    connect(ui.comboBox_Levels, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SSE2_ConfigEditorMainWnd::OnCapitalshipTypeOrLevelChanged);

    
}

void SSE2_ConfigEditorMainWnd::ReadConfig()
{
    if (m_strGamePath.isEmpty())
    {
        return;
    }

    // 阵营palyer配置文件
    QString strConfigFile;
    // 主力舰配置文件
    QString strCapitalshipAFile;
    QString strCapitalshipBFile;
    QString strCapitalshipCFile;
    QString strCapitalshipDFile;
    QString strCapitalshipEFile;

    switch (m_eFaction)
    {
    case Faction_TL:
        strConfigFile = QString("%1/entities/%2.player").arg(m_strGamePath).arg(TL_PlayerConfig);
        strCapitalshipAFile = QString("%1/entities/%2.unit").arg(m_strGamePath).arg(Trader_BattleCaptialShip);
        strCapitalshipBFile = QString("%1/entities/%2.unit").arg(m_strGamePath).arg(Trader_ColonyCaptialShip);
        strCapitalshipCFile = QString("%1/entities/%2.unit").arg(m_strGamePath).arg(Trader_CarrierCaptialShip);
        strCapitalshipDFile = QString("%1/entities/%2.unit").arg(m_strGamePath).arg(Trader_SiegeCaptialShip);
        strCapitalshipEFile = QString("%1/entities/%2.unit").arg(m_strGamePath).arg(Trader_SupportCaptialShip);
        break;
    case Faction_TR:
        strConfigFile = QString("%1/entities/%2.player").arg(m_strGamePath).arg(TR_PlayerConfig);
        strCapitalshipAFile = QString("%1/entities/%2.unit").arg(m_strGamePath).arg(Trader_BattleCaptialShip);
        strCapitalshipBFile = QString("%1/entities/%2.unit").arg(m_strGamePath).arg(Trader_ColonyCaptialShip);
        strCapitalshipCFile = QString("%1/entities/%2.unit").arg(m_strGamePath).arg(Trader_CarrierCaptialShip);
        strCapitalshipDFile = QString("%1/entities/%2.unit").arg(m_strGamePath).arg(Trader_SiegeCaptialShip);
        strCapitalshipEFile = QString("%1/entities/%2.unit").arg(m_strGamePath).arg(Trader_SupportCaptialShip);
        break;
    case Faction_VL:
        strConfigFile = QString("%1/entities/%2.player").arg(m_strGamePath).arg(VL_PlayerConfig);
        strCapitalshipAFile = QString("%1/entities/%2.unit").arg(m_strGamePath).arg(Vasari_BattleCaptialShip);
        strCapitalshipBFile = QString("%1/entities/%2.unit").arg(m_strGamePath).arg(Vasari_ColonyCaptialShip);
        strCapitalshipCFile = QString("%1/entities/%2.unit").arg(m_strGamePath).arg(Vasari_CarrierCaptialShip);
        strCapitalshipDFile = QString("%1/entities/%2.unit").arg(m_strGamePath).arg(Vasari_SiegeCaptialShip);
        strCapitalshipEFile = QString("%1/entities/%2.unit").arg(m_strGamePath).arg(Vasari_MarauderCaptialShip);
        break;
    case Faction_VR:
        strConfigFile = QString("%1/entities/%2.player").arg(m_strGamePath).arg(VR_PlayerConfig);
        strCapitalshipAFile = QString("%1/entities/%2.unit").arg(m_strGamePath).arg(Vasari_BattleCaptialShip);
        strCapitalshipBFile = QString("%1/entities/%2.unit").arg(m_strGamePath).arg(Vasari_ColonyCaptialShip);
        strCapitalshipCFile = QString("%1/entities/%2.unit").arg(m_strGamePath).arg(Vasari_CarrierCaptialShip);
        strCapitalshipDFile = QString("%1/entities/%2.unit").arg(m_strGamePath).arg(Vasari_SiegeCaptialShip);
        strCapitalshipEFile = QString("%1/entities/%2.unit").arg(m_strGamePath).arg(Vasari_MarauderCaptialShip);
        break;
    case Faction_AL:
        strConfigFile = QString("%1/entities/%2.player").arg(m_strGamePath).arg(AL_PlayerConfig);
        strCapitalshipAFile = QString("%1/entities/%2.unit").arg(m_strGamePath).arg(Advent_BattleCaptialShip);
        strCapitalshipBFile = QString("%1/entities/%2.unit").arg(m_strGamePath).arg(Advent_ColonyCaptialShip);
        strCapitalshipCFile = QString("%1/entities/%2.unit").arg(m_strGamePath).arg(Advent_CarrierCaptialShip);
        strCapitalshipDFile = QString("%1/entities/%2.unit").arg(m_strGamePath).arg(Advent_BattlePsionicCapitalShip);
        strCapitalshipEFile = QString("%1/entities/%2.unit").arg(m_strGamePath).arg(Advent_PlanetPsionicCaptialShip);
        break;
    case Faction_AR:
        strConfigFile = QString("%1/entities/%2.player").arg(m_strGamePath).arg(AR_PlayerConfig);
        strCapitalshipAFile = QString("%1/entities/%2.unit").arg(m_strGamePath).arg(Advent_BattleCaptialShip);
        strCapitalshipBFile = QString("%1/entities/%2.unit").arg(m_strGamePath).arg(Advent_ColonyCaptialShip);
        strCapitalshipCFile = QString("%1/entities/%2.unit").arg(m_strGamePath).arg(Advent_CarrierCaptialShip);
        strCapitalshipDFile = QString("%1/entities/%2.unit").arg(m_strGamePath).arg(Advent_BattlePsionicCapitalShip);
        strCapitalshipEFile = QString("%1/entities/%2.unit").arg(m_strGamePath).arg(Advent_PlanetPsionicCaptialShip);
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

    QFile fileCapitalshipA(strCapitalshipAFile);
    QFile fileCapitalshipB(strCapitalshipBFile);
    QFile fileCapitalshipC(strCapitalshipCFile);
    QFile fileCapitalshipD(strCapitalshipDFile);
    QFile fileCapitalshipE(strCapitalshipEFile);

    if (!fileCapitalshipA.open(QIODevice::ReadOnly) || !fileCapitalshipB.open(QIODevice::ReadOnly) ||
        !fileCapitalshipC.open(QIODevice::ReadOnly) || !fileCapitalshipD.open(QIODevice::ReadOnly) || !fileCapitalshipE.open(QIODevice::ReadOnly))
    {
        qDebug() << "无法打开主力舰配置文件";
        return;
    }

    QByteArray jsonDataCapitalshipA = fileCapitalshipA.readAll();
    QByteArray jsonDataCapitalshipB = fileCapitalshipB.readAll();
    QByteArray jsonDataCapitalshipC = fileCapitalshipC.readAll();
    QByteArray jsonDataCapitalshipD = fileCapitalshipD.readAll();
    QByteArray jsonDataCapitalshipE = fileCapitalshipE.readAll();
    fileCapitalshipA.close();
    fileCapitalshipB.close();
    fileCapitalshipC.close();
    fileCapitalshipD.close();
    fileCapitalshipE.close();

    ParseCapitalshipConfigFromJson(QJsonDocument::fromJson(jsonDataCapitalshipA), QJsonDocument::fromJson(jsonDataCapitalshipB),
        QJsonDocument::fromJson(jsonDataCapitalshipC), QJsonDocument::fromJson(jsonDataCapitalshipD), QJsonDocument::fromJson(jsonDataCapitalshipE));

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


void SSE2_ConfigEditorMainWnd::ParseCapitalshipConfigFromJson(const QJsonDocument& jsonDocA, const QJsonDocument& jsonDocB, const QJsonDocument& jsonDocC, const QJsonDocument& jsonDocD, const QJsonDocument& jsonDocE)
{
    // 辅助 Lambda:解析单个主力舰文件的 health.levels 并填入 stuCapitalshipInfo
    auto parseShipHealth = [&](const QJsonDocument& doc, stuCapitalshipInfo& info) {
        if (doc.isNull() || !doc.isObject())
            return;

        QJsonObject root = doc.object();
        QJsonObject healthObj = root.value("health").toObject();
        if (healthObj.isEmpty())
            return;

        QJsonArray levels = healthObj.value("levels").toArray();
        int count = qMin(levels.size(), 10); 

        for (int i = 0; i < count; ++i)
        {
            QJsonObject lvl = levels[i].toObject();
            stuCapitalshipLevelInfo& lvlInfo = info.LevelInfo[i];

            lvlInfo.MaxHull = lvl.value("max_hull_points").toDouble();
            lvlInfo.HullRestoreRate = lvl.value("hull_point_restore_rate").toDouble();
            lvlInfo.HullRestoreCooldown = lvl.value("hull_point_restore_cooldown_duration_after_damage_taken").toDouble();
            lvlInfo.HullRestoreScale = lvl.value("hull_point_restore_scalar_after_damage_taken").toDouble();
            lvlInfo.HullCrippledPercentage = lvl.value("hull_crippled_percentage").toDouble();
            lvlInfo.MaxArmor = lvl.value("max_armor_points").toDouble();
            lvlInfo.ArmorRestoreRate = lvl.value("armor_point_restore_rate").toDouble();
            lvlInfo.ArmorRestoreCooldown = lvl.value("armor_point_restore_cooldown_duration_after_damage_taken").toDouble();
            lvlInfo.ArmorRestoreScale = lvl.value("armor_point_restore_scalar_after_damage_taken").toDouble();
            lvlInfo.ArmorStrength = lvl.value("armor_strength").toDouble();
            lvlInfo.MaxShield = lvl.value("max_shield_points").toDouble();
            lvlInfo.ShieldRestoreRate = lvl.value("shield_point_restore_rate").toDouble();
            lvlInfo.ShieldRestoreCooldown = lvl.value("shield_point_restore_cooldown_duration_after_damage_taken").toDouble();
            lvlInfo.ShieldRestoreScale = lvl.value("shield_point_restore_scalar_after_damage_taken").toDouble();
        }
        };

    // 根据当前阵营选择目标数组
    stuCapitalshipInfo* targetArray = nullptr;
    switch (m_eFaction)
    {
    case Faction_TL:
    case Faction_TR:
        targetArray = m_listTradeCapitalshipInfo;
        break;
    case Faction_VL:
    case Faction_VR:
        targetArray = m_listVasariCapitalshipInfo;
        break;
    case Faction_AL:
    case Faction_AR:
        targetArray = m_listAdventCapitalshipInfo;
        break;
    default:
        return; // 无效阵营
    }

    parseShipHealth(jsonDocA, targetArray[0]);
    parseShipHealth(jsonDocB, targetArray[1]);
    parseShipHealth(jsonDocC, targetArray[2]);
    parseShipHealth(jsonDocD, targetArray[3]); 
    parseShipHealth(jsonDocE, targetArray[4]);
    UpdateCapitalshipData();
}

void SSE2_ConfigEditorMainWnd::UpdateCapitalshipData()
{
    m_iCurrentShipIndex = -1;
    m_iCurrentLevelIndex = -1;
    OnCapitalshipTypeOrLevelChanged();
}

void SSE2_ConfigEditorMainWnd::WriteCapitalshipConfigToJson()
{
    if (m_strGamePath.isEmpty())
        return;

    // ---------- 1. 先保存当前 UI 上显示的等级数据到结构体 ----------
    {
        // 获取当前阵营对应的主力舰数组（与 OnCapitalshipTypeOrLevelChanged 逻辑一致）
        stuCapitalshipInfo* targetArray = nullptr;
        switch (m_eFaction)
        {
        case Faction_TL: case Faction_TR: targetArray = m_listTradeCapitalshipInfo; break;
        case Faction_VL: case Faction_VR: targetArray = m_listVasariCapitalshipInfo; break;
        case Faction_AL: case Faction_AR: targetArray = m_listAdventCapitalshipInfo; break;
        default: return;
        }

        int shipIndex = ui.comboBox_CapitalShip->currentIndex();
        int levelIndex = ui.comboBox_Levels->currentIndex();
        if (shipIndex >= 0 && shipIndex < 5 && levelIndex >= 0 && levelIndex < 10)
        {
            stuCapitalshipLevelInfo& lvlInfo = targetArray[shipIndex].LevelInfo[levelIndex];

            lvlInfo.MaxHull = ui.lineEdit_MaxHull->text().toDouble();
            lvlInfo.HullRestoreRate = ui.lineEdit_HullRestoreRate->text().toDouble();
            lvlInfo.HullRestoreCooldown = ui.lineEdit_HullRestoreCooldown->text().toDouble();
            lvlInfo.HullRestoreScale = ui.lineEdit_HullRestoreScalar->text().toDouble();
            lvlInfo.HullCrippledPercentage = ui.lineEdit_CrippledPercentage->text().toDouble();
            lvlInfo.MaxArmor = ui.lineEdit_MaxArmor->text().toDouble();
            lvlInfo.ArmorRestoreRate = ui.lineEdit_ArmorRestoreRate->text().toDouble();
            lvlInfo.ArmorRestoreCooldown = ui.lineEdit_ArmorRestoreCooldown->text().toDouble();
            lvlInfo.ArmorRestoreScale = ui.lineEdit_ArmorRestoreScalar->text().toDouble();
            lvlInfo.ArmorStrength = ui.lineEdit_ArmorStrength->text().toDouble();
            lvlInfo.MaxShield = ui.lineEdit_MaxShield->text().toDouble();
            lvlInfo.ShieldRestoreRate = ui.lineEdit_ShieleRestoreRate->text().toDouble();
            lvlInfo.ShieldRestoreCooldown = ui.lineEdit_ShieldRestoreCooldown->text().toDouble();
            lvlInfo.ShieldRestoreScale = ui.lineEdit_Titan_16->text().toDouble();
        }
    }

    // ---------- 2. 构造五艘主力舰的 .unit 文件路径（与 ReadConfig 保持一致） ----------
    QString strShipFiles[5];
    switch (m_eFaction)
    {
    case Faction_TL:
    case Faction_TR:
        strShipFiles[0] = QString("%1/entities/%2.unit").arg(m_strGamePath).arg(Trader_BattleCaptialShip);
        strShipFiles[1] = QString("%1/entities/%2.unit").arg(m_strGamePath).arg(Trader_ColonyCaptialShip);
        strShipFiles[2] = QString("%1/entities/%2.unit").arg(m_strGamePath).arg(Trader_CarrierCaptialShip);
        strShipFiles[3] = QString("%1/entities/%2.unit").arg(m_strGamePath).arg(Trader_SiegeCaptialShip);
        strShipFiles[4] = QString("%1/entities/%2.unit").arg(m_strGamePath).arg(Trader_SupportCaptialShip);
        break;
    case Faction_VL:
    case Faction_VR:
        strShipFiles[0] = QString("%1/entities/%2.unit").arg(m_strGamePath).arg(Vasari_BattleCaptialShip);
        strShipFiles[1] = QString("%1/entities/%2.unit").arg(m_strGamePath).arg(Vasari_ColonyCaptialShip);
        strShipFiles[2] = QString("%1/entities/%2.unit").arg(m_strGamePath).arg(Vasari_CarrierCaptialShip);
        strShipFiles[3] = QString("%1/entities/%2.unit").arg(m_strGamePath).arg(Vasari_SiegeCaptialShip);
        strShipFiles[4] = QString("%1/entities/%2.unit").arg(m_strGamePath).arg(Vasari_MarauderCaptialShip);
        break;
    case Faction_AL:
    case Faction_AR:
        strShipFiles[0] = QString("%1/entities/%2.unit").arg(m_strGamePath).arg(Advent_BattleCaptialShip);
        strShipFiles[1] = QString("%1/entities/%2.unit").arg(m_strGamePath).arg(Advent_ColonyCaptialShip);
        strShipFiles[2] = QString("%1/entities/%2.unit").arg(m_strGamePath).arg(Advent_CarrierCaptialShip);
        strShipFiles[3] = QString("%1/entities/%2.unit").arg(m_strGamePath).arg(Advent_BattlePsionicCapitalShip);
        strShipFiles[4] = QString("%1/entities/%2.unit").arg(m_strGamePath).arg(Advent_PlanetPsionicCaptialShip);
        break;
    default:
        return;
    }

    // 获取当前阵营的主力舰数据数组
    stuCapitalshipInfo* targetArray = nullptr;
    switch (m_eFaction)
    {
    case Faction_TL: case Faction_TR: targetArray = m_listTradeCapitalshipInfo; break;
    case Faction_VL: case Faction_VR: targetArray = m_listVasariCapitalshipInfo; break;
    case Faction_AL: case Faction_AR: targetArray = m_listAdventCapitalshipInfo; break;
    default: return;
    }

    // ---------- 3. 逐个主力舰文件写入 ----------
    for (int shipIndex = 0; shipIndex < 5; ++shipIndex)
    {
        const stuCapitalshipInfo& shipInfo = targetArray[shipIndex];
        QFile file(strShipFiles[shipIndex]);
        if (!file.open(QIODevice::ReadOnly))
        {
            qDebug() << "WriteCapitalshipConfigToJson: 无法打开文件" << strShipFiles[shipIndex];
            continue;
        }

        QByteArray jsonData = file.readAll();
        file.close();

        QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
        if (jsonDoc.isNull() || !jsonDoc.isObject())
        {
            qDebug() << "WriteCapitalshipConfigToJson: JSON 格式错误" << strShipFiles[shipIndex];
            continue;
        }

        QJsonObject rootObj = jsonDoc.object();
        QJsonObject healthObj = rootObj.value("health").toObject();
        if (healthObj.isEmpty())
        {
            qDebug() << "WriteCapitalshipConfigToJson: 未找到 health 字段" << strShipFiles[shipIndex];
            continue;
        }

        QJsonArray levelsArray = healthObj.value("levels").toArray();
        int levelCount = levelsArray.size();
        int updateCount = qMin(levelCount, 10);   // 结构体最多10级

        for (int levelIndex = 0; levelIndex < updateCount; ++levelIndex)
        {
            const stuCapitalshipLevelInfo& lvlInfo = shipInfo.LevelInfo[levelIndex];
            QJsonObject levelObj = levelsArray[levelIndex].toObject();

            // 仅更新我们管理的字段，其他字段（如 shield_burst_restore 等）保持不变
            levelObj["max_hull_points"] = lvlInfo.MaxHull;
            levelObj["hull_point_restore_rate"] = lvlInfo.HullRestoreRate;
            levelObj["hull_point_restore_cooldown_duration_after_damage_taken"] = lvlInfo.HullRestoreCooldown;
            levelObj["hull_point_restore_scalar_after_damage_taken"] = lvlInfo.HullRestoreScale;
            levelObj["hull_crippled_percentage"] = lvlInfo.HullCrippledPercentage;
            levelObj["max_armor_points"] = lvlInfo.MaxArmor;
            levelObj["armor_point_restore_rate"] = lvlInfo.ArmorRestoreRate;
            levelObj["armor_point_restore_cooldown_duration_after_damage_taken"] = lvlInfo.ArmorRestoreCooldown;
            levelObj["armor_point_restore_scalar_after_damage_taken"] = lvlInfo.ArmorRestoreScale;
            levelObj["armor_strength"] = lvlInfo.ArmorStrength;
            levelObj["max_shield_points"] = lvlInfo.MaxShield;
            levelObj["shield_point_restore_rate"] = lvlInfo.ShieldRestoreRate;
            levelObj["shield_point_restore_cooldown_duration_after_damage_taken"] = lvlInfo.ShieldRestoreCooldown;
            levelObj["shield_point_restore_scalar_after_damage_taken"] = lvlInfo.ShieldRestoreScale;

            levelsArray[levelIndex] = levelObj;
        }

        // 如果原数组超过10级，保留多余部分不变；如果不足10级（理论上不会），不做补充
        healthObj["levels"] = levelsArray;
        rootObj["health"] = healthObj;
        jsonDoc.setObject(rootObj);

        // 写回文件（使用缩进保持与原文件相似的可读性）
        if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        {
            qDebug() << "WriteCapitalshipConfigToJson: 无法写入文件" << strShipFiles[shipIndex];
            continue;
        }
        file.write(jsonDoc.toJson(QJsonDocument::Indented));
        file.close();
    }
}

void SSE2_ConfigEditorMainWnd::OnFactionChanged(int index)
{
    m_eFaction = (eFaction)index;
    // 重置暂存索引，避免跨阵营保存
    m_iCurrentShipIndex = -1;
    m_iCurrentLevelIndex = -1;
    m_eFaction = (eFaction)index;
    
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

    refreshCapitalshipCombox();
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

    refreshCapitalshipCombox();
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
    // 写入默认起始资产配置
    WriteDefaultStartingAssetsToJson();
    // 新增主力舰数据写入
    WriteCapitalshipConfigToJson();
    QMessageBox::information(this, tr("提示"), tr("配置文件已成功修改！"));
}

void SSE2_ConfigEditorMainWnd::OnSaveBackup()
{
}

void SSE2_ConfigEditorMainWnd::OnTip()
{
    QMessageBox::information(this, tr("提示"), tr("注意在完成修改切换下拉框前写入配置"));
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


    ui.comboBox_Levels->addItem(tr("1"));
    ui.comboBox_Levels->addItem(tr("2"));
    ui.comboBox_Levels->addItem(tr("3"));
    ui.comboBox_Levels->addItem(tr("4"));
    ui.comboBox_Levels->addItem(tr("5"));
    ui.comboBox_Levels->addItem(tr("6"));
    ui.comboBox_Levels->addItem(tr("7"));
    ui.comboBox_Levels->addItem(tr("8"));
    ui.comboBox_Levels->addItem(tr("9"));
    ui.comboBox_Levels->addItem(tr("10"));
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

void SSE2_ConfigEditorMainWnd::OnCapitalshipTypeOrLevelChanged()
{
    // 1. 获取当前阵营对应的主力舰数组
    stuCapitalshipInfo* targetArray = nullptr;
    switch (m_eFaction)
    {
    case Faction_TL: case Faction_TR: targetArray = m_listTradeCapitalshipInfo; break;
    case Faction_VL: case Faction_VR: targetArray = m_listVasariCapitalshipInfo; break;
    case Faction_AL: case Faction_AR: targetArray = m_listAdventCapitalshipInfo; break;
    default: return;
    }

    // 2. 保存旧等级的数据（如果有效）
    if (m_iCurrentShipIndex >= 0 && m_iCurrentShipIndex < 5 &&
        m_iCurrentLevelIndex >= 0 && m_iCurrentLevelIndex < 10)
    {
        stuCapitalshipLevelInfo& oldLvl = targetArray[m_iCurrentShipIndex].LevelInfo[m_iCurrentLevelIndex];

        oldLvl.MaxHull = ui.lineEdit_MaxHull->text().toDouble();
        oldLvl.HullRestoreRate = ui.lineEdit_HullRestoreRate->text().toDouble();
        oldLvl.HullRestoreCooldown = ui.lineEdit_HullRestoreCooldown->text().toDouble();
        oldLvl.HullRestoreScale = ui.lineEdit_HullRestoreScalar->text().toDouble();
        oldLvl.HullCrippledPercentage = ui.lineEdit_CrippledPercentage->text().toDouble();
        oldLvl.MaxArmor = ui.lineEdit_MaxArmor->text().toDouble();
        oldLvl.ArmorRestoreRate = ui.lineEdit_ArmorRestoreRate->text().toDouble();
        oldLvl.ArmorRestoreCooldown = ui.lineEdit_ArmorRestoreCooldown->text().toDouble();
        oldLvl.ArmorRestoreScale = ui.lineEdit_ArmorRestoreScalar->text().toDouble();
        oldLvl.ArmorStrength = ui.lineEdit_ArmorStrength->text().toDouble();
        oldLvl.MaxShield = ui.lineEdit_MaxShield->text().toDouble();
        oldLvl.ShieldRestoreRate = ui.lineEdit_ShieleRestoreRate->text().toDouble();
        oldLvl.ShieldRestoreCooldown = ui.lineEdit_ShieldRestoreCooldown->text().toDouble();
        oldLvl.ShieldRestoreScale = ui.lineEdit_Titan_16->text().toDouble(); // 若已改名请换为 lineEdit_ShieldRestoreScalar
    }

    // 3. 获取新索引
    int shipIndex = ui.comboBox_CapitalShip->currentIndex();
    int levelIndex = ui.comboBox_Levels->currentIndex();

    if (shipIndex < 0 || shipIndex >= 5 || levelIndex < 0 || levelIndex >= 10)
        return;

    // 4. 加载新数据到界面
    const stuCapitalshipLevelInfo& lvlInfo = targetArray[shipIndex].LevelInfo[levelIndex];

    ui.lineEdit_MaxHull->setText(QString::number(lvlInfo.MaxHull, 'f', 1));
    ui.lineEdit_HullRestoreRate->setText(QString::number(lvlInfo.HullRestoreRate, 'f', 1));
    ui.lineEdit_HullRestoreCooldown->setText(QString::number(lvlInfo.HullRestoreCooldown, 'f', 1));
    ui.lineEdit_HullRestoreScalar->setText(QString::number(lvlInfo.HullRestoreScale, 'f', 1));
    ui.lineEdit_CrippledPercentage->setText(QString::number(lvlInfo.HullCrippledPercentage, 'f', 2));
    ui.lineEdit_MaxArmor->setText(QString::number(lvlInfo.MaxArmor, 'f', 1));
    ui.lineEdit_ArmorRestoreRate->setText(QString::number(lvlInfo.ArmorRestoreRate, 'f', 1));
    ui.lineEdit_ArmorRestoreCooldown->setText(QString::number(lvlInfo.ArmorRestoreCooldown, 'f', 1));
    ui.lineEdit_ArmorRestoreScalar->setText(QString::number(lvlInfo.ArmorRestoreScale, 'f', 1));
    ui.lineEdit_ArmorStrength->setText(QString::number(lvlInfo.ArmorStrength, 'f', 1));
    ui.lineEdit_MaxShield->setText(QString::number(lvlInfo.MaxShield, 'f', 1));
    ui.lineEdit_ShieleRestoreRate->setText(QString::number(lvlInfo.ShieldRestoreRate, 'f', 1));
    ui.lineEdit_ShieldRestoreCooldown->setText(QString::number(lvlInfo.ShieldRestoreCooldown, 'f', 1));
    ui.lineEdit_Titan_16->setText(QString::number(lvlInfo.ShieldRestoreScale, 'f', 1)); // 同上

    ui.lineEdit_NextExp->setText("0");

    // 5. 更新记录的当前索引
    m_iCurrentShipIndex = shipIndex;
    m_iCurrentLevelIndex = levelIndex;
}