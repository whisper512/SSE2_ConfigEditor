#pragma once

#include <QString>
#include <QStringList>
#include <QVector>
#include <QMap>

// ============================================
// 阵营 .player 文件名字符串宏
// ============================================
#define TL_PlayerConfig "trader_loyalist"
#define TR_PlayerConfig "trader_rebel"
#define VL_PlayerConfig "vasari_loyalist"
#define VR_PlayerConfig "vasari_rebel"
#define AL_PlayerConfig "advent_loyalist"
#define AR_PlayerConfig "advent_rebel"

// ============================================
// 主力舰 .unit 文件名字符串宏
// ============================================
#define Advent_BattleCaptialShip        "advent_battle_capital_ship"
#define Advent_ColonyCaptialShip        "advent_colony_capital_ship"
#define Advent_CarrierCaptialShip       "advent_carrier_capital_ship"
#define Advent_BattlePsionicCapitalShip "advent_battle_psionic_capital_ship"
#define Advent_PlanetPsionicCaptialShip "advent_planet_psionic_capital_ship"

#define Trader_BattleCaptialShip   "Trader_battle_capital_ship"
#define Trader_ColonyCaptialShip   "Trader_colony_capital_ship"
#define Trader_CarrierCaptialShip  "Trader_carrier_capital_ship"
#define Trader_SiegeCaptialShip    "Trader_siege_capital_ship"
#define Trader_SupportCaptialShip  "Trader_support_capital_ship"

#define Vasari_BattleCaptialShip   "vasari_battle_capital_ship"
#define Vasari_ColonyCaptialShip   "vasari_colony_capital_ship"
#define Vasari_CarrierCaptialShip  "vasari_carrier_capital_ship"
#define Vasari_SiegeCaptialShip    "vasari_siege_capital_ship"
#define Vasari_MarauderCaptialShip "vasari_marauder_capital_ship"

// ============================================
// 阵营枚举
// ============================================
enum eFaction
{
    Faction_TL,
    Faction_TR,
    Faction_VL,
    Faction_VR,
    Faction_AL,
    Faction_AR,
    Faction_Count
};

// ============================================
// 主力舰索引枚举
// ============================================
enum eCapitalshipIndex
{
    Capitalship_A,
    Capitalship_B,
    Capitalship_C,
    Capitalship_D,
    Capitalship_E
};

// ============================================
// 主力舰单等级数据
// ============================================
struct stuCapitalshipLevelInfo
{
    double MaxHull = 0.0;
    double HullRestoreRate = 0.0;
    double HullRestoreCooldown = 0.0;
    double HullRestoreScale = 0.0;
    double HullCrippledPercentage = 0.0;
    double MaxArmor = 0.0;
    double ArmorRestoreRate = 0.0;
    double ArmorRestoreCooldown = 0.0;
    double ArmorRestoreScale = 0.0;
    double ArmorStrength = 0.0;
    double MaxShield = 0.0;
    double ShieldRestoreRate = 0.0;
    double ShieldRestoreCooldown = 0.0;
    double ShieldRestoreScale = 0.0;
};

// ============================================
// 主力舰结构体
// ============================================
struct stuCapitalshipInfo
{
    eCapitalshipIndex eCapitalship = Capitalship_A;
    stuCapitalshipLevelInfo LevelInfo[10];
};

// ============================================
// 单位限制配置
// ============================================
struct FactionUnitLimits
{
    int titan = 1;
    int superCapitalShip = 1;
    int starStarbase = 4;
    int planetStarbase = 1;
};

// ============================================
// 起始资产配置
// ============================================
struct FactionStartingAssets
{
    int credits = 1000;
    int metal = 400;
    int crystal = 250;
    bool hasCredits = true;   // 瓦萨里阵营没有 credits 字段
};

// ============================================
// 单个阵营的全部可编辑数据
// ============================================
struct FactionData
{
    eFaction      faction;
    QString       displayName;
    QString       playerFileName;              // 无 .player 后缀
    QStringList   capitalShipFileNames;        // 5个 .unit 文件名
    QStringList   capitalShipDisplayNames;     // 5个中文名

    FactionUnitLimits      unitLimits;
    QVector<int>           maxSupplyLevels;
    FactionStartingAssets  startingAssets;
    QVector<stuCapitalshipInfo> capitalShips;

    // 辅助
    bool hasCredits()       const { return startingAssets.hasCredits; }
    int  supplyLevelCount() const { return maxSupplyLevels.size(); }

    FactionData()
        : faction(Faction_TL)
    {
        capitalShips.resize(5);
        // 直接使用枚举常量，避免 static_cast 可能引起的编译器警告
        capitalShips[0].eCapitalship = Capitalship_A;
        capitalShips[1].eCapitalship = Capitalship_B;
        capitalShips[2].eCapitalship = Capitalship_C;
        capitalShips[3].eCapitalship = Capitalship_D;
        capitalShips[4].eCapitalship = Capitalship_E;
    }
};

// ============================================
// 生成 6 个阵营的默认配置（不含 JSON 数据，仅文件名/显示名等）
// 实现放在对应的 .cpp 文件中
// ============================================
inline QMap<eFaction, FactionData> CreateDefaultFactionConfigs()
{
    QMap<eFaction, FactionData> map;

    // T 族忠诚派
    {
        FactionData d;
        d.faction = Faction_TL;
        d.displayName = QStringLiteral("贸易联盟忠诚派");
        d.playerFileName = TL_PlayerConfig;
        d.capitalShipFileNames = QStringList{
            Trader_BattleCaptialShip,
            Trader_ColonyCaptialShip,
            Trader_CarrierCaptialShip,
            Trader_SiegeCaptialShip,
            Trader_SupportCaptialShip
        };
        d.capitalShipDisplayNames = QStringList{ "科尔", "艾肯", "瓦索", "玛莎", "杜诺夫" };
        d.startingAssets.hasCredits = true;
        d.maxSupplyLevels.resize(6);
        map.insert(Faction_TL, d);
    }

    // T 族反叛派
    {
        FactionData d;
        d.faction = Faction_TR;
        d.displayName = QStringLiteral("贸易联盟反叛派");
        d.playerFileName = TR_PlayerConfig;
        d.capitalShipFileNames = QStringList{
            Trader_BattleCaptialShip,
            Trader_ColonyCaptialShip,
            Trader_CarrierCaptialShip,
            Trader_SiegeCaptialShip,
            Trader_SupportCaptialShip
        };
        d.capitalShipDisplayNames = QStringList{ "科尔", "艾肯", "瓦索", "玛莎", "杜诺夫" };
        d.startingAssets.hasCredits = true;
        d.maxSupplyLevels.resize(6);
        map.insert(Faction_TR, d);
    }

    // V 族忠诚派
    {
        FactionData d;
        d.faction = Faction_VL;
        d.displayName = QStringLiteral("瓦萨里忠诚派");
        d.playerFileName = VL_PlayerConfig;
        d.capitalShipFileNames = QStringList{
            Vasari_BattleCaptialShip,
            Vasari_ColonyCaptialShip,
            Vasari_CarrierCaptialShip,
            Vasari_SiegeCaptialShip,
            Vasari_MarauderCaptialShip
        };
        d.capitalShipDisplayNames = QStringList{ "克尔图", "加拉苏", "基兰查", "瓦诺克斯", "安特可" };
        d.startingAssets.hasCredits = false;
        d.maxSupplyLevels.resize(7);
        map.insert(Faction_VL, d);
    }

    // V 族反叛派
    {
        FactionData d;
        d.faction = Faction_VR;
        d.displayName = QStringLiteral("瓦萨里反叛派");
        d.playerFileName = VR_PlayerConfig;
        d.capitalShipFileNames = QStringList{
            Vasari_BattleCaptialShip,
            Vasari_ColonyCaptialShip,
            Vasari_CarrierCaptialShip,
            Vasari_SiegeCaptialShip,
            Vasari_MarauderCaptialShip
        };
        d.capitalShipDisplayNames = QStringList{ "克尔图", "加拉苏", "基兰查", "瓦诺克斯", "安特可" };
        d.startingAssets.hasCredits = false;
        d.maxSupplyLevels.resize(7);
        map.insert(Faction_VR, d);
    }

    // A 族忠诚派
    {
        FactionData d;
        d.faction = Faction_AL;
        d.displayName = QStringLiteral("圣临忠诚派");
        d.playerFileName = AL_PlayerConfig;
        d.capitalShipFileNames = QStringList{
            Advent_BattleCaptialShip,
            Advent_ColonyCaptialShip,
            Advent_CarrierCaptialShip,
            Advent_BattlePsionicCapitalShip,
            Advent_PlanetPsionicCaptialShip
        };
        d.capitalShipDisplayNames = QStringList{ "光辉", "先祖", "宁静", "超度", "天启" };
        d.startingAssets.hasCredits = true;
        d.maxSupplyLevels.resize(6);
        map.insert(Faction_AL, d);
    }

    // A 族反叛派
    {
        FactionData d;
        d.faction = Faction_AR;
        d.displayName = QStringLiteral("圣临反叛派");
        d.playerFileName = AR_PlayerConfig;
        d.capitalShipFileNames = QStringList{
            Advent_BattleCaptialShip,
            Advent_ColonyCaptialShip,
            Advent_CarrierCaptialShip,
            Advent_BattlePsionicCapitalShip,
            Advent_PlanetPsionicCaptialShip
        };
        d.capitalShipDisplayNames = QStringList{ "光辉", "先祖", "宁静", "超度", "天启" };
        d.startingAssets.hasCredits = true;
        d.maxSupplyLevels.resize(6);
        map.insert(Faction_AR, d);
    }

    return map;
}
