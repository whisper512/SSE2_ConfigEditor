#include "SSE2_ConfigEditorMainWnd.h"

#include <QTimer>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QVector>

SSE2_ConfigEditorMainWnd::SSE2_ConfigEditorMainWnd(QWidget* parent)
    : QMainWindow(parent),
    m_pActionOpenGamePath(nullptr),
    m_pActionWriteConfig(nullptr),
    m_pActionSaveBackup(nullptr),
    m_strGamePath(""),
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
    // 创建六个阵营的默认数据
    m_factionDataMap = CreateDefaultFactionConfigs();

    // 默认选中第一个阵营(贸易联盟忠诚派)
    m_pCurrentFactionData = &m_factionDataMap[Faction_TL];
}

void SSE2_ConfigEditorMainWnd::refreshCapitalshipCombox()
{
    ui.comboBox_CapitalShip->clear();
    if (!m_pCurrentFactionData)
        return;

    const QStringList& names = m_pCurrentFactionData->capitalShipDisplayNames;
    for (const QString& name : names)
        ui.comboBox_CapitalShip->addItem(name);
}

void SSE2_ConfigEditorMainWnd::ConnectSlots()
{
    connect(ui.comboBox_faction, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &SSE2_ConfigEditorMainWnd::OnFactionChanged);

    connect(ui.lineEdit_Titan, &QLineEdit::editingFinished, this, &SSE2_ConfigEditorMainWnd::OnEditFinished);
    connect(ui.lineEdit_SuperCapitalship, &QLineEdit::editingFinished, this, &SSE2_ConfigEditorMainWnd::OnEditFinished);
    connect(ui.lineEdit_starStarbase, &QLineEdit::editingFinished, this, &SSE2_ConfigEditorMainWnd::OnEditFinished);
    connect(ui.lineEdit_planetStarbase, &QLineEdit::editingFinished, this, &SSE2_ConfigEditorMainWnd::OnEditFinished);

    connect(ui.comboBox_CapitalShip, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &SSE2_ConfigEditorMainWnd::OnCapitalshipTypeOrLevelChanged);
    connect(ui.comboBox_Levels, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &SSE2_ConfigEditorMainWnd::OnCapitalshipTypeOrLevelChanged);
}

void SSE2_ConfigEditorMainWnd::ReadConfig()
{
    if (m_strGamePath.isEmpty() || !m_pCurrentFactionData)
        return;

    const FactionData& faction = *m_pCurrentFactionData;

    // 读取 .player 文件
    QString playerFilePath = QString("%1/entities/%2.player")
        .arg(m_strGamePath, faction.playerFileName);
    QFile file(playerFilePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "无法打开配置文件:" << playerFilePath;
        return;
    }
    QJsonDocument jsonDoc = QJsonDocument::fromJson(file.readAll());
    file.close();

    ParseUnitLimitConfigFromJson(jsonDoc);
    ParseMaxSupplyConfigFromJson(jsonDoc);
    ParseDefaultStartingAssetsFromJson(jsonDoc);

    // 读取 5 个主力舰文件
    QVector<QJsonDocument> shipDocs(5);
    bool allOpened = true;
    for (int i = 0; i < 5; ++i)
    {
        QString shipPath = QString("%1/entities/%2.unit")
            .arg(m_strGamePath, faction.capitalShipFileNames[i]);
        QFile shipFile(shipPath);
        if (shipFile.open(QIODevice::ReadOnly))
        {
            shipDocs[i] = QJsonDocument::fromJson(shipFile.readAll());
            shipFile.close();
        }
        else
        {
            qDebug() << "无法打开主力舰文件:" << shipPath;
            allOpened = false;
        }
    }
    if (allOpened)
    {
        ParseCapitalshipConfigFromJson(shipDocs[0], shipDocs[1], shipDocs[2],
            shipDocs[3], shipDocs[4]);
    }
}

void SSE2_ConfigEditorMainWnd::ParseUnitLimitConfigFromJson(const QJsonDocument& jsonDoc)
{
    if (jsonDoc.isNull() || !jsonDoc.isObject() || !m_pCurrentFactionData)
        return;

    QJsonObject jsonObj = jsonDoc.object();
    QJsonObject unitLimitsObj = jsonObj.value("unit_limits").toObject();

    // global
    QJsonArray globalArray = unitLimitsObj.value("global").toArray();
    for (const QJsonValue& value : globalArray)
    {
        QJsonObject item = value.toObject();
        QString tag = item.value("tag").toString();
        int limit = item.value("unit_limit").toInt();
        if (tag == "titan")
            m_pCurrentFactionData->unitLimits.titan = limit;
        else if (tag == "super_capital_ship")
            m_pCurrentFactionData->unitLimits.superCapitalShip = limit;
    }

    // star
    QJsonArray starArray = unitLimitsObj.value("star").toArray();
    for (const QJsonValue& value : starArray)
    {
        QJsonObject item = value.toObject();
        if (item.value("tag").toString() == "starbase")
            m_pCurrentFactionData->unitLimits.starStarbase = item.value("unit_limit").toInt();
    }

    // planet
    QJsonArray planetArray = unitLimitsObj.value("planet").toArray();
    for (const QJsonValue& value : planetArray)
    {
        QJsonObject item = value.toObject();
        if (item.value("tag").toString() == "starbase")
            m_pCurrentFactionData->unitLimits.planetStarbase = item.value("unit_limit").toInt();
    }

    UpdateUnitsLimitData();
}

void SSE2_ConfigEditorMainWnd::ParseMaxSupplyConfigFromJson(const QJsonDocument& jsonDoc)
{
    if (jsonDoc.isNull() || !jsonDoc.isObject() || !m_pCurrentFactionData)
        return;

    QJsonObject jsonObj = jsonDoc.object();
    QJsonObject maxSupplyObj = jsonObj.value("max_supply").toObject();
    QJsonArray levelsArray = maxSupplyObj.value("levels").toArray();

    QVector<int>& levels = m_pCurrentFactionData->maxSupplyLevels;
    levels.clear();
    for (const QJsonValue& value : levelsArray)
    {
        QJsonObject item = value.toObject();
        levels.append(item.value("max_supply").toInt());
    }
    UpdateMaxSupplyData();
}

void SSE2_ConfigEditorMainWnd::ParseCapitalshipConfigFromJson(
    const QJsonDocument& jsonDocA, const QJsonDocument& jsonDocB,
    const QJsonDocument& jsonDocC, const QJsonDocument& jsonDocD,
    const QJsonDocument& jsonDocE)
{
    if (!m_pCurrentFactionData)
        return;

    auto parseShipHealth = [&](const QJsonDocument& doc, stuCapitalshipInfo& info)
        {
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
                stuCapitalshipLevelInfo& li = info.LevelInfo[i];
                li.MaxHull = lvl.value("max_hull_points").toDouble();
                li.HullRestoreRate = lvl.value("hull_point_restore_rate").toDouble();
                li.HullRestoreCooldown = lvl.value("hull_point_restore_cooldown_duration_after_damage_taken").toDouble();
                li.HullRestoreScale = lvl.value("hull_point_restore_scalar_after_damage_taken").toDouble();
                li.HullCrippledPercentage = lvl.value("hull_crippled_percentage").toDouble();
                li.MaxArmor = lvl.value("max_armor_points").toDouble();
                li.ArmorRestoreRate = lvl.value("armor_point_restore_rate").toDouble();
                li.ArmorRestoreCooldown = lvl.value("armor_point_restore_cooldown_duration_after_damage_taken").toDouble();
                li.ArmorRestoreScale = lvl.value("armor_point_restore_scalar_after_damage_taken").toDouble();
                li.ArmorStrength = lvl.value("armor_strength").toDouble();
                li.MaxShield = lvl.value("max_shield_points").toDouble();
                li.ShieldRestoreRate = lvl.value("shield_point_restore_rate").toDouble();
                li.ShieldRestoreCooldown = lvl.value("shield_point_restore_cooldown_duration_after_damage_taken").toDouble();
                li.ShieldRestoreScale = lvl.value("shield_point_restore_scalar_after_damage_taken").toDouble();
            }
        };

    QVector<stuCapitalshipInfo>& ships = m_pCurrentFactionData->capitalShips;
    parseShipHealth(jsonDocA, ships[0]);
    parseShipHealth(jsonDocB, ships[1]);
    parseShipHealth(jsonDocC, ships[2]);
    parseShipHealth(jsonDocD, ships[3]);
    parseShipHealth(jsonDocE, ships[4]);

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
    if (m_strGamePath.isEmpty() || !m_pCurrentFactionData)
        return;

    FactionData& faction = *m_pCurrentFactionData;

    // 1. 保存当前 UI 显示的等级数据
    int shipIndex = ui.comboBox_CapitalShip->currentIndex();
    int levelIndex = ui.comboBox_Levels->currentIndex();
    if (shipIndex >= 0 && shipIndex < 5 && levelIndex >= 0 && levelIndex < 10)
    {
        stuCapitalshipLevelInfo& li = faction.capitalShips[shipIndex].LevelInfo[levelIndex];
        li.MaxHull = ui.lineEdit_MaxHull->text().toDouble();
        li.HullRestoreRate = ui.lineEdit_HullRestoreRate->text().toDouble();
        li.HullRestoreCooldown = ui.lineEdit_HullRestoreCooldown->text().toDouble();
        li.HullRestoreScale = ui.lineEdit_HullRestoreScalar->text().toDouble();
        li.HullCrippledPercentage = ui.lineEdit_CrippledPercentage->text().toDouble();
        li.MaxArmor = ui.lineEdit_MaxArmor->text().toDouble();
        li.ArmorRestoreRate = ui.lineEdit_ArmorRestoreRate->text().toDouble();
        li.ArmorRestoreCooldown = ui.lineEdit_ArmorRestoreCooldown->text().toDouble();
        li.ArmorRestoreScale = ui.lineEdit_ArmorRestoreScalar->text().toDouble();
        li.ArmorStrength = ui.lineEdit_ArmorStrength->text().toDouble();
        li.MaxShield = ui.lineEdit_MaxShield->text().toDouble();
        li.ShieldRestoreRate = ui.lineEdit_ShieleRestoreRate->text().toDouble();
        li.ShieldRestoreCooldown = ui.lineEdit_ShieldRestoreCooldown->text().toDouble();
        li.ShieldRestoreScale = ui.lineEdit_Titan_16->text().toDouble();
    }

    // 2. 写入 5 个文件
    for (int i = 0; i < 5; ++i)
    {
        QString filePath = QString("%1/entities/%2.unit")
            .arg(m_strGamePath, faction.capitalShipFileNames[i]);
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly))
            continue;

        QJsonDocument jsonDoc = QJsonDocument::fromJson(file.readAll());
        file.close();
        if (jsonDoc.isNull() || !jsonDoc.isObject())
            continue;

        QJsonObject rootObj = jsonDoc.object();
        QJsonObject healthObj = rootObj.value("health").toObject();
        if (healthObj.isEmpty())
            continue;

        QJsonArray levelsArray = healthObj.value("levels").toArray();
        int updateCount = qMin(levelsArray.size(), 10);
        const stuCapitalshipInfo& ship = faction.capitalShips[i];

        for (int lvl = 0; lvl < updateCount; ++lvl)
        {
            const stuCapitalshipLevelInfo& li = ship.LevelInfo[lvl];
            QJsonObject levelObj = levelsArray[lvl].toObject();
            levelObj["max_hull_points"] = li.MaxHull;
            levelObj["hull_point_restore_rate"] = li.HullRestoreRate;
            levelObj["hull_point_restore_cooldown_duration_after_damage_taken"] = li.HullRestoreCooldown;
            levelObj["hull_point_restore_scalar_after_damage_taken"] = li.HullRestoreScale;
            levelObj["hull_crippled_percentage"] = li.HullCrippledPercentage;
            levelObj["max_armor_points"] = li.MaxArmor;
            levelObj["armor_point_restore_rate"] = li.ArmorRestoreRate;
            levelObj["armor_point_restore_cooldown_duration_after_damage_taken"] = li.ArmorRestoreCooldown;
            levelObj["armor_point_restore_scalar_after_damage_taken"] = li.ArmorRestoreScale;
            levelObj["armor_strength"] = li.ArmorStrength;
            levelObj["max_shield_points"] = li.MaxShield;
            levelObj["shield_point_restore_rate"] = li.ShieldRestoreRate;
            levelObj["shield_point_restore_cooldown_duration_after_damage_taken"] = li.ShieldRestoreCooldown;
            levelObj["shield_point_restore_scalar_after_damage_taken"] = li.ShieldRestoreScale;

            levelsArray[lvl] = levelObj;
        }

        healthObj["levels"] = levelsArray;
        rootObj["health"] = healthObj;
        jsonDoc.setObject(rootObj);

        if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        {
            file.write(jsonDoc.toJson(QJsonDocument::Indented));
            file.close();
        }
    }
}

void SSE2_ConfigEditorMainWnd::OnFactionChanged(int index)
{
    // 根据下拉框的索引设置当前阵营
    eFaction newFaction = static_cast<eFaction>(index);
    if (newFaction < Faction_TL || newFaction >= Faction_Count)
        return;

    m_pCurrentFactionData = &m_factionDataMap[newFaction];

    // 重置主力舰索引
    m_iCurrentShipIndex = -1;
    m_iCurrentLevelIndex = -1;

    // 显示/隐藏瓦萨里专用控件
    bool isVasari = (newFaction == Faction_VL || newFaction == Faction_VR);
    ui.label_Maxsupply6->setVisible(isVasari);
    ui.lineEdit_Maxsupply6->setVisible(isVasari);
    ui.label_default_starting_credit->setVisible(!isVasari);
    ui.lineEdit_default_starting_credit->setVisible(!isVasari);

    refreshCapitalshipCombox();
    ReadConfig();
}

void SSE2_ConfigEditorMainWnd::OnOpenGamePath()
{
    QString strFilePath = QFileDialog::getOpenFileName(
        this, tr("选择游戏可执行文件"), "", tr("可执行文件 (*.exe)"));
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
    if (m_strGamePath.isEmpty() || !m_pCurrentFactionData)
        return;

    WriteUnitLimitConfigToJson();
    WriteMaxSupplyConfigToJson();
    WriteDefaultStartingAssetsToJson();
    WriteCapitalshipConfigToJson();
    QMessageBox::information(this, tr("提示"), tr("配置文件已成功修改！"));
}

void SSE2_ConfigEditorMainWnd::OnSaveBackup()
{
    // 保留空实现
}

void SSE2_ConfigEditorMainWnd::OnTip()
{
    QMessageBox::information(this, tr("提示"), tr("注意在完成修改切换下拉框前写入配置"));
}

void SSE2_ConfigEditorMainWnd::UpdateUnitsLimitData()
{
    if (!m_pCurrentFactionData)
        return;
    const auto& lim = m_pCurrentFactionData->unitLimits;
    ui.lineEdit_Titan->setText(QString::number(lim.titan));
    ui.lineEdit_SuperCapitalship->setText(QString::number(lim.superCapitalShip));
    ui.lineEdit_starStarbase->setText(QString::number(lim.starStarbase));
    ui.lineEdit_planetStarbase->setText(QString::number(lim.planetStarbase));
}

void SSE2_ConfigEditorMainWnd::WriteUnitLimitConfigToJson()
{
    if (m_strGamePath.isEmpty() || !m_pCurrentFactionData)
        return;

    QString filePath = QString("%1/entities/%2.player")
        .arg(m_strGamePath, m_pCurrentFactionData->playerFileName);
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
        return;

    QJsonDocument jsonDoc = QJsonDocument::fromJson(file.readAll());
    file.close();
    if (jsonDoc.isNull() || !jsonDoc.isObject())
        return;

    QJsonObject jsonObj = jsonDoc.object();
    QJsonObject unitLimitsObj = jsonObj.value("unit_limits").toObject();
    const auto& lim = m_pCurrentFactionData->unitLimits;

    // global
    QJsonArray globalArray = unitLimitsObj.value("global").toArray();
    for (int i = 0; i < globalArray.size(); ++i)
    {
        QJsonObject item = globalArray[i].toObject();
        QString tag = item.value("tag").toString();
        if (tag == "titan")
            item["unit_limit"] = lim.titan;
        else if (tag == "super_capital_ship")
            item["unit_limit"] = lim.superCapitalShip;
        globalArray[i] = item;
    }
    unitLimitsObj["global"] = globalArray;

    // star
    QJsonArray starArray = unitLimitsObj.value("star").toArray();
    for (int i = 0; i < starArray.size(); ++i)
    {
        QJsonObject item = starArray[i].toObject();
        if (item.value("tag").toString() == "starbase")
            item["unit_limit"] = lim.starStarbase;
        starArray[i] = item;
    }
    unitLimitsObj["star"] = starArray;

    // planet
    QJsonArray planetArray = unitLimitsObj.value("planet").toArray();
    for (int i = 0; i < planetArray.size(); ++i)
    {
        QJsonObject item = planetArray[i].toObject();
        if (item.value("tag").toString() == "starbase")
            item["unit_limit"] = lim.planetStarbase;
        planetArray[i] = item;
    }
    unitLimitsObj["planet"] = planetArray;

    jsonObj["unit_limits"] = unitLimitsObj;
    jsonDoc.setObject(jsonObj);

    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        file.write(jsonDoc.toJson());
        file.close();
    }
}

void SSE2_ConfigEditorMainWnd::UpdateMaxSupplyData()
{
    if (!m_pCurrentFactionData)
        return;

    const QVector<int>& levels = m_pCurrentFactionData->maxSupplyLevels;
    int expectedCount = m_pCurrentFactionData->supplyLevelCount();
    if (levels.size() != expectedCount)
    {
        QMessageBox::warning(this, tr("警告"), tr("max_supply 数据不完整，请检查配置文件！"));
        return;
    }

    for (int i = 0; i < levels.size() && i < 7; ++i)
    {
        switch (i)
        {
        case 0: ui.lineEdit_Maxsupply0->setText(QString::number(levels[i])); break;
        case 1: ui.lineEdit_Maxsupply1->setText(QString::number(levels[i])); break;
        case 2: ui.lineEdit_Maxsupply2->setText(QString::number(levels[i])); break;
        case 3: ui.lineEdit_Maxsupply3->setText(QString::number(levels[i])); break;
        case 4: ui.lineEdit_Maxsupply4->setText(QString::number(levels[i])); break;
        case 5: ui.lineEdit_Maxsupply5->setText(QString::number(levels[i])); break;
        case 6: ui.lineEdit_Maxsupply6->setText(QString::number(levels[i])); break;
        }
    }
}

void SSE2_ConfigEditorMainWnd::WriteMaxSupplyConfigToJson()
{
    if (m_strGamePath.isEmpty() || !m_pCurrentFactionData)
        return;

    // 从界面收集最新值
    QVector<int>& levels = m_pCurrentFactionData->maxSupplyLevels;
    levels.clear();
    levels.append(ui.lineEdit_Maxsupply0->text().toInt());
    levels.append(ui.lineEdit_Maxsupply1->text().toInt());
    levels.append(ui.lineEdit_Maxsupply2->text().toInt());
    levels.append(ui.lineEdit_Maxsupply3->text().toInt());
    levels.append(ui.lineEdit_Maxsupply4->text().toInt());
    levels.append(ui.lineEdit_Maxsupply5->text().toInt());
    if (m_pCurrentFactionData->supplyLevelCount() == 7) // 瓦萨里
        levels.append(ui.lineEdit_Maxsupply6->text().toInt());

    QString filePath = QString("%1/entities/%2.player")
        .arg(m_strGamePath, m_pCurrentFactionData->playerFileName);
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
        return;

    QJsonDocument jsonDoc = QJsonDocument::fromJson(file.readAll());
    file.close();
    if (jsonDoc.isNull() || !jsonDoc.isObject())
        return;

    QJsonObject jsonObj = jsonDoc.object();
    QJsonObject maxSupplyObj = jsonObj.value("max_supply").toObject();

    QJsonArray levelsArray;
    for (int val : levels)
    {
        QJsonObject item;
        item["max_supply"] = val;
        levelsArray.append(item);
    }
    maxSupplyObj["levels"] = levelsArray;
    jsonObj["max_supply"] = maxSupplyObj;
    jsonDoc.setObject(jsonObj);

    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        file.write(jsonDoc.toJson());
        file.close();
    }
}

void SSE2_ConfigEditorMainWnd::ParseDefaultStartingAssetsFromJson(const QJsonDocument& jsonDoc)
{
    if (jsonDoc.isNull() || !jsonDoc.isObject() || !m_pCurrentFactionData)
        return;

    QJsonObject jsonObj = jsonDoc.object();
    QJsonObject assets = jsonObj.value("default_starting_assets").toObject();

    if (m_pCurrentFactionData->hasCredits())
        m_pCurrentFactionData->startingAssets.credits = assets.value("credits").toInt(1000);
    m_pCurrentFactionData->startingAssets.metal = assets.value("metal").toInt(400);
    m_pCurrentFactionData->startingAssets.crystal = assets.value("crystal").toInt(250);

    UpdateDefaultStartingAssetsData();
}

void SSE2_ConfigEditorMainWnd::UpdateDefaultStartingAssetsData()
{
    if (!m_pCurrentFactionData)
        return;

    const auto& assets = m_pCurrentFactionData->startingAssets;
    if (m_pCurrentFactionData->hasCredits())
        ui.lineEdit_default_starting_credit->setText(QString::number(assets.credits));
    ui.lineEdit_default_starting_metal->setText(QString::number(assets.metal));
    ui.lineEdit_default_starting_crystal->setText(QString::number(assets.crystal));
}

void SSE2_ConfigEditorMainWnd::WriteDefaultStartingAssetsToJson()
{
    if (m_strGamePath.isEmpty() || !m_pCurrentFactionData)
        return;

    // 从界面同步数值
    FactionStartingAssets& assets = m_pCurrentFactionData->startingAssets;
    if (m_pCurrentFactionData->hasCredits())
        assets.credits = ui.lineEdit_default_starting_credit->text().toInt();
    assets.metal = ui.lineEdit_default_starting_metal->text().toInt();
    assets.crystal = ui.lineEdit_default_starting_crystal->text().toInt();

    QString filePath = QString("%1/entities/%2.player")
        .arg(m_strGamePath, m_pCurrentFactionData->playerFileName);
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
        return;

    QJsonDocument jsonDoc = QJsonDocument::fromJson(file.readAll());
    file.close();
    if (jsonDoc.isNull() || !jsonDoc.isObject())
        return;

    QJsonObject jsonObj = jsonDoc.object();
    QJsonObject assetsObj = jsonObj.value("default_starting_assets").toObject();
    if (m_pCurrentFactionData->hasCredits())
        assetsObj["credits"] = assets.credits;
    assetsObj["metal"] = assets.metal;
    assetsObj["crystal"] = assets.crystal;
    jsonObj["default_starting_assets"] = assetsObj;
    jsonDoc.setObject(jsonObj);

    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        file.write(jsonDoc.toJson());
        file.close();
    }
}

void SSE2_ConfigEditorMainWnd::IntiEditor()
{
    ui.lineEdit_Titan->setValidator(new QIntValidator(0, 50, this));
    ui.lineEdit_SuperCapitalship->setValidator(new QIntValidator(0, 50, this));
    ui.lineEdit_starStarbase->setValidator(new QIntValidator(0, 50, this));
    ui.lineEdit_planetStarbase->setValidator(new QIntValidator(0, 50, this));

    // 添加阵营下拉选项（顺序须与 eFaction 枚举一致）
    ui.comboBox_faction->addItem(tr("贸易联盟忠诚派"));   // Faction_TL
    ui.comboBox_faction->addItem(tr("贸易联盟反叛派"));   // Faction_TR
    ui.comboBox_faction->addItem(tr("瓦萨里忠诚派"));     // Faction_VL
    ui.comboBox_faction->addItem(tr("瓦萨里反叛派"));     // Faction_VR
    ui.comboBox_faction->addItem(tr("圣临忠诚派"));       // Faction_AL
    ui.comboBox_faction->addItem(tr("圣临反叛派"));       // Faction_AR

    for (int i = 1; i <= 10; ++i)
        ui.comboBox_Levels->addItem(QString::number(i));
}

void SSE2_ConfigEditorMainWnd::OnEditFinished()
{
    if (!m_pCurrentFactionData)
        return;

    m_pCurrentFactionData->unitLimits.titan = ui.lineEdit_Titan->text().toInt();
    m_pCurrentFactionData->unitLimits.superCapitalShip = ui.lineEdit_SuperCapitalship->text().toInt();
    m_pCurrentFactionData->unitLimits.starStarbase = ui.lineEdit_starStarbase->text().toInt();
    m_pCurrentFactionData->unitLimits.planetStarbase = ui.lineEdit_planetStarbase->text().toInt();

    if (m_pCurrentFactionData->hasCredits())
        m_pCurrentFactionData->startingAssets.credits = ui.lineEdit_default_starting_credit->text().toInt();
    m_pCurrentFactionData->startingAssets.metal = ui.lineEdit_default_starting_metal->text().toInt();
    m_pCurrentFactionData->startingAssets.crystal = ui.lineEdit_default_starting_crystal->text().toInt();
}

void SSE2_ConfigEditorMainWnd::OnCapitalshipTypeOrLevelChanged()
{
    if (!m_pCurrentFactionData)
        return;

    QVector<stuCapitalshipInfo>& ships = m_pCurrentFactionData->capitalShips;

    // 保存旧等级数据
    if (m_iCurrentShipIndex >= 0 && m_iCurrentShipIndex < 5 &&
        m_iCurrentLevelIndex >= 0 && m_iCurrentLevelIndex < 10)
    {
        stuCapitalshipLevelInfo& old = ships[m_iCurrentShipIndex].LevelInfo[m_iCurrentLevelIndex];
        old.MaxHull = ui.lineEdit_MaxHull->text().toDouble();
        old.HullRestoreRate = ui.lineEdit_HullRestoreRate->text().toDouble();
        old.HullRestoreCooldown = ui.lineEdit_HullRestoreCooldown->text().toDouble();
        old.HullRestoreScale = ui.lineEdit_HullRestoreScalar->text().toDouble();
        old.HullCrippledPercentage = ui.lineEdit_CrippledPercentage->text().toDouble();
        old.MaxArmor = ui.lineEdit_MaxArmor->text().toDouble();
        old.ArmorRestoreRate = ui.lineEdit_ArmorRestoreRate->text().toDouble();
        old.ArmorRestoreCooldown = ui.lineEdit_ArmorRestoreCooldown->text().toDouble();
        old.ArmorRestoreScale = ui.lineEdit_ArmorRestoreScalar->text().toDouble();
        old.ArmorStrength = ui.lineEdit_ArmorStrength->text().toDouble();
        old.MaxShield = ui.lineEdit_MaxShield->text().toDouble();
        old.ShieldRestoreRate = ui.lineEdit_ShieleRestoreRate->text().toDouble();
        old.ShieldRestoreCooldown = ui.lineEdit_ShieldRestoreCooldown->text().toDouble();
        old.ShieldRestoreScale = ui.lineEdit_Titan_16->text().toDouble();
    }

    int shipIdx = ui.comboBox_CapitalShip->currentIndex();
    int levelIdx = ui.comboBox_Levels->currentIndex();
    if (shipIdx < 0 || shipIdx >= 5 || levelIdx < 0 || levelIdx >= 10)
        return;

    const stuCapitalshipLevelInfo& li = ships[shipIdx].LevelInfo[levelIdx];
    ui.lineEdit_MaxHull->setText(QString::number(li.MaxHull, 'f', 1));
    ui.lineEdit_HullRestoreRate->setText(QString::number(li.HullRestoreRate, 'f', 1));
    ui.lineEdit_HullRestoreCooldown->setText(QString::number(li.HullRestoreCooldown, 'f', 1));
    ui.lineEdit_HullRestoreScalar->setText(QString::number(li.HullRestoreScale, 'f', 1));
    ui.lineEdit_CrippledPercentage->setText(QString::number(li.HullCrippledPercentage, 'f', 2));
    ui.lineEdit_MaxArmor->setText(QString::number(li.MaxArmor, 'f', 1));
    ui.lineEdit_ArmorRestoreRate->setText(QString::number(li.ArmorRestoreRate, 'f', 1));
    ui.lineEdit_ArmorRestoreCooldown->setText(QString::number(li.ArmorRestoreCooldown, 'f', 1));
    ui.lineEdit_ArmorRestoreScalar->setText(QString::number(li.ArmorRestoreScale, 'f', 1));
    ui.lineEdit_ArmorStrength->setText(QString::number(li.ArmorStrength, 'f', 1));
    ui.lineEdit_MaxShield->setText(QString::number(li.MaxShield, 'f', 1));
    ui.lineEdit_ShieleRestoreRate->setText(QString::number(li.ShieldRestoreRate, 'f', 1));
    ui.lineEdit_ShieldRestoreCooldown->setText(QString::number(li.ShieldRestoreCooldown, 'f', 1));
    ui.lineEdit_Titan_16->setText(QString::number(li.ShieldRestoreScale, 'f', 1));

    ui.lineEdit_NextExp->setText("0");

    m_iCurrentShipIndex = shipIdx;
    m_iCurrentLevelIndex = levelIdx;
}
